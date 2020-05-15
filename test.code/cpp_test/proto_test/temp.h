#include <iostream>
#include <stdio.h>
#include <string>

//#include "test.pb.h"

template<class NS, class DOC>
class Pb {
 public:
  Pb() {};
  virtual ~Pb() {};
  virtual void parse(NS* pb_);

};

template<class NS, class DOC>
void Pb<NS, DOC>::parse(NS* pb_) {
  std::string index[10];
  index[0] = "AAA";
  index[1] = "BBB";
  index[2] = "CCC";
  
  //NS pb_;

  google::protobuf::Message* message = pb_;

  const google::protobuf::Descriptor *descriptor = message->GetDescriptor();
  const google::protobuf::Reflection *reflection = message->GetReflection();
  const google::protobuf::FieldDescriptor* field = descriptor->FindFieldByName("docs");
  // 判断字段类型是一个msg类型
  if (NULL == field ||
      field->type() != google::protobuf::FieldDescriptor::TYPE_MESSAGE ||
      field->cpp_type() != google::protobuf::FieldDescriptor::CPPTYPE_MESSAGE ||
      !field->is_repeated()) {
    printf("get field docs descriptor error.\n");
    return ;
  }
  printf("docs is repeated.\n");
  for (int i = 0; i < 2; i++) {
    google::protobuf::Message* mess = pb_->add_docs();
    int32_t count = reflection->FieldSize(*message,field);
    printf("field count = %d\n", count);
    //   google::protobuf::Message* mess = (reflection->MutableRepeatedMessage(message, field, count - 1));
    const google::protobuf::Descriptor *fd = mess->GetDescriptor();
    const google::protobuf::Reflection *ref = mess->GetReflection();
    if (NULL == fd) {
      printf("get field desc of docs error.\n");
      return ;
    }
    for(int32_t idx = 0; idx < fd->field_count(); ++idx) {
      const google::protobuf::FieldDescriptor* f = fd->field(idx);
      ref->SetString(mess, f, index[i].c_str());
      //ref->SetString(mess, f, index[i] + f->name() + "中文.jpb");
      printf("field value: %s\n", ref->GetString(*mess, f).c_str());
    }
  }

  char buf[1024];
  std::string ans1;
  bool ret = pb_->SerializeToString(&ans1);
  printf("buf len = %d\n", ans1.length());
  int32_t size = pb_->ByteSize();
  printf("byte size = %d\n", pb_->ByteSize());
  printf("ret = %d\n", ret);
  printf("debug: \n%s\n.", pb_->DebugString().c_str());
  printf("hello\n");
  pb_->Clear();
  ret = pb_->ParseFromString(ans1);
  printf("%s\n", pb_->DebugString().c_str());
  printf("%d\n", ret);
  printf("%s\n", pb_->msg2().c_str());
  printf("%s\n", pb_->msg().c_str());
  for (int i = 0; i < pb_->docs_size();i++) {
    const DOC& doc = pb_->docs(i) ;
    printf("field value %s\n", doc.bid_adid().c_str());
  }


  return;

}
