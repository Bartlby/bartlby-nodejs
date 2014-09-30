#pragma GCC diagnostic ignored "-fpermissive"
#pragma GCC diagnostic ignored "-Wwrite-strings"
#pragma GCC diagnostic ignored "-Wunused-variable"


#include <node.h>
#include <nan.h>
#include "bartlby.hpp"





using namespace v8;

static char * dlmsg;

static v8::Persistent<v8::FunctionTemplate> BTLCore::constructor;
    

static char *TO_CHAR(v8::Handle<v8::Value> val) {
    v8::String::Utf8Value utf8(val->ToString());

    int len = utf8.length() + 1;
    char *str = (char *) calloc(sizeof(char), len);
    strncpy(str, *utf8, len);

    return str;
}

void BTLCore::Init(Handle<Object> target) {
    NanScope();


    v8::Local<v8::FunctionTemplate> tpl = NanNew<v8::FunctionTemplate>(New);
    Local<String> name = NanNew<String>("Instance");



    NanAssignPersistent(constructor, tpl);
    tpl->SetClassName(NanNew<v8::String>("constructor"));
    tpl->InstanceTemplate()->SetInternalFieldCount(1);

    // Add all prototype methods, getters and setters here.
    NODE_SET_PROTOTYPE_METHOD(tpl, "CFG", Value);
    NODE_SET_PROTOTYPE_METHOD(tpl, "getService", getService);
    NODE_SET_PROTOTYPE_METHOD(tpl, "getInfo", getInfo);

    
    NODE_SET_PROTOTYPE_METHOD(tpl, "addService", addService);


    NODE_SET_PROTOTYPE_METHOD(tpl, "close", CoreClose);

    // This has to be last, otherwise the properties won't show up on the
    // object in JavaScript.
    target->Set(name, tpl->GetFunction());
}

BTLCore::BTLCore(char * cfg)
    : ObjectWrap(),
      cfg_(cfg) {}


NAN_METHOD(BTLCore::New) {
    NanScope();

    if (!args.IsConstructCall()) {
        return NanThrowTypeError("ee");
    }

    if (args.Length() < 1) {
        return NanThrowTypeError("First argument must be a number");
    }

    // Creates a new instance object of this type and wraps it.
    BTLCore* obj = new BTLCore(TO_CHAR(args[0]->ToString()));
    

    obj->shm_addr_=bartlby_get_shm(obj->cfg_);
    obj->soHandle_=bartlby_get_sohandle(obj->cfg_);

    obj->Wrap(args.This());

    return args.This();
}
NAN_METHOD(BTLCore::addService) {
    NanScope();
    Local<Object> obj = args[0]->ToObject();
    

    NanReturnValue(obj->Get(NanNew<v8::String>("service_name"))->ToString());
 }

NAN_METHOD(BTLCore::getInfo) {
    NanScope();
    Local<Object> return_obj = NanNew<Object>();

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



    //return_obj->Set(NanNew<v8::String>("autor"),NanNew<v8::String>(GetAutorStr));
    return_obj->Set(NanNew<v8::String>("version"),NanNew<v8::String>(GetVersionStr));
    return_obj->Set(NanNew<v8::String>("name"),NanNew<v8::String>(GetNameStr));

    if(obj->shm_addr_ != NULL) {
        shm_hdr=(struct shm_header *)(void *)obj->shm_addr_;
        return_obj->Set(NanNew<v8::String>("services"),NanNew<v8::Integer>(shm_hdr->svccount));
        return_obj->Set(NanNew<v8::String>("workers"),NanNew<v8::Integer>(shm_hdr->wrkcount));
        return_obj->Set(NanNew<v8::String>("version"),NanNew<v8::String>(shm_hdr->version));
        
        
    } else {

    }





    free(GetAutorStr);
    free(GetVersionStr);
    free(GetNameStr);

    NanReturnValue(return_obj);
}


NAN_METHOD(BTLCore::CoreClose) {
    NanScope();
    Local<Object> return_obj = NanNew<v8::Object>();


    // Retrieves the pointer to the wrapped object instance.
    BTLCore* obj = ObjectWrap::Unwrap<BTLCore>(args.This());


   
    free(obj->cfg_);
    shmdt(obj->shm_addr_);   
    if(obj->soHandle_ != NULL) dlclose(obj->soHandle_);

    NanReturnValue(return_obj);
}

NAN_METHOD(BTLCore::getService) {
    NanScope();
    Local<Object> return_obj = NanNew<v8::Object>();

    // Retrieves the pointer to the wrapped object instance.
    BTLCore* obj = ObjectWrap::Unwrap<BTLCore>(args.This());

    return_obj->Set(NanNew<v8::String>("service_id"),NanNew<v8::Integer>(123));
    return_obj->Set(NanNew<v8::String>("service_name"),NanNew<v8::String>("asdf"));

    NanReturnValue(return_obj);
}

NAN_METHOD(BTLCore::Value) {
    NanScope();

    // Retrieves the pointer to the wrapped object instance.
    BTLCore* obj = ObjectWrap::Unwrap<BTLCore>(args.This());

    NanReturnValue(NanNew<v8::String>(obj->cfg_));
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

