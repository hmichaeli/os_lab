/* vigenere_module.c: Example char device module.
 *
 */
/* Kernel Programming */
#define MODULE
#define LINUX
#define __KERNEL__

#include <linux/kernel.h>  	
#include <linux/module.h>
#include <linux/fs.h>       		
#include <asm/uaccess.h>
#include <linux/errno.h>  

#include "vigenere_module.h"

#define VIGENERE_DEVICE "vigenere_device"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Anonymous");

/* globals */
int my_major = 0; /* will hold the major # of my device driver */

struct file_operations my_fops = {
    .open = my_open,
    .release = my_release,
    .read = my_read,
    .write = my_write,
    .ioctl = my_ioctl
};

/* our globals */
int other_pid = 0;
char * buffer_ptr = NULL;
int read_idx = 0;
int buffer_size = 0;

int init_module(void)
{
    // my_major = register_chrdev(my_major, VIGENERE_DEVICE, &my_fops);
    my_major = register_chrdev(0, VIGENERE_DEVICE, &my_fops);

    if (my_major < 0)
    {
        printk(KERN_WARNING "can't get dynamic major\n");
        return my_major;
    }

    //
    // do_init();
    //
    return 0;
}


void cleanup_module(void)
{
    unregister_chrdev(my_major, VIGENERE_DEVICE);

    if (buffer_ptr != NULL){
        kfree(buffer_ptr);
    }
    buffer_ptr = NULL;
    other_pid = 0;
    read_idx = 0;
    buffer_size = 0;
    //
    // do clean_up();
    //
    return;
}


int my_open(struct inode *inode, struct file *filp)
{
    // TODO: check if need to do something

    return 0;
}


int my_release(struct inode *inode, struct file *filp)
{
    return 0;
}


ssize_t my_read(struct file *filp, char *buf, size_t count, loff_t *f_pos)
{
    //
    // Do read operation.
    return 0; 
}


ssize_t my_write(struct file *filp, const char *buf, size_t count, loff_t *f_pos)
{
    //
    // Do write operation.
    if (count == 0){
        return -EFAULT;
    }

    int cur_pid = current->pid;
    int code = other_pid + cur_pid;
    int new_size = buffer_size + count;
    char * new_buffer_ptr = kalloc(new_size);
    if (new_buffer_ptr == NULL){
        return -ENOMEM;
    }
    // copy old buffer
    for(int i=0; i < buffer_size; i++){
        new_buffer_ptr[i] = buffer_ptr[i];
    }
    kfree(buffer_ptr);

    // copy input
    
    

    printk("check ~~~");
    return 0; 
}


int my_ioctl(struct inode *inode, struct file *filp, unsigned int cmd, unsigned long arg)
{
    switch(cmd)
    {
    case MY_OP1:
	//
	// handle OP 1.
	//
	break;

    default:
	return -ENOTTY;
    }

    return 0;
}

/* help functions */

char encrypt_char(char c, int code){
    char str[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
    int len = 62;
    int i = 0
    for (i=0; i<len; i++){
        if(str[i] == c){
            break;
        }
    }
    if(i == len){
        //c not in array
        return c; 
    }
    return str[(i + code) % len];

}