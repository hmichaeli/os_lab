#include <linux/sched.h>  
#include <linux/slab.h>
#include <asm/uaccess.h>

#define MIN(x,y) ((x)<(y)?(x):(y))

int sys_push_TODO(pid_t pid, char* message, int message_size)
{
    printk("get task struce from pid\n");
    struct task_struct * p = find_task_by_pid(pid);
    printk("[sys_push_TODO] pid: %d\n", pid);
    // printk("[sys_push_TODO] message: %s\n", *message);
    printk("[sys_push_TODO] message_size: %d\n", message_size);

    if (p == NULL){
        printk("[sys_push_TODO] faild find_task_by_pid\n");
        return -ENOMEM;
    }
    printk("create new todo node\n");
    todo_node * new_node = (todo_node *)kmalloc(sizeof(todo_node), GFP_KERNEL);
    if(new_node == NULL){
        printk("[sys_push_TODO] faild malloc new_node\n");
        return -ENOMEM;
    }
    new_node->description_size = message_size;
    new_node->description = (char *)kmalloc(sizeof(char)*message_size, GFP_KERNEL);
    if(new_node->description == NULL){
        printk("[sys_push_TODO] faild malloc new_node->description\n");
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
 
}


int sys_peek_TODO(pid_t pid, char* message, int message_size)
{
    printk("get task struce from pid\n");
    struct task_struct * p = find_task_by_pid(pid);
    printk("[sys_peek_TODO] pid: %d\n", pid);
    // printk("[sys_push_TODO] message: %s\n", *message);
    printk("[sys_peek_TODO] message_size: %d\n", message_size);

    if (p == NULL){
        printk("[sys_peek_TODO] faild find_task_by_pid\n");
        return -ESRCH;
    }


	if(list_empty(&(p->todo_stack))){
		printk("[sys_peek_TODO] pid todo stack is empty\n");
		return -EINVAL;
	}
	else{
		printk("[sys_peek_TODO] get p todo stack first element\n");
		todo_node * cur_node = list_entry(p->todo_stack.next, todo_node, list_node);
		
        if (cur_node->description_size > message_size){
    		printk("[sys_peek_TODO] message size is too long\n");
    		return -EINVAL;

        }
        printk("[sys_peek_TODO] copy to user:");
        if (cur_node->description == NULL){
            printk("[sys_peek_TODO] description is NULL\n");
    		return -EINVAL;
        }
        if (message == NULL){
            printk("[sys_peek_TODO] message is NULL\n");
    		return -EINVAL;
        }
        int res = copy_to_user(message, cur_node->description, cur_node->description_size);
        if (res != 0){
            printk("[sys_peek_TODO] copy_to_user failed");
            return -EFAULT;
        }
        return cur_node->description_size;
	}
}


int sys_pop_TODO(pid_t pid)
{
    printk("get task struce from pid\n");
    struct task_struct * p = find_task_by_pid(pid);
    printk("[sys_pop_TODO] pid: %d\n", pid);
    // printk("[sys_push_TODO] message: %s\n", *message);

    if (p == NULL){
        printk("[sys_pop_TODO] faild find_task_by_pid\n");
        return -ESRCH;
    }
    
	if(list_empty(&(p->todo_stack))){
		printk("[sys_pop_TODO] pid todo stack is empty\n");
		return -EINVAL;
	}

    if (p->todo_stack.next == NULL){
        printk("[sys_pop_TODO] todo_stack.next is NULL\n");
		return -EINVAL;
    }
    
    printk("[sys_pop_TODO] remove TODO from top of the list\n");
    todo_node * tmp = list_entry(p->todo_stack.next, todo_node, list_node);
    list_del(p->todo_stack.next);
    
    printk("[sys_pop_TODO] free memory\n");
    kfree(tmp->description);
    kfree(tmp);

    int res = 0;
    printk("[sys_push_TODO] return res = 0\n");
    return res;
 
}
