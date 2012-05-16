/*
 * injectso.h
 *
 *  Created on: 2012-5-4
 *      Author: amos
 */

#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <dirent.h>
#include <sys/mman.h>

#include "ptrace_tools.h"
#include "injectso.h"


#define  MAX_PATH 0x100

#define REMOTE_ADDR( addr, local_base, remote_base ) ( (uint32_t)(addr) + (uint32_t)(remote_base) - (uint32_t)(local_base) )

#define LIBC_PATH 	"/system/lib/libc.so"
#define LINKER_PATH "/system/bin/linker"

extern uint32_t _dlopen_addr_s,
	_dlopen_param1_s,
	_dlsym_addr_s,
	_dlsym_param2_s,
	_inject_start_s,
	_inject_end_s,
	_inject_function_param_s,
	_saved_cpsr_s,
	_saved_r0_pc_s;


void* get_libarary_addr( pid_t pid, const char* module_name )
{
	FILE *fp;
	long addr = 0;
	char *pch;
	char filename[32];
	char line[1024];

	if ( pid < 0 )
	{
		/* self process */
		snprintf( filename, sizeof(filename), "/proc/self/maps", pid );
	}
	else
	{
		snprintf( filename, sizeof(filename), "/proc/%d/maps", pid );
	}

	fp = fopen( filename, "r" );

	if ( fp != NULL )
	{
		while ( fgets( line, sizeof(line), fp ) )
		{
			if ( strstr( line, module_name ) )
			{
				pch = strtok( line, "-" );
				addr = strtoul( pch, NULL, 16 );

				if ( addr == 0x8000 )
					addr = 0;

				break;
			}
		}

		fclose( fp ) ;
	}

	return (void *)addr;
}

void* get_remote_module_addr( pid_t target_pid, const char* module_name, void* local_addr )
{
	void* local_handle, *remote_handle;

	local_handle = get_libarary_addr( -1, module_name );

	remote_handle = get_libarary_addr( target_pid, module_name );

	return (void *)( (uint32_t)local_addr + (uint32_t)remote_handle - (uint32_t)local_handle );
}

int inject_process( pid_t pid, const char *library_path, const char *function_name, void *param, size_t param_size )
{
	struct pt_regs regs, saved_regs;

	struct Hook hook = {NULL,0,NULL,NULL,NULL};

	hook.remote_dlopen_addr = get_remote_module_addr( pid, LINKER_PATH, (void *)dlopen );
	hook.remote_dlsym_addr = get_remote_module_addr( pid, LINKER_PATH, (void *)dlsym );
	hook.remote_mmap = get_remote_module_addr( pid, LIBC_PATH, (void *)mmap );

	uint8_t *map_base;

	uint8_t *dlopen_param1_ptr,
	*dlsym_param2_ptr,
	*saved_r0_pc_ptr,
	*inject_param_ptr,
	*remote_code_ptr,
	*local_code_ptr;

	uint32_t code_length;

	long parameters[10];

	printf( "[+] Injecting process: %d\n", pid );

	ptrace_attach(pid);

	ptrace_getregs(pid,&regs);

	memcpy( &saved_regs, &regs, sizeof(regs) );

	printf( "[+] Remote mmap address: %x\n", hook.remote_mmap );

	/* setting mmap parameters */
	parameters[0] = 0;		// addr
	parameters[1] = 0x4000; // size
	parameters[2] = PROT_READ | PROT_WRITE | PROT_EXEC;  // prot
	parameters[3] =  MAP_ANONYMOUS | MAP_PRIVATE; 		// flags
	parameters[4] = 0; //fd
	parameters[5] = 0; //offset

	printf( "[+] Calling mmap in target process.\n" );

	ptrace_call(pid,(uint32_t)hook.remote_mmap,parameters,6,&regs);

	ptrace_getregs(pid,&regs);

	printf( "[+] Target process returned from mmap, return value=%x, pc=%x \n", regs.ARM_r0, regs.ARM_pc );

	map_base = (uint8_t *)regs.ARM_r0;

	printf( "[+] Get imports: dlopen: %x, dlsym: %x\n", hook.remote_dlopen_addr, hook.remote_dlsym_addr );

	remote_code_ptr = map_base + 0x3C00;
	local_code_ptr = (uint8_t *)&_inject_start_s;


	_dlopen_addr_s = (uint32_t)hook.remote_dlopen_addr;
	_dlsym_addr_s = (uint32_t)hook.remote_dlsym_addr;

	printf( "[+] Inject code start: %x, end: %x\n", local_code_ptr, &_inject_end_s );

	code_length = (uint32_t)&_inject_end_s - (uint32_t)&_inject_start_s;
	dlopen_param1_ptr = local_code_ptr + code_length + 0x20;
	dlsym_param2_ptr = dlopen_param1_ptr + MAX_PATH;
	saved_r0_pc_ptr = dlsym_param2_ptr + MAX_PATH;
	inject_param_ptr = saved_r0_pc_ptr + MAX_PATH;


	/* dlopen parameter 1: library name */
	strcpy( dlopen_param1_ptr, library_path );
	_dlopen_param1_s = REMOTE_ADDR( dlopen_param1_ptr, local_code_ptr, remote_code_ptr );
	printf( "[+] _dlopen_param1_s: %x\n", _dlopen_param1_s );

	/* dlsym parameter 2: function name */
	strcpy( dlsym_param2_ptr, function_name );
	_dlsym_param2_s = REMOTE_ADDR( dlsym_param2_ptr, local_code_ptr, remote_code_ptr );
	printf( "[+] _dlsym_param2_s: %x\n", _dlsym_param2_s );

	/* saved cpsr */
	_saved_cpsr_s = saved_regs.ARM_cpsr;

	/* saved r0-pc */
	memcpy( saved_r0_pc_ptr, &(saved_regs.ARM_r0), 16 * 4 ); // r0 ~ r15
	_saved_r0_pc_s = REMOTE_ADDR( saved_r0_pc_ptr, local_code_ptr, remote_code_ptr );
	printf( "[+] _saved_r0_pc_s: %x\n", _saved_r0_pc_s );

	/* Inject function parameter */
	memcpy( inject_param_ptr, param, param_size );
	_inject_function_param_s = REMOTE_ADDR( inject_param_ptr, local_code_ptr, remote_code_ptr );
	printf( "[+] _inject_function_param_s: %x\n", _inject_function_param_s );

	printf( "[+] Remote shellcode address: %x\n", remote_code_ptr );
	poke_text( pid, remote_code_ptr, local_code_ptr, 0x400 );

	memcpy( &regs, &saved_regs, sizeof(regs) );

	regs.ARM_sp = (long)remote_code_ptr;
	regs.ARM_pc = (long)remote_code_ptr;

	ptrace_setregs( pid, &regs );
	ptrace_detach( pid );

	return 0;
}
