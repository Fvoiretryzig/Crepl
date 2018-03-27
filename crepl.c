#include<stdio.h>
#include<dlfcn.h>
#include<stdlib.h>
#include <unistd.h>
#include<string.h>

char code[2048];
char *filename = "temp_code.c";
char *libname = "temp_code.so";
int cmd_id = 0;

void *handle;

void *func_lookup(char *name)
{
	//void *handle;  
    char *error;  
          
    int (*func )();  
    handle = dlopen("./temp_code.so", RTLD_LAZY);  
    if(!handle){  
		fputs(dlerror(), stderr);  
		//exit(1);  
		return 0;
	}      
	func = dlsym(handle, name);  
	if((error = dlerror()) != NULL){  
		fputs(error, stderr);  
		//exit(1);  
		return 0;
	}  
    return func;   
}

int main() 
{
	/*创建文件，用来存放代码*/
	char *cmd_so = "gcc temp_code.c -shared -fPIC -o temp_code.so -ldl";
	FILE *fp = fopen(filename, "w+");
	printf(">> ");
	while(fgets(code, sizeof(code), stdin) != NULL){
		if(code[0] == 'i' && code[1] == 'n' && code[2] == 't'){		//如果为函数,生成一个动态链接库
			fprintf(fp, "%s", code);
			if(system(cmd_so)){
				printf("error while linking\n");
				continue;
			}
		}
		else if(code[0] == 'e' && code[1] == 'x' && code[2] == 'i' && code[3] == 't'){
			 break;
		}
		else{		//如果是表达式，如果是字母开头是函数，如果是数字开头是表达式
			char expr_name[32] = "__expr_wrap_";
			//printf("this is before sprintf\n");
			sprintf(expr_name, "%s%d", expr_name, cmd_id++);
			printf("expr_name:%s\n", expr_name);
			code[strlen(code)-1] = '\0';
			fprintf(fp, "int %s(){return %s;}\n", expr_name, code);
			printf("this is before system\n");
			if(system(cmd_so)){		//把求值变为函数再加入到动态库中
				printf("error while linking\n");
				printf(">> ");
				continue;
			}	
			printf("this is after syscall\n");
			int (*func)() = func_lookup(expr_name); // 查找XXX对应的函数
			if(func == 0){
				printf("\n>> ");
				continue;
			}
			int value = func(); // 通过函数指针调用
			printf(">> %s = %d.\n", code, value);	
			dlclose(handle);	
		}
		printf(">>");
	}
	fclose(fp); //remove(filename);
	//remove(libname);
	return 0;
}
