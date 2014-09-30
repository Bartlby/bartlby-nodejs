#ifndef MODULENAME_HPP
#define MODULENAME_HPP

#include <node.h>
#include <nan.h>
#include "bartlby_core.h"


// Do not include this line. It's generally frowned upon to use namespaces
// in header files as it may cause issues with other code that includes your
// header file.
// using namespace v8;


class BTLCore : public node::ObjectWrap {
public:
    static v8::Persistent<v8::FunctionTemplate> constructor;
    static void Init(v8::Handle<v8::Object> target);

protected:
    BTLCore(char * cfg);

    static NAN_METHOD( New);
    static NAN_METHOD( Value);
    static NAN_METHOD( getService);
    static NAN_METHOD( addService);
    static NAN_METHOD( getInfo);
    static NAN_METHOD( CoreClose);


    // Your own object variables here
    char * cfg_;
    void * shm_addr_;
    void * soHandle_;
};




#endif
