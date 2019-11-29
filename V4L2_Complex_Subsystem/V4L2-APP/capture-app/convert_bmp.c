#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <asm/types.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <linux/videodev2.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <stdio.h>

typedef struct {
    char type[2];
    unsigned int file_size;
    unsigned int reserved;
    unsigned int offset;
} __attribute__((__packed__)) HEADER;



typedef struct {
    unsigned int header_size;
    unsigned int width;
    unsigned int height;
    unsigned short planes;
    unsigned short bits_per_pixel;
    unsigned int compression;
    unsigned int imagesize;
    unsigned int hresolution;
    unsigned int vresolution;
    unsigned int ncolors;
    unsigned int important_colors;
} __attribute__((__packed__)) INFOHEADER;



typedef struct {
    HEADER header;
    INFOHEADER infoheader;
    unsigned char *data;
} __attribute__((__packed__)) BITMAP;



/*
 * @func fill_bmp_header
 *
 * @desc fill bmp header informations
 */
void fill_bmp_header(BITMAP *bmp, unsigned long image_size, 
        int wd, int ht, int bits) {
    /* fill header info */
    bmp->header.type[0] = 'B';
    bmp->header.type[1] = 'M';

    /* fill information header */
    bmp->infoheader.header_size = sizeof(INFOHEADER);
    bmp->infoheader.width = wd;
    bmp->infoheader.height = ht;
    bmp->infoheader.planes = 1;
    bmp->infoheader.bits_per_pixel = bits;
    bmp->infoheader.compression = 0;
    bmp->infoheader.hresolution = 0;
    bmp->infoheader.vresolution = 0;
    bmp->infoheader.ncolors = bits == 8 ? 0x100 : 0;
    bmp->infoheader.important_colors = 0;

    /* find image data offset */
    bmp->header.offset = sizeof(HEADER) 
        + sizeof(INFOHEADER) 
        + 4 * bmp->infoheader.ncolors;
}
/*
 * @func yuv444tobgr888
 *
 * @desc convert YUV to RGB
*/
void yuv444torgb888(unsigned char yuv[], unsigned char bgr[]) {
    int r, g, b;
    b = yuv[0] + 1.772 * (yuv[1] - 128);
    bgr[0] = ((b < 0) ? 0 : ((b > 255) ? 255 : b));
    g = yuv[0] - 0.344 * (yuv[1] - 128) - 0.714 * (yuv[2] - 128);
    bgr[1] = ((g < 0) ? 0 : ((g > 255) ? 255 : g));
    r = yuv[0] + 1.402 * (yuv[2] - 128);
    bgr[2] = ((r < 0) ? 0 : ((r > 255) ? 255 : r));
}

/*
 * @func converttobmp
 *
 * @desc convert the image to bmp and save into a file
 */
void converttobmp(unsigned char *buffer) {
    char imgfile[128];
    int imgsavefd;
    int count = 0;
    int imagesize;
    int index;
    unsigned char *data;
    unsigned char yuv[3], bgr[3];
    BITMAP bmp;
    unsigned int width=640,height=480,i;	
    printf("converte to bmp format...");

    imagesize = width * height * 2;
    fill_bmp_header(&bmp, imagesize, width, -height, 24);

    bmp.infoheader.imagesize = (imagesize * 3) / 2;
    bmp.header.file_size = sizeof(HEADER) 
        + sizeof(INFOHEADER) + bmp.infoheader.imagesize;
    printf("\nheader size=%d,infoheader size=%d\n",sizeof(HEADER),sizeof(INFOHEADER));
    printf("bmp.infoheader.imagesize=%d\n",bmp.infoheader.imagesize);	
    /* allocate memory */
    bmp.data = (unsigned char *) malloc(bmp.infoheader.imagesize);
    data = bmp.data;

#if 0
    do {
        sprintf(imgfile, 
                "yuv_image_%dx%d_%d.yuyv", 
                width, height, count++);
        imgsavefd = open(imgfile, 
                O_CREAT | O_WRONLY | O_EXCL, 
                S_IRWXU | S_IRWXG | S_IRWXO);
        if (imgsavefd >= 0) break;
    } while (1);
    write(imgsavefd, buffer, imagesize);
    close(imgsavefd);
    printf("file name = %s\n", imgfile);	
#endif

    do {
        sprintf(imgfile, 
                "capture_image_%dx%d_%d.bmp", 
                width, height, count++);
        imgsavefd = open(imgfile, 
                O_CREAT | O_WRONLY | O_EXCL, 
                S_IRWXU | S_IRWXG | S_IRWXO);
        if (imgsavefd >= 0) break;
    } while (1);

    printf("file name = %s\n", imgfile);
    /* converting to bmp data */
    data = bmp.data;
    for (index = 0; index < imagesize; index += 4) {
        yuv[0] = buffer[index];
        yuv[1] = buffer[index+1];
        yuv[2] = buffer[index + 3];
        yuv444torgb888(yuv, bgr);
        memcpy(data, (const void *) bgr, 3);
        data += 3;
        yuv[0] = buffer[index + 2];
        yuv444torgb888(yuv, bgr);

        memcpy(data, (const void *) bgr, 3);
        data += 3;
    }


    /* write header and info header of bmp */
    printf("write header details\n");
    write(imgsavefd, (const void *) &bmp.header, sizeof(HEADER));
    printf("write infoheader details\n");
    write(imgsavefd, (const void *) &bmp.infoheader, sizeof(INFOHEADER));
    printf("write image data\n");
    write(imgsavefd, (const void *) bmp.data, bmp.infoheader.imagesize);
    printf("%s is saved...\n", imgfile);

    close(imgsavefd);
    free(bmp.data);
}


