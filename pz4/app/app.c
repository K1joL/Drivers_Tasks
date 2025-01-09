#include <stdio.h>

#include <stdlib.h>

#include <fcntl.h>

#include <unistd.h>

#include <string.h>

#include <errno.h>

#include <stdint.h>



#define DEVICE_NAME "/dev/my_char_dev"

#define SYSFS_START "/sys/class/new_dev_1/my_char_dev/start_timer"

#define SYSFS_STOP "/sys/class/new_dev_1/my_char_dev/stop_timer"

#define SYSFS_RESET "/sys/class/new_dev_1/my_char_dev/reset_var"



int read_global_var() {

    int fd;

    int value;

    fd = open(DEVICE_NAME, O_RDONLY);

    if (fd < 0) {

        perror("Failed to open device for reading");

        return -1;

    }

    if (read(fd, &value, sizeof(value)) != sizeof(value)) {

       perror("Failed to read from device");

        close(fd);

        return -1;

    }

   close(fd);

    return value;

}





int write_global_var(int value) {

    int fd;

    fd = open(DEVICE_NAME, O_WRONLY);

    if (fd < 0) {

      perror("Failed to open device for writing");

       return -1;

      }

   if (write(fd, &value, sizeof(value)) != sizeof(value)) {

       perror("Failed to write to device");

        close(fd);

       return -1;

     }

   close(fd);

  return 0;

}

int write_sysfs(const char *path, const char *value) {

    int fd;

    fd = open(path, O_WRONLY);

    if (fd < 0) {

        perror("Failed to open sysfs file for writing");

        return -1;

    }

   if (write(fd, value, strlen(value)) == -1) {

       perror("Failed to write to sysfs file");

       close(fd);

        return -1;

   }

  close(fd);

   return 0;

}



int main() {

    int value;



    printf("Initial value of global_var: %d\n", read_global_var());



    printf("Setting global_var to 5\n");

   if (write_global_var(5) == -1) {

      return EXIT_FAILURE;

    }

    printf("Current value of global_var: %d\n", read_global_var());



    printf("Starting timer\n");

   if (write_sysfs(SYSFS_START, "1") == -1) {

      return EXIT_FAILURE;

    }

   sleep(5);

   printf("Current value of global_var: %d\n", read_global_var());



    printf("Stopping timer\n");

   if (write_sysfs(SYSFS_STOP, "1") == -1) {

        return EXIT_FAILURE;

   }



    printf("Resetting global_var\n");

    if (write_sysfs(SYSFS_RESET, "1") == -1) {

     return EXIT_FAILURE;

    }

    printf("Current value of global_var: %d\n", read_global_var());

    printf("Setting global_var to 10\n");

    if (write_global_var(10) == -1) {

       return EXIT_FAILURE;

    }

     printf("Current value of global_var: %d\n", read_global_var());

    return EXIT_SUCCESS;

}