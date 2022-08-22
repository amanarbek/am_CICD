#ifndef SRC_GREP_S21_GREP_H_
#define SRC_GREP_S21_GREP_H_
#include <regex.h>
#include <stdio.h>

union bitmask {
  unsigned int options;
  struct {
    unsigned int e : 1;
    unsigned int f : 1;
    unsigned int i : 1;
    unsigned int v : 1;
    unsigned int c : 1;
    unsigned int l : 1;
    unsigned int n : 1;
    unsigned int h : 1;
    unsigned int s : 1;
    unsigned int o : 1;
    unsigned int first_pattern : 1;
    unsigned int many_files : 1;
    unsigned int regex_on : 1;
    unsigned int mult : 8;
  } bytes;
};

int parsing(union bitmask* options, regex_t* template, char** argv);
int getpatters(union bitmask* options, char** patterns, size_t* size_patterns,
               char** argv);
void addpattern(union bitmask* options, char** patterns, size_t* size_patterns,
                char** argv, char* pattern);
int compare(union bitmask* options, char* buf_out, size_t* size, char** argv,
            size_t* line_num, size_t* needles, int* compare_regex, int i);

#endif  // SRC_GREP_S21_GREP_H_
