// Copyright (c) 2014 Alibaba Inc. All rights reserved.
//
// Author: lianjiang.yulj@taobao.com (Lianjiang Yu)
//
// basic atomic

#ifndef BASIC_PROC_SET_PROCTITLE_H_
#define BASIC_PROC_SET_PROCTITLE_H_

#include <string>

namespace basic_proc {

extern char* g_proc_last;

// backup argv
char** SaveArgv(int32_t argc, char** argv);

// backup environ
void InitSetProcTitle(int32_t argc, char** argv);

// set proc title
void SetProcTitle(char** argv, const std::string& title);

}  // namespace basic_proc

#endif  // BASIC_PROC_SET_PROCTITLE_H_

