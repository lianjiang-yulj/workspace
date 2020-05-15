#protoc test.proto --cpp_out=.
g++ -g -ggdb -O0 -o test test.cpp wxb_doc_info.pb.cc wxb_query_info.pb.cc wxb_search_info.pb.cc banner_doc_info.pb.cc banner_query_info.pb.cc banner_search_info.pb.cc -L/usr/local/lib/ -lprotobuf -I/usr/local/include -I../context -I../../../cban_imatch2_cn_4_2_1/
