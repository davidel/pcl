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
#include <stdarg.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>
#include <pthread.h>
#include <pcl.h>


#define MIN_MEASURE_TIME 2000000ULL
#define CO_STACK_SIZE (8 * 1024)


static unsigned long long getustime(void)
{
	struct timeval tm;

	gettimeofday(&tm, NULL);

	return tm.tv_sec * 1000000ULL + tm.tv_usec;
}

static void switch_bench(void *data)
{
	volatile unsigned long *sw_counter = (unsigned long *) data;

	for (;;) {
		(*sw_counter)--;
		co_resume();
	}
}

static void *run_test(void *data)
{
	int i, ntimes;
	coroutine_t coro;
	unsigned long nswitches, sw_counter;
	unsigned long long ts, te;

	fprintf(stdout, "[%p] measuring co_create+co_delete performance ...\n",
		pthread_self());
	fflush(stdout);

	ntimes = 10000;
	do {
		ts = getustime();
		for (i = 0; i < ntimes; i++) {
			if ((coro = co_create(switch_bench, &sw_counter, NULL,
					      CO_STACK_SIZE)) != NULL)
				co_delete(coro);
		}
		te = getustime();
		ntimes *= 4;
	} while ((te - ts) < MIN_MEASURE_TIME);

	fprintf(stdout, "[%p] %g usec\n", pthread_self(),
		(double) (te - ts) / (double) ntimes);

	if ((coro = co_create(switch_bench, &sw_counter, NULL,
			      CO_STACK_SIZE)) != NULL) {
		fprintf(stdout, "[%p] measuring switch performance ...\n",
			pthread_self());
		fflush(stdout);

		sw_counter = nswitches = 10000;
		do {
			ts = getustime();
			while (sw_counter)
				co_call(coro);
			te = getustime();
			sw_counter = (nswitches *= 4);
		} while ((te - ts) < MIN_MEASURE_TIME);

		fprintf(stdout, "[%p] %g usec\n", pthread_self(),
			(double) (te - ts) / (double) (2 * nswitches));

		co_delete(coro);
	}

	return NULL;
}

static void *thread_proc(void *data)
{
	void *result;

	co_thread_init();
	result = run_test(data);
	co_thread_cleanup();

	return result;
}

int main(int argc, char **argv)
{
	int i, nthreads;
	pthread_t *thids;

	nthreads = 1;
	for (i = 1; i < argc; i++) {
		if (strcmp(argv[i], "-n") == 0) {
			if (++i < argc)
				nthreads = atoi(argv[i]);
		}
	}
	if (nthreads == 1)
		run_test(NULL);
	else {
		thids = (pthread_t *) malloc(nthreads * sizeof(pthread_t));
		for (i = 0; i < nthreads; i++) {
			if (pthread_create(&thids[i], NULL, thread_proc,
					   NULL)) {
				perror("creating worker threads");
				return 1;
			}
		}
		for (i = 0; i < nthreads; i++)
			pthread_join(thids[i], NULL);
	}

	return 0;
}

