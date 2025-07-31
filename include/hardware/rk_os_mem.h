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

/*                      RK官方内存排布方案                 */
#ifndef __RK_OS_MEM_H__
#define __RK_OS_MEM_H__

#ifdef __cplusplus
extern "C" {
#endif

/*
    os_malloc 函数：
        功能：分配指定大小和对齐方式的内存块
        参数：
        memptr：用于存储分配的内存指针的地址
        alignment：内存对齐要求（必须是 2 的幂）
        size：要分配的内存大小（字节）
        实现：直接调用 posix_memalign，这是一个 POSIX 标准函数，支持指定对齐方式的内存分配
        返回值：返回 posix_memalign 的结果（0 表示成功，非 0 表示错误）
*/
int os_malloc(void **memptr, size_t alignment, size_t size);

/*
    功能：重新分配内存块大小
        参数：
        src：原内存块指针
        dst：用于存储新内存块指针的地址
        alignment：对齐要求（当前实现未使用）
        size：新的内存块大小
        实现：调用标准库的 realloc 函数，但忽略了对齐参数（这可能是一个潜在问题）
        返回值：成功返回 0，失败返回 - 1
*/
int os_realloc(void *src, void **dst, size_t alignment, size_t size);

/*
    os_free 函数：
        功能：释放之前分配的内存块
        参数：ptr 要释放的内存指针
        实现：直接调用标准库的 free 函数
*/
void os_free(void *ptr);

#ifdef __cplusplus
}
#endif

#endif /*__RK_OS_MEM_H__*/
