#include<stdio.h>
#include<dlfcn.h>
#include<stdlib.h>
#include <unistd.h>
#include<string.h>

char code[2048];
char *filename = "code.c";
char *filename_cp = "code_cp.c";
char *libname = "code.so";
char *cmd_so = "gcc code.c -Werror -shared -fPIC -o code.so -ldl";
int cmd_id = 0; void *handle;

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

void copyFile(FILE* fp, FILE* fp_cp)
{
	char ch ;
	fclose(fp);
	if ((fp = fopen(filename,"w+")) == NULL)
	{
		printf("canot find the in.txt file!\n");
		exit(0);
	}
	ch = fgetc(fp_cp);
	printf("%c\n", ch);
	while (ch!=EOF)
	{	printf("this is while\n");
		printf("%c\n", ch);
		fputc(ch,fp);
		putchar(ch);
		ch = fgetc(fp_cp);
	}
	if(system(cmd_so)){
		printf("what??????\n");
		exit(1);
	}
	return ;
}

int main() 
{
	/*创建文件，用来存放代码*/
	char *cmd_so = "gcc code.c -Werror -shared -fPIC -o code.so -ldl";
	FILE *fp = fopen(filename, "w+");
	FILE *fp_cp = fopen(filename_cp, "w+");
	printf(">> ");
	while(fgets(code, sizeof(code), stdin) != NULL){
		if(code[0] == 'i' && code[1] == 'n' && code[2] == 't'){		//如果为函数,生成一个动态链接库
			fprintf(fp, "%s", code);
			fflush(fp);
			if(system(cmd_so)){
				printf("error while linking\n");
				continue;
			}
			fprintf(fp_cp, "%s", code);	//存一个副本
			fflush(fp_cp);
		}
		else if(code[0] == 'e' && code[1] == 'x' && code[2] == 'i' && code[3] == 't'){	//退出
			 break;
		}
		else{		//如果是表达式，如果是字母开头是函数，如果是数字开头是表达式
			char expr_name[32] = "__expr_wrap_";
			sprintf(expr_name, "%s%d", expr_name, cmd_id++);
			code[strlen(code)-1] = '\0';
			fprintf(fp, "int %s(){return %s;}\n", expr_name, code);
			fflush(fp);
			if(system(cmd_so)){		//把求值变为函数再加入到动态库中
				printf("error while linking\n");
				printf(">> ");
				//把存的正确的复制过去再编译？
				copyFile(fp, fp_cp);
				continue;
			}	
			int (*func)() = func_lookup(expr_name); // 查找XXX对应的函数
			//printf("this is after lookup\n");
			//printf("this is func:0x%x\n", func);
			if(func == 0){
				printf("\n>> ");
				continue;
			}
			int value = func(); // 通过函数指针调用
			printf(">> %s = %d.\n", code, value);	
			dlclose(handle);	
			fprintf(fp_cp, "int %s(){return %s;}\n", expr_name, code);
			fflush(fp_cp);
			printf("haha\n");
		}
		printf(">> ");
	}
	fclose(fp); remove(filename);
	fclose(fp_cp); remove(filename_cp);
	remove(libname);
	return 0;
}
