#include <linux/sched.h>  
#include <linux/slab.h>
#include <asm/uaccess.h>

int sys_push_TODO(void){

    // __asm__
    // (
    // "movl %%ebx, %0;"
    // "movl %%ecx, %1;"
    // "movl %%edx, %1;"
    // :"m" (pid) ,"m" (message) ,"m"(message_size)
    // );
    int pid;
    const char * message;
    ssize_t message_size;
    printk("insert regiseters to vars:\n");
    __asm__("movl %%ebx,%0" : "=r"(pid));
    __asm__("movl %%ecx,%0" : "=r"(message));
    __asm__("movl %%edx,%0" : "=r"(message_size));
    printk("insert regiseters to vars - done\n");
    printk("get task struce from pid\n");
    struct task_struct * p = find_task_by_pid(pid);
    printk("create new todo node\n");
    todo_node * new_node = (todo_node *)kmalloc(sizeof(todo_node), GFP_KERNEL);
    if(new_node == NULL){
        printk("[fork] faild malloc new_node\n");
        return -ENOMEM;
    }
    new_node->description_size = message_size;
    new_node->description = (char *)kmalloc(sizeof(char)*message_size, GFP_KERNEL);
    if(new_node->description == NULL){
        printk("[fork] faild malloc new_node->description\n");
        return -ENOMEM;
    }
    copy_from_user(new_node->description, message, message_size);
    printk("[sys_push_TODO] INIT_LIST_HEAD\n");
    INIT_LIST_HEAD(&(new_node->list_node));
    printk("[sys_push_TODO] list_add\n");
    list_add(&(new_node->list_node), &(p->todo_stack));		 

    // TODO: check which process do to which (only sons)
    // TODO: 
    int res = 0;
    printk("[sys_push_TODO] return res = 0\n");
    return res;
    // __asm__("movl %0, %%eax;":"=m"(res));

}