/*************************************************************************
	> File Name: watch.c
	> Author: Mark Wang
	> Mail: mark83136@126.com
	> Created Time: Sun Jan 28 23:58:55 2018
	> Function: 跳线帽拿掉看门狗程序开始运行 200秒不喂狗系统会自动重启
 ************************************************************************/
#include <stdio.h>
#include "stdlib.h"  
#include "stdio.h"  
#include "string.h"
#include "unistd.h"
#include "fcntl.h"   //define O_WRONLY and O_RDONLY  


//GPIO7_A3_d   用户空间的编号为 219

 //user_pin: PB4
#define SYSFS_GPIO_EXPORT       "/sys/class/gpio/export"  
#define SYSFS_GPIO_PIN_VAL      "219"   
#define SYSFS_GPIO_DIR          "/sys/class/gpio/gpio219/direction"
#define SYSFS_GPIO_DIR_VAL      "out"  
#define SYSFS_GPIO_VAL          "/sys/class/gpio/gpio219/value"
#define SYSFS_GPIO_VAL_H        "1"
#define SYSFS_GPIO_VAL_L        "0"

int moveaction() {
	int fd;
	FILE* fp;

	fp = popen("sudo chmod 777 /sys/class/gpio/gpio219/direction", "r");
	fd = open(SYSFS_GPIO_DIR, O_WRONLY);
	if (fd == -1)
	{
		printf("ERR: Gpio pin direction open error.\n");
		return EXIT_FAILURE;
	}
	write(fd, SYSFS_GPIO_DIR_VAL, sizeof(SYSFS_GPIO_DIR_VAL));
	fp = popen("sudo chmod 777 /sys/class/gpio/gpio219/value", "r");
	//输出复位信号: 拉高>100ns
	fd = open(SYSFS_GPIO_VAL, O_RDWR);
	if (fd == -1)
	{
		printf("ERR: Gpio pin pin value open error.\n");
		return EXIT_FAILURE;
	}

	//循环喂狗
	while (1)
	{
		write(fd, SYSFS_GPIO_VAL_H, sizeof(SYSFS_GPIO_VAL_H));
		printf("SYSFS_GPIO_VAL_H\n");
		usleep(1000000);
		write(fd, SYSFS_GPIO_VAL_L, sizeof(SYSFS_GPIO_VAL_L));
		printf("SYSFS_GPIO_VAL_L\n");
		usleep(1000000);
	}

	write(fd, SYSFS_GPIO_VAL_H, sizeof(SYSFS_GPIO_VAL_H));
	close(fd);

	return 0;
}


//程序要长驻后台运行 否则这个电平翻转会终止
int main()
{
	int fd;
	FILE* fp;
	fp = popen("sudo chmod 777 /sys/class/gpio/export", "r");

	if (access("/sys/class/gpio/gpio219", F_OK) == 0)
	{
		printf("sys/class/gpio/gpio219 exists.\n");

		moveaction();
	}
	else
	{
		printf("sys/class/gpio/gpio219 not exists.\n");
		//打开端口/sys/class/gpio# echo 219 > export
		fd = open(SYSFS_GPIO_EXPORT, O_WRONLY);
		if (fd == -1)
		{
			printf("ERR: Gpio pin open error.\n");
			return EXIT_FAILURE;
		}
		write(fd, SYSFS_GPIO_PIN_VAL, sizeof(SYSFS_GPIO_PIN_VAL));
		moveaction();
	}
	return 0;
}


