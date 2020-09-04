#include <linux/sched.h>  
#include <linux/slab.h>
#include <asm/uaccess.h>

#define MIN(x,y) ((x)<(y)?(x):(y))

// helper function - checks if the input pid is the current process or its ancestors.
int check_ancestor(pid_t pid){
    pid_t cur_pid = current->pid;
    printk("[check_ancestor] pid:     %d\n", pid);
    printk("[check_ancestor] cur_pid: %d\n", cur_pid);
    if (pid == cur_pid){
        printk("[check_ancestor][OK] self pid return 0\n");
        return 0;
    }
    struct task_struct * pts = find_task_by_pid(pid);

    while(pts != NULL && pts->pid != 1){
        if (pts->pid == cur_pid){
            printk("[check_ancestor][OK] ancestor pid return 0\n");
            return 0;
        }
        pts = pts->p_opptr;
    }
    if(pts == NULL){
        printk("[check_ancestor] pts is NULL - return -1\n");
    }
    if(pts->pid == 1){
        printk("[check_ancestor] pts->pid is 1 - return -1\n");
    }
    return -1;
}


int sys_push_TODO(pid_t pid, char* message, int message_size)
{
    //get the task struct and validate the action
	printk("get task struce from pid\n");
    struct task_struct * p = find_task_by_pid(pid);
    printk("[sys_push_TODO] pid: %d\n", pid);
    // printk("[sys_push_TODO] message: %s\n", *message);
    printk("[sys_push_TODO] message_size: %d\n", message_size);

    if (p == NULL){
        printk("[sys_push_TODO] faild find_task_by_pid\n");
        return -ESRCH;
    }

    if(check_ancestor(pid) != 0){
        printk("[sys_push_TODO] illegal pid\n");
        return -ESRCH;     
    }
	//create the new TODO node
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
    // add the TODO node into the todo_stack of the requested proccess.
	list_add(&(new_node->list_node), &(p->todo_stack));		 
    int res = 0;
    printk("[sys_push_TODO] return res = 0\n");
    return res;
 
}


int sys_peek_TODO(pid_t pid, char* message, int message_size)
{
	//get the task struct and validate the action
    printk("get task struce from pid\n");
    struct task_struct * p = find_task_by_pid(pid);
    printk("[sys_peek_TODO] pid: %d\n", pid);
    // printk("[sys_push_TODO] message: %s\n", *message);
    printk("[sys_peek_TODO] message_size: %d\n", message_size);

    if (p == NULL){
        printk("[sys_peek_TODO] faild find_task_by_pid\n");
        return -ESRCH;
    }

    if(check_ancestor(pid) != 0){
        printk("[sys_peek_TODO] illegal pid\n");
        return -ESRCH;     
    }

	//empty stack -> exit
	if(list_empty(&(p->todo_stack))){
		printk("[sys_peek_TODO] pid todo stack is empty\n");
		return -EINVAL;
	}
	else{ //there are TODO's
		printk("[sys_peek_TODO] get p todo stack first element\n");
		todo_node * cur_node = list_entry(p->todo_stack.next, todo_node, list_node);
		
        if (cur_node->description_size > message_size){
    		printk("[sys_peek_TODO] message size is too long\n");
    		return -EINVAL;

        }
        printk("[sys_peek_TODO] copy to user\n");
        if (cur_node->description == NULL){
            printk("[sys_peek_TODO] description is NULL\n");
    		return -EFAULT;
        }
        if (message == NULL){
            printk("[sys_peek_TODO] message is NULL\n");
    		return -EINVAL;
        }
        int res = copy_to_user(message, cur_node->description, cur_node->description_size);
        if (res != 0){
            printk("[sys_peek_TODO] copy_to_user failed\n");
            return -EFAULT;
        }
        res = cur_node->description_size;
        printk("[sys_peek_TODO] return res = %d\n", res);
        return res;
	}
}


int sys_pop_TODO(pid_t pid)
{
    //get the task struct and validate the action
    printk("get task struce from pid\n");
    struct task_struct * p = find_task_by_pid(pid);
    printk("[sys_pop_TODO] pid: %d\n", pid);
    // printk("[sys_push_TODO] message: %s\n", *message);

    if (p == NULL){
        printk("[sys_pop_TODO] faild find_task_by_pid\n");
        return -ESRCH;
    }
    
    if(check_ancestor(pid) != 0){
        printk("[sys_pop_TODO] illegal pid\n");
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
    //remove the TODO from the list.
    printk("[sys_pop_TODO] remove TODO from top of the list\n");
    todo_node * tmp = list_entry(p->todo_stack.next, todo_node, list_node);
    list_del(p->todo_stack.next);
    //destroy the TODO node (free memory etc.)
    printk("[sys_pop_TODO] free memory\n");
    kfree(tmp->description);
    kfree(tmp);

    int res = 0;
    printk("[sys_push_TODO] return res = 0\n");
    return res;
 
}
