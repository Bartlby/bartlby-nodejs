#pragma GCC diagnostic ignored "-fpermissive"
#pragma GCC diagnostic ignored "-Wwrite-strings"
#pragma GCC diagnostic ignored "-Wunused-variable"


#include <node.h>
#include "bartlby.hpp"



using namespace v8;

static char * dlmsg;

Persistent<FunctionTemplate> BTLCore::constructor;


static char *TO_CHAR(v8::Handle<v8::Value> val) {
    v8::String::Utf8Value utf8(val->ToString());

    int len = utf8.length() + 1;
    char *str = (char *) calloc(sizeof(char), len);
    strncpy(str, *utf8, len);

    return str;
}

void BTLCore::Init(Handle<Object> target) {
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
    NODE_SET_PROTOTYPE_METHOD(constructor, "getInfo", getInfo);

    
    NODE_SET_PROTOTYPE_METHOD(constructor, "addService", addService);


    NODE_SET_PROTOTYPE_METHOD(constructor, "close", CoreClose);

    // This has to be last, otherwise the properties won't show up on the
    // object in JavaScript.
    target->Set(name, constructor->GetFunction());
}

BTLCore::BTLCore(char * cfg)
    : ObjectWrap(),
      cfg_(cfg) {}


Handle<Value> BTLCore::New(const Arguments& args) {
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
    BTLCore* obj = new BTLCore(TO_CHAR(args[0]->ToString()));
    

    obj->shm_addr_=bartlby_get_shm(obj->cfg_);
    obj->soHandle_=bartlby_get_sohandle(obj->cfg_);

    obj->Wrap(args.This());

    return args.This();
}
Handle<Value> BTLCore::addService(const Arguments& args) {
    HandleScope scope;
    Local<Object> obj = args[0]->ToObject();
    

    return scope.Close(obj->Get(String::New("service_name"))->ToString());
 }

Handle<Value> BTLCore::getInfo(const Arguments& args) {
    HandleScope scope;
    Local<Object> return_obj = Object::New();

    char * (*GetAutor)();
    char * (*GetVersion)();
    char * (*GetName)();
    char * GetAutorStr;
    char * GetVersionStr;
    char * GetNameStr;


    struct shm_header * shm_hdr;



    // Retrieves the pointer to the wrapped object instance.
    BTLCore* obj = ObjectWrap::Unwrap<BTLCore>(args.This());


    LOAD_SYMBOL(GetAutor,obj->soHandle_, "GetAutor");
    LOAD_SYMBOL(GetVersion,obj->soHandle_, "GetVersion");
    LOAD_SYMBOL(GetName,obj->soHandle_, "GetName");
        
    GetAutorStr=GetAutor();
    GetVersionStr=GetVersion();
    GetNameStr=GetName();



    return_obj->Set(v8::String::NewSymbol("autor"),String::New(GetAutorStr));
    return_obj->Set(v8::String::NewSymbol("version"),String::New(GetVersionStr));
    return_obj->Set(v8::String::NewSymbol("name"),String::New(GetNameStr));

    if(obj->shm_addr_ != NULL) {
        shm_hdr=(struct shm_header *)(void *)obj->shm_addr_;
        return_obj->Set(v8::String::NewSymbol("services"),Integer::New(shm_hdr->svccount));
        return_obj->Set(v8::String::NewSymbol("workers"),Integer::New(shm_hdr->wrkcount));
        return_obj->Set(v8::String::NewSymbol("version"),String::New(shm_hdr->version));
        
        
    } else {

    }





    free(GetAutorStr);
    free(GetVersionStr);
    free(GetNameStr);

    return scope.Close(return_obj);
}


Handle<Value> BTLCore::CoreClose(const Arguments& args) {
    HandleScope scope;
    Local<Object> return_obj = Object::New();


    // Retrieves the pointer to the wrapped object instance.
    BTLCore* obj = ObjectWrap::Unwrap<BTLCore>(args.This());


   
    free(obj->cfg_);
    shmdt(obj->shm_addr_);   
    if(obj->soHandle_ != NULL) dlclose(obj->soHandle_);

    return scope.Close(return_obj);
}

Handle<Value> BTLCore::getService(const Arguments& args) {
    HandleScope scope;
    Local<Object> return_obj = Object::New();

    // Retrieves the pointer to the wrapped object instance.
    BTLCore* obj = ObjectWrap::Unwrap<BTLCore>(args.This());

    return_obj->Set(v8::String::NewSymbol("service_id"),Integer::New(123));
    return_obj->Set(v8::String::NewSymbol("service_name"),String::New("asdf"));

    return scope.Close(return_obj);
}

Handle<Value> BTLCore::Value(const Arguments& args) {
    HandleScope scope;

    // Retrieves the pointer to the wrapped object instance.
    BTLCore* obj = ObjectWrap::Unwrap<BTLCore>(args.This());

    return scope.Close(String::New(obj->cfg_));
}







void RegisterModule(Handle<Object> target) {
    BTLCore::Init(target);
}



void * bartlby_get_sohandle(char * cfgfile) {
    char * data_lib;
    void * SOHandle;
    
    
    data_lib=getConfigValue("data_library", cfgfile);
    if(data_lib == NULL) {
            //php_error(E_WARNING, "cannot find data_lib key in %s", cfgfile);  
            return NULL;
    }
    SOHandle=dlopen(data_lib, RTLD_LAZY);
    if((dlmsg=dlerror()) != NULL) {
                    //php_error(E_ERROR, "DL Error: %s", dlmsg);
            return NULL;
        }   
        free(data_lib);
        return SOHandle;
} 





char * getConfigValue(char * key, char * fname) {
    FILE * fp;
    char  str[1024];

    char * tok;
    
    fp=fopen(fname, "r");
    if(!fp) {
    //  php_error(E_WARNING, "Config file '%s' failed", fname);
        return NULL;
    }
    
    while(fgets(str,1024,fp) != NULL) {
        str[strlen(str)-1]='\0';
        tok=strtok(str, "=");
        if(tok != NULL) {
                if(strcmp(tok, key) == 0) {
                        tok=strtok(NULL, "=");
                        if(tok == NULL) {
                                return NULL;
                        }
                        if(tok[strlen(tok)-1] == '\r') {
                            tok[strlen(tok)-1]='\0';
                        }
                        fclose(fp);
                        return strdup(tok);
                        
                } else {
                    continue;
                }
                    
        }
            
    }
    
    
    fclose(fp);
    
    
    return NULL;
}




void * bartlby_get_shm(char * cfgfile) {
    char * shmtok;
    void * bartlby_address;
    int shm_id;

    shmtok = getConfigValue("shm_key",cfgfile );
    if(shmtok == NULL) {
        return NULL;    
    }       
    
    
    shm_id = shmget(ftok(shmtok, 32), 0,  0777);
    free(shmtok);
    if(shm_id != -1) {
        bartlby_address=shmat(shm_id,NULL,0);
        

        return bartlby_address;
    } else {
        //FIXME ERROR ATTACHING
        return NULL;
    }

    
}




NODE_MODULE(bartlby, RegisterModule);

