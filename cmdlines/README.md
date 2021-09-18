## INTRODUCTION:
cmdlines is a simple wrapper for C++ programmer loading arguments from commandline

## REQUIREMENTS:
- [glog](https://github.com/google/glog): C++ implementation of the Google logging module
- [abseil-cpp](https://github.com/abseil/abseil-cpp): Abseil Common Libraries (C++), an enhanced cpp library from Google 

## COMPILE:
mkdir build && cmake ..

## USAGE:
```bash
./my_app_project --input_text=tensorflow/compiler/xla/hlo_files/a_and_b_simple.hlo --policy_config=tensorflow/compiler/xla/hlo_files/pp_scheduling_config_4x10.json  --pp_stage_size=4 --print_output --rank_info="0:1, 1:1, 2:1, 3:1" --num_partitions=10 --use_large_float_range
WARNING: Logging before InitGoogleLogging() is written to STDERR
I0918 11:22:38.387632  5981 main.cc:170] 0:1, 1:1, 2:1, 3:1 = [0:1; 1:1; 2:1; 3:1]
I0918 11:22:38.387778  5981 main.cc:171] use_large_float_range = 1
I0918 11:22:38.387789  5981 main.cc:172] num_partitions = 10
I0918 11:22:38.387796  5981 main.cc:173] pp_stage_size = 4
I0918 11:22:38.387806  5981 main.cc:174] unified_literal = 0
I0918 11:22:38.387815  5981 main.cc:175] input_text = tensorflow/compiler/xla/hlo_files/a_and_b_simple.hlo
```