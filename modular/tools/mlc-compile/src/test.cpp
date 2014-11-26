#include "foundation.h"
#include "foundation-auto.h"
#include "script.h"

extern "C" void MCStringExecPutStringAfter(void);
extern "C" void MCArithmeticExecAddIntegerToInteger(void);
void dummy(void)
{
    MCStringExecPutStringAfter();
    MCArithmeticExecAddIntegerToInteger();
}

static MCScriptModuleRef load_module(const char *p_filename)
{
    FILE *t_file;
    t_file = fopen(p_filename, "rb");
    if (t_file == NULL)
        return NULL;
    
    fseek(t_file, 0, SEEK_END);
    long t_length;
    t_length = ftell(t_file);
    fseek(t_file, 0, SEEK_SET);
    
    void *t_mem;
    t_mem = malloc(t_length);
    fread(t_mem, t_length, 1, t_file);
    fclose(t_file);
    
    MCStreamRef t_stream;
    MCScriptModuleRef t_module;
    MCMemoryInputStreamCreate(t_mem, t_length, t_stream);
    if (!MCScriptCreateModuleFromStream(t_stream, t_module))
        t_module = NULL;
    MCValueRelease(t_stream);
    
    free(t_mem);
    
    return t_module;
}

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        fprintf(stderr, "invalid arguments\n");
        exit(1);
    }
    
    MCInitialize();
    
    // Skip command arg.
    argc -= 1;
    argv += 1;
    
    MCScriptModuleRef t_module;
    for(int i = 0; i < argc; i++)
    {
        t_module = load_module(argv[i]);
        
        if (t_module == NULL ||
            !MCScriptEnsureModuleIsUsable(t_module))
        {
            fprintf(stderr, "'%s' failed to initialize\n", argv[i]);
            exit(1);
        }
    }
    
    // The last module we create an instance of and attempt to execute
    // a handler.
    bool t_success;
    t_success = true;
    
    MCScriptInstanceRef t_instance;
    if (t_success)
        t_success = MCScriptCreateInstanceOfModule(t_module, t_instance);
    
    MCValueRef t_result;
    if (t_success)
        t_success = MCScriptCallHandlerOfInstance(t_instance, MCNAME("test"), nil, 0, t_result);
    
    if (t_success)
        MCLog("Executed test with result %@", t_result);

    MCFinalize();
    
    return 0;
}