/*
 * ptrace_tools.c
 *
 *  Created on: 2012-5-4
 *      Author: amos
 */

#include "ptrace_tools.h"
#include <signal.h>

void ptrace_attach(pid_t pid){

	int state;
	if(ptrace(PTRACE_ATTACH,pid,NULL,0)<0){
		die(__LINE__,__FILE__);
	}
	waitpid(pid,&state,0);
	if(ptrace(PTRACE_SYSCALL,pid,NULL,0)<0){
		die(__LINE__,__FILE__);
	}
	waitpid(pid,&state,0);
}

void ptrace_getregs(pid_t pid,struct pt_regs *regs){
	if(ptrace(PTRACE_GETREGS,pid,NULL,regs)<0){
		die(__LINE__,__FILE__);
	}
}

void ptrace_setregs(pid_t pid, struct pt_regs *regs){
	if(ptrace(PTRACE_SETREGS,pid,NULL,regs)<0){
		die(__LINE__,__FILE__);
	}
}

void ptrace_cont(pid_t pid){
	if(	ptrace(PTRACE_CONT,pid,NULL,NULL)<0){
		die(__LINE__,__FILE__);
	}
}

void ptrace_detach(pid_t pid){
	if(ptrace(PTRACE_DETACH,pid,NULL,NULL)<0){
		die(__LINE__,__FILE__);
	}
}

void ptrace_call(pid_t pid,uint32_t addr,uint32_t* params,uint32_t params_num,struct pt_regs* regs){

	uint32_t i;

//	if(params_num>4){
//		regs->ARM_r0 = params[0];
//		regs->ARM_r1 = params[1];
//		regs->ARM_r2 = params[2];
//		regs->ARM_r3 = params[3];
//
//		regs->ARM_sp -= (params_num - 4) * sizeof(long) ;
//		poke_text( pid, regs->ARM_sp, (void*)&params[4], (params_num - 4) * sizeof(long) );
//	}
//	else{
//		uint32_t i;
//		for ( i = 0; i < params_num; i ++ )
//		{
//			regs->uregs[i] = params[i];
//		}
//	}

	for ( i = 0; i < params_num && i < 4; i ++ )
	{
		regs->uregs[i] = params[i];
	}

	if ( i < params_num )
	{
		regs->ARM_sp -= (params_num - i) * sizeof(long);
		poke_text( pid, regs->ARM_sp, (void*)&params[i], (params_num - i) * sizeof(long) );
	}

	regs->ARM_pc = addr;
	if ( regs->ARM_pc & 1 )
	{
		/* thumb */
		regs->ARM_pc &= (~1u);
		regs->ARM_cpsr |= (1u<<5);
	}
	else
	{
		/* arm */
		regs->ARM_cpsr &= ~(1u<<5);
	}

	//fake saved return address, triggering a SIGSEGV to catch
	regs->ARM_lr = 0x00000000;

	ptrace_setregs( pid, regs );

	ptrace_cont(pid);

	int status;
	int id = 0;
	do {
		id = waitpid(pid, &status, WUNTRACED);	//The process was blocked ....! Why..?
		printf("[+] id:%d,signal:%d\n",id,WSTOPSIG(status));
	} while ((id != pid) || (WSTOPSIG(status) != SIGSEGV));

//	waitpid(pid,NULL,WUNTRACED);
}

void poke_text(pid_t pid, uint8_t* addr, void *buf, size_t blen)
{
	int i = 0;

	size_t alen = blen + sizeof(size_t) - (blen % sizeof(size_t)); /* word align */

	char *ptr = NULL;

	if ((ptr = (char *)malloc(alen)) == NULL)
		die(__LINE__,__FILE__);

	memset(ptr, 0, alen);

	memcpy(ptr, buf, blen);

	for (i = 0; i < blen; i += sizeof(size_t)) {
		if (ptrace(PTRACE_POKETEXT, pid, (void *)(addr + i), (void *)*(size_t *)&ptr[i]) < 0)
			die(__LINE__,__FILE__);
	}

	free(ptr);
}

void peek_text(pid_t pid, size_t addr, void *buf, size_t blen)
{
	int i = 0;
	size_t word = 0;
	for (i = 0; i < blen; i += sizeof(size_t)) {
		word = ptrace(PTRACE_PEEKTEXT, pid, (void *)(addr + i), NULL);
		memcpy(&buf[i], &word, sizeof(word));
	}
}
