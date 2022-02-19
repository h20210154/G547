#include <linux/version.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/kdev_t.h>
#include <linux/device.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/random.h>
#include <linux/ioctl.h>


static dev_t value; 					/*variable for the device number*/
static struct cdev c_dev; 				/*variable for the character device structure */
static struct class *cls; 				/*variable for the device class*/

/*-------------------Defining Macros that will return the Request numbers generated--------------------*/

#define ADC_ALLIGN _IOW('a','b',char*)
#define ADC_CHANNEL _IOW('a','c',int32_t*)
#define ADC_MODE _IOW('a','d', char*)


char allignment, mode;						/*mode and allignment will be saved in these variables*/	
uint32_t channel;						/*Channel will be saved in this variable*/
uint16_t adc_data,random_num;
uint16_t my_random_num(void);				/*Function Declaration for the generation of Random Number.(Function Definin is given below in the code)*/


/*------------------Defining all the file operation Functions----------------------------------*/

static int ADC_file_open(struct inode *i, struct file *f)		/*when open function called from user space, this is executed*/
{
	printk(KERN_ALERT "ADC Device file: open() \n");
	return 0;
}

static int ADC_file_close(struct inode *i, struct file *f)		/*when close function called from user space, this is executed*/
{
	printk(KERN_ALERT "ADC Device file: close() \n");
	return 0;
}

static ssize_t ADC_file_read(struct file *f, char __user *buffer, size_t len, loff_t *off)   /*when read function called from user space, this is executed*/
{
	printk(KERN_ALERT "ADC device file: read() \n");					/*All the functionality is defined in the Read () function.*/
    	
    	adc_data = my_random_num();
    	
	if (allignment == 'L' || allignment == 'l')
	{
		
		adc_data = adc_data & 0XFFF0;							/*Doing the AND operation to make data allignment LEFT*/
		copy_to_user((int16_t*)buffer, &adc_data, 2);
		printk(KERN_INFO "Data is Successfully Left Alligned and data passed to user\n");
		
	}
	
	else if(allignment == 'R' || allignment == 'r')
	{
		
		adc_data = adc_data & 0X0FFF;						/*Doing the AND operation to make data allignment RIGHT*/
		copy_to_user((int16_t*)buffer, &adc_data, 2);
		printk(KERN_INFO "Data is Successfully Right Alligned and data passed to user\n");
	}
	
	else printk(KERN_ALERT "WRONG INPUT-ALLIGNMENT");
	
	
	if (mode == 'S' || mode == 's')
	{
		printk(KERN_INFO "The entered mode is single shot");
	}
	
	else if (mode == 'C' || mode == 'c')
	{
		printk(KERN_INFO "The Entered Mode is Continous");
	}
	
	else printk(KERN_ALERT "WRONG INPUT-MODE");
	
	if (channel > 0 && channel <=8)
	{
		printk(KERN_INFO "The Channel selected is %d", channel);
	}
	
	else printk(KERN_ALERT "WRONG INPUT-CHANNEL");
	
	return 0;
}

/*---------------------------------------Taking the data from the user based on the request code----------------------------------------------------------------*/


static long my_adc(struct file *file, unsigned int req_code, unsigned long user_data)
{
         switch(req_code) {
                		case ADC_ALLIGN : 									
					{
						copy_from_user(&allignment, (char*)user_data, sizeof(allignment));
						
						 break;
							
					}
				case ADC_CHANNEL :
					
					{
						copy_from_user(&channel, (uint32_t*)user_data, sizeof(channel));
						
						break;
					}
                       
                       
                		case ADC_MODE:
					{
						copy_from_user(&mode, (char*)user_data, sizeof(mode));
						
						break;
					}
                      
                		default: break;
                        
                        
        }
       
      
	
	 return 0;
}

/*-------------------------Function for the Generation of the Random Number--------------------------*/

uint16_t my_random_num(void)
{
    uint16_t j;
    uint16_t *random_num = &j;			/*The variable stored everytime the program runs is allocated different location in memory, that location is the random number*/
    return (uint16_t)random_num;
  }
  
static struct file_operations fops =
{
	.owner = THIS_MODULE,
	.open = ADC_file_open,
	.release = ADC_file_close,
	.read = ADC_file_read,
	.unlocked_ioctl = my_adc
};

/*------------------------------------Device Registration and class creation------------------------------------------------------------------------*/

static int __init mychar_init(void)
{
	printk(KERN_ALERT "adc-dev driver registered");
	
	if((alloc_chrdev_region(&value, 177, 1, "BITS-PILANI"))<0)			/*Creation of MAJOR and MINOR numbers*/
	{
		return -1;
	}
	
	if((cls=class_create(THIS_MODULE, "adcdev")) == NULL)			/*Class Creation*/
	{
		unregister_chrdev_region(value,1);
		return -1;
	}
	
	if((device_create(cls, NULL, value, NULL, "adc-dev")) == NULL)    		/*Device file creation with the name "adc-dev"*/
	{
		class_destroy(cls);
		unregister_chrdev_region(value,1);
		return -1;
	}

	cdev_init(&c_dev, &fops);
	if(cdev_add(&c_dev, value, 1) < 0)
	{
		
		unregister_chrdev_region(value,1);
		return -1;
	}
	
	return 0;
}

/*------------------------------------EXIT function------------------------------------------------------------------------*/

static void __exit mychar_exit(void)
{
	cdev_del(&c_dev);
	device_destroy(cls,value);
	class_destroy(cls);
	unregister_chrdev_region(value,1);
	printk(KERN_ALERT "adc-dev driver unregistered \n");
}

module_init(mychar_init);
module_exit(mychar_exit);

MODULE_DESCRIPTION("ADC charater driver program "); 
MODULE_AUTHOR("MAYANK");
MODULE_LICENSE("GPL");
