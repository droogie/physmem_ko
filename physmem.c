#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/mm.h>
#include <linux/device.h>
#include <linux/cdev.h>

#define DEVICE_NAME "physmem"

static dev_t  dev;
static struct cdev c_dev;
static struct class *cl;

int device_mmap(struct file *filp, struct vm_area_struct *vma)
{
    unsigned long offset = vma->vm_pgoff << PAGE_SHIFT;

    if (offset >= __pa(high_memory) || (filp->f_flags & O_SYNC))
        vma->vm_flags |= VM_IO;
    vma->vm_flags |= (VM_DONTEXPAND | VM_DONTDUMP);

    if (io_remap_pfn_range(vma, vma->vm_start, offset, 
        vma->vm_end-vma->vm_start, vma->vm_page_prot))
        return -EAGAIN;
    return 0;
}

static struct file_operations fops = {
    .mmap = device_mmap,
};

int init_module(void)
{

    if (alloc_chrdev_region(&dev, 0, 1, DEVICE_NAME) < 0)
        goto fail;

    if ((cl = class_create(THIS_MODULE, DEVICE_NAME)) == NULL)
        goto class_create_fail;

    if (device_create(cl, NULL, dev, NULL, DEVICE_NAME) == NULL)
        goto device_create_fail;

    cdev_init(&c_dev, &fops);
    if (cdev_add(&c_dev, dev, 1) == -1)
        goto device_add_fail;

    printk(KERN_INFO "Loaded physmem device");
	return 0;

    device_add_fail:
        device_destroy(cl, dev);
    device_create_fail:
        class_destroy(cl);
    class_create_fail:
        unregister_chrdev_region(dev, 1);
    fail:
        return -1;
}

void cleanup_module(void)
{
    cdev_del(&c_dev);
    device_destroy(cl, dev);
    class_destroy(cl);
    unregister_chrdev_region(dev, 1);
    printk(KERN_INFO "Unloaded physmem device");
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("https://github.com/droogie");
MODULE_DESCRIPTION("An unrestricted /dev/mem implementation");