# refcnt.h: Header-only reference counting library in C


Instead of using malloc/free, use refcnt_malloc/refcnt_unref. When you copy
a pointer, use refcnt_ref. When you are done with a pointer, use
refcnt_unref.

There is also a refcnt_strdup function that uses refcnt_malloc, so the
resulting string must be freed with refcnt_unref.

This library is thread-safe with the exception of refcnt_realloc. If you
need to use refcnt_realloc in a multithreaded environment, you must
synchronize access to the reference.

If you define REFCNT_CHECK, references passed into refcnt_ref and and
refcnt_unref will be checked that they were created by refcnt_malloc. This is
useful for debugging, but it will slow down your program somewhat.

If you define REFCNT_TRACE, refcnt_malloc and refcnt_unref will print
the line number and file name where they were called. This is useful for
debugging memory leaks or use after free errors. Right now this only works
with compilers that support compound statements in macros. (IE GCC and Clang)

