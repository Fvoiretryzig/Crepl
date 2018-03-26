#include <stdio.h>
#include <dlfcn.h>

#define DLL_FILE_NAME "libcac.so"

int main()
{
    void *handle;
    int (*func)(int, int);
    char *error;
    int a = 30;
    int b = 5;

    handle = dlopen(DLL_FILE_NAME, RTLD_NOW);
    if (handle == NULL)
    {
    fprintf(stderr, "Failed to open libaray %s error:%s\n", DLL_FILE_NAME, dlerror());
    return -1;
    }

    func = dlsym(handle, "add");
    printf("%d + %d = %d\n", a, b, func(a, b));

    func = dlsym(handle, "sub");
    printf("%d + %d = %d\n", a, b, func(a, b));

    func = dlsym(handle, "div");
    printf("%d + %d = %d\n", a, b, func(a, b));
    
    func = dlsym(handle, "mul");
    printf("%d + %d = %d\n", a, b, func(a, b));

    dlclose(handle);
    return 0;
}
