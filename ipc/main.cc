#include "logging.h"

#include "arg_parse.h"
#include "backtrace_service.h"
#include "ring_buffer.h"
#include "share_mem.h"
#include "thread_pool.h"

#include <memory>
#include <string>
#include <thread>

std::shared_ptr<argparse::ArgumentParser> args_parser(int argc, char **argv) {

  std::shared_ptr<argparse::ArgumentParser> program =
      std::make_shared<argparse::ArgumentParser>(argv[0]);

  program->add_argument("-r", "--role")
      .required()
      .help("Speficy the role for this instance:  server or client");

  program->add_argument("--num_producer")
      .required()
      .help("Speficy the num of threads for producers");
  program->add_argument("--num_consumer")
      .required()
      .help("Speficy the num of threads for consumers");

  program->add_argument("--buffer_size")
      .required()
      .help("Speficy the buffer size in bytes for shm");

  program->add_argument("--is_overwrite")
      .required()
      .help("Speficy the buffer size can be overwrite? or not");
  try {
    program->parse_args(argc, argv);
  } catch (const std::exception &err) {
    std::cerr << err.what() << std::endl;
    std::cerr << *program;
    std::exit(1);
  }

  return std::move(program);
}

struct Config {
  std::string role;
  int32_t num_producers;
  int32_t num_consumers;
  int32_t buf_size_in_bytes;
  bool is_overwrite;
};

void benchmarking_shm(struct Config *conf_ptr) {
  CHECK(conf_ptr != nullptr) << "Invalid config";

  auto shm_obj = shm::SharedMemory(std::string(__func__));
  void *addr = nullptr;
  volatile int8_t *signal_addr = nullptr; // last byte for signal to terminate
  if (conf_ptr->role == "owner") {
    addr = shm_obj.get_virtual_addr(conf_ptr->buf_size_in_bytes, true,
                                    conf_ptr->is_overwrite);
    signal_addr =
        reinterpret_cast<int8_t *>(addr) + conf_ptr->buf_size_in_bytes - 1;
    *signal_addr = 55;
    std::string msg = "hello, I'm " + conf_ptr->role;
    std::memcpy(addr, msg.c_str(), msg.size());
    while (*signal_addr == 55) {
      VLOG(1) << conf_ptr->role << " is working in the background...";
      std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    VLOG(1) << conf_ptr->role
            << " get message: " << reinterpret_cast<char *>(addr)
            << ", signal=" << int(*signal_addr);

  } else if (conf_ptr->role == "borrower") {
    addr = shm_obj.get_virtual_addr(conf_ptr->buf_size_in_bytes, false);
    signal_addr =
        reinterpret_cast<int8_t *>(addr) + conf_ptr->buf_size_in_bytes - 1;
    VLOG(1) << conf_ptr->role
            << " get message: " << reinterpret_cast<char *>(addr);
    std::string msg = "hello, I'm " + conf_ptr->role;
    std::memcpy(addr, msg.c_str(), msg.size());
    *signal_addr = 66;
  } else {
    LOG(FATAL) << "Unknown role " << conf_ptr->role;
  }
  return;
}

void benchmarking_ringbuffer(struct Config *conf_ptr) {
  CHECK(conf_ptr != nullptr) << "Invalid config";
  char *buf_place =
      new char[1024l * 1000 * 6 * sizeof(int) + sizeof(wilt::Ring_)];
  CHECK(buf_place != nullptr) << "Invalid buffer: empty";

  // std::shared_ptr<wilt::Ring<int>> rb =
  //     std::make_shared<wilt::Ring<int>>(1024l * 1000 * 6);
  wilt::Ring<int> *rb =
      new (buf_place) wilt::Ring<int>(1024l * 1000 * 6, buf_place);

  VLOG(0) << RLOG::make_string("sizeof(Ring):%d, sizeof(Ring_):%d",
                               sizeof(wilt::Ring<int>), sizeof(wilt::Ring_))
          << RLOG::make_string(", rb=%p, place_buf=%p sizeof(ring_)=%d", rb,
                               buf_place, sizeof(wilt::Ring_))
          << ", info: " << rb->info();

  BS::thread_pool producer_pool(conf_ptr->num_producers);
  BS::thread_pool consumer_pool(conf_ptr->num_consumers);
  for (int idx = 0; idx < conf_ptr->num_producers; idx++) {
    auto ret = producer_pool.submit_task([&rb, conf_ptr]() {
      VLOG(1) << "Producer is writing data into the queue";
      for (int rep = 0; rep < 10000l * conf_ptr->num_consumers; rep++) {
        while (!rb->try_write(rep))
          ;
      }
    });
  };
  std::vector<std::future<int64_t>> result_placeholder;
  for (int idx = 0; idx < conf_ptr->num_consumers; idx++) {
    auto task = consumer_pool.submit_task([&rb, conf_ptr]() -> int64_t {
      VLOG(1) << "Consumer is reading data into the queue";
      int64_t result = 0;
      for (int rep = 0; rep < 10000l * conf_ptr->num_producers; rep++) {
        int data = 0;
        while (!rb->try_read(data))
          ;
        result += data;
      }
      return result;
    });
    result_placeholder.emplace_back(std::move(task));
  }

  producer_pool.wait();
  consumer_pool.wait();
  int64_t result = 0;
  for (auto &each : result_placeholder) {
    result += each.get();
  }

  delete[] buf_place;

  VLOG(0) << "Summation for (1+....+10000)*"
          << conf_ptr->num_consumers * conf_ptr->num_producers << "=" << result;
  return;
}

int main(int argc, char **argv) {
  VLOG(1) << "Init in main function";

  auto args_str = args_parser(argc, argv);
  struct Config conf = {
      .role = args_str->get<std::string>("-r"),
      .num_producers = std::stoi(args_str->get<std::string>("--num_producer")),
      .num_consumers = std::stoi(args_str->get<std::string>("--num_consumer")),
      .buf_size_in_bytes =
          std::stoi(args_str->get<std::string>("--buffer_size")),
      .is_overwrite = static_cast<bool>(
          std::stoi(args_str->get<std::string>("--is_overwrite")))};

  //   benchmarking_shm(&conf);
  benchmarking_ringbuffer(&conf);

  return 0;
}
