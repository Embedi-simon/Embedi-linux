#include <linux/module.h>
#include <linux/init.h>
#include <linux/cdev.h>

#define MEM_SIZE         200
#define MINOR_COUNT      10
#define BASE_MINOR       0

struct char_dev_t {
    struct cdev *c_dev;
    char *private;
};

static struct char_dev_t *char_dev;

static ssize_t char_read(struct file *file, char __user *buf, size_t size, loff_t *pos)
{
    struct char_dev_t *dev = NULL;

    if (file == NULL) {
        printk(KERN_ERR "file null\n");
        return -EINVAL;
    }

    dev = file->private_data;
    if (dev == NULL) {
        printk(KERN_ERR "dev null");
        return -EINVAL;
    }

    if (size + *pos > MEM_SIZE) {
        size = MEM_SIZE - *pos;
    }

    if (copy_to_user(buf, (dev->private + *pos), size)) {
        printk(KERN_ERR "copy_to_user fail\n");
    } else {
        *pos += size;
        printk(KERN_INFO "read %lu bytes to %llu\n", size, *pos);
    }
    return size;
}

static ssize_t char_write(struct file *file, const char __user *buff, size_t size, loff_t *pos)
{
    struct char_dev_t *dev = NULL;

    if (file == NULL) {
        printk(KERN_ERR "file null\n");
        return -EINVAL;
    }

    dev = file->private_data;
    if (dev == NULL) {
        printk(KERN_ERR "dev null");
        return -EINVAL;
    }

    if (size + *pos > MEM_SIZE) {
        size = MEM_SIZE - *pos;
    }

    if (copy_from_user(dev->private + *pos, buff, size)) {
        printk(KERN_ERR "copy_from_user fail\n");
    } else {
        *pos += size;
    }

    printk(KERN_INFO "write %lu bytes to %llu\n", size, *pos);
    return size;
}

static int char_open(struct inode *node, struct file *file)
{
    if (file == NULL) {
        printk(KERN_ERR "file null\n");
        return -1;
    }

    file->private_data = char_dev;
    file->f_pos = 0;
    printk(KERN_INFO "char open");
    return 0;
}

static loff_t char_llseek(struct file *file, loff_t offset, int orign)
{
    loff_t pos = 0;

    switch(orign){
        case SEEK_SET:
            pos = offset;
            break;
        case SEEK_CUR:
            pos = file->f_pos + offset;
            break;
        case SEEK_END:
            pos = MEM_SIZE + offset;
            break;
        default:
            return -EINVAL;
    }
    file->f_pos = pos;
    printk(KERN_INFO "pos = %llu\n", pos);
    return file->f_pos;
}

struct file_operations char_ops = {
    .owner  = THIS_MODULE,
    .write  = char_write,
    .read   = char_read,
    .open   = char_open,
    .llseek = char_llseek,
};

static int __init char_init(void)
{
    int ret = 0;
    dev_t dev_id = 0;

    char_dev = kzalloc(sizeof(struct char_dev_t), GFP_KERNEL);
    if (char_dev == NULL) {
        printk(KERN_ERR "kzalloc fail\n");
        return -1;
    }

    char_dev->private = kzalloc(sizeof(char) * MEM_SIZE, GFP_KERNEL);
    if (char_dev->private == NULL) {
        printk(KERN_ERR "private data kzalloc fail\n");
        goto kzalloc_err;
    }

    char_dev->c_dev = cdev_alloc();
    if (char_dev->c_dev == NULL) {
        printk(KERN_ERR "cdev_alloc fail\n");
        ret = -1;
        goto alloc_err;
    }

    ret = alloc_chrdev_region(&dev_id, BASE_MINOR, MINOR_COUNT, "embedi_char");
    if (ret < 0) {
        printk(KERN_ERR "alloc_chrdev_region fail\n");
        goto region_err;
    }

    char_dev->c_dev->owner = THIS_MODULE;
    cdev_init(char_dev->c_dev, &char_ops);
    ret = cdev_add(char_dev->c_dev, dev_id, MINOR_COUNT);
    if (ret < 0) {
        printk(KERN_ERR "cdev_add fail\n");
        goto add_err;
    }

    printk(KERN_INFO "char init\n");
    return ret;

add_err:
    unregister_chrdev_region(dev_id, MINOR_COUNT);
region_err:
    kfree(char_dev->c_dev);
alloc_err:
    kfree(char_dev->private);
kzalloc_err:
    kfree(char_dev);
    return ret;
}

static void __exit char_exit(void)
{
    unregister_chrdev_region(char_dev->c_dev->dev, MINOR_COUNT);
    kfree(char_dev->c_dev);
    kfree(char_dev->private);
    kfree(char_dev);
    printk(KERN_INFO "char exit\n");
}

module_init(char_init);
module_exit(char_exit);
MODULE_LICENSE("GPL");
