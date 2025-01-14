#include <stdio.h>
#include <stdint.h>
#include <string.h>

extern void* switchRSP(void*);
extern void* getrsp(void*);

#define getrsp(void) getrsp(&osRBP)
//#define switchRSP(void) switchRSP(os)
#define stackSize ((2*1024)*8)

char ms[stackSize];
void *ts = (void*)(&ms[stackSize]);

int main(int argc, char *argv[]) {
	void *osRBP = NULL;
	void *os = getrsp(&osRBP);
	            
	printf("Allocate %dKB as new stack beginning at:0x%p\n",stackSize/1024,&ms[0]);
	printf("Current RSP: 0x%p\n", os);
	
	       size_t frameSz = (osRBP == 0 ? ((uintptr_t)os | (uintptr_t)0xFFF)-(uintptr_t)(os-0x10) : osRBP - (os-0x10));
	ts = ts - frameSz;
	
	printf("Copy Current Frame (Size:%d Bytes) RSP:0x%p -> New Stack:0x%p\n",frameSz, os,ts);
	memcpy(ts,os,frameSz);
	
	os = switchRSP(ts);
	printf("New RSP via 'sPointer = switchRSP(0x%p);' RSP: 0x%p\n",ts,getrsp());
	
	os = switchRSP(os);
	printf("swap back via 'sPointer = switchRSP(sPointer);' RSP: 0x%p\n",getrsp());
	
	os = switchRSP(os);
	printf("swap again via 'sPointer = switchRSP(sPointer);' RSP: 0x%p\n",getrsp());
	os = switchRSP(os);
	printf("swap back via 'sPointer = switchRSP(sPointer);' RSP: 0x%p\n",getrsp());
	
	os = switchRSP(os);
	printf("swap again via 'sPointer = switchRSP(sPointer);' RSP: 0x%p\n",getrsp());
	
	printf("\nNOTE: The alternating printf calls were executed on different stack spaces.\n");
	printf("Press Enter to Exit\n");
	getchar();
	return 0;
}

asm(".section .mine,\"x\"\n\t"
	".align 8\n"
	"getrsp:\n"
	"\tmov rax,rsp\n"
	"\tmov [rcx],rbp\n"
	"\tret\n"
	"switchRSP:\n"	
        "\tpop rdx\n"
        "\tpush rdx\n"
        "\tmov rax,rsp\n"	        
        "\tmov rsp,ts\n"
        "\ttest rcx,rcx\n"
        "\tcmovnz rsp,rcx\n"	
        "\tmov qword ptr gs:[0x10],rsp\n"
        "\tadd rsp,0x8\n"
        "\tpush rdx\n"
        "\tsub rbp,rax\n"
        "\tadd rbp,rsp\n"
        "\tret\n");   
        
        
        
        
        
        