/*
 * Copyright 2015 Rockchip Electronics Co. LTD
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#if defined(linux) && !defined(__ANDROID__)
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include "rk_os_env.h"

#define ENV_BUF_SIZE_LINUX  1024

// 获取环境变量的长整形结果
RK_S32 os_get_env_u32(const char *name, RK_U32 *value, RK_U32 default_value)
{
    char *ptr = getenv(name);
    if (NULL == ptr) {
        *value = default_value;
    } else {
        char *endptr;
        // 选择转换的进制
        int base = (ptr[0] == '0' && ptr[1] == 'x') ? (16) : (10);
        errno = 0;
        // 将字符串转换为无符号长整形,如果没问题的话endptr一般为ptr的尾部
        *value = strtoul(ptr, &endptr, base);
        if (errno || (ptr == endptr)) {
            errno = 0;
            *value = default_value;
        }
    }
    return 0;
}

// 获取环境变量的字符串格式
RK_S32 os_get_env_str(const char *name, const char **value, const char *default_value)
{
    *value = getenv(name);
    if (NULL == *value) {
        *value = default_value;
    }
    return 0;
}

// 将值数据value转换为字符串保存在name指针中
RK_S32 os_set_env_u32(const char *name, RK_U32 value)
{
    char buf[ENV_BUF_SIZE_LINUX];
    snprintf(buf, sizeof(buf) - 1, "%u", value);
    return setenv(name, buf, 1);
}

// 将字符数据复制给name指针
RK_S32 os_set_env_str(const char *name, char *value)
{
    return setenv(name, value, 1);
}

#endif

