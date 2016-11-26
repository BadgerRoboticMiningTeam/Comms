#include <stdio.h>
#include <linux/i2c-dev.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <string.h>
#include <stdint.h>

#define INVERT_ALL          0xBE
#define INVERT_LED0         0xEF
#define INVERT_LED1         0xDE
#define INVERT_LED2         0xAD
#define INVERT_LED3         0xFE


int main(int argc, char *argv[])
{
    int fd;
    int help = 0;
    int i2c_read = 0;
    char *i2c_device = NULL;
    int i2c_address = -1;

    for (int i = 0; i < argc; i++)
    {
        if (strcmp(argv[i], "-d") == 0)
        {
            if (i + 1 < argc)
                i2c_device = argv[i + 1];
        }
        else if (strcmp(argv[i], "-a") == 0)
        {
            if (i + 1 < argc)
            {
                int num_scanned = sscanf(argv[i + 1],"0x%x", &i2c_address);
                if (num_scanned != 1)
                    num_scanned = sscanf(argv[i + 1], "%d", &i2c_address);
                if (num_scanned != 1) // not found
                    i2c_address = -1;
            }
                
        }
        else if (strcmp(argv[i], "--read") == 0)
        {
            i2c_read = 1;
        }
        else if (strcmp(argv[i], "-h") == 0)
        {
            help = 1;
        }
    }

    if (argc < 2 || i2c_address < 0 || help || !i2c_device)
    {
        printf("usage: %s -d /dev/i2c-X -a 0x## [--read] [-h]\n", argv[0]);
        return 1;
    }

    if ((fd = open(i2c_device, O_RDWR)) < 0)
    {
        printf("Failed to open %s!\n", i2c_device);
        return 1;
    }

    if (ioctl(fd, I2C_SLAVE, i2c_address) < 0)
    {
        printf("ioctl failed for the i2c device [slave].\n");
        close(fd);
        return 1;
    }

    if (i2c_read)
    {
        uint8_t buffer[16];
        memset(buffer, 0, 16);

        while (1)
        {
            int bytes_read = read(fd, buffer, 16);
            if (bytes_read < 0)
            {
                printf("read failed!\n");
                break;
            }

            printf("Buffer contents: ");
            for (int i = 0; i < 16; i++)
                printf("0x%x ", buffer[i]);
            memset(buffer, 0, 16);
        }
    }
    else
    {
        uint8_t packets[] = { INVERT_ALL, INVERT_LED0, INVERT_LED1, INVERT_LED2, INVERT_LED3 };

        for (int i = 0; ; i = (i + 1) % 5)
        {
            uint8_t data = packets[i];
            int bytes_written = write(fd, &data, 1);
            printf("[%s] Wrote 0x%x to %s.\n", bytes_written == 1 ? "OK" : "FAIL", data, i2c_device); 
            sleep(1);
        }
    }

    close(fd);
    return 0;
}
