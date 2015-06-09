#ifndef BUILDING_NODE_EXTENSION
#define BUILDING_NODE_EXTENSION
#endif

#include <node.h>
#include <v8.h>

#include <iomanip>
#include <limits>
#include <sstream>

#include "Int64.h"

using namespace node;
using namespace std;
using namespace v8;

Persistent<Function> Int64::constructor;

void Int64::Init(Handle<Object> exports) {
  
  Isolate *isolate = Isolate::GetCurrent();

  Local<FunctionTemplate> tpl = FunctionTemplate::New(isolate, New);
  tpl->SetClassName(v8::String::NewFromUtf8(isolate, "Int64"));
  tpl->InstanceTemplate()->SetInternalFieldCount(1);

  NODE_SET_PROTOTYPE_METHOD(tpl, "toNumber", ToNumber);
  NODE_SET_PROTOTYPE_METHOD(tpl, "valueOf", ValueOf);
  NODE_SET_PROTOTYPE_METHOD(tpl, "toString", ToString);
  NODE_SET_PROTOTYPE_METHOD(tpl, "toUnsignedDecimalString", ToUnsignedDecimalString);
  NODE_SET_PROTOTYPE_METHOD(tpl, "toSignedDecimalString", ToSignedDecimalString);
  NODE_SET_PROTOTYPE_METHOD(tpl, "equals", Equals);
  NODE_SET_PROTOTYPE_METHOD(tpl, "compare", Compare);
  NODE_SET_PROTOTYPE_METHOD(tpl, "high32", High32);
  NODE_SET_PROTOTYPE_METHOD(tpl, "low32", Low32);
  NODE_SET_PROTOTYPE_METHOD(tpl, "shiftLeft", ShiftLeft);
  NODE_SET_PROTOTYPE_METHOD(tpl, "shiftRight", ShiftRight);
  NODE_SET_PROTOTYPE_METHOD(tpl, "and", And);
  NODE_SET_PROTOTYPE_METHOD(tpl, "or", Or);
  NODE_SET_PROTOTYPE_METHOD(tpl, "xor", Xor);
  NODE_SET_PROTOTYPE_METHOD(tpl, "add", Add);
  NODE_SET_PROTOTYPE_METHOD(tpl, "sub", Sub);

  Local<Function> tplFunction = tpl->GetFunction();
  constructor.Reset(isolate, tplFunction);

  exports->Set(v8::String::NewFromUtf8(isolate, "Int64"), tpl->GetFunction());
}

Int64::Int64() {
  mValue = 0;
}

Int64::Int64(const Local<Number>& n) {
  mValue = static_cast<uint64_t>(n->NumberValue());
}

Int64::Int64(const Local<Number>& hi, const Local<Number>& lo) {
  uint32_t highBits = static_cast<uint32_t>(hi->NumberValue());
  uint32_t lowBits = static_cast<uint32_t>(lo->NumberValue());
  mValue =
    (static_cast<uint64_t>(highBits) << 32) |
    (static_cast<uint64_t>(lowBits));
}

Int64::Int64(const Local<String>& s) {
  String::Utf8Value utf8(s);
  stringstream ss;
  char* ps = *utf8;
  if (utf8.length() > 2 && ps[0] == '0' && ps[1] == 'x') {
    ss << hex << (ps + 2);
  } else {
    ss << ps;
  }
  ss >> mValue;
}

Int64::~Int64() {}

void Int64::New(const v8::FunctionCallbackInfo<Value>& args) {
  
  Isolate *isolate = Isolate::GetCurrent();

  Int64* obj = NULL;

  if (args.Length() == 0) {
    obj = new Int64();
  }
  else if (args.Length() == 1) {

    if (args[0]->IsNumber()) {

      obj = new Int64(args[0]->ToNumber());
    } 
    else if (args[0]->IsString()) {

      obj = new Int64(args[0]->ToString());
    }
  } 
  else if (args.Length() == 2) {

    if (args[0]->IsNumber() && args[1]->IsNumber()) {

      obj = new Int64(args[0]->ToNumber(), args[1]->ToNumber());
    }
  }

  if (obj == NULL) {

    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "Wrong arguments")));
    args.GetReturnValue().SetUndefined();
  }
  
  obj->Wrap(args.This());
  args.GetReturnValue().Set(args.This());
}

void Int64::ToNumber(const v8::FunctionCallbackInfo<Value>& args) {

  Isolate *isolate = args.GetReturnValue().GetIsolate();

  Int64* obj = ObjectWrap::Unwrap<Int64>(args.This());

  if (obj->mValue >= 1ull << 53) {

    args.GetReturnValue().Set(Number::New(isolate, numeric_limits<double>::infinity()));
  }
  else {

    args.GetReturnValue().Set(Number::New(isolate, static_cast<double>(obj->mValue)));
  }
}

void Int64::ValueOf(const v8::FunctionCallbackInfo<Value>& args) {
  ToNumber(args);
}

void Int64::ToString(const v8::FunctionCallbackInfo<Value>& args) {

  Isolate *isolate = args.GetReturnValue().GetIsolate();
  HandleScope scope(isolate);
  Int64* obj = ObjectWrap::Unwrap<Int64>(args.This());
  
  std::ostringstream o;
  o << "0x" << hex << setfill('0') << setw(16) << obj->mValue;

  Local<String> returnObj = String::NewFromUtf8(isolate, o.str().c_str());

  args.GetReturnValue().Set(returnObj);
}

void Int64::ToUnsignedDecimalString(const v8::FunctionCallbackInfo<Value>& args) {

  Isolate *isolate = args.GetReturnValue().GetIsolate();
  HandleScope scope(isolate);

  Int64* obj = ObjectWrap::Unwrap<Int64>(args.This());

  std::ostringstream o;
  o << obj->mValue;

  Local<String> returnObj = String::NewFromUtf8(isolate, o.str().c_str());

  args.GetReturnValue().Set(returnObj);
}

void Int64::ToSignedDecimalString(const v8::FunctionCallbackInfo<Value>& args) {

  Isolate *isolate = args.GetReturnValue().GetIsolate();
  HandleScope scope(isolate);

  Int64* obj = ObjectWrap::Unwrap<Int64>(args.This());

  std::ostringstream o;
  o << (static_cast<int64_t>(obj->mValue));

  Local<String> returnObj = String::NewFromUtf8(isolate, o.str().c_str());

  args.GetReturnValue().Set(returnObj);
}

void Int64::Equals(const v8::FunctionCallbackInfo<Value>& args) {

  Isolate *isolate = args.GetReturnValue().GetIsolate();
  HandleScope scope(isolate);

  if (args.Length() < 1) {
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "Argument required")));
    args.GetReturnValue().SetUndefined();
  }
  else if (!args[0]->IsObject()) {
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "Object expected")));
    args.GetReturnValue().SetUndefined();
  }
  else {
    Int64* obj = ObjectWrap::Unwrap<Int64>(args.This());
    Int64* otherObj = ObjectWrap::Unwrap<Int64>(args[0]->ToObject());

    bool isEqual = obj->mValue == otherObj->mValue;

    args.GetReturnValue().Set(Boolean::New(isolate, isEqual));
  }
}

void Int64::Compare(const v8::FunctionCallbackInfo<Value>& args) {
  Isolate *isolate = args.GetReturnValue().GetIsolate();
  HandleScope scope(isolate);
  if (args.Length() < 1) {
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "Argument required")));
    args.GetReturnValue().SetUndefined();
  }
  else if (!args[0]->IsObject()) {
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "Object expected")));
    args.GetReturnValue().SetUndefined();
  }
  else {
    Int64* obj = ObjectWrap::Unwrap<Int64>(args.This());
    Int64* otherObj = ObjectWrap::Unwrap<Int64>(args[0]->ToObject());
    int32_t cmp = 0;
    if (obj->mValue < otherObj->mValue) {
      cmp = -1;
    }
    else if (obj->mValue > otherObj->mValue) {
      cmp = 1;
    }

    args.GetReturnValue().Set(Int32::New(isolate, cmp));
  }
}

void Int64::High32(const v8::FunctionCallbackInfo<Value>& args) {
  Isolate *isolate = args.GetReturnValue().GetIsolate();
  HandleScope scope(isolate);
  Int64* obj = ObjectWrap::Unwrap<Int64>(args.This());
  uint32_t highBits = static_cast<uint32_t>(obj->mValue >> 32);
  args.GetReturnValue().Set(Int32::NewFromUnsigned(args.GetReturnValue().GetIsolate(), highBits));
}

void Int64::Low32(const v8::FunctionCallbackInfo<Value>& args) {
  Isolate *isolate = args.GetReturnValue().GetIsolate();
  HandleScope scope(isolate);
  Int64* obj = ObjectWrap::Unwrap<Int64>(args.This());
  uint32_t lowBits = static_cast<uint32_t>(obj->mValue & 0xffffffffull);
  args.GetReturnValue().Set(Int32::NewFromUnsigned(args.GetReturnValue().GetIsolate(), lowBits));
}

void Int64::ShiftLeft(const v8::FunctionCallbackInfo<Value>& args) {
  Isolate *isolate = args.GetReturnValue().GetIsolate();
  HandleScope scope(isolate);
  if (args.Length() < 1) {
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "Argument required")));
    args.GetReturnValue().SetUndefined();
  }
  else if (!args[0]->IsNumber()) {
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "Integer expected")));
    args.GetReturnValue().SetUndefined();
  }
  else {
    
    Int64* obj = ObjectWrap::Unwrap<Int64>(args.This());
    
    uint64_t shiftBy = static_cast<uint64_t>(args[0]->ToNumber()->NumberValue());
    uint64_t value = obj->mValue << shiftBy;
    
    Local<Value> argv[2] = {
      Int32::NewFromUnsigned(isolate, static_cast<uint32_t>(value >> 32)),
      Int32::NewFromUnsigned(isolate, static_cast<uint32_t>(value & 0xffffffffull))
    };

    Local<Object> instance = Local<Function>::New(isolate, constructor)->NewInstance(2, argv);
    args.GetReturnValue().Set(instance);
  }
}

void Int64::ShiftRight(const v8::FunctionCallbackInfo<Value>& args) {
  Isolate *isolate = args.GetReturnValue().GetIsolate();
  HandleScope scope(isolate);
  if (args.Length() < 1) {
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "Argument required")));
    args.GetReturnValue().SetUndefined();
  }
  else if (!args[0]->IsNumber()) {
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "Integer expected")));
    args.GetReturnValue().SetUndefined();
  }
  else {
    Int64* obj = ObjectWrap::Unwrap<Int64>(args.This());
    uint64_t shiftBy = static_cast<uint64_t>(args[0]->ToNumber()->NumberValue());
    uint64_t value = obj->mValue >> shiftBy;
    Local<Value> argv[2] = {
      Int32::NewFromUnsigned(isolate, static_cast<uint32_t>(value >> 32)),
      Int32::NewFromUnsigned(isolate, static_cast<uint32_t>(value & 0xffffffffull))
    };
    Local<Object> instance = Local<Function>::New(isolate, constructor)->NewInstance(2, argv);
    args.GetReturnValue().Set(instance);
  }
}

void Int64::And(const v8::FunctionCallbackInfo<Value>& args) {
  Isolate *isolate = args.GetReturnValue().GetIsolate();
  HandleScope scope(isolate);
  if (args.Length() < 1) {
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "Argument required")));
    args.GetReturnValue().SetUndefined();
    return;
  }
  Int64* obj = ObjectWrap::Unwrap<Int64>(args.This());
  uint64_t value;
  if (args[0]->IsNumber()) {
    value = obj->mValue & args[0]->IntegerValue();
  } else if (args[0]->IsObject()) {
    Int64* otherObj = ObjectWrap::Unwrap<Int64>(args[0]->ToObject());
    value = obj->mValue & otherObj->mValue;
  } else {
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "Object or number expected")));
    args.GetReturnValue().SetUndefined();
    return;
  }
  Local<Value> argv[2] = {
    Int32::NewFromUnsigned(isolate, static_cast<uint32_t>(value >> 32)),
    Int32::NewFromUnsigned(isolate, static_cast<uint32_t>(value & 0xffffffffull))
  };
  Local<Object> instance = Local<Function>::New(isolate, constructor)->NewInstance(2, argv);
  args.GetReturnValue().Set(instance);
}

void Int64::Or(const v8::FunctionCallbackInfo<Value>& args) {
  Isolate *isolate = args.GetReturnValue().GetIsolate();
  HandleScope scope(isolate);
  if (args.Length() < 1) {
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "Argument required")));
    args.GetReturnValue().SetUndefined();
    return;
  }
  Int64* obj = ObjectWrap::Unwrap<Int64>(args.This());
  uint64_t value;
  if (args[0]->IsNumber()) {
    value = obj->mValue | args[0]->IntegerValue();
  } else if (args[0]->IsObject()) {
    Int64* otherObj = ObjectWrap::Unwrap<Int64>(args[0]->ToObject());
    value = obj->mValue | otherObj->mValue;
  } else {
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "Object or number expected")));
    args.GetReturnValue().SetUndefined();
    return;
  }
  Local<Value> argv[2] = {
    Int32::NewFromUnsigned(isolate, static_cast<uint32_t>(value >> 32)),
    Int32::NewFromUnsigned(isolate, static_cast<uint32_t>(value & 0xffffffffull))
  };
  Local<Object> instance = Local<Function>::New(isolate, constructor)->NewInstance(2, argv);
  args.GetReturnValue().Set(instance);
}

void Int64::Xor(const v8::FunctionCallbackInfo<Value>& args) {
  Isolate *isolate = args.GetReturnValue().GetIsolate();
  HandleScope scope(isolate);
  if (args.Length() < 1) {
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "Argument required")));
    args.GetReturnValue().SetUndefined();
    return;
  }
  Int64* obj = ObjectWrap::Unwrap<Int64>(args.This());
  uint64_t value;
  if (args[0]->IsNumber()) {
    value = obj->mValue ^ args[0]->IntegerValue();
  } else if (args[0]->IsObject()) {
    Int64* otherObj = ObjectWrap::Unwrap<Int64>(args[0]->ToObject());
    value = obj->mValue ^ otherObj->mValue;
  } else {
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "Object or number expected")));
    args.GetReturnValue().SetUndefined();
    return;
  }
  Local<Value> argv[2] = {
    Int32::NewFromUnsigned(isolate, static_cast<uint32_t>(value >> 32)),
    Int32::NewFromUnsigned(isolate, static_cast<uint32_t>(value & 0xffffffffull))
  };
  Local<Object> instance = Local<Function>::New(isolate, constructor)->NewInstance(2, argv);
  args.GetReturnValue().Set(instance);
}

void Int64::Add(const v8::FunctionCallbackInfo<Value>& args) {
  Isolate *isolate = args.GetReturnValue().GetIsolate();
  HandleScope scope(isolate);
  if (args.Length() < 1) {
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "Argument required")));
    args.GetReturnValue().SetUndefined();
    return;
  }
  Int64* obj = ObjectWrap::Unwrap<Int64>(args.This());
  uint64_t value;
  if (args[0]->IsNumber()) {
    value = obj->mValue + args[0]->IntegerValue();
  } else if (args[0]->IsObject()) {
    Int64* otherObj = ObjectWrap::Unwrap<Int64>(args[0]->ToObject());
    value = obj->mValue + otherObj->mValue;
  } else {
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "Object or number expected")));
    args.GetReturnValue().SetUndefined();
    return;
  }
  Local<Value> argv[2] = {
    Int32::NewFromUnsigned(isolate, static_cast<uint32_t>(value >> 32)),
    Int32::NewFromUnsigned(isolate, static_cast<uint32_t>(value & 0xffffffffull))
  };
  Local<Object> instance = Local<Function>::New(isolate, constructor)->NewInstance(2, argv);
  args.GetReturnValue().Set(instance);
}

void Int64::Sub(const v8::FunctionCallbackInfo<Value>& args) {
  Isolate *isolate = args.GetReturnValue().GetIsolate();
  HandleScope scope(isolate);
  if (args.Length() < 1) {
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "Argument required")));
    args.GetReturnValue().SetUndefined();
    return;
  }
  Int64* obj = ObjectWrap::Unwrap<Int64>(args.This());
  uint64_t value;
  if (args[0]->IsNumber()) {
    value = obj->mValue - args[0]->IntegerValue();
  } else if (args[0]->IsObject()) {
    Int64* otherObj = ObjectWrap::Unwrap<Int64>(args[0]->ToObject());
    value = obj->mValue - otherObj->mValue;
  } else {
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "Object or number expected")));
    args.GetReturnValue().SetUndefined();
    return;
  }
  Local<Value> argv[2] = {
    Int32::NewFromUnsigned(isolate, static_cast<uint32_t>(value >> 32)),
    Int32::NewFromUnsigned(isolate, static_cast<uint32_t>(value & 0xffffffffull))
  };
  Local<Object> instance = Local<Function>::New(isolate, constructor)->NewInstance(2, argv);
  args.GetReturnValue().Set(instance);
}
