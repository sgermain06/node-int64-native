#ifndef INT64_H
#define INT64_H

#include <node.h>
#include <v8.h>
#include <node_object_wrap.h>

using namespace node;
using namespace v8;

class Int64 : public ObjectWrap {
 public:
  static void Init(Handle<v8::Object> exports);

 private:
  Int64();
  Int64(const Local<Number>& n);
  Int64(const Local<Number>& hi, const Local<Number>& lo);
  Int64(const Local<String>& s);
  ~Int64();

  static Persistent<Function> constructor;
  static void New(const v8::FunctionCallbackInfo<Value>& args);
  static void ToNumber(const v8::FunctionCallbackInfo<Value>& args);
  static void ValueOf(const v8::FunctionCallbackInfo<Value>& args);
  static void ToString(const v8::FunctionCallbackInfo<Value>& args);
  static void ToUnsignedDecimalString(const v8::FunctionCallbackInfo<Value>& args);
  static void ToSignedDecimalString(const v8::FunctionCallbackInfo<Value>& args);
  static void Equals(const v8::FunctionCallbackInfo<Value>& args);
  static void Compare(const v8::FunctionCallbackInfo<Value>& args);
  static void High32(const v8::FunctionCallbackInfo<Value>& args);
  static void Low32(const v8::FunctionCallbackInfo<Value>& args);
  static void ShiftLeft(const v8::FunctionCallbackInfo<Value>& args);
  static void ShiftRight(const v8::FunctionCallbackInfo<Value>& args);
  static void And(const v8::FunctionCallbackInfo<Value>& args);
  static void Or(const v8::FunctionCallbackInfo<Value>& args);
  static void Xor(const v8::FunctionCallbackInfo<Value>& args);
  static void Add(const v8::FunctionCallbackInfo<Value>& args);
  static void Sub(const v8::FunctionCallbackInfo<Value>& args);

  uint64_t mValue;
};

#endif
