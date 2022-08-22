#include "./s21_grep.h"  // Copyright 2022 <Windhelg>

#include <getopt.h>
#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char* argv[]) {
  int result = 0;
  union bitmask options = {0};
  regex_t template;
  char* patterns = NULL;
  if (!(patterns = malloc(BUFSIZ * sizeof(char)))) {
    result = 1;
  } else {
    options.bytes.mult++;
    int flags = 0;
    size_t size_patterns = 0;
    do {
      flags = getopt(argc, argv, "e:ivclnhsf:o");
      if ('i' == flags) {
        options.bytes.i |= 1U;
      } else if ('e' == flags) {
        if (!(options.bytes.e)) options.bytes.e |= 1U;
        getpatters(&options, &patterns, &size_patterns, argv);
      } else if ('f' == flags) {
        if (!(options.bytes.f)) options.bytes.f |= 1U;
        getpatters(&options, &patterns, &size_patterns, argv);
      } else if ('o' == flags && !(options.bytes.o)) {
        options.bytes.o |= 1U;
      } else if ('v' == flags) {
        options.bytes.v |= 1U;
      } else if ('c' == flags) {
        options.bytes.c |= 1U;
      } else if ('l' == flags) {
        options.bytes.l |= 1U;
      } else if ('n' == flags) {
        options.bytes.n |= 1U;
      } else if ('h' == flags) {
        options.bytes.h |= 1U;
      } else if ('s' == flags) {
        options.bytes.s |= 1U;
      }
    } while (-1 != flags);
    if (optind < argc) {
      if (!options.bytes.e && !options.bytes.f)
        getpatters(&options, &patterns, &size_patterns, argv);
      if (options.bytes.i) {
        if (!(regcomp(&template, patterns, REG_ICASE)))
          options.bytes.regex_on |= 1U;
      } else {
        if (!(regcomp(&template, patterns, REG_EXTENDED)))
          options.bytes.regex_on |= 1U;
      }
      if ((argc - optind) > 1) options.bytes.many_files |= 1U;
      while (optind < argc) {
        parsing(&options, &template, argv);
      }
    }
    if (patterns) {
      free(patterns);
    }
    if (options.bytes.regex_on) regfree(&template);
    result = 0;
  }
  return result;
}

int parsing(union bitmask* options, regex_t* template, char** argv) {
  int result = 0;
  FILE* fp = fopen(argv[optind++], "r");
  if (NULL == fp) {
    if (options->bytes.s)
      result = 0;
    else
      printf("s21_grep: %s: No such file or directory\n", argv[optind - 1]);
  } else {
    char buf[BUFSIZ];
    char buf_out[BUFSIZ];
    size_t line_num = 1;
    size_t needles = 0;
    while (NULL != fgets(buf, BUFSIZ, fp)) {
      char* pbuf = buf;
      char* pbuf_out = buf_out;
      size_t size_buf = strlen(pbuf);
      size_t size_buf_out = 0;
      if (options->bytes.o) {
        for (int i = 0;; ++i) {
          regmatch_t pmatch;
          int compare_regex = regexec(template, pbuf, 1, &pmatch, 0);
          result = compare(options, buf_out, &size_buf_out, argv, &line_num,
                           &needles, &compare_regex, i);
          if (1 == result) {
            printf("%s\n", argv[optind - 1]);
            break;
          }
          if (options->bytes.c) break;
          if (options->bytes.v && !compare_regex) {
            result = 1;
            break;
          }
          if (compare_regex) break;
          size_t len = pmatch.rm_eo - pmatch.rm_so;
          snprintf(buf_out + size_buf_out, len + 1, "%s", pbuf + pmatch.rm_so);
          pbuf += pmatch.rm_eo;
          if (!options->bytes.c) printf("%s\n", buf_out);
          buf_out[0] = '\0';
          size_buf_out = 0;
        }
      } else {
        int compare_regex = regexec(template, buf, 0, 0, 0);
        result = compare(options, pbuf_out, &size_buf_out, argv, &line_num,
                         &needles, &compare_regex, 0);
        if (1 == result) {
          printf("%s\n", argv[optind - 1]);
          break;
        }
        if (options->bytes.c) continue;
        if (!compare_regex)
          size_buf_out +=
              snprintf(pbuf_out + size_buf_out, size_buf + 1, "%s", pbuf);
        else
          continue;
      }
      if (1 == result) break;
      if (options->bytes.o) continue;
      if ('\n' == buf_out[size_buf_out - 1]) buf_out[size_buf_out - 1] = '\0';
      if (size_buf_out > 0) {
        printf("%s\n", buf_out);
        buf_out[0] = '\0';
      }
    }
    if (1 != result) {
      if ((options->bytes.many_files) && !(options->bytes.h) &&
          !(options->bytes.l) && (options->bytes.c))
        printf("%s:%zu\n", argv[optind - 1], needles);
      else if (options->bytes.c && !(options->bytes.l))
        printf("%zu\n", needles);
    }
    fclose(fp);
  }
  return result;
}

int getpatters(union bitmask* options, char** patterns, size_t* size_patterns,
               char** argv) {
  int result = 0;
  if (options->bytes.f) {
    FILE* fp = fopen(optarg, "r");
    if ((NULL == fp)) {
      printf("s21_grep: %s: No such file or directory\n", optarg);
      result = 1;
    } else {
      char buf[BUFSIZ];
      while (NULL != fgets(buf, BUFSIZ, fp)) {
        size_t local_size = strlen(buf);
        if ('\n' == buf[0]) continue;
        if ('\n' == buf[local_size - 1]) {
          buf[local_size - 1] = '\0';
          local_size--;
        }
        addpattern(options, patterns, size_patterns, argv, buf);
      }
      fclose(fp);
    }
  } else if (options->bytes.e) {
    addpattern(options, patterns, size_patterns, argv, optarg);
  } else {
    addpattern(options, patterns, size_patterns, argv, argv[optind++]);
  }
  return result;
}

void addpattern(union bitmask* options, char** patterns, size_t* size_patterns,
                char** argv, char* pattern) {
  size_t size = strlen(pattern);
  *size_patterns += size;
  if (*size_patterns > BUFSIZ * options->bytes.mult) {
    *patterns = realloc(*patterns, BUFSIZ);
    options->bytes.mult++;
  }
  if (options->bytes.first_pattern) {
    (*patterns)[*size_patterns - size] = '|';
    (*size_patterns)++;
  }
  snprintf(*patterns + (*size_patterns - size), size + 1, "%s", pattern);
  if (!options->bytes.first_pattern) options->bytes.first_pattern |= 1U;
}

int compare(union bitmask* options, char* pbuf_out, size_t* size_buf_out,
            char** argv, size_t* line_num, size_t* needles, int* compare_regex,
            int i) {
  int result = 0;
  if (options->bytes.v) {
    if (*compare_regex > 0)
      --(*compare_regex);
    else
      ++(*compare_regex);
  }
  if (options->bytes.l && !(*compare_regex)) result = 1;
  if (options->bytes.c && !(*compare_regex)) (*needles)++;
  if ((options->bytes.many_files) && !(options->bytes.h)) {
    int len = strlen(argv[optind - 1]);
    *size_buf_out =
        snprintf(pbuf_out + *size_buf_out, len + 2, "%s:", argv[optind - 1]);
  }
  if ((options->bytes.n) && !(options->bytes.c)) {
    size_t len = 1;
    size_t copy_line = *line_num;
    while (copy_line >= 10) {
      len++;
      copy_line = copy_line / 10;
    }
    if (i > 0) {
      snprintf(pbuf_out + *size_buf_out, len + 2, "%zu:", *line_num - 1);
      *size_buf_out += len + 1;
    } else {
      snprintf(pbuf_out + *size_buf_out, len + 2, "%zu:", (*line_num)++);
      *size_buf_out += len + 1;
    }
  }
  return result;
}
