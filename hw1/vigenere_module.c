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


//hagay
#include <linux/slab.h>
///

#define VIGENERE_DEVICE "vigenere_device"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Anonymous");

#define MIN(x,y) ((x)<(y)?(x):(y))
// static char str[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
// static int buf_strlen = 62;

const char str[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
const int buf_strlen = 62;

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

/* help function signatures*/
char encrypt_char(char c, int code);
char decrypt_char(char c, int code);
int get_code_len(int code);
int get_digit(int num,int id);
int encrypt(char* src, char* dest, int length, int code);
int decrypt(char* src, char* dest, int length, int code);


int init_module(void)
{
    printk("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
    printk("-----------init_module-------------------\n");
    // my_major = register_chrdev(my_major, VIGENERE_DEVICE, &my_fops);
    my_major = register_chrdev(0, VIGENERE_DEVICE, &my_fops);

    
    if (my_major == -1){
        // TODO: check if need this
    	return -EFAULT;
    }
    if (my_major < 0)
    {
        printk(KERN_WARNING "can't get dynamic major\n");
        return my_major;
    }
    printk("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
    return 0;
}


void cleanup_module(void)
{   
    printk("cleanup_module\n");
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
    printk("ooooooooooooo\n");
    printk("filp %p\n", filp);
    MOD_INC_USE_COUNT;
    printk("ooooooooooooo\n");
    return 0;

}


int my_release(struct inode *inode, struct file *filp)
{
    printk("my release was called\n");
    MOD_DEC_USE_COUNT;
    return 0;
}

void debug_put_char(char* buf){
    char * c1 = kmalloc(1, GFP_KERNEL);
    *c1 = 'd';
    printk("c1 %c \n", *c1);

    copy_to_user(buf, c1, 1);

}

ssize_t my_read(struct file* filp, char* buf, size_t count, loff_t* f_pos)
{
    
    printk("rrrrrrrrrrrrrrrrrrrrrrr\n");
    printk("filp %p\n", filp);
    printk("my_read was called\nbuffer:\n");
    printk("%s\n", buffer_ptr);
    printk("read_idx: %d\n", read_idx);
    if (NULL == buf || NULL == buffer_ptr) {
        //TODOL: check what if buffer is just empty
        // TODO: check read before write
        // return 0;
        return -EFAULT;
    }
    // Do read operation.
    int cur_pid = current->pid;
    int code = other_pid + cur_pid;
    // int codeLen = get_code_len(code);

    int readLen = MIN(count, buffer_size - read_idx);

    // if readLen == 0 do nothing
    if (readLen == 0){
        return 0;
    }

    printk("readLen %d\n", readLen);
    char* decrypted = (char*)kmalloc((readLen) * sizeof(char), GFP_KERNEL);
    if (NULL == decrypted) {
        return -EFAULT;
    }
    decrypt(buffer_ptr + read_idx, decrypted, readLen, code);
    printk("new decrypted: %s\n", decrypted);
    printk("decrypted: %s\n", decrypted);
    printk("user buf before: %s\n", buf);
    int written = copy_to_user(buf, decrypted, readLen);
    printk("written: %d\n", written);
    printk("user buf after: %s\n", buf);
    kfree(decrypted);
    if (0 != written) {
        return -ENOMEM;
    }
    else {
        read_idx = readLen + read_idx;
        printk("readLen: %d\n", readLen);
        printk("rrrrrrrrrrrrrrrrrrrrrrr\n");
        return readLen;
    }

}


ssize_t my_write(struct file *filp, const char *buf, size_t count, loff_t *f_pos)
{    
    printk("wwwwwwwwwwwwwwwwwwwwww\n");
    printk("filp %p\n", filp);
    printk("my_write was called\n");
    printk("%s\n", buf);
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
    // int codeLen = get_code_len(code);

    int new_size = buffer_size + count;
    char * new_buffer_ptr = (char *)kmalloc(new_size * sizeof(char), GFP_KERNEL);
    if (new_buffer_ptr == NULL){
        return -ENOMEM;
    }
    int i = 0;
    // copy old buffer
    for(; i < buffer_size; i++){
        new_buffer_ptr[i] = buffer_ptr[i];
    }

    // copy input
    char * tmp_buffer = (char *)kmalloc(count, GFP_KERNEL);
    if (tmp_buffer == NULL){
        kfree(new_buffer_ptr);
        return -ENOMEM;
    }
    int bytes_not_copied = copy_from_user(tmp_buffer, buf, count);
    if (0 != bytes_not_copied) {
        kfree(new_buffer_ptr);
        kfree(tmp_buffer);
        return -EFAULT;
    }
    else
    {
        kfree(buffer_ptr);
        //encrypt and assign
        // int j = 0;
        // for (; buffer_size+j < new_size; j++) {
        //     new_buffer_ptr[i+j] = encrypt_char(new_buffer_ptr[i+j],get_digit(code, (j+buffer_size) % codeLen));
        // }
        
        encrypt(tmp_buffer, new_buffer_ptr + buffer_size, count, code);

        buffer_ptr = new_buffer_ptr;
        buffer_size = new_size;
        printk("buffer after my_write: %s\n", buffer_ptr);
        printk("wwwwwwwwwwwwwwwwwwwwww\n");
        return count;
    }
}

int encrypt(char* src, char* dest, int length, int code){

    int offset = buffer_size;
    int i = 0; 
    for(; i<length; i++){
        int id = (offset + i) % get_code_len(code);
        int x = get_digit(code, id);
        dest[i] = encrypt_char(src[i], x);
    }
    return 0;
}


int decrypt(char* src, char* dest, int length, int code){

    int offset = read_idx;
    int i = 0; 
    for(; i<length; i++){
        int id = (offset + i) % get_code_len(code);
        int x = get_digit(code, id);
        dest[i] = decrypt_char(src[i], x);
    }
    return 0;
}

int my_restart() {
    printk("restart-restart-restart-restart\n");
    read_idx = 0;
    printk("restart-restart-restart-restart\n");
    return 0;
}

int my_reset() {
    printk("reset-reset-reset-reset\n");
    read_idx = 0;
    kfree(buffer_ptr);
    buffer_ptr = NULL;
    buffer_size = 0;
    printk("reset-reset-reset-reset\n");
    return 0;
}

int my_set_other_pid(unsigned long arg) {
    printk("setpid-setpid\n");
    other_pid = arg;
    printk("setpid-setpid\n");
    return 0;
}

int my_set_debug_pid(unsigned long arg) {
    printk("dddddddddddd\n");
    if (arg != 1 && arg != 0) {
        return -EFAULT;
    }
    else
    {
        debug_flag = arg;
        printk("dddddddddddd\n");
        return 0;
    }
    
}

int my_ioctl(struct inode *inode, struct file *filp, unsigned int cmd, unsigned long arg)
{   
    printk("----ioctl-----\n");
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
    printk("ioctl didn't return in switch\n");
}

/* help functions */



char encrypt_char(char c, int code){
    if (1 == debug_flag) {
        return c;
    }
    int i = 0;
    for (i=0; i<buf_strlen; i++){
        if(str[i] == c){
            break;
        }
    }
    if(i == buf_strlen){
        //c not in array
        return c; 
    }
    return str[(i + code) % buf_strlen];

}

char decrypt_char(char c, int code) {
    if (1 == debug_flag) {
        return c;
    }
    int i = 0;
        for (i = 0; i < buf_strlen; i++) {
            if (str[i] == c) {
                break;
            }
        }
    if (i == buf_strlen) {
        //c not in array
        return c;
    }
    // printk("%d\n",(i - code) % buf_strlen);
    int x = (i - code) % buf_strlen;
    while (x < 0){
        x += buf_strlen;
    }
    return str[x];
}

int get_code_len(int code) {
    int cnt = 0;
    while (code> 0) {
        code = code/10;
        cnt++;
    }
    return cnt;
}

int get_digit(int num, int id) {
    int len = get_code_len(num);
    int powered = 1;
    int i = 0;
    for (i = 0; i < len-id-1; i++) {
        powered *= 10;
    }

    return (num /powered)%10;
}
