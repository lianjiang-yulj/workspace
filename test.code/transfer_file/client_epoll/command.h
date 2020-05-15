#ifndef _COMMAND_H_
#define _COMMAND_H_

#include "common.h"

void print_usage(char *prog_name);
int parse_cmd_line(int argc, char * const argv[], cmdline_param* cp);

#endif
