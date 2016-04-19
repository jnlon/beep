#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <err.h>

#include <sys/ioctl.h>
#include <dev/wscons/wsconsio.h>

void beep(long delay_ms, int freq_hz) {

  struct wskbd_bell_data wsb;
  int consolefd = open("/dev/console", O_WRONLY);

  wsb.which = WSKBD_BELL_DOALL;
  wsb.pitch = freq_hz;
  wsb.period = delay_ms;
  wsb.volume = 100;
  ioctl(consolefd, WSKBDIO_COMPLEXBELL, &wsb);
  usleep(delay_ms*1000);
}

int main(int argc, char* argv[]) {

  int freq_hz = 400;
  long delay_ms = 100;
  long repititions = 1;

  char ch;
  while ((ch = getopt(argc, argv, "e:f:l:r:d:D:nschv")) != -1) {

    switch ((char)ch) {
      case 'e': printf("e: %s\n", optarg); break;
      case 'f':
        freq_hz = atoi(optarg);
      break;
      case 'l': 
        delay_ms = atol(optarg);
      break;
      case 'r': 
        repititions = atol(optarg);
      break;
      case 'd': printf("d: %s\n", optarg); break;
      case 'D': printf("D: %s\n", optarg); break;
      case 'n': printf("n\n"); break;
      case 's': printf("s\n"); break;
      case 'c': printf("c\n"); break;
      case 'h': printf("h\n"); break;
      case 'v': printf("v\n"); break;
    }
  }

  fflush(stdout);

  int r;
  for (r=0;r<repititions;r++)
    beep(delay_ms, freq_hz);
  
  return 0;
}
