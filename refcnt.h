/******************************************************************************
 * Generic reference counting library in pure C with only standard library
 * dependencies. This is a header-only library, so you can just include
 * this file in your project and use it.
 *
 * Instead of using malloc/free, use refcnt_malloc/refcnt_unref. When you copy
 * a pointer, use refcnt_ref. When you are done with a pointer, use
 * refcnt_unref.
 *
 * There is also a refcnt_strdup function that uses refcnt_malloc, so the
 * resulting string must be freed with refcnt_unref.
 *
 * This library is thread-safe with the exception of refcnt_realloc. If you
 * need to use refcnt_realloc in a multithreaded environment, you must
 * synchronize access to the reference.
 *
 * If you define REFCNT_CHECK, references passed into refcnt_ref and and
 * refcnt_unref will be checked that they were created by refcnt_malloc. This is
 * useful for debugging, but it will slow down your program somewhat.
 *
 * If you define REFCNT_TRACE, refcnt_malloc and refcnt_unref will print
 * the line number and file name where they were called. This is useful for
 * debugging memory leaks or use after free errors. Right now this only works
 * with compilers that support compound statements in macros. (IE GCC and Clang)
 *
 * Copyright (C) 2022 Doug Johnson (dougvj@bonnevilleinformatics.com)
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *    * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *    * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *    * Neither the name of the author nor the names of its contributors may
 *    be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *****************************************************************************/
#ifndef _REFCNT_H_
#include <assert.h>
#include <stdatomic.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>

#define maybe_unused __attribute__((unused))

typedef struct {
#ifdef REFCNT_CHECK
  int magic;
#endif
  atomic_uint refcount;
  char data[];
} _refcnt;

#ifdef REFCNT_TRACE
#define _REFCNT_TRACE(call) ({ \
  fprintf(stderr, "%s:%d:(%s) %s", __FILE__, __LINE__, __FUNCTION__, #call); \
  call; \
})
#define refcnt_malloc _refcnt_malloc
#define refcnt_realloc _refcnt_realloc
#define refcnt_ref _refcnt_ref
#define refcnt_unref _refcnt_unref
#define refcnt_strdup _refcnt_strdup
#endif

static maybe_unused void *refcnt_malloc(size_t len) {
  _refcnt *ref = malloc(sizeof(_refcnt) + len);
  if (ref == NULL) {
    return NULL;
  }
#ifdef REFCNT_CHECK
  ref->magic = 0xdeadbeef;
#endif
  atomic_init(&ref->refcount, 1);
  return ref->data;
}

static maybe_unused void *refcnt_realloc(void *ptr, size_t len) {
  _refcnt *ref = (void*)(ptr - offsetof(_refcnt, data));
#ifdef REFCNT_CHECK
  assert(ref->magic == 0xdeadbeef);
#endif
  ref = realloc(ref, sizeof(_refcnt) + len);
  if (ref == NULL) {
    return NULL;
  }
  return ref->data;
}

static maybe_unused void *refcnt_ref(void *ptr) {
  _refcnt *ref = (void *)(ptr - offsetof(_refcnt, data));
#ifdef REFCNT_CHECK
  assert(ref->magic == 0xdeadbeef && "Invalid refcnt pointer");
#endif
  atomic_fetch_add(&ref->refcount, 1);
  return ref->data;
}

static maybe_unused void refcnt_unref(void *ptr) {
  _refcnt *ref = (void *)(ptr - offsetof(_refcnt, data));
#ifdef REFCNT_CHECK
  assert(ref->magic == 0xdeadbeef && "Invalid refcnt pointer");
#endif
  if (atomic_fetch_sub(&ref->refcount, 1) == 1) {
    free(ref);
  }
}

static maybe_unused char *refcnt_strdup(char *str) {
  _refcnt *ref = malloc(sizeof(_refcnt) + strlen(str) + 1);
  if (ref == NULL) {
    return NULL;
  }
#ifdef REFCNT_CHECK
  ref->magic = 0xdeadbeef;
#endif
  atomic_init(&ref->refcount, 1);
  strcpy(ref->data, str);
  return ref->data;
}

#ifdef REFCNT_TRACE
#undef refcnt_malloc
#undef refcnt_realloc
#undef refcnt_ref
#undef refcnt_unref
#undef refcnt_strdup
#define refcnt_malloc(len) _REFCNT_TRACE(_refcnt_malloc(len))
#define refcnt_realloc(ptr, len) _REFCNT_TRACE(_refcnt_realloc(ptr, len))
#define refcnt_ref(ptr) _REFCNT_TRACE(_refcnt_ref(ptr))
#define refcnt_unref(ptr) _REFCNT_TRACE(_refcnt_unref(ptr))
#define refcnt_strdup(ptr) _REFCNT_TRACE(_refcnt_strdup(ptr))
#endif
#else

#endif // _REFCNT_H_
