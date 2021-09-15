

#ifndef NEWPLAN_TOOLKIT_CORE_PLATFORM_DEFAULT_CORD_H_
#define NEWPLAN_TOOLKIT_CORE_PLATFORM_DEFAULT_CORD_H_

// It seems CORD doesn't work well with CUDA <= 10.2
#if !defined(__CUDACC__) || ((defined(__CUDACC__) && CUDA_VERSOIN > 10020))

#include "absl/strings/cord.h"
#define TF_CORD_SUPPORT 1

#endif // __CUDACC__

#endif // NEWPLAN_TOOLKIT_CORE_PLATFORM_DEFAULT_CORD_H_
