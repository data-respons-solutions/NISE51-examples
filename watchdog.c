/*
 * watchdog.c
 *
 *  Created on: 2 juli 2018
 *      Author: ms
 */

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>

#include <linux/watchdog.h>
 
int main(void)
{
	const char *dev = "/dev/watchdog";
	const int32_t timeout = 5;
	int32_t fd = 0;
	int32_t bootstatus = 0;
	
	fd = open(dev, O_RDWR);
	if (fd < 0) {
		fprintf(stderr, "open: %s\n", strerror(errno));
		return -1;
	}
	
	printf("Watchdog: %s\n", dev);

	if (ioctl(fd, WDIOC_SETTIMEOUT, &timeout)  != 0) {
		fprintf(stderr, "ioctl: WDIOC_SETTIMEOUT: %s\n", strerror(errno));
		return -1;
	}
	
	printf("Timeout: %ds\n", timeout);

	if (ioctl(fd, WDIOC_GETBOOTSTATUS, &bootstatus) != 0) {
		fprintf(stderr, "ioctl: WDIOC_GETBOOTSTATUS: %s\n", strerror(errno));
		return -1;
	}
	
	printf("Boot cause: %s\n", bootstatus ? "WATCHDOG" : "POWER_ON");
	
	if (ioctl(fd, WDIOC_KEEPALIVE, NULL) != 0) {
		fprintf(stderr, "ioctl: WDIOC_KEEPALIVE: %s\n", strerror(errno));
		return -1;
	}
	
	printf("Watchdog kicked\n");
	
	sleep(timeout + 1);
	
	if (close(fd) != 0) {
		fprintf(stderr, "close: %s\n", strerror(errno));
		return -1;
	}
	
	printf("Watchdog closed\n");

   return 0;
}