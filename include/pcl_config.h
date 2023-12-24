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


#if !defined(PCL_CONFIG_H)
#define PCL_CONFIG_H

#if defined(HAVE_CONFIG_H)
#include "config.h"
#elif defined(HAVE_WINCONFIG_H)
#include "winconfig.h"
#endif /* #if defined(HAVE_CONFIG_H) */

#if defined(__APPLE__)
/*
 * Need to use setjmp/longjmp on OSX, since the ucontext bits are
 * both broken and deprecated.
 */
#define CO_MULTI_THREAD

#elif defined(HAVE_GETCONTEXT) && defined(HAVE_MAKECONTEXT) && defined(HAVE_SWAPCONTEXT)

/*
 * Use this if the system has a working getcontext/makecontext/swapcontext
 * implementation.
 */
#define CO_USE_UCONEXT
/*
 * Use threads.
 */
#define CO_MULTI_THREAD

#elif defined(HAVE_SIGACTION)

/*
 * Use this to have the generic signal implementation (not working on
 * Windows). Suggested on generic Unix implementations or on Linux with
 * CPU different from x86 family.
 */
#define CO_USE_SIGCONTEXT

/*
 * Use this in conjuction with CO_USE_SIGCONTEXT to use the sigaltstack
 * environment (suggested when CO_USE_SIGCONTEXT is defined).
 */
#if defined(HAVE_SIGALTSTACK)
#define CO_HAS_SIGALTSTACK
#endif

#else
/*
 * This will be using setjmp/longjmp
 */

/*
 * Use threads.
 */
#define CO_MULTI_THREAD

#endif

#endif

