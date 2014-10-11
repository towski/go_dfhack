#ifndef _MY_PACKAGE_FOO_HPP_
#define _MY_PACKAGE_FOO_HPP_

#define LINUX_BUILD
#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif
    void Connect();
    void Update();
    struct CDwarf* GetDwarf(int i);
#ifdef __cplusplus
}
#endif

#endif
