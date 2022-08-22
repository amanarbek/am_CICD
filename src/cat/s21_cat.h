#ifndef SRC_CAT_S21_CAT_H_
#define SRC_CAT_S21_CAT_H_

#include <stdio.h>

union bitmask {
  unsigned char options;
  struct {
    unsigned char number_nonblank : 1;
    unsigned char show_ends : 1;
    unsigned char show_nonprinting : 1;
    unsigned char number : 1;
    unsigned char squeeze_blank : 1;
    unsigned char show_tabs : 1;
    unsigned char blank : 1;
  } bytes;
};

void parsing(union bitmask* options, char** argv);
void output(union bitmask* options, char* buf_out, unsigned int size);
#endif  // SRC_CAT_S21_CAT_H_
