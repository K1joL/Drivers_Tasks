#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/ioctl.h>
#include <errno.h>

#define DEVICE_NAME "/dev/stariy_driver"

#define IOCTL_RESET_BUFFER _IO('a', 1)
#define IOCTL_SET_SIZE _IOW('a', 2, int)

#define BUFFER_SIZE 1024

void print_buffer(int fd, size_t size) {
  char* read_buf = (char*)malloc(size);
  if (read_buf == NULL) {
        perror("Failed to allocate read buffer");
        return;
    }

  ssize_t bytes_read = read(fd, read_buf, size);

    if (bytes_read == -1) {
        perror("Failed to read from device");
        free(read_buf);
        return;
    }
    printf("Buffer content:\n");
    for (size_t i = 0; i < bytes_read; ++i) {
        printf("%c", read_buf[i]);
    }
    printf("\n");
    free(read_buf);
}


int main() {
    int fd;
    char write_buf[BUFFER_SIZE] = "Hello from user space!";
    char read_buf[BUFFER_SIZE];
    int new_size;
    int current_buffer_size = BUFFER_SIZE;


    // Открываем устройство
    fd = open(DEVICE_NAME, O_RDWR);
    if (fd < 0) {
        perror("Failed to open device");
        return EXIT_FAILURE;
    }

    printf("Device opened successfully.\n");

    // Чтение буфера
    printf("Initial buffer:\n");
    print_buffer(fd, current_buffer_size);

    // Записываем данные в устройство
    if (strlen(write_buf) > current_buffer_size) {
         printf("Not enough space in buffer. Not writing.\n");
     } else {
        ssize_t bytes_written = write(fd, write_buf, strlen(write_buf));
      if (bytes_written < 0) {
        perror("Failed to write to device");
         close(fd);
        return EXIT_FAILURE;
      }
        printf("Written %zd bytes to device.\n", bytes_written);
    }

    // Чтение буфера
    printf("Buffer after writing:\n");
    print_buffer(fd, current_buffer_size);

    // Вызываем IOCTL_RESET_BUFFER
    if (ioctl(fd, IOCTL_RESET_BUFFER) < 0) {
        perror("ioctl (reset buffer) failed");
         close(fd);
        return EXIT_FAILURE;
      }
    printf("Buffer reset with ioctl.\n");

    // Чтение буфера
    printf("Buffer after reset:\n");
    print_buffer(fd, current_buffer_size);

    // Записываем новые данные
    strcpy(write_buf, "New data after reset");
     if (strlen(write_buf) > current_buffer_size) {
           printf("Not enough space in buffer. Not writing.\n");
        } else {
            ssize_t bytes_written = write(fd, write_buf, strlen(write_buf));
         if (bytes_written < 0) {
            perror("Failed to write to device after reset");
            close(fd);
            return EXIT_FAILURE;
         }
        printf("Written %zd bytes after reset.\n", bytes_written);
      }
      // Чтение буфера
    printf("Buffer after writing after reset:\n");
    print_buffer(fd, current_buffer_size);

    // Вызываем IOCTL_SET_SIZE
    new_size = 2048;
    if (ioctl(fd, IOCTL_SET_SIZE, new_size) < 0) {
       perror("ioctl (set size) failed");
       close(fd);
        return EXIT_FAILURE;
      }
    printf("Buffer size changed to %d.\n", new_size);
    current_buffer_size = new_size;
    // Чтение буфера
    printf("Buffer after resize:\n");
    print_buffer(fd, current_buffer_size);

    // Пытаемся записать в новый буфер.
    strcpy(write_buf, "Data in new resized buffer");
    if (strlen(write_buf) > current_buffer_size) {
       printf("Not enough space in buffer. Not writing.\n");
    } else {
          ssize_t bytes_written = write(fd, write_buf, strlen(write_buf));
        if (bytes_written < 0) {
          perror("Failed to write to resized buffer");
            close(fd);
           return EXIT_FAILURE;
        }
     printf("Written %zd bytes to resized buffer.\n", bytes_written);
   }

    // Чтение буфера
    printf("Buffer after writing to resized buffer:\n");
    print_buffer(fd, current_buffer_size);

    // Закрываем устройство
    close(fd);
    printf("Device closed.\n");

    return EXIT_SUCCESS;
}