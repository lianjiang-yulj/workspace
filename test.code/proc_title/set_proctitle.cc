// Copyright (c) 2014 Alibaba Inc. All rights reserved.
//
// Author: lianjiang.yulj@taobao.com (Lianjiang Yu)
//
// basic atomic

#include "set_proctitle.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>

extern char** environ;

namespace basic_proc {

char* g_proc_last = NULL;

char** SaveArgv(int32_t argc, char** argv) {
  char** save_argv = new(std::nothrow) char*[argc];
  for (int32_t i = 0; i < argc; ++i) {
    save_argv[i] = ::strdup(argv[i]);
  }

  return save_argv;
}

void InitSetProcTitle(int32_t argc, char** argv) {
  size_t size = 0;
  for (int32_t i = 0; NULL != environ[i]; ++i) {
    size += ::strlen(environ[i]) + 1;
  }

  unsigned char* raw = new(std::nothrow) unsigned char[size];
  /*
  for (int32_t i = 0; NULL != environ[i]; ++i) {
    size_t env_len = ::strlen(environ[i]) + 1;
    ::memcpy(raw, environ[i], env_len); 
    environ[i] = raw;
    raw += env_len;
  }
  */
  
  g_proc_last = argv[0];
  for (int32_t i = 0; NULL != argv[i]; ++i) {
    if (g_proc_last == argv[i]) {
      g_proc_last = argv[i] + ::strlen(argv[i]) + 1;
    }
  }

  for (int32_t i = 0; NULL != environ[i]; ++i) {
    if (g_proc_last == environ[i]) {
      size = ::strlen(environ[i]) + 1;
      g_proc_last = environ[i] + size;
      ::memcpy(raw, (unsigned char*)environ[i], size); 
      environ[i] = (char*)raw;
      raw += size;
    }
  }

  g_proc_last--;
}

void SetProcTitle(char** argv, const std::string& title) {
  argv[1] = NULL;
  char* p = argv[0];
  ::memset(p, 0x00, g_proc_last - p);
  size_t max_len = (size_t)(g_proc_last - p);
  size_t copy_len = title.size() + 1 < max_len ? title.size() + 1 : max_len;
  ::memcpy(p, title.data(), copy_len);
}

}  // namespace basic_proc

