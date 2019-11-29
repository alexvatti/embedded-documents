/*
v4l2 - camera application
*/

#include<stdio.h>
#include<stdarg.h>
#include<string.h>
#include<linux/types.h>
#include<fcntl.h>
#include<linux/videodev2.h>
#include<linux/stat.h>
#include <sys/mman.h>

#define DEV_NODE "/dev/video0"
#define PIXEL_FORMAT V4L2_PIX_FMT_UYVY
int fd=-1;
char *user_ptrs[4];
int sizeimage;

int open_device()
{
	fd=open(DEV_NODE,O_RDWR);
	if(fd < 0){
		printf("device open failed\n");
		return -1;
	}
}

int close_device()
{
	if(fd>0){
		close(fd);
	}
}

int device_capabilities()
{
	struct v4l2_capability cap;
	int ret;
	ret=ioctl(fd,VIDIOC_QUERYCAP,&cap);

	if(cap.capabilities & V4L2_CAP_VIDEO_CAPTURE){
		printf("Device Support the video capture mode\n");
		return 0;
	}else
		return -1;
}

int get_set_format()
{
	int ret;
	struct v4l2_format fmt;
	fmt.fmt.pix.field=V4L2_FIELD_NONE;
	fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	ret=ioctl(fd,VIDIOC_G_FMT, &fmt);
	if(ret < 0)
		printf("Unable to get device get format\n");
	else {
		printf("width=%d\n",fmt.fmt.pix.width);
		printf("height=%d\n",fmt.fmt.pix.height);
		printf("bytesperline=%d\n",fmt.fmt.pix.bytesperline);
		printf("image size=%d\n",fmt.fmt.pix.sizeimage);
		printf("colorspace=%d\n",fmt.fmt.pix.colorspace);
		sizeimage=fmt.fmt.pix.sizeimage;
	}
}

int request_buffers()
{
	struct v4l2_requestbuffers buffers;
	int ret;	
	buffers.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	buffers.count=3;
	buffers.memory= V4L2_MEMORY_MMAP;
	ret=ioctl(fd, VIDIOC_REQBUFS, &buffers);
	if(ret < 0){
		printf("request buffers failed\n");
	}
}

int query_buffers()
{
	struct v4l2_buffer buffer;
	int i,ret;

	for(i=0;i<3;i++) {
		buffer.index=i;
		buffer.type=V4L2_BUF_TYPE_VIDEO_CAPTURE;
        	buffer.memory= V4L2_MEMORY_MMAP;
		ret=ioctl(fd,VIDIOC_QUERYBUF,&buffer);
		if(ret < 0) {
			printf("querybuf failed at index=%d\n",i);
		}
		user_ptrs[i]=mmap(0,sizeimage, PROT_READ , MAP_SHARED ,fd,0);
		ret=ioctl(fd,VIDIOC_QBUF,&buffer);
		if(ret < 0) {
			printf("QBUF after querybuf failed\n");
		}
	}
}

int stream_on()
{
	int ret;
	int type=V4L2_BUF_TYPE_VIDEO_CAPTURE;
	ret = ioctl(fd,VIDIOC_STREAMON,&type);
	if(ret < 0) {
		printf("stream on failed\n");
	}
}

int stream_off()
{
	int ret;
	int type=V4L2_BUF_TYPE_VIDEO_CAPTURE;
	ret = ioctl(fd, VIDIOC_STREAMOFF, &type);
	if(ret < 0){
		printf("stream off failed\n");
	}
}

int process_image()
{
	struct v4l2_buffer buffer;
	buffer.type=V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buffer.memory= V4L2_MEMORY_MMAP;
	int n=20;
	int skip=1;
	int ret;
	while(n > 1) {
		ret=ioctl(fd,VIDIOC_DQBUF,&buffer);
		if(ret < 0) {
			printf("DQBUF failed\n");
		}
		n--;
		if(skip>10){
			converttobmp((unsigned char *) user_ptrs[buffer.index]);
		}
		skip++;
		ret=ioctl(fd,VIDIOC_QBUF,&buffer);
		if(ret < 0) {
			printf("QBUF failed\n");
		}

	}
}

int clear_buffers()
{
	struct v4l2_requestbuffers buffers;
        int ret,i;

        for(i=0;i<3;i++) {
		munmap(user_ptrs[i],sizeimage);
	}
        buffers.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buffers.count=0;
        buffers.memory= V4L2_MEMORY_MMAP;
        ret=ioctl(fd, VIDIOC_REQBUFS, &buffers);
        if(ret < 0){
                printf("clearing : request buffers failed\n");
        }
}
	

		
int main()
{
	int ret;
	ret = open_device();
	if(ret < 0) {
		printf("Device open failed\n");
		return -1;
	}
	ret = device_capabilities();
	if(ret < 0) {
		printf("Device does not support video capture\n");
	}
	get_set_format();
	request_buffers();
	query_buffers();
	stream_on();
	process_image();
	stream_off();
	clear_buffers();
	close_device();
}
