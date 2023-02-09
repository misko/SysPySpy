#define _GNU_SOURCE
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dlfcn.h>
#include <signal.h>
#include <string.h>
#include "syspyspy.h"

static pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

//the system calls we are going to intercept
static void* (*real_malloc)(size_t)=NULL;
static void* (*real_calloc)(size_t,size_t)=NULL;
static void* (*real_free)(size_t)=NULL;
static void* (*real_realloc)(void*, size_t)=NULL;
static void* (*real_reallocarray)(void*,size_t,size_t)=NULL;
static pid_t (*real_fork)(void)=NULL;

//vars to read in from env
static size_t size_alloc_calls=DEFAULT_ALLOC_SIZE;

enum load_status { LIBRARY_FAIL=-1,
                  NOT_LOADED=0,
                  LOADING_MEM_FUNCS=1,
                  LOADING_LIBRARY=2,
                  LOADED=3
                  };

static enum load_status load_state=NOT_LOADED;

static void bigmaac_init(void)
{
    pthread_mutex_lock(&lock);
    if (load_state==LIBRARY_FAIL) {
        return; //error initializing
    }
    if (load_state!=NOT_LOADED) {
        pthread_mutex_unlock(&lock);
        fprintf(stderr,"Already init %d\n",load_state);
        return;
    }
    fprintf(stderr,"Loading SysPySpy! PID:%d PPID:%d\n",getpid(),getppid());
    load_state=LOADING_MEM_FUNCS;
    real_malloc = dlsym(RTLD_NEXT, "malloc");
    real_free = dlsym(RTLD_NEXT, "free");
    real_calloc = dlsym(RTLD_NEXT, "calloc");
    real_realloc = dlsym(RTLD_NEXT, "realloc");
    real_reallocarray = dlsym(RTLD_NEXT, "reallocarray");
    real_fork = dlsym(RTLD_NEXT, "fork");
    if (!real_malloc || !real_free || !real_calloc || !real_realloc || !real_reallocarray) {
        fprintf(stderr, "Error in `dlsym`: %s\n", dlerror());
    }
    load_state=LOADING_LIBRARY;

    const char * env_size_alloc_calls=getenv("SIZE_ALLOC_CALLS");
    if (env_size_alloc_calls!=NULL) {
        sscanf(env_size_alloc_calls, "%zu", &size_alloc_calls);
    }


    load_state=LOADED;
    pthread_mutex_unlock(&lock);
}


void size_to_units(size_t size, char * unit, float * value_ptr) {
	float value = size;
	char * units = "BKMGTX";
	for (int i=0; i<strlen(units); i++) {
		if (value<1024.0) {
			*unit=units[i];
			*value_ptr=value;
			return;
		}
		value/=1024.0;
	}
	*unit=units[strlen(units)-1];
	*value_ptr=value;
}

void report_alloc_size(char * func_name, size_t size) {
	char unit; float value; 
	size_to_units(size, &unit, &value);
	fprintf(stderr,"SysPySpy: %s(%lu) %0.2f%c\n",func_name,size,value,unit);
#ifdef SYSPYSPY_SIGNAL
	kill(getpid(), SIGUSR1);
#endif
}

// BigMaac C library memory functions

void *malloc(size_t size)
{
    if(load_state==NOT_LOADED && real_malloc==NULL) {
        bigmaac_init();
    }
    if (size>size_alloc_calls) {
	report_alloc_size("malloc",size);
    }
    return real_malloc(size);
}

void *calloc(size_t count, size_t size)
{
    if (load_state>NOT_LOADED && load_state<LOADED) {
        return NULL;
    }

    if(load_state==NOT_LOADED || real_malloc==NULL) {
        bigmaac_init();
    }

    if (size>size_alloc_calls) {
	report_alloc_size("calloc",size);
    }

    return  real_calloc(count,size);
}

void *reallocarray(void * ptr, size_t size,size_t count) {
    if(load_state==NOT_LOADED && real_malloc==NULL) {
        bigmaac_init();
    }

    if (size*count>size_alloc_calls) {
	report_alloc_size("reallocarray",size);
    }

    return  real_reallocarray(ptr,size,count);
}

void *realloc(void * ptr, size_t size)
{
    if(load_state==NOT_LOADED && real_malloc==NULL) {
        bigmaac_init();
    }

    if (size>size_alloc_calls) {
	report_alloc_size("realloc",size);
    }

    return  real_realloc(ptr,size);
}

void free(void* ptr) {
    if(load_state==NOT_LOADED && real_malloc==NULL) {
        bigmaac_init();
    }

    //TODO implement tracking of large memory allocs and report when they are free'd

    real_free((size_t)ptr);
}

