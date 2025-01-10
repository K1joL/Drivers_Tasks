#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <time.h>
#include <unistd.h>

#define DEVICE_FILE "/dev/mytest"
#define NUM_OF_REACTIONS 500

// ioctl command
#define MYTEST_GET_REACTIONS _IOR('m', 0, time_t *)
#define MYTEST_GET_COUNT _IOR('m', 1, int *)

int main() {
  int fd, i;
  char buffer[1024];
  ssize_t bytes_read;
  long long *reaction_times;
  int num_times = 0;
  int reaction_count;

  fd = open(DEVICE_FILE, O_RDWR);
  if (fd < 0) {
    perror("Failed to open device");
    return 1;
  }
  int try_count = 0;
  i = 0;
  while (i < NUM_OF_REACTIONS) {
    bytes_read = read(fd, buffer, 1);
    if (bytes_read <= 0) {
      try_count++;
      if (try_count % 100 == 0) {
        // printf("No event available\n");
        try_count = 0;
      }
      usleep(1000);
      continue;
    }
    i++;
    // send reaction
    int dummy_id = 0;
    if (write(fd, &dummy_id, sizeof(int)) == -1) {
      perror("Error while sending response");
    }
      // printf("Event %d received\n", i);
  }
  // Get number of reaction times via ioctl
  if (ioctl(fd, MYTEST_GET_COUNT, &reaction_count) == -1) {
    perror("ioctl MYTEST_GET_COUNT failed");
    close(fd);
    return 1;
  }

  printf("Number of reactions is: %d\n", reaction_count);

  // Allocate memory for reactions
  reaction_times = (long long *)malloc(sizeof(long long) * reaction_count);
  if (reaction_times == NULL) {
    perror("Error while allocating memory");
    close(fd);
    return 1;
  }

  // Get reaction times via ioctl
  if (ioctl(fd, MYTEST_GET_REACTIONS, reaction_times) == -1) {
    perror("ioctl MYTEST_GET_REACTIONS failed");
    free(reaction_times);
    close(fd);
    return 1;
  }

  printf("Recorded %d reaction times:\n", reaction_count);
  for (i = 0; i < reaction_count; ++i) {
    printf("Reaction time %d: %lld ns\n", i, reaction_times[i]);
  }
  free(reaction_times);

  close(fd);
  return 0;
}