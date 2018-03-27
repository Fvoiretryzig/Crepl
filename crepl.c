#include<stdio.h>
#include<dlfcn.h>
#include<stdlib.h>
#include <unistd.h>
#include<string.h>

char code[2048];
char *filename = "code.c";
char *libname = "code.so";
//如果编译不报错才把正确的代码加入到code.c中
//char *check_filename = "check_code.c";
//char *check_libname = "check_code.so";
int cmd_id = 0;

void *handle;

void *func_lookup(char *name)
{
	//void *handle;  
    char *error;  
          
    int (*func )();  
    handle = dlopen("./code.so", RTLD_LAZY);  
    if(!handle){  	
		fputs(dlerror(), stderr);  
		return 0;
	}      
	func = dlsym(handle, name);  
	if((error = dlerror()) != NULL){  
		fputs(error, stderr);  
		return 0;
	}  
    return func;   
}

int main() 
{
	/*创建文件，用来存放代码*/
	char *cmd_so = "gcc code.c -Werror -shared -fPIC -o code.so -ldl";
	//char *cmd_check = "gcc check_code.c -Werror -shared -fPIC -o check_code.so -ldl";
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
		else{		
			char expr_name[32] = "__expr_wrap_";
			code[strlen(code)-1] = '\0';
			sprintf(expr_name, "%s%d", expr_name, cmd_id++);
			fprintf(fp, "int %s(){return %s;}\n", expr_name, code);
			fflush(fp);	
			if(system(cmd_so)){		//把求值变为函数再加入到动态库中
				printf("error while linking\n");
				printf(">> ");
				continue;
			}
			int (*func)() = func_lookup(expr_name, 0); // 查找XXX对应的函数
			if(func == 0){
				printf("\n>> ");
				continue;
			int value = func(); // 通过函数指针调用
			printf(">> %s = %d.\n", code, value);	
			dlclose(handle);					
			}			
						
			//}	

	
		}
		printf(">> ");
	}
	fclose(fp); remove(filename);
	//remove(check_filename);
	remove(libname);
	return 0;
}
