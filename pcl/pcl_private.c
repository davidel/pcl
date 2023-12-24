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


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pcl_config.h"
#include "pcl.h"
#include "pcl_private.h"

static cothread_ctx *co_get_global_ctx(void)
{
	static cothread_ctx tctx;

	if (tctx.co_curr == NULL)
		tctx.co_curr = &tctx.co_main;

	return &tctx;
}

#if !defined(CO_MULTI_THREAD)
/*
 * Simple case, the single thread one ...
 */

int co_thread_init(void)
{
	return 0;
}

void co_thread_cleanup(void)
{

}

cothread_ctx *co_get_thread_ctx(void)
{
	return co_get_global_ctx();
}

#else
/*
 * MultiThread cases ...
 */

#if defined(_WIN32) && defined(_MSC_VER)
/*
 * On Windows, we can use the native TLS capabilities. Pretty easy ...
 */
static __declspec(thread) cothread_ctx *tctx;

int co_thread_init(void)
{
	if ((tctx = (cothread_ctx *)
	     malloc(sizeof(cothread_ctx))) == NULL) {
		perror("allocating context");
		return -1;
	}
	memset(tctx, 0, sizeof(*tctx));
	tctx->co_curr = &tctx->co_main;

	return 0;
}

void co_thread_cleanup(void)
{
	free(tctx);
}

cothread_ctx *co_get_thread_ctx(void)
{
	/*
	 * Even in MT mode, allows for the main thread to not call
	 * the co_thread_init()/co_thread_cleanup() functions.
	 */
	return tctx != NULL ? tctx: co_get_global_ctx();
}

#else
/*
 * On Unix, we use pthread. Sligthly more complicated ...
 */
#include <pthread.h>

static int valid_key;
static pthread_key_t key;
static pthread_once_t once_control = PTHREAD_ONCE_INIT;

static void co_once_init(void)
{
	if (pthread_key_create(&key, free))
		perror("creating TLS key");
	else
		valid_key++;
}

int co_thread_init(void)
{
	cothread_ctx *tctx;

	pthread_once(&once_control, co_once_init);
	if (!valid_key)
		return -1;

	if ((tctx = (cothread_ctx *)
	     malloc(sizeof(cothread_ctx))) == NULL) {
		perror("allocating context");
		return -1;
	}
	memset(tctx, 0, sizeof(*tctx));
	tctx->co_curr = &tctx->co_main;
	if (pthread_setspecific(key, tctx)) {
		perror("setting thread context");
		free(tctx);
		return -1;
	}

	return 0;
}

void co_thread_cleanup(void)
{

}

cothread_ctx *co_get_thread_ctx(void)
{
	cothread_ctx *tctx = (cothread_ctx *)
		(valid_key ? pthread_getspecific(key): NULL);

	/*
	 * Even in MT mode, allows for the main thread to not call
	 * the co_thread_init()/co_thread_cleanup() functions.
	 */
	return tctx != NULL ? tctx: co_get_global_ctx();
}

#endif
#endif

