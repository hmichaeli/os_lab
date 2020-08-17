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

#define MIN(x,y) ((x)<(y)?(x):(y))
static char str[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
static int strlen = 62;

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
unsigned long debug_flag = 0;

int init_module(void)
{
    // my_major = register_chrdev(my_major, VIGENERE_DEVICE, &my_fops);
    my_major = register_chrdev(0, VIGENERE_DEVICE, &my_fops);

    if (my_major < 0)
    {
        printk(KERN_WARNING "can't get dynamic major\n");
        return my_major;
    }
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
    MOD_INC_USE_COUNT;
    return 0;
}


int my_release(struct inode *inode, struct file *filp)
{
    cleanup_module();
    MOD_DEC_USE_COUNT;
    return 0;
}

ssize_t my_read(struct file* filp, char* buf, size_t count, loff_t* f_pos)
{
    if (NULL == buf || NULL == buffer_ptr) {
        return -EFAULT;
    }
    // Do read operation.
    int cur_pid = current->pid;
    int code = other_pid + cur_pid;
    codeLen = get_code_len(code);

    int readLen = MIN(count, buffer_size - read_idx);
    char* decrypted = kalloc(readLen);
    if (NULL == decrypted) {
        return -EFAULT;
    }

    for (int i = 0; i < readLen; i++) {
        decrypted[i] = decrypt_char(buffer_ptr[read_idx + i], get_digit(code, (read_idx + i) % codeLen);
    }
    
    int written = copy_to_user(buf, &decrypted, readLen); //TODO: ask what todo in case there is a partial copy. overwrite the buffer with zeros?
    kfree(decrypted);

    if (0 != written) {
        return -ENOMEM;
    }
    else {
        read_idx = readLen + written;
        return written;
    }
}


ssize_t my_write(struct file *filp, const char *buf, size_t count, loff_t *f_pos)
{
    (void)f_pos;
    // Do write operation.
    if (count == 0){
        return -EINVAL;
    }
    if (NULL == buf) {
        return -EFAULT;
    }

    int cur_pid = current->pid;
    int code = other_pid + cur_pid;
    codeLen = get_code_len(code);

    int new_size = buffer_size + count;
    char * new_buffer_ptr = kalloc(new_size);
    if (new_buffer_ptr == NULL){
        return -ENOMEM;
    }
    int i = 0;
    // copy old buffer
    for(; i < buffer_size; i++){
        new_buffer_ptr[i] = buffer_ptr[i];
    }

    // copy input
    int bytes_not_copied = copy_from_user(&(new_buffer_ptr[i]), buf, count);
    if (0 != bytes_not_copied) {
        kfree(new_buffer_ptr);
        return -EFAULT
    }
    else
    {
        kfree(buffer_ptr);
        //encrypt and assign
        int j = 0;
        for (; buffer_size+j < new_size; j++) {
            new_buffer_ptr[i+j] = encrypt_char(new_buffer_ptr[i+j],get_digit(code, j % codeLen));
        }
        buffer_ptr = new_buffer_ptr;
        buffer_size = new_size;
        return count;
    }
}

int my_restart() {
    read_idx = 0;
    return 0;
}

int my_reset() {
    read_idx = 0;
    kfree(buffer_ptr);
    buffer_ptr = NULL;
    buffer_size = 0;
    return 0;
}

int my_set_other_pid(unsigned long arg) {
    other_pid = arg;
    return 0;
}

int my_set_debug_pid(unsigned long arg) {
    if (arg != 1 || arg != 0) {
        return -EFAULT;
    }
    else
    {
        debug_flag = arg;
        return 0;
    }
    
}

int my_ioctl(struct inode *inode, struct file *filp, unsigned int cmd, unsigned long arg)
{
    switch(cmd)
    {
    case MY_RESTART:
        return my_restart();
    case MY_RESET:
        return my_reset(arg);
    case MY_SET_OTHER_PID:
        return my_set_other_pid(arg);
    case MY_SET_DEBUG_PID:
        return my_set_debug_pid(arg);
    default:
	return -ENOTTY;
    }
}

/* help functions */

char encrypt_char(char c, int code){
    if (1 == debug_flag) {
        return c;
    }
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

char decrypt_char(char c, int code) {
    if (1 == debug_flag) {
        return c;
    }
    int i = 0
        for (i = 0; i < len; i++) {
            if (str[i] == c) {
                break;
            }
        }
    if (i == len) {
        //c not in array
        return c;
    }
    return str[(i - code) % len];
}

int get_code_len(int code) {
    int cnt = 0;
    while (code> 0) {
        code = int(code/10);
        cnt++;
    }
    return cnt;
}

int get_digit(int num,int id) {
    int len = get_code_len(num);
    int powered = 1;
    for (int i = 0; i < len-id-1; i++) {
        powered *= 10;
    }

    return (num /powered)%10;
}