#pragma once

#if __cplusplus >= 201703L
#define __NVL_CPP17 1
#else
#define __NVL_CPP14 1
#endif

#if __NVL_CPP17
#define __NVL_RETURN_MOVE(arg) arg
#else
#define __NVL_RETURN_MOVE(arg) std::move(arg)
#endif
