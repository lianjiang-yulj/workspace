#include "command.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <getopt.h>

/** 
 * ÃüÁîÐÐ°ïÖú
  */ 
void print_usage(char *prog_name)
{   
	fprintf(stderr, "%s -h host -p port -r request_thread_cnt -c conn_cnt -w write_cnt -o filename\n"
			"    -H, --host              server ip\n"
			"    -p, --port              server port\n"
			"    -r, --request_cnt       request thread count\n"
			"    -c, --conn_cnt          connection thread count\n"
			"    -w, --write_cnt         write thread count\n"
			"    -o, --file              output filename\n"
			"    -h, --help              display this help and exit\n"
			"eg: %s -h localhost -p 5000 -c 2 -w 3 -o file\n\n", prog_name, prog_name);
}

/**
 * ½âÎöÃüÁîÐÐ
 */
int parse_cmd_line(int argc, char * const argv[], cmdline_param *cp)
{
	int                     opt;
	const char              *opt_string = "H:p:r:c:w:o:h";
	struct option           long_opts[] = {
		{"host", 1, NULL, 'H'},
		{"port", 1, NULL, 'p'},
		{"request_cnt", 1, NULL, 'r'},
		{"conn_cnt", 1, NULL, 'c'},
		{"write_cnt", 1, NULL, 'w'},
		{"file", 1, NULL, 'o'},
		{"help", 0, NULL, 'h'},
		{0, 0, 0, 0}
	};  

	opterr = 0;

	while ((opt = getopt_long(argc, argv, opt_string, long_opts, NULL)) != -1) {
		switch (opt) {
			case 'H':
				cp->host = strdup(optarg);
				break;
			case 'p':
				cp->port = atoi(optarg);
				break;
			case 'r':
				cp->request_thread_cnt = MIN(atoi(optarg), MAX_THREAD_CNT);
				break;
			case 'c':
				cp->conn_thread_cnt = MIN(atoi(optarg), MAX_THREAD_CNT);
				break;
			case 'w':
				cp->write_thread_cnt = MIN(atoi(optarg), MAX_THREAD_CNT);
				break;
			case 'o':
				cp->filename = strdup(optarg);
				break;
			case 'h':
			default:
				print_usage(argv[0]);
				break;
		}
	}
	if (!cp->host || !cp->filename) {
		print_usage(argv[0]);
		return 1;
	}

	return 0;
}

