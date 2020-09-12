#include <linux/sched.h>  
#include <linux/slab.h>
#include <asm/uaccess.h>

# include <linux/timer.h>

#include <linux/signal.h>

#include <linux/sys_policy.h>

#define MIN(x,y) ((x)<(y)?(x):(y))

//

static struct timer_list timers;

// help functions titles
// int set_sleep_policy(struct task_struct * p, int time);
// int set_kill_policy(struct task_struct * p, int time);

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

unsigned long sec_to_jiffy(int seconds){
    return (unsigned long)(seconds * HZ);
}

void wake_up_policy(unsigned long  data){
    pid_t cur_pid = current->pid;
    printk("[wake_up] cur_pid: %d\n", cur_pid);
    pid_t wake_pid = (pid_t)data;
    printk("[wake_up] pid to wake: %d\n", wake_pid);
   // TODO: check pid
    struct task_struct * p = find_task_by_pid(wake_pid);
    printk("[wake_up] get task_struct. policy_id: %d policy_value: %d\n", p->policy_id, p->policy_value);

    printk("[wake_up] wake_up_process\n");
    wake_up_process(p);
    printk("[wake_up] post wake_up_process. policy_id: %d policy_value: %d\n", p->policy_id, p->policy_value);

    if(p->pending_policy_id != -1){
        printk("[wake_up] there is a pending policy\n");
        p->policy_id = p->pending_policy_id;
        p->policy_value = p->pending_policy_value;
        p->pending_policy_id = -1;

        if(p->policy_id == 1){
            printk("[wake_up] set new sleep policy\n");
            set_sleep_policy(p, p->policy_value);
        }
        else if(p->policy_id == 2){
            printk("[wake_up] set new kill policy\n");
            set_kill_policy(p, p->policy_value);
        }
        //TODO: do new policy
    }
    else{
        printk("[wake_up] change active_policy to 0\n");
        p->active_policy = 0;
    }
    printk("[wake_up] done. policy_id: %d policy_value: %d\n", p->policy_id, p->policy_value);

}

void kill_policy(unsigned long  data){
    pid_t cur_pid = current->pid;
    printk("[kill_policy] cur_pid: %d\n", cur_pid);
    pid_t kill_pid = (pid_t)data;
    printk("[kill_policy] pid to kill: %d\n", kill_pid);
   // TODO: check pid
    // struct task_struct * p = find_task_by_pid(kill_pid);
    
    // TODO: check if policy was changed
    
    siginfo_t info;
    kill_proc_info(SIGKILL, &info,  kill_pid);
    // schedule();
}

// functions to activeate pending policy after wakeup
int set_sleep_policy(struct task_struct * p, int time){
    signed long jif_time = sec_to_jiffy(time);
    printk("[set_sleep_policy] set p state to TASK_INTERRUPTIBLE\n");
    p->state = TASK_INTERRUPTIBLE;
    printk("[set_sleep_policy] set_tsk_need_resched \n");
    set_tsk_need_resched(p);
    printk("[set_sleep_policy] set wake_up timer\n"); 
    //TODO:    
    p->real_timer.expires = jiffies + jif_time;
    p->real_timer.data = p->pid;
    p->real_timer.function = wake_up_policy;
    printk("[set_sleep_policy] add wake_up timer to TS\n");
    add_timer(&(p->real_timer));

    printk("[set_sleep_policy] change active_policy to 1 \n");
    p->active_policy = 1;

    printk("[set_sleep_policy] retrun 0 \n");
    return 0;
}
int set_kill_policy(struct task_struct * p, int time){
    signed long jif_time = sec_to_jiffy(time);
    printk("[set_kill_policy] set kill timer\n");    
    p->real_timer.expires = jiffies + jif_time;
    p->real_timer.data = p->pid;
    p->real_timer.function = kill_policy;
    printk("[set_kill_policy] add kill timer to TS\n");
    add_timer(&(p->real_timer));
    
    printk("[set_kill_policy] change active_policy to 1 \n");
    p->active_policy = 1;
    
    printk("[set_kill_policy] retrun 0 \n");
    return 0;
}

//
//
//

// sys_call handlers
int sys_get_policy(pid_t pid, int* policy_id, int* policy_value){
    
    if (policy_id == NULL || policy_value == NULL){
        printk("[sys_get_policy] recived invalid pointer\n");
        return -EINVAL;
    }

    printk("[sys_get_policy] get task struce from pid\n");
    struct task_struct * p = find_task_by_pid(pid);
    printk("[sys_get_policy] pid: %d\n", pid);

    if (p == NULL){
        printk("[sys_get_policy] faild find_task_by_pid\n");
        return -ESRCH;
    }
    if(check_ancestor(pid) != 0){
        printk("[sys_get_policy] illegal pid\n");
        return -ESRCH;     
    }

    int res = copy_to_user(policy_id, &(p->policy_id), sizeof(int));
    if (res != 0){
        printk("[sys_get_policy] copy_to_user failed\n");
        return -EFAULT;
    }
    
    res = copy_to_user(policy_value, &(p->policy_value), sizeof(int));
    if (res != 0){
        printk("[sys_get_policy] copy_to_user failed\n");
        return -EFAULT;
    }
    return 0;
}

int sys_set_policy(pid_t pid, int policy_id, int policy_value){
    
    printk("[sys_set_policy] get task struce from pid\n");
    struct task_struct * p = find_task_by_pid(pid);
    printk("[sys_set_policy] pid: %d\n", pid);

    if (p == NULL){
        printk("[sys_set_policy] faild find_task_by_pid\n");
        return -ESRCH;
    }
    if(check_ancestor(pid) != 0){
        printk("[sys_set_policy] illegal pid\n");
        return -ESRCH;     
    }

    if (policy_id < 0 || policy_id > 2 || policy_value < 0){
        printk("[sys_set_policy] illegal policy\n");
        return -EINVAL;     
    }

    int old_policy_id = p->policy_id;
    int old_policy_value = p->policy_value;
    
    if(p->active_policy){
        printk("[sys_set_policy] there is active policy\n");
        if(old_policy_id == 1){
            printk("[sys_set_policy] there is active sleep - update penfing policy\n");
            p->pending_policy_id = policy_id;
            p->pending_policy_value = policy_value; 
            return 0;
        }

        else if (old_policy_id == 2){
            printk("[sys_set_policy] there is active kill - abort - delete timer\n");
            del_timer(&p->real_timer);
        }
    }


    p->policy_id = policy_id;
    p->policy_value = policy_value;
    printk("[sys_set_policy] old policy: %d, value: %d in process: %d  task_struct \n", old_policy_id, old_policy_value, pid);
    printk("[sys_set_policy] set policy: %d, value: %d in process: %d  task_struct \n", policy_id, policy_value, pid);



    signed long jif_time = sec_to_jiffy(policy_value);
    printk("[sys_set_policy] policy_value: [seconds]: %d, [jiffy]: %d\n", policy_value, jif_time);
    
            
    if(policy_id == 0){
        printk("[sys_set_policy] set_policy 0 - do nothing");
    }
    else if (policy_id == 1){
        printk("[sys_set_policy] call set_sleep_policy\n");
        set_sleep_policy(p, policy_value);
    }
    else if (policy_id == 2){
        printk("[sys_set_policy] call set_kill_policy\n");
        set_kill_policy(p, policy_value);
    }
    else{
        printk("[sys_set_policy] illegal policy\n");
        return -EINVAL;     
    }
    return 0;
    
    // // Handle set_policy curent process
    // if (pid == current->pid){
    //     printk("[sys_set_policy] pid == current->pid\n");
        
    //     if(policy_id == 0){
    //         printk("[sys_set_policy] set_policy 0 - do nothing");
    //     }
    //     if (policy_id == 1){
    //         printk("[sys_set_policy] call set_sleep_policy\n");
    //         set_sleep_policy(p, policy_value);

    //         // printk("[sys_set_policy] set p state to TASK_INTERRUPTIBLE\n");
    //         // p->state = TASK_INTERRUPTIBLE;
    //         // printk("[sys_set_policy] set_tsk_need_resched \n");
    //         // set_tsk_need_resched(p);
    //         // printk("[sys_set_policy] set wake_up timer\n");    
    //         // p->real_timer.expires = jiffies + jif_time;
    //         // p->real_timer.data = pid;
    //         // p->real_timer.function = wake_up_policy;
    //         // printk("[sys_set_policy] add wake_up timer to TS\n");
    //         // add_timer(&(p->real_timer));
    //         // printk("[sys_set_policy] retrun 0 \n");
    //         // return 0;
    //     }
    //     if (policy_id == 2){
    //         printk("[sys_set_policy] call set_kill_policy\n");
    //         set_kill_policy(p, policy_value);

    //         // printk("[sys_set_policy] set kill timer\n");    
    //         // p->real_timer.expires = jiffies + jif_time;
    //         // p->real_timer.data = pid;
    //         // p->real_timer.function = kill_policy;
    //         // printk("[sys_set_policy] add kill timer to TS\n");
    //         // add_timer(&(p->real_timer));
    //         // printk("[sys_set_policy] retrun 0 \n");
    //         // return 0;
    //     }
    // }
    // // Handle set_policy to other process
    // else{
    //     printk("[sys_set_policy] pid != current->pid\n");
        
  
    //     if(policy_id == 0){
    //         printk("[sys_set_policy] set_policy 0 - do nothing");
    //     }
        
    //     if (policy_id == 1){
    //         printk("[sys_set_policy] set p state to TASK_INTERRUPTIBLE\n");
    //         p->state = TASK_INTERRUPTIBLE;
            
    //         printk("[sys_set_policy] set_tsk_need_resched \n");
    //         set_tsk_need_resched(p);
        
    //         printk("[sys_set_policy] set wake_up timer\n");    
    //         p->real_timer.expires = jiffies + jif_time;
    //         p->real_timer.data = pid;
    //         p->real_timer.function = wake_up_policy;
    //         printk("[sys_set_policy] add wake_up timer to TS\n");
    //         add_timer(&(p->real_timer));
    //         printk("[sys_set_policy] retrun 0 \n");
    //         return 0;
    //     }
        
    //     if(policy_id == 2){
    //         printk("[sys_set_policy] set kill timer\n");    
    //         p->real_timer.expires = jiffies + jif_time;
    //         p->real_timer.data = pid;
    //         p->real_timer.function = kill_policy;
    //         printk("[sys_set_policy] add kill timer to TS\n");
    //         add_timer(&(p->real_timer));
    //         printk("[sys_set_policy] retrun 0 \n");
    //         return 0;
    //     }
        
    // }

}


