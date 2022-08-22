#include "./s21_cat.h"  // Copyright 2022 <Windhelg>

#include <getopt.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char* argv[]) {
  union bitmask options = {0U};
  int flags = 0U;
  const char* short_options = "bevEnstT";
  const struct option long_options[] = {
      {"number-nonblank", no_argument, NULL, 'b'},
      {"number", no_argument, NULL, 'n'},
      {"squeeze-blank", no_argument, NULL, 's'},
      {NULL, 0, NULL, 0}};

  do {
    flags = getopt_long(argc, argv, short_options, long_options, NULL);

    if ('b' == flags) options.bytes.number_nonblank |= 1U;
    if ('E' == flags) options.bytes.show_ends |= 1U;
    if ('n' == flags) options.bytes.number |= 1U;
    if ('s' == flags) options.bytes.squeeze_blank |= 1U;
    if ('T' == flags) options.bytes.show_tabs |= 1U;
    if ('v' == flags) options.bytes.show_nonprinting |= 1U;
    if ('e' == flags) {
      options.bytes.show_ends |= 1U;
      options.bytes.show_nonprinting |= 1U;
    }
    if ('t' == flags) {
      options.bytes.show_nonprinting |= 1U;
      options.bytes.show_tabs |= 1U;
    }
  } while (-1 != flags);
  if (optind < argc) {
    while (optind < argc) {
      parsing(&options, argv);
    }
  }
}

void parsing(union bitmask* options, char** argv) {
  FILE* fp = fopen(argv[optind++], "r");
  if (NULL == fp) {
    printf("s21_cat: %s: No such file or directory\n", argv[optind - 1]);
    return;
  }
  static unsigned int counter = 1;
  char buf;
  while (!feof(fp)) {
    char buf_out[BUFSIZ];
    unsigned int size = 0;
    while (!feof(fp)) {
      buf = fgetc(fp);
      if (buf < 0) break;
      if ('\n' == buf) {
        buf_out[size++] = buf;
        break;
      } else {
        buf_out[size++] = buf;
      }
    }
    if ((options->bytes.squeeze_blank)) {
      if (!(options->bytes.blank) && ('\n' == buf_out[0]))
        options->bytes.blank |= 1U;
      else if (('\n' == buf_out[0]) && (options->bytes.blank &= 1U))
        continue;
      else
        options->bytes.blank &= ~1U;
    }
    if ((options->bytes.number_nonblank) || (options->bytes.number)) {
      if ((options->bytes.number_nonblank) && !('\n' == buf_out[0]) ||
          (options->bytes.number) && !(options->bytes.number_nonblank))
        printf("%6u\t", counter++);
    }
    output(options, buf_out, size);
  }
  fclose(fp);
}

void output(union bitmask* options, char* buf_out, unsigned int size) {
  for (unsigned int i = 0; i < size; ++i) {
    if (options->bytes.show_nonprinting) {
      if (127 == buf_out[i]) {  // DEL
        printf("^?");
        continue;
      }
      if ((options->bytes.show_ends) && (buf_out[i] != '\t') &&
          (buf_out[i] < 32)) {
        if ('\n' == buf_out[i]) {
          printf("$\n");
          continue;
        }
        printf("^%c", buf_out[i] + 64);
      } else if ((options->bytes.show_tabs) && (buf_out[i] != '\n') &&
                 (buf_out[i] < 32)) {
        printf("^%c", buf_out[i] + 64);
      } else if (options->bytes.show_nonprinting && (buf_out[i] != '\t') &&
                 (buf_out[i] != '\n') && (buf_out[i] < 32)) {
        printf("^%c", buf_out[i] + 64);
      } else {
        fputc(buf_out[i], stdout);
      }
    } else if ((options->bytes.show_ends) && ('\n' == buf_out[i])) {
      printf("$\n");
    } else if ((options->bytes.show_tabs) && ('\t' == buf_out[i])) {
      fputs("^I", stdout);
      continue;
    } else {
      fputc(buf_out[i], stdout);
    }
  }
}
