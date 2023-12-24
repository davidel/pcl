/*    Copyright 2023 Davide Libenzi
 * 
 *    Licensed under the Apache License, Version 2.0 (the "License");
 *    you may not use this file except in compliance with the License.
 *    You may obtain a copy of the License at
 * 
 *        http://www.apache.org/licenses/LICENSE-2.0
 * 
 *    Unless required by applicable law or agreed to in writing, software
 *    distributed under the License is distributed on an "AS IS" BASIS,
 *    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    See the License for the specific language governing permissions and
 *    limitations under the License.
 * 
 */


#if !defined(PCL_H)
#define PCL_H

#ifdef __cplusplus
#define PCLXC extern "C"
#else
#define PCLXC
#endif

typedef void *coroutine_t;

PCLXC int co_thread_init(void);
PCLXC void co_thread_cleanup(void);

PCLXC coroutine_t co_create(void (*func)(void *), void *data, void *stack,
			    int size);
PCLXC void co_delete(coroutine_t coro);
PCLXC void co_call(coroutine_t coro);
PCLXC void co_resume(void);
PCLXC void co_exit_to(coroutine_t coro);
PCLXC void co_exit(void);
PCLXC coroutine_t co_current(void);
PCLXC void *co_get_data(coroutine_t coro);
PCLXC void *co_set_data(coroutine_t coro, void *data);

#endif

