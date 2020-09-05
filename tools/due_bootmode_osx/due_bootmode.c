#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>

int main(int argc, char *argv[]) {
    if(argc < 2) {
        printf("error: %s called without argument\n", argv[0]);
        return 1;
    }

    printf("Using device %s.\n", argv[1]);

    // Open DUE.
    int devfd = open(argv[1], O_RDWR | O_NOCTTY | O_NDELAY);
    if (devfd == -1) {
        printf("Failed to open device\n");
        return 1;
    }

    // Fetch options.
    struct termios options;
    speed_t speed = 1200;

    if (tcgetattr(devfd, &options) == -1)
    {
        printf("Failed tcgetattr\n");
        close(devfd);
        return 1;
    }

    // Change baud rate to 1200.
    if (cfsetispeed(&options, speed) || cfsetospeed(&options, speed))
    {
        printf("Failed cfsetispeed\n");
        close(devfd);
        return 1;
    }

    // Enable hardware flow control.
    options.c_cflag |= CCTS_OFLOW;
    options.c_cflag |= CRTS_IFLOW;

    // Byte size.
    options.c_cflag &= ~CSIZE;
    options.c_cflag |= CS8;

    // Disable parity.
    options.c_cflag &= ~PARENB;
    options.c_cflag &= ~PARODD;
    options.c_iflag &= ~(INPCK | ISTRIP);

    // Stop bit one.
    options.c_cflag &= ~CSTOPB;

    if (tcsetattr(devfd, TCSANOW, &options))
    {
        printf("Failed tcsetattr\n");
        close(devfd);
        return 1;
    }

    // Sleep.
    sleep(1);

    // Close.
    close(devfd);

    printf("Finished due_bootmode\n");
    return 0;
}