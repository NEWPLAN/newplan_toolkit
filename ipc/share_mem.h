#ifndef __SHARED_MEMORY_IPC_H__
#define __SHARED_MEMORY_IPC_H__
#include <cstddef>

#include "logging.h"
#include <cstring>
#include <fcntl.h> /* For O_* constants */

#include <string>
#include <sys/mman.h>
#include <sys/stat.h> /* For mode constants */

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#include <unistd.h>

namespace shm
{
    class SharedMemory
    {
    public:
        explicit SharedMemory(std::string key) : key_(key)
        {
            LOG(INFO) << "Create a shared_memory object: " << info();
        }

        virtual ~SharedMemory()
        {
            LOG(INFO) << "Release a shared_memory object: " << info();
            release();
        }

        void *get_virtual_addr(size_t buf_size, bool take_ownership,
                               bool overwrite = false)
        {
            if (!is_initialized)
            {
                int open_flag = 0;
                if (take_ownership)
                {
                    is_owner_ = true;
                    open_flag += O_CREAT;
                    if (!overwrite)
                        open_flag += O_EXCL;
                    else
                        open_flag += O_TRUNC;
                }
                else
                {
                    is_owner_ = false;
                }
                open_flag += O_RDWR;
                fd_ = shm_open(key_.c_str(), open_flag, 0777);
                CHECK(fd_ >= 0) << "Failed open the shm, for reason:" << strerror(errno)
                                << ", obj_info: " << info();

                CHECK(buf_size > 0) << "Invalid buffer size: " << buf_size;
                int ret = ftruncate(fd_, buf_size);
                bufffer_size = buf_size;

                CHECK(ret == 0) << RLOG::make_string(
                                       "Failed to truncate the buffer(fd=%d) to size=%d",
                                       fd_, buf_size)
                                << ", for reason:" << strerror(errno)
                                << ", obj_info: " << info();

                // clang-format off
      mem_domain_ = mmap(NULL, buf_size, 
                    PROT_READ | PROT_WRITE, MAP_SHARED, 
                    fd_, 0);
                // clang-format on
                CHECK(mem_domain_) << "Failed mmap addr for shared_mem: " << info();

                is_initialized = true;
            }
            else
            {
                LOG(WARNING) << "The shm_obj has been initialized: " << info();
            }

            return mem_domain_;
        }

        std::string info()
        {
            std::string result;
            result += RLOG::make_string(
                "key = %s, fd = %d, memory_domain = %x, is_owner = %s", key_.c_str(),
                fd_, mem_domain_, (is_owner_ == true ? "true" : "false"));
            return result;
        }

    private:
        void release()
        {
            int ret = 0;
            std::string op = "";
            if (mem_domain_ != nullptr)
            {
                if ((ret = munmap(mem_domain_, bufffer_size)) != 0)
                {
                    op = "unmmap the mem buffer";
                    goto handler_release_err;
                }
                mem_domain_ = nullptr;
                bufffer_size = 0;
            }

            if (fd_ != -1 && is_owner_)
            {
                if ((ret = shm_unlink(key_.c_str())) != 0)
                {
                    op = "unlinked the shared_obj";
                    goto handler_release_err;
                }
                fd_ = -1;
            }
            return;

        handler_release_err:
            CHECK(ret == 0) << "Failed to " << op << " for reason: " << strerror(errno)
                            << ", ctx:" << info();
            return;
        }

    private:
        int fd_ = -1;
        bool is_initialized = false;
        void *mem_domain_ = nullptr;
        bool is_owner_ = false;
        size_t bufffer_size = -1;
        std::string key_ = "";
    };
} // namespace shm
#endif