#include <node.h>

#include "bartlby.hpp"
#include <stdio.h>      /* printf, scanf, NULL */
#include <stdlib.h>     /* calloc, exit, free */
#include <stdio.h>
#include <string.h>

using namespace v8;


Persistent<FunctionTemplate> MyObject::constructor;


static char *TO_CHAR(v8::Handle<v8::Value> val) {
    v8::String::Utf8Value utf8(val->ToString());

    int len = utf8.length() + 1;
    char *str = (char *) calloc(sizeof(char), len);
    strncpy(str, *utf8, len);

    return str;
}

void MyObject::Init(Handle<Object> target) {
    HandleScope scope;

    Local<FunctionTemplate> tpl = FunctionTemplate::New(New);
    Local<String> name = String::NewSymbol("Instance");

    constructor = Persistent<FunctionTemplate>::New(tpl);
    // ObjectWrap uses the first internal field to store the wrapped pointer.
    constructor->InstanceTemplate()->SetInternalFieldCount(1);
    constructor->SetClassName(name);

    // Add all prototype methods, getters and setters here.
    NODE_SET_PROTOTYPE_METHOD(constructor, "CFG", Value);

    NODE_SET_PROTOTYPE_METHOD(constructor, "getService", getService);

    // This has to be last, otherwise the properties won't show up on the
    // object in JavaScript.
    target->Set(name, constructor->GetFunction());
}

MyObject::MyObject(char * cfg)
    : ObjectWrap(),
      cfg_(cfg) {}


Handle<Value> MyObject::New(const Arguments& args) {
    HandleScope scope;

    if (!args.IsConstructCall()) {
        return ThrowException(Exception::TypeError(
            String::New("Use the new operator to create instances of this object."))
        );
    }

    if (args.Length() < 1) {
        return ThrowException(Exception::TypeError(
            String::New("First argument must be a number")));
    }

    // Creates a new instance object of this type and wraps it.
    MyObject* obj = new MyObject(TO_CHAR(args[0]->ToString()));
    obj->Wrap(args.This());

    return args.This();
}

Handle<Value> MyObject::getService(const Arguments& args) {
    HandleScope scope;
    Local<Object> return_obj = Object::New();

    // Retrieves the pointer to the wrapped object instance.
    MyObject* obj = ObjectWrap::Unwrap<MyObject>(args.This());

    return_obj->Set(v8::String::NewSymbol("service_id"),Integer::New(123));
    return_obj->Set(v8::String::NewSymbol("service_name"),String::New("asdf"));

    return scope.Close(return_obj);
}

Handle<Value> MyObject::Value(const Arguments& args) {
    HandleScope scope;

    // Retrieves the pointer to the wrapped object instance.
    MyObject* obj = ObjectWrap::Unwrap<MyObject>(args.This());

    return scope.Close(String::New(obj->cfg_));
}








void RegisterModule(Handle<Object> target) {
    MyObject::Init(target);
}

NODE_MODULE(bartlby, RegisterModule);

