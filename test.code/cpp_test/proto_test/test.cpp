// Copyright (c) 2014 Alibaba Inc. All rights reserved.
//
// Author: lianjiang.yulj@taobao.com (Lianjiang Yu)
//
// test fill_template.h

#include <stdint.h>
#include <stdio.h>
#include <string>

#include <google/protobuf/message.h>
#include <google/protobuf/descriptor.h>

#include "./banner_search_info.pb.h"
#include "./banner_doc_info.pb.h"
#include "./banner_query_info.pb.h"
#include "./wxb_search_info.pb.h"
#include "./wxb_doc_info.pb.h"
#include "./wxb_query_info.pb.h"

#include "fill_template.h"

int32_t main() {

  banner_pb::SearchInfo si;

  bool ret = interface_proto::FillTemplate::FillDocs<banner_pb::SearchInfo, banner_pb::Doc> (&si);
  printf("ret = %d\n", ret);
  ret = interface_proto::FillTemplate::FillQueryParam<banner_pb::SearchInfo, banner_pb::QueryParam, banner_pb::RewriteQueryParam> (&si, "rw_query");
  printf("ret = %d\n", ret);

  /*
  wxb_pb::SearchInfo wsi;

  ret = interface_proto::FillTemplate::FillDocs<wxb_pb::SearchInfo, wxb_pb::Doc> (&wsi);
  printf("ret = %d\n", ret);

  wxb_pb::SearchInfo wsi;
  ret = interface_proto::FillTemplate::FillQueryParam<wxb_pb::SearchInfo, wxb_pb::QueryParam, wxb_pb::RewriteQueryParam> (&wsi, "rw_query");
  printf("ret = %d\n", ret);
  */

  return 0;
}
