#include <fcntl.h>
#include <linux/soundcard.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stdint.h>
#include "../headers/logic.h"
#include "../headers/system_logic.h"
#include "../headers/globals.h"
#include "../headers/utils.h"

#if defined TARGET_RG35XX
#define SYSFS_CPUFREQ_DIR "/sys/devices/system/cpu/cpu0/cpufreq"
#define SYSFS_CPUFREQ_LIST SYSFS_CPUFREQ_DIR "/scaling_available_frequencies"
#define SYSFS_CPUFREQ_SET SYSFS_CPUFREQ_DIR "/scaling_max_freq"
#define SYSFS_CPUFREQ_CUR SYSFS_CPUFREQ_DIR "/scaling_cur_freq"
#endif

volatile uint32_t *memregs;
int32_t memdev = 0;
int oldCPU;

void to_string(char str[], int num)
{
    int i, rem, len = 0, n;

    n = num;
    while (n != 0)
    {
        len++;
        n /= 10;
    }
    for (i = 0; i < len; i++)
    {
        rem = num % 10;
        num = num / 10;
        str[len - (i + 1)] = rem + '0';
    }
    str[len] = '\0';
}

void setCPU(uint32_t mhz)
{
	currentCPU = mhz;
	#if defined TARGET_RG35XX
		char strMhz[10];
		printf("CPUFREQ_SET: %s\n", SYSFS_CPUFREQ_SET);
		int fd = open(SYSFS_CPUFREQ_SET, O_RDWR);
		to_string(strMhz, (mhz * 1));
		ssize_t ret = write(fd, strMhz, strlen(strMhz));
		if (ret==-1) {
			logMessage("ERROR", "setCPU", "Error writting to file");
		}
		close(fd);
		char temp[300];
		snprintf(temp,sizeof(temp),"CPU speed set: %d",currentCPU);
		logMessage("INFO","setCPU",temp);
	#endif
}

void turnScreenOnOrOff(int state) {
	const char *path = "/sys/class/graphics/fb0/blank";
	const char *blank = state ? "0" : "1";
	int fd = open(path, O_RDWR);
	int ret = write(fd, blank, strlen(blank));
	if (ret==-1) {
		generateError("FATAL ERROR", 1);
	}
	close(fd);
}

void clearTimer() {
	if (timeoutTimer != NULL) {
		SDL_RemoveTimer(timeoutTimer);
	}
	timeoutTimer = NULL;
}

uint32_t suspend() {
	if(timeoutValue!=0&&hdmiEnabled==0) {
		clearTimer();
		oldCPU=currentCPU;
		turnScreenOnOrOff(0);
		isSuspended=1;
	}
	return 0;
};

void resetScreenOffTimer() {
#ifndef TARGET_PC
	if(isSuspended) {
		turnScreenOnOrOff(1);
		currentCPU=oldCPU;
		isSuspended=0;
	}
	clearTimer();
	timeoutTimer=SDL_AddTimer(timeoutValue * 1e3, suspend, NULL);
#endif
}

void initSuspendTimer() {
	timeoutTimer=SDL_AddTimer(timeoutValue * 1e3, suspend, NULL);
	isSuspended=0;
	logMessage("INFO","initSuspendTimer","Suspend timer initialized");
}

void HW_Init()
{
	logMessage("INFO","HW_Init","HW Initialized");
}

void rumble() {
// add moto support
	logMessage("INFO","rumble","rumble");

}

int getBatteryLevel() {
	
	int charge=30;
	int charging=0;

	FILE *f = fopen("/sys/class/power_supply/battery/capacity", "r");
	fscanf(f, "%i", &charge);
	fclose(f);
	logMessage("INFO","Battery", "getBatteryLevel");
	char temp[300];
	snprintf(temp,sizeof(temp),"Battery charge: %d", charge);
	logMessage("INFO","Battery", temp);

	f = fopen("/sys/class/power_supply/battery/charger_online", "r");
	fscanf(f, "%i", &charging);
	fclose(f);

	snprintf(temp,sizeof(temp),"Battery charging: %d", charging);
	logMessage("INFO","Battery", temp);

	if (charging==1) {
		return 6;
	}

	if (charge<=20)          return 1;
    else if (charge<=40)  return 2;
    else if (charge<=60)  return 3;
    else if (charge<=80)  return 4;
    else if (charge<=100) return 5;
    else                  return 6;
}

int getCurrentBrightness() {
	printf("getCurrentBrightness\n");
	int level;
	FILE *f = fopen("/sys/class/backlight/backlight.2/brightness", "r");
	if (f==NULL) {
		logMessage("INFO","getCurrentBrightness","Error, file not found");
		return 12;
	} else {
		int ret = fscanf(f, "%i", &level);
		if(ret==-1) {
			logMessage("INFO","getCurrentBrightness","Error reading file");
			return 12;
		}
		fclose(f);
	}
	return level;
}

int getMaxBrightness() {
	printf("getMaxBrightness\n");

	int level;
	FILE *f = fopen("/sys/class/backlight/backlight.2/max_brightness", "r");
	if (f==NULL) {
		logMessage("INFO","getMaxBrightness","Error, file not found");
		return 12;
	} else {
		int ret = fscanf(f, "%i", &level);
		if(ret==-1) {
			logMessage("INFO","getMaxBrightness","Error reading file");
			return 12;
		}
		fclose(f);
	}
	return level;
}

void setBrightness(int value) {
	printf("setBrightness: %d\n",value);

	FILE *f = fopen("/sys/class/backlight/backlight.2/brightness", "w");
	if (f!=NULL) {
		fprintf(f, "%d", value);
		fclose(f);
	}
	// Add the brightness value to the persistent storage
	FILE *fb = fopen("/boot/boot/brightness", "w");
	if (fb!=NULL) {
		fprintf(fb, "%d", value);
		fclose(fb);
	}
}

int get_master_volume() {
    FILE* fp;

    char output[256];
    char volume_str[4];
    char command[256];

    snprintf(command, sizeof(command), "%s get '%s'", "/usr/bin/amixer", "DAC PA");

    //printf("Command: %s\n", command);

    fp = popen(command, "r");
    if (fp == NULL) {
        printf("Failed to run amixer command.\n");
        return 1;
    }

    while (fgets(output, sizeof(output), fp) != NULL) {
        if (strncmp(output, "  Mono:", 7) == 0) {
            sscanf(output, "  Mono: %2s", volume_str);
            break;
        }
    }

    //printf("volume_str: %s\n", volume_str);

    pclose(fp);

    return atoi(volume_str);
}

