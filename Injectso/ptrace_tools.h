/*
 * ptrace_tools.h
 *
 *  Created on: 2012-5-4
 *      Author: amos
 */

#ifndef PTRACE_TOOLS_H_
#define PTRACE_TOOLS_H_

#include <sys/ptrace.h>
#include <sys/wait.h>

#ifdef __cpluscplus
extern "C"{
#endif

void ptrace_attach(pid_t pid);

void ptrace_setregs(pid_t pid,struct pt_regs* regs);

void ptrace_getregs(pid_t pid,struct pt_regs* regs);

void ptrace_cont(pid_t pid);

void ptrace_detach(pid_t pid);

void ptrace_call(pid_t pid,uint32_t addr,uint32_t* params,uint32_t params_num,struct pt_regs* regs);

void poke_text(pid_t pid,uint8_t* dest,void* buf,size_t blen);

void peek_text(pid_t pid,size_t src,void* buf,size_t size);

#ifdef __cpluscplus
}
#endif

#endif /* PTRACE_TOOLS_H_ */
