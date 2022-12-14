#ifndef IOCTL_STORE_H
#define IOCTL_STORE_H

struct key_value{
	int key;
	int value;
};

// int key = 4;

#define WR_VALUE _IOW('a', 'c', struct key_value *)
#define RD_VALUE _IOR('a', 'b', int32_t *)
#define GET_VALUE_FOR_KEY _IOWR('a', 'd', int32_t *)
#define IOCTL_DISPLAY _IO('a' , 0)

#endif
