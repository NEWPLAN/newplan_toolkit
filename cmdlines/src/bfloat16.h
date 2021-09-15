/* Copyright 2020 The NEWPLAN_TOOLKIT Authors. All Rights Reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
==============================================================================*/

#ifndef NEWPLAN_TOOLKIT_CORE_PLATFORM_BFLOAT16_BFLOAT16_H_
#define NEWPLAN_TOOLKIT_CORE_PLATFORM_BFLOAT16_BFLOAT16_H_

// clang-format off
#include "byte_order.h"

// clang-format on

namespace newplan_toolkit
{
    struct bfloat16
    {
        unsigned short int data;

    public:
        bfloat16()
        {
            data = 0;
        }
        //cast to float
        operator float()
        {
            unsigned int proc = data << 16;
            return *reinterpret_cast<float *>(&proc);
        }
        //cast to bfloat16
        bfloat16 &operator=(float float_val)
        {
            data = (*reinterpret_cast<unsigned int *>(&float_val)) >> 16;
            return *this;
        }
    };
} // namespace newplan_toolkit

#endif // NEWPLAN_TOOLKIT_CORE_LIB_BFLOAT16_BFLOAT16_H_
