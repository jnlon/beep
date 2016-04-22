/* A simple implementation of the beep command using wscons */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <getopt.h>

#include <sys/ioctl.h>
#include <dev/wscons/wsconsio.h>

#define VERSION_STRING "beep_for_bsd 0.1\n"

// Default beep configuration
#define FREQUENCY 400     // 400ms
#define DURATION 200      // 200ms
#define DELAY 0           // no delay between beeps
#define REPITITIONS 1     // beep once
#define DELAY_LAST (-1)   // no
#define INPUT_STREAM (-1) // no

// Different input modes
#define INPUT_STREAM_CHAR 0
#define INPUT_STREAM_LINE 1

// Console file descriptor
int consolefd;

// Beep configuration
typedef struct beep_args_s {
  int freq_hz;
  long duration_ms;
  long delay_ms;
  int repititions;
  int delay_on_last_beep;
  int input_stream;
} beep_args_t;

// Structure for getopt_long
static struct option longopts[] =
{
    { "new", no_argument, NULL, 'n'},
    { "version", no_argument, NULL, 'v'},
    { "help", no_argument, NULL, 'h'}
};

// Close the console and exit
void exit_cleanly(int code) {
  fflush(stdout);
  close(consolefd);
  exit(code);
}

void print_help_exit(int code) {
  printf("Usage:\n");
  printf("beep [-f freq] [-l length] [-r reps] [-d delay] ");
  printf("[-D delay] [-s] [-c]\n");
  printf("beep [options...] [-n] [--new] [options...] ...\n");
  printf("beep [-h] [--help]\n");
  printf("beep [-v] [-V] [--version]\n");

  exit_cleanly(code);
}

void print_version_exit(int code) {
  printf(VERSION_STRING);
  exit_cleanly(code);
}

// Set beep arguments to defaults
void reset_args(beep_args_t *args) {
  args->freq_hz = FREQUENCY;
  args->duration_ms = DURATION;
  args->delay_ms = DELAY;
  args->repititions = REPITITIONS;
  args->delay_on_last_beep = DELAY_LAST;
  args->input_stream = INPUT_STREAM;
}

// Actually do the beeping
void beep(long duration_ms, int freq_hz, int consolefd) {

  struct wskbd_bell_data wsb;

  wsb.which = WSKBD_BELL_DOALL;
  wsb.pitch = freq_hz;
  wsb.period = duration_ms;
  wsb.volume = 100;

  ioctl(consolefd, WSKBDIO_COMPLEXBELL, &wsb);

  usleep(duration_ms*1000);
}

// Send a short beep to stop the speaker, then exit
void interrupt_handler(int signal) {
  beep(0, 0, consolefd);
  exit_cleanly(2);
}

// Beep for every char on stdin
void beep_on_char(beep_args_t *args, int consolefd) {

  while (1) {

    char c = getchar();

    if (feof(stdin))
      break;

    putchar(c);
    fflush(stdout);

    beep(args->duration_ms, args->freq_hz, consolefd);
    usleep(args->delay_ms*1000);

  }
}

// Beep for every line on stdin
void beep_on_line(beep_args_t *args, int consolefd) {
  
  while (1) {

    char c = getchar();

    if (feof(stdin))
      break;

    putchar(c);
    fflush(stdout);

    if (c == '\n') {
      beep(args->duration_ms, args->freq_hz, consolefd);
      usleep(args->delay_ms*1000);
    }
  }
}

// Decide how to beep based on config
void beep_run(beep_args_t *args, int consolefd) {

  // -s
  if (args->input_stream == INPUT_STREAM_LINE) {
    beep_on_line(args, consolefd);
    return;
  }

  // -c
  if (args->input_stream == INPUT_STREAM_CHAR) {
    beep_on_char(args, consolefd);
    return;
  }

  // Everything else
  int r;
  for (r=0;r<args->repititions;r++) {

    beep(args->duration_ms, args->freq_hz, consolefd);

    if (args->delay_on_last_beep == -1 && r == args->repititions-1) 
      break;

    usleep(args->delay_ms*1000);
  }
}

// Process commandline, return the number of arguments left
int get_beep_args(int argc, char *argv[], beep_args_t *args) {

  int args_left = argc;

  char ch;
  while ((ch = getopt_long(argc, argv, "e:f:l:r:d:D:nschvV", longopts, NULL)) != -1) {

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
      case 's': args->input_stream = INPUT_STREAM_LINE; 
      break;
      case 'c': args->input_stream = INPUT_STREAM_CHAR; 
      break;
      case 'h': print_help_exit(0); break;
      case 'v': print_version_exit(0); break;
      case 'V': print_version_exit(0); break;
      case '?': print_help_exit(1); break;
    }
    args_left -= 1;
  }
  return 0;
}

int main(int argc, char *argv[]) {

  beep_args_t args;
  reset_args(&args);
  int args_left = argc;

  // Stop beeping on process termination 
  signal(SIGINT, interrupt_handler);
  signal(SIGTERM, interrupt_handler);

  consolefd = open("/dev/console", O_WRONLY);

  while (args_left > 0) {
    args_left = get_beep_args(argc, argv, &args);
    beep_run(&args, consolefd);
    reset_args(&args);
  }

  close(consolefd);
  return 0;
}
