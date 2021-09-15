#include <iostream>
#include "command_line_flags.h"
#include <string>
#include <vector>
#include <unordered_map>
#include <glog/logging.h>

struct AutoParallelOption //
{
    bool use_large_float_range = false;
    float abs_error_bound = 0.0f;
    float rel_error_bound = 0.0f;
    std::string input_format = "hlo";
    std::string input_text = "";
    std::string policy_config = "";
    int iterations = 1;
    bool print_hlo = false;
    int num_replicas = 1;
    int pp_stage_size = 4;
    bool execute_shared = false;
    bool enable_spmd_pass = false;
    bool print_literals = false;
    bool print_input = false;
    bool print_output = false;
    bool unified_literal = false;
    int num_partitions = 1;
    std::string rank_info = "";

    std::unordered_map<int, // the id of a pp stage
                       int> // the size of the stage
        stage_info;
    std::vector<std::vector<int>> resource_map;

    std::vector<std::vector<int>> &load_cluster_info() //
    {
        std::vector<std::string> tmp_rank_info = tokenize(this->rank_info, ',');
        for (auto &each_info : tmp_rank_info) //
        {
            std::vector<std::string> id_size_pair = tokenize(each_info, ':');
            CHECK(id_size_pair.size() == 2)
                << "Invalid of id:size pair of pp rank info";
            int id = std::atoi(id_size_pair[0].c_str());
            int size = std::atoi(id_size_pair[1].c_str());
            CHECK(stage_info.find(id) == stage_info.end())
                << "Stage id(" << id << ") with size:" << stage_info[id]
                << " already exists";
            stage_info[id] = size;
        }

        resource_map.resize(stage_info.size());
        for (auto &each_info : stage_info)
        {
            CHECK(each_info.first < resource_map.size())
                << "Invalid stage index: " << each_info.first << "-->"
                << resource_map.size() << "/" << stage_info.size();
            resource_map[each_info.first].resize(each_info.second);
        }
        int global_idx = 0;
        for (int stage_idx = 0; stage_idx < resource_map.size(); stage_idx++)
        {
            for (int rank_idx = 0; rank_idx < resource_map[stage_idx].size();
                 rank_idx++)
            {
                resource_map[stage_idx][rank_idx] = global_idx;
                global_idx += 1;
            }
        }

        return this->resource_map;
    }

    std::vector<std::string> tokenize(std::string &str, char delim) //
    {
        size_t start;
        size_t end = 0;
        std::vector<std::string> out;

        while ((start = str.find_first_not_of(delim, end)) != std::string::npos)
        {
            end = str.find(delim, start);
            out.push_back(str.substr(start, end - start));
        }
        return std::move(out);
    }
};

static AutoParallelOption get_options(int argc, char **argv)
{
    AutoParallelOption opts;
    std::vector<tensorflow::Flag> flag_list = {
        tensorflow::Flag("print_literals", &opts.print_literals,
                         "Print the input and result literals to stdout."),

        tensorflow::Flag(
            "use_large_float_range", &opts.use_large_float_range,
            "Generate floating point values using a large uniform-log "
            "distribution as opposed to a small uniform distribution."),
        tensorflow::Flag(
            "abs_error_bound", &opts.abs_error_bound,
            "The absolute error bound used when comparing the test and "
            "reference results."),
        tensorflow::Flag(
            "rel_error_bound", &opts.rel_error_bound,
            "The relative error bound used when comparing the test and "
            "reference results."),
        tensorflow::Flag("input_format", &opts.input_format,
                         "The format of the input file. Valid values:\n"
                         "  hlo : HLO textual format\n"
                         "  pb : xla::HloProto in binary proto format\n"
                         "  pbtxt : xla::HloProto in text proto format"),
        tensorflow::Flag(
            "input_text", &opts.input_text,
            "A path to a file containing the HLO module. Can also pass "
            "a this as argv[1], but this flag is more explicit."),
        tensorflow::Flag(
            "iterations", &opts.iterations,
            "The number of times to run the module. Each iteration will be run "
            "with different input data."),
        tensorflow::Flag("print_hlo", &opts.print_hlo,
                         "decide whether print the hlo ir or not"),
        tensorflow::Flag(
            "num_partitions", &opts.num_partitions,
            "decide how many partitions to used in model paralleism"),

        tensorflow::Flag("num_replicas", &opts.num_replicas,
                         "decide how many replicas to used in data paralleism"),
        tensorflow::Flag("execute_shared", &opts.execute_shared,
                         "decide whether using executable shared"),
        tensorflow::Flag("enable_spmd_pass", &opts.enable_spmd_pass,
                         "decide whether enable spmd pass"),
        tensorflow::Flag(
            "policy_config", &opts.policy_config,
            "A path to a file containing the split policy for a input_text."),

        tensorflow::Flag("pp_stage_size", &opts.pp_stage_size,
                         "the size of pp stage"),
        tensorflow::Flag("unified_literal", &opts.unified_literal,
                         "use unified_literal for input"),
        tensorflow::Flag("print_input", &opts.print_input,
                         "print the input literals"),
        tensorflow::Flag("print_output", &opts.print_output,
                         "print the output literals"),
        tensorflow::Flag("rank_info", &opts.rank_info,
                         "Speficy the rank info in format of "
                         "pp_id_0:pp_0_size [,pp_id_i:pp_i_size,...]")

    };

    //  xla::AppendDebugOptionsFlags(&flag_list);
    // The usage string includes the message at the top of the file, the
    // DebugOptions flags and the flags defined above.

    bool parse_ok = tensorflow::Flags::Parse(&argc, argv, flag_list);
    //tensorflow::port::InitMain("", &argc, &argv);
    if (!parse_ok)
    {
        VLOG(0) << tensorflow::Flags::Usage(argv[0], flag_list);
        LOG(FATAL) << "cannot parse cmd data";
    }

    return opts;
}

#include "str_util.h"

int main(int argc, char **argv)
{
    auto opts = get_options(argc, argv);
    std::vector<std::string> rank_info = tensorflow::str_util::Split(opts.rank_info, ',');
    VLOG(0) << opts.rank_info << " = [" << absl::StrJoin(rank_info, ";") + "]";

    return 0;
}