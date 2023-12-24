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


#if !defined(PCL_PRIVATE_H)
#define PCL_PRIVATE_H

#include <stdio.h>
#include <stdlib.h>
#include "pcl_config.h"
#include "pcl.h"

#if defined(CO_USE_UCONEXT)
#include <ucontext.h>

typedef ucontext_t co_core_ctx_t;
#else
#include <setjmp.h>

typedef jmp_buf co_core_ctx_t;
#endif

/*
 * The following value must be power of two (N^2).
 */
#define CO_STK_ALIGN 256
#define CO_STK_COROSIZE ((sizeof(coroutine) + CO_STK_ALIGN - 1) & ~(CO_STK_ALIGN - 1))
#define CO_MIN_SIZE (4 * 1024)

typedef struct s_co_ctx {
	co_core_ctx_t cc;
} co_ctx_t;

typedef struct s_coroutine {
	co_ctx_t ctx;
	int alloc;
	struct s_coroutine *caller;
	struct s_coroutine *restarget;
	void (*func)(void *);
	void *data;
} coroutine;

typedef struct s_cothread_ctx {
	coroutine co_main;
	coroutine *co_curr;
	coroutine *co_dhelper;
	coroutine *dchelper;
	char stk[CO_MIN_SIZE];
} cothread_ctx;

cothread_ctx *co_get_thread_ctx(void);

#endif

