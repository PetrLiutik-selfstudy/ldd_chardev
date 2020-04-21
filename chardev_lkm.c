#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/init.h>
#include <linux/cdev.h>
#include <linux/slab.h>

static char buffer[PAGE_SIZE];
struct cdev* my_cdev;
dev_t my_dev;
char devname[] = "chardev_demo";

static int device_open(struct inode* inode, struct file* file)
{
  return 0;
}

static ssize_t device_write(struct file* file, const char* buf, size_t lbuf, loff_t* ppos) {
  int nbytes = lbuf - copy_from_user(buffer+ *ppos, buf, lbuf);
  *ppos += nbytes;
  pr_info("Recieved data from app %s, nbytes=%d\n", buffer, nbytes);
  return nbytes;
}

static ssize_t device_read(struct file* file, char* buf, size_t lbuf, loff_t* ppos) {
  int nbytes = 0;
  int maxbytes = 0;
  int bytes_to_do = 0;

  maxbytes = PAGE_SIZE - *ppos;
  
  if(maxbytes > lbuf)
    bytes_to_do = lbuf;
  else
    bytes_to_do = maxbytes;
  
  nbytes = bytes_to_do - copy_to_user(buf, buffer+ *ppos, bytes_to_do);
  *ppos += nbytes;

  pr_info("Send data to app %s, nbytes=%d\n", buffer, nbytes);
  return nbytes;
}

static struct file_operations device_fops = {
  .owner = THIS_MODULE,
  .write = device_write,
  .open  = device_open,
  .read  = device_read,
};

static int __init device_init(void)
{
  int status = 0;
  //my_dev = MKDEV(100, 0);
  //register_chrdev_region(my_dev, 1, devname);
  status = alloc_chrdev_region(&my_dev, 0, 1, devname);
  if(status < 0) {
    pr_info("Failed to allocate chrdev region\n");
    return status;
  }
  printk(KERN_INFO "Major number=%d, minor number=%d\n", MAJOR(my_dev), MINOR(my_dev));

  my_cdev = cdev_alloc();
  cdev_init(my_cdev, &device_fops);

  status = cdev_add(my_cdev, my_dev, 1);

  if(status < 0) {
    pr_info("Driver init failed\n");
    return status;
  }
  printk(KERN_INFO "Chardev driver init\n");
	
  memset(buffer, '\0', PAGE_SIZE);
  return 0;
}

static void __exit device_exit(void)
{
  cdev_del(my_cdev);
  unregister_chrdev_region(my_dev, 1);
  printk(KERN_INFO "Chardev driver exit\n");
}

module_init(device_init);
module_exit(device_exit);

MODULE_AUTHOR("PetrL");
MODULE_DESCRIPTION("Character device demo");
MODULE_LICENSE("GPL");

