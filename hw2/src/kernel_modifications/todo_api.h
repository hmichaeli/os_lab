#ifndef TODO_API_H
#define TODO_API_H
#include <linux/ioctl.h>
#include <stdio.h>

int push_TODO(pid_t pid, const char *TODO_description, ssize_t description_size){
    // printf("push_TODO API\n");
    // printf("pid %d\n", pid);
    // printf("description_size %d\n", description_size);
    int res;
    // __asm__("movl %%eax,%0;": "=m"(res));
    // printf("%d\n",res);
    // printf("AA\n");

    __asm__(
    "pushl %%eax;"
    "pushl %%ebx;"
    "pushl %%ecx;"
    "pushl %%edx;"
    "movl $243, %%eax;"
    "movl %1, %%ebx;"
    "movl %2, %%ecx;"
    "movl %3, %%edx;"
    "int $0x80;"
    "movl %%eax,%0;"
    "popl %%edx;"
    "popl %%ecx;"
    "popl %%ebx;"
    "popl %%eax;"
    : "=m" (res)
    : "m" (pid) ,"m" (TODO_description) ,"m"(description_size)
    );
    // printf("[PUSH_TODO_API] assembly executed\n");
    if (res >= (unsigned long)(-125))
    {
        errno = -res;
        res = -1;
    }
    // printf("[PUSH_TODO_API] return %d\n", res);
    return  res;
}
/*
a. Description:
Add a TODO to the TODO’s stack of a process identified by pid. TODO_description is a string of
chars of size description_size.
b. Return value:
i. on failure: -1
ii. on success: 0
c. On failure errno should contain one of following values:
i. “ENOMEM” (Out of memory): Failure allocating memory.
ii. “ESRCH” (No such process): No such process exists, or the current process is not allowed to manage the TODO’s stack of the process identified by pid.
iii. “EFAULT” (Bad address): Error copying from user space.
iv. “EINVAL” (Invalid argument) TODO_description is NULL or description_size < 1.
*/
ssize_t peek_TODO(pid_t pid, char *TODO_description, ssize_t description_size){
    // printf("peekh_TODO API\n");
    // printf("pid %d\n", pid);
    // printf("description_size %d\n", description_size);
    int res;
    // __asm__("movl %%eax,%0;": "=m"(res));
    // printf("%d\n",res);
    // printf("AA\n");

    __asm__(
    "pushl %%eax;"
    "pushl %%ebx;"
    "pushl %%ecx;"
    "pushl %%edx;"
    "movl $244, %%eax;"
    "movl %1, %%ebx;"
    "movl %2, %%ecx;"
    "movl %3, %%edx;"
    "int $0x80;"
    "movl %%eax,%0;"
    "popl %%edx;"
    "popl %%ecx;"
    "popl %%ebx;"
    "popl %%eax;"
    : "=m" (res)
    : "m" (pid) ,"m" (TODO_description) ,"m"(description_size)
    );
    // printf("[PEEK_TODO_API] assembly executed\n");
    if (res >= (unsigned long)(-125))
    {
        errno = -res;
        res = -1;
    }
    // printf("[PEEK_TODO_API] return %d\n", res);
    return  res;
}

/*
a. Description:
Get the description of TODO at the top of the TODO’s stack of process pid. The description is
returned in TODO_description which is a string of chars of size description_size.
Note: It is possible to call this function with description_size larger than the size of the actual
TODO’s description. It is the responsibility of this function to copy only valid chars (it shouldn’t
copy chars beyond the TODO’s description).
b. Return value:
i. on failure: -1
ii. on success: size of the string copied into TODO_description.
c. On failure errno should contain one of following values:
i. “ESRCH” (No such process): No such process exists, or the current process is not allowed to manage the TODO’s stack of the process identified by pid.
ii. “EFAULT” (Bad address): Error copying to user space.
iii. “EINVAL” (Invalid argument) The TODO’s stack is empty, TODO_description is NULL, or
description _size < the size of the description of the top-most TODO.
*/
int pop_TODO(pid_t pid){
    // printf("pop_TODO API\n");
    // printf("pid %d\n", pid);
    int res;
    // __asm__("movl %%eax,%0;": "=m"(res));
    // printf("%d\n",res);
    // printf("AA\n");

    __asm__(
    "pushl %%eax;"
    "pushl %%ebx;"
    "movl $245, %%eax;"
    "movl %1, %%ebx;"
    "int $0x80;"
    "movl %%eax,%0;"
    "popl %%ebx;"
    "popl %%eax;"
    : "=m" (res)
    : "m" (pid) 
    );
    // printf("[POP_TODO_API] assembly executed\n");
    if (res >= (unsigned long)(-125))
    {
        errno = -res;
        res = -1;
    }
    // printf("[POP_TODO_API] return %d\n", res);
    return  res;
}
/*
a. Description:
Delete a TODO from the top of the TODO’s stack of process pid.
b. Return value:
i. on failure: -1
ii. on success: 0
c. On failure errno should contain one of following values:
i. “ESRCH” (No such process): No such process exists, or the current process is not allowed to manage the TODO’s stack of the process identified by pid.
ii. “EINVAL” (Invalid argument) The TODO’s stack is empty.
*/
#endif
