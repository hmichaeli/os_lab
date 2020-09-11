#include <linux/sched.h>  
#include <linux/slab.h>
#include <asm/uaccess.h>

# include <linux/timer.h>

#define MIN(x,y) ((x)<(y)?(x):(y))

//

static struct timer_list timers;

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

void wake_up_policy(){
    pid_t pid = current->pid;
    printk("[wake_up] pid: %d\n", pid);
    printk("[wake_up] set task = RUNNING: \n");
    current->state = TASK_RUNNING;
    printk("[wake_up] schedule: \n");
    schedule();
}

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

    printk("[sys_set_policy] set policy: %d, value: %d in process: %d  task_struct \n", policy_id, policy_value, pid);
    p->policy_id = policy_id;
    p->policy_value = policy_value;

    signed long jif_time = sec_to_jiffy(policy_value);
    printk("[sys_set_policy] policy_value: [seconds]: %d, [jiffy]: %d\n", policy_value, jif_time);
    // Handle set_policy curent process
    if (pid == current->pid){
        printk("[sys_set_policy] pid == current->pid\n");
        if (policy_id == 1){
            set_current_state(TASK_INTERRUPTIBLE);
            printk("[sys_set_policy] schedule_timeout\n");
            schedule_timeout(jif_time);
            printk("[sys_set_policy] retrun 0 \n");
            return 0;
        }
        if (policy_id == 2){
        
        }
    }
    // Handle set_policy to other process
    else{
        printk("[sys_set_policy] pid != current->pid\n");
        if (policy_id == 1){
            printk("[sys_set_policy] set p state to TASK_INTERRUPTIBLE\n");
            p->state = TASK_INTERRUPTIBLE;
            printk("[sys_set_policy] set wake_up timer\n");
            
            p->real_timer.expires = jiffies + jif_time;
            p->real_timer.data = 0;
            p->real_timer .function = wake_up_policy;
            printk("[sys_set_policy] set wake_up timer\n");
            add_timer(&(p->real_timer));
            printk("[sys_set_policy] retrun 0 \n");
            return 0;
        }
    }

}


