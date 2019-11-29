#define DRIVER_AUTHOR "Praveen Kumar Vuppala <prem441@gmail.com>"
#define DRIVER_CARD "SHIKHARA DRD Client Driver"
#define DRIVER_DESC "USB Driver for sending the bulk data"
#define DRIVER_VERSION "1.0"
#define DRIVER_NAME "usb_shikhara_client"

#include <linux/usb.h>
#include <linux/hid.h>
#include <linux/slab.h>
#include <linux/export.h>
#include <linux/module.h>
#include <linux/printk.h>
#include <linux/mod_devicetable.h>
#include <uapi/linux/usb/ch9.h>
#include <linux/uaccess.h>
#define MIN(a,b) (((a) <= (b)) ? (a) : (b))

static unsigned char bulk_buf[512];
static int shikhara_open(struct inode *inode, struct file *file);
static struct usb_device_id shikhara_usb_driver_id_table[] = {
        {USB_DEVICE_AND_INTERFACE_INFO(0x04E8, 0x1234,USB_CLASS_VENDOR_SPEC, 0x0, 0x0) },
        { }
};
MODULE_DEVICE_TABLE(usb, shikhara_usb_driver_id_table);

typedef struct {
	struct usb_device      *usbdev;         
	struct usb_interface   *interface;      
	unsigned char          *bulk_in_buf;    
	size_t                  bulk_in_len;    
	__u8                    bulk_in_addr;   
	__u8                    bulk_out_addr;  

} device_t;

device_t *shikhara_device;

static ssize_t shikhara_write(struct file *file, const char *buffer, size_t write_count, loff_t *ppos)
{

	int retval;
	int wrote_cnt = MIN(write_count, 512);

	if (copy_from_user(bulk_buf, buffer, MIN(write_count, 512)))
		return -EFAULT;

	retval = usb_bulk_msg((device_t *)shikhara_device, 
			      usb_sndbulkpipe(shikhara_device, 0x2),
			      bulk_buf, MIN(write_count, 512),
			      &wrote_cnt, 5000);
	
	if (retval)
	{
        	printk(KERN_ERR "Bulk message returned %d\n", retval);
	        return retval;
	}

	return wrote_cnt;
}

static ssize_t shikhara_read(struct file *file, char *buffer, size_t count, loff_t *ppos)
{

	int retval;
	int read_cnt;
    
	retval = usb_bulk_msg(shikhara_device, 
			      usb_rcvbulkpipe(shikhara_device, 0x81), 
			      bulk_buf, 
			      512, 
			      &read_cnt, 
			      5000);
	
	if (retval)  {
        	printk(KERN_ERR "Bulk message returned %d\n", retval);
	        return retval;
    	}
	if (copy_to_user(buffer, bulk_buf, MIN(count, read_cnt)))
	        return -EFAULT;

	return MIN(count, read_cnt);
}

static struct file_operations shikhara_fops =
{
	.owner   = THIS_MODULE,      
	.read    = shikhara_read,    
	.write   = shikhara_write,   
	.open    = shikhara_open,    
};

static struct usb_class_driver shikhara_class = {
	.name        = "shikhara-m%d",
	.fops        = &shikhara_fops,       
};

static void shikhara_usb_driver_disconnect(struct usb_interface *intf)
{
	usb_deregister_dev(intf, &shikhara_class);

}

static int shikhara_usb_driver_probe(struct usb_interface *intf, const struct usb_device_id *id)
{
				
	struct usb_host_interface *iface_desc;
	struct usb_endpoint_descriptor *endpoint;
	int retval = -ENOMEM;
	int i;

	shikhara_device = kzalloc(sizeof(device_t), GFP_KERNEL);

	shikhara_device->usbdev = usb_get_dev(interface_to_usbdev(intf));
        shikhara_device->interface = intf;

	iface_desc = intf->cur_altsetting;
	
	for (i = 0; i < iface_desc->desc.bNumEndpoints; ++i) {
		endpoint = &iface_desc->endpoint[i].desc;
		
		if (!shikhara_device->bulk_in_addr &&  usb_endpoint_is_bulk_in(endpoint)) {
				shikhara_device->bulk_in_len =  le16_to_cpu(endpoint->wMaxPacketSize);
				shikhara_device->bulk_in_addr =  endpoint->bEndpointAddress;
	                        shikhara_device->bulk_in_buf  =  kzalloc(shikhara_device->bulk_in_len, GFP_KERNEL);
        	}
	
		if (!shikhara_device->bulk_out_addr && usb_endpoint_is_bulk_out(endpoint)) 
		                shikhara_device->bulk_out_addr = endpoint->bEndpointAddress;
        	
	}

	if (!(shikhara_device->bulk_in_addr && shikhara_device->bulk_out_addr)) 
			     return retval;
   	
	shikhara_device = (device_t *)interface_to_usbdev(intf);
        retval = usb_register_dev(intf, &shikhara_class);
		
	if (retval) {
	     usb_set_intfdata(intf, NULL);
             return retval;
   	}
	
	printk("shikhara device now attached to /dev/shikhara\n");
	return 0;

}
static struct usb_driver shikhara_driver = {
        .name                   = DRIVER_NAME,
        .probe                  = shikhara_usb_driver_probe,
	.disconnect             = shikhara_usb_driver_disconnect,
        .id_table               = shikhara_usb_driver_id_table,
};
static int shikhara_open(struct inode *inode, struct file *file)
{
	printk("Opened......\n");
	return 0;
}

static int __init shikhara_init(void)
{
	int retval = usb_register(&shikhara_driver);

        if (retval)
                pr_err(KBUILD_MODNAME ": usb_register failed. Error number %d\n", retval);

        return retval;
}

static void __exit shikhara_exit(void)
{
        usb_deregister(&shikhara_driver);
}

module_init(shikhara_init);
module_exit(shikhara_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR(DRIVER_AUTHOR);
MODULE_DESCRIPTION(DRIVER_DESC);
MODULE_VERSION(DRIVER_VERSION);

