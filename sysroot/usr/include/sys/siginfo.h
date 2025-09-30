//FROM FILE COMES FROM TLIBC (tayoky 2025)
//SEE https://github.com/tayoky/tlibc FOR LASTED VERSION

#include <sys/cdefs.h>

_BEGIN_C_HEADER

#ifndef SYS_SIGINFO
#define SYS_SIGINFO

#include <sys/types.h>

union sigval {
	int	sival_int;	    //integer value
	void	*sival_ptr;	//pointer value
};

typedef struct {
	int si_signo;          //signal number
	int si_code;           //signal code
	int si_errno;          //error value
	pid_t si_pid;          //sending process pid
	uid_t si_uid;          //real user id of the sending process
	void *si_addr;         //faulting address (signal specific)
	int si_status;         //exit status
	long si_band;          //unused
	union sigval si_value; //signal value/argument
} siginfo_t;

#endif
_END_C_HEADER
