#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/list.h>
#include <linux/slab.h>
#include <linux/fs.h>
#include <linux/ioctl.h>
#include <linux/uaccess.h>
#include <linux/mutex.h>

#include "ioctl_store.h"

/* Meta Information */
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Danish 4 GNU/Linux");
MODULE_DESCRIPTION("Key Value Store");

static DEFINE_MUTEX(mlock);


//Key Value Pair
struct store_struct{
	struct list_head list_member;
	int key;
	int value;
};

//Add a node to a linked list
static void add_node(int arg, int arg2, struct list_head *head)
{
	struct list_head *iter;
	struct store_struct *objPtr;

	mutex_lock(&mlock);
		list_for_each(iter, head){
		objPtr = list_entry(iter,struct store_struct, list_member);
			if (objPtr->key == arg){
				objPtr->value = arg2;
				mutex_unlock(&mlock);
				return;
			}

		}

		//struct store_struct *storePtr = kmalloc(sizeof(struct store_struct), GFP_KERNEL);
		objPtr = kmalloc(sizeof(struct store_struct), GFP_KERNEL);
		objPtr->key = arg;
		objPtr->value = arg2;
		INIT_LIST_HEAD(&objPtr->list_member);
		list_add(&objPtr -> list_member, head);
	mutex_unlock(&mlock);
}

//Print all nodes in the linked list
static void display(struct list_head *head)
{
	struct list_head *iter;
	struct store_struct *objPtr;

	list_for_each(iter, head){
	objPtr = list_entry(iter,struct store_struct, list_member);
		printk("key = %d and value = %d ", objPtr->key ,objPtr->value);

	}
}

// Find a node in the linked list.
static int find_node(int key, struct list_head *head)
{
	struct list_head *iter;
	struct store_struct *objPtr;

	//mutex_lock(&mlock);
		list_for_each(iter, head){
		objPtr = list_entry(iter,struct store_struct, list_member);
			if (objPtr->key == key)
				return objPtr->value;
		}
	//mutex_unlock(&mlock);
		return -1;
}
/**
 * this function is called, when the device file is opened
 */
static int file_open(struct inode *device_file, struct file *instance){
	printk("open was called");
	return 0;
}	

/**
 * this function is called, when the device file is closed
 */
static int file_closed(struct inode *device_file, struct file *instance){
	printk("closed is called");
	return 0;
}



int32_t answer = 0;
LIST_HEAD(storeHead);

//ioctl commands

static long int my_ioctl(struct file *file, unsigned cmd, unsigned long arg) {
	struct key_value msg;
	switch(cmd){
		case WR_VALUE: //Insert a key-value pair if not exists or Update value if exists.
			if(copy_from_user(&msg, (struct key_value *) arg, sizeof(msg)))
				printk("ioctl - Error copying data from user!\n");
			else
			{
				add_node(msg.key, msg.value, &storeHead);
				//display(&storeHead);
			}
			break;
		case RD_VALUE:
			if(copy_to_user((int32_t *) arg, &answer, sizeof(answer)))
				printk("error copying data to user \n");
			else
				printk("the answer is copied\n");
			break;
		case GET_VALUE_FOR_KEY: // Fetch value for a given key. If key is not found it returns -1.
			if(copy_from_user(&answer, (int32_t *) arg, sizeof(answer)))
				printk("ioctl - Error copying data from user!\n");
			else
			{
				int val;
				val = find_node(answer, &storeHead);
				if(copy_to_user((int32_t *) arg, &val, sizeof(val)))
					printk("error copying data to user \n");
				else
					printk("the value found in %d \n", val);
			}
			break;
		case IOCTL_DISPLAY: //Display full linked-list
			display(&storeHead);
			break;

	}
	return 0;
}

static struct file_operations fops = {
	.owner = THIS_MODULE,
	.open = file_open,
	.release = file_closed,
	.unlocked_ioctl = my_ioctl 
};




#define MYMAJOR 64

/**
 * @brief This function is called, when the module is loaded into the kernel
 */
static int __init ModuleInit(void) {
	int retval;
	printk("Hello, Kernel!\n");
	/* register device nr. */
	retval = register_chrdev(MYMAJOR, "key_value_store", &fops);
	if(retval == 0) {
		printk("key_value_store - registered Device number Major: %d, Minor: %d\n", MYMAJOR, 0);
	}
	else if(retval > 0) {
		printk("ioctl_value_store - registered Device number Major: %d, Minor: %d\n", retval>>20, retval&0xfffff);
	}
	else {
		printk("Could not register device number!\n");
		return -1;
	}
	return 0;
}

/**
 * @brief This function is called, when the module is removed from the kernel
 */
static void __exit ModuleExit(void) {
	unregister_chrdev(MYMAJOR, "key_value_store");
	printk("Goodbye, Kernel\n");
}

module_init(ModuleInit);
module_exit(ModuleExit);



