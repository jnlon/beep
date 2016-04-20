#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <err.h>

#include <sys/ioctl.h>
#include <dev/wscons/wsconsio.h>

// Default beep configuration
#define FREQUENCY 400
#define DURATION 200
#define DELAY 0
#define REPITITIONS 1
#define DELAY_LAST (-1)

typedef struct beep_args_s {
  int freq_hz;
  long duration_ms;
  long delay_ms;
  int repititions;
  int delay_on_last_beep;
} beep_args_t;

void reset_args(beep_args_t *args) {
  args->freq_hz = FREQUENCY;
  args->duration_ms = DURATION;
  args->delay_ms = DELAY;
  args->repititions = REPITITIONS;
  args->delay_on_last_beep = DELAY_LAST;
}

void beep(long duration_ms, int freq_hz, int consolefd) {

  struct wskbd_bell_data wsb;

  wsb.which = WSKBD_BELL_DOALL;
  wsb.pitch = freq_hz;
  wsb.period = duration_ms;
  wsb.volume = 100;

  ioctl(consolefd, WSKBDIO_COMPLEXBELL, &wsb);

  usleep(duration_ms*1000);
}

void beep_run(beep_args_t args, int consolefd) {

  int r;
  for (r=0;r<args.repititions;r++) {

    beep(args.duration_ms, args.freq_hz, consolefd);

    if (args.delay_on_last_beep == -1 && r == args.repititions-1) 
      break;

    usleep(args.delay_ms*1000);
  }
}

int get_beep_args(int argc, char *argv[], beep_args_t *args) {

  int args_left = argc;

  char ch;
  while ((ch = getopt(argc, argv, "e:f:l:r:d:D:nschv")) != -1) {

    //printf("argc: %d\n", argc);

    switch ((char)ch) {
      case 'e': printf("e: %s\n", optarg); break;
      case 'f':
        args->freq_hz = atoi(optarg);
      break;
      case 'l': 
        args->duration_ms = atol(optarg);
      break;
      case 'r': 
        args->repititions = atol(optarg);
      break;
      case 'd': 
        args->delay_ms = atol(optarg);
      break;
      case 'D': 
        args->delay_ms = atol(optarg); 
        args->delay_on_last_beep = 0; 
      break;
      case 'n':  
        return args_left;
      break;
      case 's': printf("s\n"); break;
      case 'c': printf("c\n"); break;
      case 'h': printf("h\n"); break;
      case 'v': printf("v\n"); break;
    }
    args_left -= 1;
  }
  return 0;
}

int main(int argc, char *argv[]) {

  beep_args_t args;

  reset_args(&args);

  int consolefd = open("/dev/console", O_WRONLY);

  int args_left = argc;

  while (args_left > 0) {
    args_left = get_beep_args(argc, argv, &args);
    beep_run(args, consolefd);
    reset_args(&args);
  }

  close(consolefd);
  return 0;
}
