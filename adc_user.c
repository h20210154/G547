
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>             
#include <unistd.h>             
#include <sys/ioctl.h>        

#define ADC_ALLIGN _IOW('a','b',char*)
#define ADC_CHANNEL _IOW('a','c',int32_t*)
#define ADC_MODE _IOW('a','d', char*)

#define DEVICE_FILE_NAME "/dev/adc-dev"


int main()
{
    int fd, ret_val;
	char allignment, mode;
	int channel;
	
	fd = open(DEVICE_FILE_NAME, O_RDWR);
	
	   if (fd < 0) 
	{
        printf("Can't open device file: %s\n", DEVICE_FILE_NAME);
        exit(-1);
        }
	
	printf("Please enter the channel you want to enter in : \n");
	scanf(" %d", &channel);

	printf("Please enter the mode you want to enter in, 'S' for single shot and 'C' for continous: \n");
	scanf(" %c", &mode);
	
	int fflush( FILE *stream );
	int fflush( FILE *stream );
	
	printf("Please enter the alligment, 'L' for left and 'R' for right: \n");
	scanf(" %c", &allignment);
	int fflush( FILE *stream );

    fd = open(DEVICE_FILE_NAME, O_RDWR);
	
	if((channel < 8 && channel >0) && (allignment == 'L' || allignment == 'R' || allignment == 'l' || allignment == 'r') && (mode == 'S' || mode == 'C' || mode == 's' || mode == 'c')) 
	{
	ioctl(fd, ADC_CHANNEL, (int32_t*)&channel);
	ioctl(fd, ADC_MODE, (char*)&mode);
	ioctl(fd, ADC_ALLIGN, (char*)&allignment);
	read(fd,&ret_val,2);
        printf("The ADC data is %.4x\n", ret_val);
	}
	
	else printf("Wrong Input\n");
	
    if (fd < 0) 
	{
        printf("Can't open device file: %s\n", DEVICE_FILE_NAME);
        exit(-1);
    }
    

    close(fd);
    return 0;
}
