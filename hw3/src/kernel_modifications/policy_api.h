#ifndef TODO_API_H
#define TODO_API_H
#include <linux/ioctl.h>
#include <stdio.h>



// a. Description:
// Sets a policy for the process identified by pid. A policy is defined by policy_id and policy_value.
// b. Return value:
// i. On failure: -1
// ii. On success: 0
// c. On failure errno should contain one of following values:
// i. “ESRCH” (No such process): No such process exists, or the current process is not allowed to set a policy for the process identified by pid.
// ii. “EINVAL” (Invalid argument) policy_id is not 0,1, or 2, or policy_value < 0.
int set_policy(pid_t pid, int policy_id, int policy_value){
    int res;
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
    : "m" (pid) ,"m" (policy_id) ,"m"(policy_value)
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



// a. Description:
// Gets the policy for the process identified by pid. Returns the result in policy_id and policy_value.
// b. Return value:
// i. On failure: -1
// ii. On success: 0
// c. On failure errno should contain one of following values:
// i. “ESRCH” (No such process): No such process exists, or the current process is not allowed to set a policy for the process identified by pid.
// ii. “EINVAL” (Invalid argument) policy_id is NULL, or policy_value is NULL.
// iii. “EFAULT” (Bad address): Error copying to user space.
int get_policy(pid_t pid, int* policy_id, int* policy_value){
    int res;
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
    : "m" (pid) ,"m" (policy_id) ,"m"(policy_value)
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


#endif
