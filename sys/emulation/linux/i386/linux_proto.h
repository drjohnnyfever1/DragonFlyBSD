/*
 * System call prototypes.
 *
 * DO NOT EDIT-- To regenerate this file, edit syscalls.master followed
 *               by running make sysent in the same directory.
 */

#ifndef _LINUX_SYSPROTO_H_
#define	_LINUX_SYSPROTO_H_

#include <sys/select.h>

#include <sys/signal.h>

#include <sys/acl.h>

#include <sys/msgport.h>

#include <sys/sysmsg.h>

#include <sys/syslink.h>

#define	PAD_(t)	(sizeof(register_t) <= sizeof(t) ? \
		0 : sizeof(register_t) - sizeof(t))

#define	nosys	linux_nosys
struct	linux_fork_args {
#ifdef _KERNEL
	struct sysmsg sysmsg;
#endif
	register_t dummy;
};
struct	linux_open_args {
#ifdef _KERNEL
	struct sysmsg sysmsg;
#endif
	char *	path;	char path_[PAD_(char *)];
	l_int	flags;	char flags_[PAD_(l_int)];
	l_int	mode;	char mode_[PAD_(l_int)];
};
struct	linux_waitpid_args {
#ifdef _KERNEL
	struct sysmsg sysmsg;
#endif
	l_pid_t	pid;	char pid_[PAD_(l_pid_t)];
	l_int *	status;	char status_[PAD_(l_int *)];
	l_int	options;	char options_[PAD_(l_int)];
};
struct	linux_creat_args {
#ifdef _KERNEL
	struct sysmsg sysmsg;
#endif
	char *	path;	char path_[PAD_(char *)];
	l_int	mode;	char mode_[PAD_(l_int)];
};
struct	linux_link_args {
#ifdef _KERNEL
	struct sysmsg sysmsg;
#endif
	char *	path;	char path_[PAD_(char *)];
	char *	to;	char to_[PAD_(char *)];
};
struct	linux_unlink_args {
#ifdef _KERNEL
	struct sysmsg sysmsg;
#endif
	char *	path;	char path_[PAD_(char *)];
};
struct	linux_execve_args {
#ifdef _KERNEL
	struct sysmsg sysmsg;
#endif
	char *	path;	char path_[PAD_(char *)];
	char **	argp;	char argp_[PAD_(char **)];
	char **	envp;	char envp_[PAD_(char **)];
};
struct	linux_chdir_args {
#ifdef _KERNEL
	struct sysmsg sysmsg;
#endif
	char *	path;	char path_[PAD_(char *)];
};
struct	linux_time_args {
#ifdef _KERNEL
	struct sysmsg sysmsg;
#endif
	l_time_t *	tm;	char tm_[PAD_(l_time_t *)];
};
struct	linux_mknod_args {
#ifdef _KERNEL
	struct sysmsg sysmsg;
#endif
	char *	path;	char path_[PAD_(char *)];
	l_int	mode;	char mode_[PAD_(l_int)];
	l_dev_t	dev;	char dev_[PAD_(l_dev_t)];
};
struct	linux_chmod_args {
#ifdef _KERNEL
	struct sysmsg sysmsg;
#endif
	char *	path;	char path_[PAD_(char *)];
	l_mode_t	mode;	char mode_[PAD_(l_mode_t)];
};
struct	linux_lchown16_args {
#ifdef _KERNEL
	struct sysmsg sysmsg;
#endif
	char *	path;	char path_[PAD_(char *)];
	l_uid16_t	uid;	char uid_[PAD_(l_uid16_t)];
	l_gid16_t	gid;	char gid_[PAD_(l_gid16_t)];
};
struct	linux_stat_args {
#ifdef _KERNEL
	struct sysmsg sysmsg;
#endif
	char *	path;	char path_[PAD_(char *)];
	struct ostat *	statbuf;	char statbuf_[PAD_(struct ostat *)];
};
struct	linux_lseek_args {
#ifdef _KERNEL
	struct sysmsg sysmsg;
#endif
	l_uint	fdes;	char fdes_[PAD_(l_uint)];
	l_off_t	off;	char off_[PAD_(l_off_t)];
	l_int	whence;	char whence_[PAD_(l_int)];
};
struct	linux_getpid_args {
#ifdef _KERNEL
	struct sysmsg sysmsg;
#endif
	register_t dummy;
};
struct	linux_mount_args {
#ifdef _KERNEL
	struct sysmsg sysmsg;
#endif
	char *	specialfile;	char specialfile_[PAD_(char *)];
	char *	dir;	char dir_[PAD_(char *)];
	char *	filesystemtype;	char filesystemtype_[PAD_(char *)];
	l_ulong	rwflag;	char rwflag_[PAD_(l_ulong)];
	void *	data;	char data_[PAD_(void *)];
};
struct	linux_oldumount_args {
#ifdef _KERNEL
	struct sysmsg sysmsg;
#endif
	char *	path;	char path_[PAD_(char *)];
};
struct	linux_setuid16_args {
#ifdef _KERNEL
	struct sysmsg sysmsg;
#endif
	l_uid16_t	uid;	char uid_[PAD_(l_uid16_t)];
};
struct	linux_getuid16_args {
#ifdef _KERNEL
	struct sysmsg sysmsg;
#endif
	register_t dummy;
};
struct	linux_stime_args {
#ifdef _KERNEL
	struct sysmsg sysmsg;
#endif
	register_t dummy;
};
struct	linux_ptrace_args {
#ifdef _KERNEL
	struct sysmsg sysmsg;
#endif
	l_long	req;	char req_[PAD_(l_long)];
	l_long	pid;	char pid_[PAD_(l_long)];
	l_long	addr;	char addr_[PAD_(l_long)];
	l_long	data;	char data_[PAD_(l_long)];
};
struct	linux_alarm_args {
#ifdef _KERNEL
	struct sysmsg sysmsg;
#endif
	l_uint	secs;	char secs_[PAD_(l_uint)];
};
struct	linux_fstat_args {
#ifdef _KERNEL
	struct sysmsg sysmsg;
#endif
	l_uint	fd;	char fd_[PAD_(l_uint)];
	struct ostat *	up;	char up_[PAD_(struct ostat *)];
};
struct	linux_pause_args {
#ifdef _KERNEL
	struct sysmsg sysmsg;
#endif
	register_t dummy;
};
struct	linux_utime_args {
#ifdef _KERNEL
	struct sysmsg sysmsg;
#endif
	char *	fname;	char fname_[PAD_(char *)];
	struct l_utimbuf *	times;	char times_[PAD_(struct l_utimbuf *)];
};
struct	linux_access_args {
#ifdef _KERNEL
	struct sysmsg sysmsg;
#endif
	char *	path;	char path_[PAD_(char *)];
	l_int	flags;	char flags_[PAD_(l_int)];
};
struct	linux_nice_args {
#ifdef _KERNEL
	struct sysmsg sysmsg;
#endif
	l_int	inc;	char inc_[PAD_(l_int)];
};
struct	linux_kill_args {
#ifdef _KERNEL
	struct sysmsg sysmsg;
#endif
	l_int	pid;	char pid_[PAD_(l_int)];
	l_int	signum;	char signum_[PAD_(l_int)];
};
struct	linux_rename_args {
#ifdef _KERNEL
	struct sysmsg sysmsg;
#endif
	char *	from;	char from_[PAD_(char *)];
	char *	to;	char to_[PAD_(char *)];
};
struct	linux_mkdir_args {
#ifdef _KERNEL
	struct sysmsg sysmsg;
#endif
	char *	path;	char path_[PAD_(char *)];
	l_int	mode;	char mode_[PAD_(l_int)];
};
struct	linux_rmdir_args {
#ifdef _KERNEL
	struct sysmsg sysmsg;
#endif
	char *	path;	char path_[PAD_(char *)];
};
struct	linux_pipe_args {
#ifdef _KERNEL
	struct sysmsg sysmsg;
#endif
	l_ulong *	pipefds;	char pipefds_[PAD_(l_ulong *)];
};
struct	linux_times_args {
#ifdef _KERNEL
	struct sysmsg sysmsg;
#endif
	struct l_times_argv *	buf;	char buf_[PAD_(struct l_times_argv *)];
};
struct	linux_brk_args {
#ifdef _KERNEL
	struct sysmsg sysmsg;
#endif
	l_ulong	dsend;	char dsend_[PAD_(l_ulong)];
};
struct	linux_setgid16_args {
#ifdef _KERNEL
	struct sysmsg sysmsg;
#endif
	l_gid16_t	gid;	char gid_[PAD_(l_gid16_t)];
};
struct	linux_getgid16_args {
#ifdef _KERNEL
	struct sysmsg sysmsg;
#endif
	register_t dummy;
};
struct	linux_signal_args {
#ifdef _KERNEL
	struct sysmsg sysmsg;
#endif
	l_int	sig;	char sig_[PAD_(l_int)];
	l_handler_t	handler;	char handler_[PAD_(l_handler_t)];
};
struct	linux_geteuid16_args {
#ifdef _KERNEL
	struct sysmsg sysmsg;
#endif
	register_t dummy;
};
struct	linux_getegid16_args {
#ifdef _KERNEL
	struct sysmsg sysmsg;
#endif
	register_t dummy;
};
struct	linux_umount_args {
#ifdef _KERNEL
	struct sysmsg sysmsg;
#endif
	char *	path;	char path_[PAD_(char *)];
	l_int	flags;	char flags_[PAD_(l_int)];
};
struct	linux_ioctl_args {
#ifdef _KERNEL
	struct sysmsg sysmsg;
#endif
	l_uint	fd;	char fd_[PAD_(l_uint)];
	l_uint	cmd;	char cmd_[PAD_(l_uint)];
	l_ulong	arg;	char arg_[PAD_(l_ulong)];
};
struct	linux_fcntl_args {
#ifdef _KERNEL
	struct sysmsg sysmsg;
#endif
	l_uint	fd;	char fd_[PAD_(l_uint)];
	l_uint	cmd;	char cmd_[PAD_(l_uint)];
	l_ulong	arg;	char arg_[PAD_(l_ulong)];
};
struct	linux_olduname_args {
#ifdef _KERNEL
	struct sysmsg sysmsg;
#endif
	register_t dummy;
};
struct	linux_ustat_args {
#ifdef _KERNEL
	struct sysmsg sysmsg;
#endif
	l_dev_t	dev;	char dev_[PAD_(l_dev_t)];
	struct l_ustat *	ubuf;	char ubuf_[PAD_(struct l_ustat *)];
};
struct	linux_getppid_args {
#ifdef _KERNEL
	struct sysmsg sysmsg;
#endif
	register_t dummy;
};
struct	linux_sigaction_args {
#ifdef _KERNEL
	struct sysmsg sysmsg;
#endif
	l_int	sig;	char sig_[PAD_(l_int)];
	l_osigaction_t *	nsa;	char nsa_[PAD_(l_osigaction_t *)];
	l_osigaction_t *	osa;	char osa_[PAD_(l_osigaction_t *)];
};
struct	linux_sgetmask_args {
#ifdef _KERNEL
	struct sysmsg sysmsg;
#endif
	register_t dummy;
};
struct	linux_ssetmask_args {
#ifdef _KERNEL
	struct sysmsg sysmsg;
#endif
	l_osigset_t	mask;	char mask_[PAD_(l_osigset_t)];
};
struct	linux_setreuid16_args {
#ifdef _KERNEL
	struct sysmsg sysmsg;
#endif
	l_uid16_t	ruid;	char ruid_[PAD_(l_uid16_t)];
	l_uid16_t	euid;	char euid_[PAD_(l_uid16_t)];
};
struct	linux_setregid16_args {
#ifdef _KERNEL
	struct sysmsg sysmsg;
#endif
	l_gid16_t	rgid;	char rgid_[PAD_(l_gid16_t)];
	l_gid16_t	egid;	char egid_[PAD_(l_gid16_t)];
};
struct	linux_sigsuspend_args {
#ifdef _KERNEL
	struct sysmsg sysmsg;
#endif
	l_int	hist0;	char hist0_[PAD_(l_int)];
	l_int	hist1;	char hist1_[PAD_(l_int)];
	l_osigset_t	mask;	char mask_[PAD_(l_osigset_t)];
};
struct	linux_sigpending_args {
#ifdef _KERNEL
	struct sysmsg sysmsg;
#endif
	l_osigset_t *	mask;	char mask_[PAD_(l_osigset_t *)];
};
struct	linux_sethostname_args {
#ifdef _KERNEL
	struct sysmsg sysmsg;
#endif
	char *	hostname;	char hostname_[PAD_(char *)];
	u_int	len;	char len_[PAD_(u_int)];
};
struct	linux_setrlimit_args {
#ifdef _KERNEL
	struct sysmsg sysmsg;
#endif
	l_uint	resource;	char resource_[PAD_(l_uint)];
	struct l_rlimit *	rlim;	char rlim_[PAD_(struct l_rlimit *)];
};
struct	linux_old_getrlimit_args {
#ifdef _KERNEL
	struct sysmsg sysmsg;
#endif
	l_uint	resource;	char resource_[PAD_(l_uint)];
	struct l_rlimit *	rlim;	char rlim_[PAD_(struct l_rlimit *)];
};
struct	linux_getgroups16_args {
#ifdef _KERNEL
	struct sysmsg sysmsg;
#endif
	l_uint	gidsetsize;	char gidsetsize_[PAD_(l_uint)];
	l_gid16_t *	gidset;	char gidset_[PAD_(l_gid16_t *)];
};
struct	linux_setgroups16_args {
#ifdef _KERNEL
	struct sysmsg sysmsg;
#endif
	l_uint	gidsetsize;	char gidsetsize_[PAD_(l_uint)];
	l_gid16_t *	gidset;	char gidset_[PAD_(l_gid16_t *)];
};
struct	linux_old_select_args {
#ifdef _KERNEL
	struct sysmsg sysmsg;
#endif
	struct l_old_select_argv *	ptr;	char ptr_[PAD_(struct l_old_select_argv *)];
};
struct	linux_symlink_args {
#ifdef _KERNEL
	struct sysmsg sysmsg;
#endif
	char *	path;	char path_[PAD_(char *)];
	char *	to;	char to_[PAD_(char *)];
};
struct	linux_ostat_args {
#ifdef _KERNEL
	struct sysmsg sysmsg;
#endif
	char *	path;	char path_[PAD_(char *)];
	struct ostat *	statbuf;	char statbuf_[PAD_(struct ostat *)];
};
struct	linux_readlink_args {
#ifdef _KERNEL
	struct sysmsg sysmsg;
#endif
	char *	name;	char name_[PAD_(char *)];
	char *	buf;	char buf_[PAD_(char *)];
	l_int	count;	char count_[PAD_(l_int)];
};
struct	linux_uselib_args {
#ifdef _KERNEL
	struct sysmsg sysmsg;
#endif
	char *	library;	char library_[PAD_(char *)];
};
struct	linux_reboot_args {
#ifdef _KERNEL
	struct sysmsg sysmsg;
#endif
	l_int	magic1;	char magic1_[PAD_(l_int)];
	l_int	magic2;	char magic2_[PAD_(l_int)];
	l_uint	cmd;	char cmd_[PAD_(l_uint)];
	void *	arg;	char arg_[PAD_(void *)];
};
struct	linux_readdir_args {
#ifdef _KERNEL
	struct sysmsg sysmsg;
#endif
	l_uint	fd;	char fd_[PAD_(l_uint)];
	struct l_dirent *	dent;	char dent_[PAD_(struct l_dirent *)];
	l_uint	count;	char count_[PAD_(l_uint)];
};
struct	linux_mmap_args {
#ifdef _KERNEL
	struct sysmsg sysmsg;
#endif
	struct l_mmap_argv *	ptr;	char ptr_[PAD_(struct l_mmap_argv *)];
};
struct	linux_truncate_args {
#ifdef _KERNEL
	struct sysmsg sysmsg;
#endif
	char *	path;	char path_[PAD_(char *)];
	l_ulong	length;	char length_[PAD_(l_ulong)];
};
struct	linux_ftruncate_args {
#ifdef _KERNEL
	struct sysmsg sysmsg;
#endif
	int	fd;	char fd_[PAD_(int)];
	long	length;	char length_[PAD_(long)];
};
struct	linux_statfs_args {
#ifdef _KERNEL
	struct sysmsg sysmsg;
#endif
	char *	path;	char path_[PAD_(char *)];
	struct l_statfs_buf *	buf;	char buf_[PAD_(struct l_statfs_buf *)];
};
struct	linux_fstatfs_args {
#ifdef _KERNEL
	struct sysmsg sysmsg;
#endif
	l_uint	fd;	char fd_[PAD_(l_uint)];
	struct l_statfs_buf *	buf;	char buf_[PAD_(struct l_statfs_buf *)];
};
struct	linux_ioperm_args {
#ifdef _KERNEL
	struct sysmsg sysmsg;
#endif
	l_ulong	start;	char start_[PAD_(l_ulong)];
	l_ulong	length;	char length_[PAD_(l_ulong)];
	l_int	enable;	char enable_[PAD_(l_int)];
};
struct	linux_socketcall_args {
#ifdef _KERNEL
	struct sysmsg sysmsg;
#endif
	l_int	what;	char what_[PAD_(l_int)];
	l_ulong	args;	char args_[PAD_(l_ulong)];
};
struct	linux_syslog_args {
#ifdef _KERNEL
	struct sysmsg sysmsg;
#endif
	l_int	type;	char type_[PAD_(l_int)];
	char *	buf;	char buf_[PAD_(char *)];
	l_int	len;	char len_[PAD_(l_int)];
};
struct	linux_setitimer_args {
#ifdef _KERNEL
	struct sysmsg sysmsg;
#endif
	l_int	which;	char which_[PAD_(l_int)];
	struct l_itimerval *	itv;	char itv_[PAD_(struct l_itimerval *)];
	struct l_itimerval *	oitv;	char oitv_[PAD_(struct l_itimerval *)];
};
struct	linux_getitimer_args {
#ifdef _KERNEL
	struct sysmsg sysmsg;
#endif
	l_int	which;	char which_[PAD_(l_int)];
	struct l_itimerval *	itv;	char itv_[PAD_(struct l_itimerval *)];
};
struct	linux_newstat_args {
#ifdef _KERNEL
	struct sysmsg sysmsg;
#endif
	char *	path;	char path_[PAD_(char *)];
	struct l_newstat *	buf;	char buf_[PAD_(struct l_newstat *)];
};
struct	linux_newlstat_args {
#ifdef _KERNEL
	struct sysmsg sysmsg;
#endif
	char *	path;	char path_[PAD_(char *)];
	struct l_newstat *	buf;	char buf_[PAD_(struct l_newstat *)];
};
struct	linux_newfstat_args {
#ifdef _KERNEL
	struct sysmsg sysmsg;
#endif
	l_uint	fd;	char fd_[PAD_(l_uint)];
	struct l_newstat *	buf;	char buf_[PAD_(struct l_newstat *)];
};
struct	linux_uname_args {
#ifdef _KERNEL
	struct sysmsg sysmsg;
#endif
	register_t dummy;
};
struct	linux_iopl_args {
#ifdef _KERNEL
	struct sysmsg sysmsg;
#endif
	l_ulong	level;	char level_[PAD_(l_ulong)];
};
struct	linux_vhangup_args {
#ifdef _KERNEL
	struct sysmsg sysmsg;
#endif
	register_t dummy;
};
struct	linux_vm86old_args {
#ifdef _KERNEL
	struct sysmsg sysmsg;
#endif
	register_t dummy;
};
struct	linux_wait4_args {
#ifdef _KERNEL
	struct sysmsg sysmsg;
#endif
	l_pid_t	pid;	char pid_[PAD_(l_pid_t)];
	l_uint *	status;	char status_[PAD_(l_uint *)];
	l_int	options;	char options_[PAD_(l_int)];
	struct l_rusage *	rusage;	char rusage_[PAD_(struct l_rusage *)];
};
struct	linux_swapoff_args {
#ifdef _KERNEL
	struct sysmsg sysmsg;
#endif
	register_t dummy;
};
struct	linux_sysinfo_args {
#ifdef _KERNEL
	struct sysmsg sysmsg;
#endif
	struct l_sysinfo *	info;	char info_[PAD_(struct l_sysinfo *)];
};
struct	linux_ipc_args {
#ifdef _KERNEL
	struct sysmsg sysmsg;
#endif
	l_uint	what;	char what_[PAD_(l_uint)];
	l_int	arg1;	char arg1_[PAD_(l_int)];
	l_int	arg2;	char arg2_[PAD_(l_int)];
	l_int	arg3;	char arg3_[PAD_(l_int)];
	void *	ptr;	char ptr_[PAD_(void *)];
	l_long	arg5;	char arg5_[PAD_(l_long)];
};
struct	linux_sigreturn_args {
#ifdef _KERNEL
	struct sysmsg sysmsg;
#endif
	struct l_sigframe *	sfp;	char sfp_[PAD_(struct l_sigframe *)];
};
struct	linux_clone_args {
#ifdef _KERNEL
	struct sysmsg sysmsg;
#endif
	l_int	flags;	char flags_[PAD_(l_int)];
	void *	stack;	char stack_[PAD_(void *)];
	void *	parent_tidptr;	char parent_tidptr_[PAD_(void *)];
	int	dummy;	char dummy_[PAD_(int)];
	void *	child_tidptr;	char child_tidptr_[PAD_(void *)];
};
struct	linux_newuname_args {
#ifdef _KERNEL
	struct sysmsg sysmsg;
#endif
	struct l_new_utsname *	buf;	char buf_[PAD_(struct l_new_utsname *)];
};
struct	linux_modify_ldt_args {
#ifdef _KERNEL
	struct sysmsg sysmsg;
#endif
	l_int	func;	char func_[PAD_(l_int)];
	void *	ptr;	char ptr_[PAD_(void *)];
	l_ulong	bytecount;	char bytecount_[PAD_(l_ulong)];
};
struct	linux_adjtimex_args {
#ifdef _KERNEL
	struct sysmsg sysmsg;
#endif
	register_t dummy;
};
struct	linux_sigprocmask_args {
#ifdef _KERNEL
	struct sysmsg sysmsg;
#endif
	l_int	how;	char how_[PAD_(l_int)];
	l_osigset_t *	mask;	char mask_[PAD_(l_osigset_t *)];
	l_osigset_t *	omask;	char omask_[PAD_(l_osigset_t *)];
};
struct	linux_create_module_args {
#ifdef _KERNEL
	struct sysmsg sysmsg;
#endif
	register_t dummy;
};
struct	linux_init_module_args {
#ifdef _KERNEL
	struct sysmsg sysmsg;
#endif
	register_t dummy;
};
struct	linux_delete_module_args {
#ifdef _KERNEL
	struct sysmsg sysmsg;
#endif
	register_t dummy;
};
struct	linux_get_kernel_syms_args {
#ifdef _KERNEL
	struct sysmsg sysmsg;
#endif
	register_t dummy;
};
struct	linux_quotactl_args {
#ifdef _KERNEL
	struct sysmsg sysmsg;
#endif
	register_t dummy;
};
struct	linux_bdflush_args {
#ifdef _KERNEL
	struct sysmsg sysmsg;
#endif
	register_t dummy;
};
struct	linux_sysfs_args {
#ifdef _KERNEL
	struct sysmsg sysmsg;
#endif
	l_int	option;	char option_[PAD_(l_int)];
	l_ulong	arg1;	char arg1_[PAD_(l_ulong)];
	l_ulong	arg2;	char arg2_[PAD_(l_ulong)];
};
struct	linux_personality_args {
#ifdef _KERNEL
	struct sysmsg sysmsg;
#endif
	l_ulong	per;	char per_[PAD_(l_ulong)];
};
struct	linux_setfsuid16_args {
#ifdef _KERNEL
	struct sysmsg sysmsg;
#endif
	l_uid16_t	uid;	char uid_[PAD_(l_uid16_t)];
};
struct	linux_setfsgid16_args {
#ifdef _KERNEL
	struct sysmsg sysmsg;
#endif
	l_gid16_t	gid;	char gid_[PAD_(l_gid16_t)];
};
struct	linux_llseek_args {
#ifdef _KERNEL
	struct sysmsg sysmsg;
#endif
	l_int	fd;	char fd_[PAD_(l_int)];
	l_ulong	ohigh;	char ohigh_[PAD_(l_ulong)];
	l_ulong	olow;	char olow_[PAD_(l_ulong)];
	l_loff_t *	res;	char res_[PAD_(l_loff_t *)];
	l_uint	whence;	char whence_[PAD_(l_uint)];
};
struct	linux_getdents_args {
#ifdef _KERNEL
	struct sysmsg sysmsg;
#endif
	l_uint	fd;	char fd_[PAD_(l_uint)];
	void *	dent;	char dent_[PAD_(void *)];
	l_uint	count;	char count_[PAD_(l_uint)];
};
struct	linux_select_args {
#ifdef _KERNEL
	struct sysmsg sysmsg;
#endif
	l_int	nfds;	char nfds_[PAD_(l_int)];
	l_fd_set *	readfds;	char readfds_[PAD_(l_fd_set *)];
	l_fd_set *	writefds;	char writefds_[PAD_(l_fd_set *)];
	l_fd_set *	exceptfds;	char exceptfds_[PAD_(l_fd_set *)];
	struct l_timeval *	timeout;	char timeout_[PAD_(struct l_timeval *)];
};
struct	linux_msync_args {
#ifdef _KERNEL
	struct sysmsg sysmsg;
#endif
	l_ulong	addr;	char addr_[PAD_(l_ulong)];
	l_size_t	len;	char len_[PAD_(l_size_t)];
	l_int	fl;	char fl_[PAD_(l_int)];
};
struct	linux_getsid_args {
#ifdef _KERNEL
	struct sysmsg sysmsg;
#endif
	l_pid_t	pid;	char pid_[PAD_(l_pid_t)];
};
struct	linux_fdatasync_args {
#ifdef _KERNEL
	struct sysmsg sysmsg;
#endif
	l_uint	fd;	char fd_[PAD_(l_uint)];
};
struct	linux_sysctl_args {
#ifdef _KERNEL
	struct sysmsg sysmsg;
#endif
	struct l___sysctl_args *	args;	char args_[PAD_(struct l___sysctl_args *)];
};
struct	linux_sched_setscheduler_args {
#ifdef _KERNEL
	struct sysmsg sysmsg;
#endif
	l_pid_t	pid;	char pid_[PAD_(l_pid_t)];
	l_int	policy;	char policy_[PAD_(l_int)];
	struct l_sched_param *	param;	char param_[PAD_(struct l_sched_param *)];
};
struct	linux_sched_getscheduler_args {
#ifdef _KERNEL
	struct sysmsg sysmsg;
#endif
	l_pid_t	pid;	char pid_[PAD_(l_pid_t)];
};
struct	linux_sched_get_priority_max_args {
#ifdef _KERNEL
	struct sysmsg sysmsg;
#endif
	l_int	policy;	char policy_[PAD_(l_int)];
};
struct	linux_sched_get_priority_min_args {
#ifdef _KERNEL
	struct sysmsg sysmsg;
#endif
	l_int	policy;	char policy_[PAD_(l_int)];
};
struct	linux_mremap_args {
#ifdef _KERNEL
	struct sysmsg sysmsg;
#endif
	l_ulong	addr;	char addr_[PAD_(l_ulong)];
	l_ulong	old_len;	char old_len_[PAD_(l_ulong)];
	l_ulong	new_len;	char new_len_[PAD_(l_ulong)];
	l_ulong	flags;	char flags_[PAD_(l_ulong)];
	l_ulong	new_addr;	char new_addr_[PAD_(l_ulong)];
};
struct	linux_setresuid16_args {
#ifdef _KERNEL
	struct sysmsg sysmsg;
#endif
	l_uid16_t	ruid;	char ruid_[PAD_(l_uid16_t)];
	l_uid16_t	euid;	char euid_[PAD_(l_uid16_t)];
	l_uid16_t	suid;	char suid_[PAD_(l_uid16_t)];
};
struct	linux_getresuid16_args {
#ifdef _KERNEL
	struct sysmsg sysmsg;
#endif
	l_uid16_t *	ruid;	char ruid_[PAD_(l_uid16_t *)];
	l_uid16_t *	euid;	char euid_[PAD_(l_uid16_t *)];
	l_uid16_t *	suid;	char suid_[PAD_(l_uid16_t *)];
};
struct	linux_vm86_args {
#ifdef _KERNEL
	struct sysmsg sysmsg;
#endif
	register_t dummy;
};
struct	linux_query_module_args {
#ifdef _KERNEL
	struct sysmsg sysmsg;
#endif
	register_t dummy;
};
struct	linux_nfsservctl_args {
#ifdef _KERNEL
	struct sysmsg sysmsg;
#endif
	register_t dummy;
};
struct	linux_setresgid16_args {
#ifdef _KERNEL
	struct sysmsg sysmsg;
#endif
	l_gid16_t	rgid;	char rgid_[PAD_(l_gid16_t)];
	l_gid16_t	egid;	char egid_[PAD_(l_gid16_t)];
	l_gid16_t	sgid;	char sgid_[PAD_(l_gid16_t)];
};
struct	linux_getresgid16_args {
#ifdef _KERNEL
	struct sysmsg sysmsg;
#endif
	l_gid16_t *	rgid;	char rgid_[PAD_(l_gid16_t *)];
	l_gid16_t *	egid;	char egid_[PAD_(l_gid16_t *)];
	l_gid16_t *	sgid;	char sgid_[PAD_(l_gid16_t *)];
};
struct	linux_prctl_args {
#ifdef _KERNEL
	struct sysmsg sysmsg;
#endif
	register_t dummy;
};
struct	linux_rt_sigreturn_args {
#ifdef _KERNEL
	struct sysmsg sysmsg;
#endif
	struct l_ucontext *	ucp;	char ucp_[PAD_(struct l_ucontext *)];
};
struct	linux_rt_sigaction_args {
#ifdef _KERNEL
	struct sysmsg sysmsg;
#endif
	l_int	sig;	char sig_[PAD_(l_int)];
	l_sigaction_t *	act;	char act_[PAD_(l_sigaction_t *)];
	l_sigaction_t *	oact;	char oact_[PAD_(l_sigaction_t *)];
	l_size_t	sigsetsize;	char sigsetsize_[PAD_(l_size_t)];
};
struct	linux_rt_sigprocmask_args {
#ifdef _KERNEL
	struct sysmsg sysmsg;
#endif
	l_int	how;	char how_[PAD_(l_int)];
	l_sigset_t *	mask;	char mask_[PAD_(l_sigset_t *)];
	l_sigset_t *	omask;	char omask_[PAD_(l_sigset_t *)];
	l_size_t	sigsetsize;	char sigsetsize_[PAD_(l_size_t)];
};
struct	linux_rt_sigpending_args {
#ifdef _KERNEL
	struct sysmsg sysmsg;
#endif
	register_t dummy;
};
struct	linux_rt_sigtimedwait_args {
#ifdef _KERNEL
	struct sysmsg sysmsg;
#endif
	register_t dummy;
};
struct	linux_rt_sigqueueinfo_args {
#ifdef _KERNEL
	struct sysmsg sysmsg;
#endif
	register_t dummy;
};
struct	linux_rt_sigsuspend_args {
#ifdef _KERNEL
	struct sysmsg sysmsg;
#endif
	l_sigset_t *	newset;	char newset_[PAD_(l_sigset_t *)];
	l_size_t	sigsetsize;	char sigsetsize_[PAD_(l_size_t)];
};
struct	linux_pread_args {
#ifdef _KERNEL
	struct sysmsg sysmsg;
#endif
	l_uint	fd;	char fd_[PAD_(l_uint)];
	char *	buf;	char buf_[PAD_(char *)];
	l_size_t	nbyte;	char nbyte_[PAD_(l_size_t)];
	l_loff_t	offset;	char offset_[PAD_(l_loff_t)];
};
struct	linux_pwrite_args {
#ifdef _KERNEL
	struct sysmsg sysmsg;
#endif
	l_uint	fd;	char fd_[PAD_(l_uint)];
	char *	buf;	char buf_[PAD_(char *)];
	l_size_t	nbyte;	char nbyte_[PAD_(l_size_t)];
	l_loff_t	offset;	char offset_[PAD_(l_loff_t)];
};
struct	linux_chown16_args {
#ifdef _KERNEL
	struct sysmsg sysmsg;
#endif
	char *	path;	char path_[PAD_(char *)];
	l_uid16_t	uid;	char uid_[PAD_(l_uid16_t)];
	l_gid16_t	gid;	char gid_[PAD_(l_gid16_t)];
};
struct	linux_getcwd_args {
#ifdef _KERNEL
	struct sysmsg sysmsg;
#endif
	char *	buf;	char buf_[PAD_(char *)];
	l_ulong	bufsize;	char bufsize_[PAD_(l_ulong)];
};
struct	linux_capget_args {
#ifdef _KERNEL
	struct sysmsg sysmsg;
#endif
	register_t dummy;
};
struct	linux_capset_args {
#ifdef _KERNEL
	struct sysmsg sysmsg;
#endif
	register_t dummy;
};
struct	linux_sigaltstack_args {
#ifdef _KERNEL
	struct sysmsg sysmsg;
#endif
	l_stack_t *	uss;	char uss_[PAD_(l_stack_t *)];
	l_stack_t *	uoss;	char uoss_[PAD_(l_stack_t *)];
};
struct	linux_sendfile_args {
#ifdef _KERNEL
	struct sysmsg sysmsg;
#endif
	register_t dummy;
};
struct	linux_vfork_args {
#ifdef _KERNEL
	struct sysmsg sysmsg;
#endif
	register_t dummy;
};
struct	linux_getrlimit_args {
#ifdef _KERNEL
	struct sysmsg sysmsg;
#endif
	l_uint	resource;	char resource_[PAD_(l_uint)];
	struct l_rlimit *	rlim;	char rlim_[PAD_(struct l_rlimit *)];
};
struct	linux_mmap2_args {
#ifdef _KERNEL
	struct sysmsg sysmsg;
#endif
	l_ulong	addr;	char addr_[PAD_(l_ulong)];
	l_ulong	len;	char len_[PAD_(l_ulong)];
	l_ulong	prot;	char prot_[PAD_(l_ulong)];
	l_ulong	flags;	char flags_[PAD_(l_ulong)];
	l_ulong	fd;	char fd_[PAD_(l_ulong)];
	l_ulong	pgoff;	char pgoff_[PAD_(l_ulong)];
};
struct	linux_truncate64_args {
#ifdef _KERNEL
	struct sysmsg sysmsg;
#endif
	char *	path;	char path_[PAD_(char *)];
	l_loff_t	length;	char length_[PAD_(l_loff_t)];
};
struct	linux_ftruncate64_args {
#ifdef _KERNEL
	struct sysmsg sysmsg;
#endif
	l_uint	fd;	char fd_[PAD_(l_uint)];
	l_loff_t	length;	char length_[PAD_(l_loff_t)];
};
struct	linux_stat64_args {
#ifdef _KERNEL
	struct sysmsg sysmsg;
#endif
	char *	filename;	char filename_[PAD_(char *)];
	struct l_stat64 *	statbuf;	char statbuf_[PAD_(struct l_stat64 *)];
	l_long	flags;	char flags_[PAD_(l_long)];
};
struct	linux_lstat64_args {
#ifdef _KERNEL
	struct sysmsg sysmsg;
#endif
	char *	filename;	char filename_[PAD_(char *)];
	struct l_stat64 *	statbuf;	char statbuf_[PAD_(struct l_stat64 *)];
	l_long	flags;	char flags_[PAD_(l_long)];
};
struct	linux_fstat64_args {
#ifdef _KERNEL
	struct sysmsg sysmsg;
#endif
	l_ulong	fd;	char fd_[PAD_(l_ulong)];
	struct l_stat64 *	statbuf;	char statbuf_[PAD_(struct l_stat64 *)];
	l_long	flags;	char flags_[PAD_(l_long)];
};
struct	linux_lchown_args {
#ifdef _KERNEL
	struct sysmsg sysmsg;
#endif
	char *	path;	char path_[PAD_(char *)];
	l_uid_t	uid;	char uid_[PAD_(l_uid_t)];
	l_gid_t	gid;	char gid_[PAD_(l_gid_t)];
};
struct	linux_getuid_args {
#ifdef _KERNEL
	struct sysmsg sysmsg;
#endif
	register_t dummy;
};
struct	linux_getgid_args {
#ifdef _KERNEL
	struct sysmsg sysmsg;
#endif
	register_t dummy;
};
struct	linux_getgroups_args {
#ifdef _KERNEL
	struct sysmsg sysmsg;
#endif
	l_int	gidsetsize;	char gidsetsize_[PAD_(l_int)];
	l_gid_t *	grouplist;	char grouplist_[PAD_(l_gid_t *)];
};
struct	linux_setgroups_args {
#ifdef _KERNEL
	struct sysmsg sysmsg;
#endif
	l_int	gidsetsize;	char gidsetsize_[PAD_(l_int)];
	l_gid_t *	grouplist;	char grouplist_[PAD_(l_gid_t *)];
};
struct	linux_chown_args {
#ifdef _KERNEL
	struct sysmsg sysmsg;
#endif
	char *	path;	char path_[PAD_(char *)];
	l_uid_t	uid;	char uid_[PAD_(l_uid_t)];
	l_gid_t	gid;	char gid_[PAD_(l_gid_t)];
};
struct	linux_setfsuid_args {
#ifdef _KERNEL
	struct sysmsg sysmsg;
#endif
	l_uid_t	uid;	char uid_[PAD_(l_uid_t)];
};
struct	linux_setfsgid_args {
#ifdef _KERNEL
	struct sysmsg sysmsg;
#endif
	l_gid_t	gid;	char gid_[PAD_(l_gid_t)];
};
struct	linux_pivot_root_args {
#ifdef _KERNEL
	struct sysmsg sysmsg;
#endif
	char *	new_root;	char new_root_[PAD_(char *)];
	char *	put_old;	char put_old_[PAD_(char *)];
};
struct	linux_mincore_args {
#ifdef _KERNEL
	struct sysmsg sysmsg;
#endif
	l_ulong	start;	char start_[PAD_(l_ulong)];
	l_size_t	len;	char len_[PAD_(l_size_t)];
	u_char *	vec;	char vec_[PAD_(u_char *)];
};
struct	linux_madvise_args {
#ifdef _KERNEL
	struct sysmsg sysmsg;
#endif
	register_t dummy;
};
struct	linux_getdents64_args {
#ifdef _KERNEL
	struct sysmsg sysmsg;
#endif
	l_uint	fd;	char fd_[PAD_(l_uint)];
	void *	dirent;	char dirent_[PAD_(void *)];
	l_uint	count;	char count_[PAD_(l_uint)];
};
struct	linux_fcntl64_args {
#ifdef _KERNEL
	struct sysmsg sysmsg;
#endif
	l_uint	fd;	char fd_[PAD_(l_uint)];
	l_uint	cmd;	char cmd_[PAD_(l_uint)];
	l_ulong	arg;	char arg_[PAD_(l_ulong)];
};
struct	linux_gettid_args {
#ifdef _KERNEL
	struct sysmsg sysmsg;
#endif
	register_t dummy;
};
struct	linux_setxattr_args {
#ifdef _KERNEL
	struct sysmsg sysmsg;
#endif
	register_t dummy;
};
struct	linux_lsetxattr_args {
#ifdef _KERNEL
	struct sysmsg sysmsg;
#endif
	register_t dummy;
};
struct	linux_fsetxattr_args {
#ifdef _KERNEL
	struct sysmsg sysmsg;
#endif
	register_t dummy;
};
struct	linux_getxattr_args {
#ifdef _KERNEL
	struct sysmsg sysmsg;
#endif
	register_t dummy;
};
struct	linux_lgetxattr_args {
#ifdef _KERNEL
	struct sysmsg sysmsg;
#endif
	register_t dummy;
};
struct	linux_fgetxattr_args {
#ifdef _KERNEL
	struct sysmsg sysmsg;
#endif
	register_t dummy;
};
struct	linux_listxattr_args {
#ifdef _KERNEL
	struct sysmsg sysmsg;
#endif
	register_t dummy;
};
struct	linux_llistxattr_args {
#ifdef _KERNEL
	struct sysmsg sysmsg;
#endif
	register_t dummy;
};
struct	linux_flistxattr_args {
#ifdef _KERNEL
	struct sysmsg sysmsg;
#endif
	register_t dummy;
};
struct	linux_removexattr_args {
#ifdef _KERNEL
	struct sysmsg sysmsg;
#endif
	register_t dummy;
};
struct	linux_lremovexattr_args {
#ifdef _KERNEL
	struct sysmsg sysmsg;
#endif
	register_t dummy;
};
struct	linux_fremovexattr_args {
#ifdef _KERNEL
	struct sysmsg sysmsg;
#endif
	register_t dummy;
};
struct	linux_tkill_args {
#ifdef _KERNEL
	struct sysmsg sysmsg;
#endif
	int	tid;	char tid_[PAD_(int)];
	int	sig;	char sig_[PAD_(int)];
};
struct	linux_sys_futex_args {
#ifdef _KERNEL
	struct sysmsg sysmsg;
#endif
	void *	uaddr;	char uaddr_[PAD_(void *)];
	int	op;	char op_[PAD_(int)];
	int	val;	char val_[PAD_(int)];
	struct l_timespec *	timeout;	char timeout_[PAD_(struct l_timespec *)];
	void *	uaddr2;	char uaddr2_[PAD_(void *)];
	int	val3;	char val3_[PAD_(int)];
};
struct	linux_sched_setaffinity_args {
#ifdef _KERNEL
	struct sysmsg sysmsg;
#endif
	l_pid_t	pid;	char pid_[PAD_(l_pid_t)];
	l_uint	len;	char len_[PAD_(l_uint)];
	l_ulong *	user_mask_ptr;	char user_mask_ptr_[PAD_(l_ulong *)];
};
struct	linux_sched_getaffinity_args {
#ifdef _KERNEL
	struct sysmsg sysmsg;
#endif
	l_pid_t	pid;	char pid_[PAD_(l_pid_t)];
	l_uint	len;	char len_[PAD_(l_uint)];
	l_ulong *	user_mask_ptr;	char user_mask_ptr_[PAD_(l_ulong *)];
};
struct	linux_set_thread_area_args {
#ifdef _KERNEL
	struct sysmsg sysmsg;
#endif
	struct l_user_desc *	desc;	char desc_[PAD_(struct l_user_desc *)];
};
struct	linux_get_thread_area_args {
#ifdef _KERNEL
	struct sysmsg sysmsg;
#endif
	struct l_user_desc *	desc;	char desc_[PAD_(struct l_user_desc *)];
};
struct	linux_fadvise64_args {
#ifdef _KERNEL
	struct sysmsg sysmsg;
#endif
	register_t dummy;
};
struct	linux_exit_group_args {
#ifdef _KERNEL
	struct sysmsg sysmsg;
#endif
	int	rval;	char rval_[PAD_(int)];
};
struct	linux_epoll_create_args {
#ifdef _KERNEL
	struct sysmsg sysmsg;
#endif
	l_int	size;	char size_[PAD_(l_int)];
};
struct	linux_epoll_ctl_args {
#ifdef _KERNEL
	struct sysmsg sysmsg;
#endif
	l_int	epfd;	char epfd_[PAD_(l_int)];
	l_int	op;	char op_[PAD_(l_int)];
	l_int	fd;	char fd_[PAD_(l_int)];
	struct linux_epoll_event *	event;	char event_[PAD_(struct linux_epoll_event *)];
};
struct	linux_epoll_wait_args {
#ifdef _KERNEL
	struct sysmsg sysmsg;
#endif
	l_int	epfd;	char epfd_[PAD_(l_int)];
	struct linux_epoll_event *	events;	char events_[PAD_(struct linux_epoll_event *)];
	l_int	maxevents;	char maxevents_[PAD_(l_int)];
	l_int	timeout;	char timeout_[PAD_(l_int)];
};
struct	linux_set_tid_address_args {
#ifdef _KERNEL
	struct sysmsg sysmsg;
#endif
	int *	tidptr;	char tidptr_[PAD_(int *)];
};
struct	linux_clock_settime_args {
#ifdef _KERNEL
	struct sysmsg sysmsg;
#endif
	clockid_t	which;	char which_[PAD_(clockid_t)];
	struct l_timespec *	tp;	char tp_[PAD_(struct l_timespec *)];
};
struct	linux_clock_gettime_args {
#ifdef _KERNEL
	struct sysmsg sysmsg;
#endif
	clockid_t	which;	char which_[PAD_(clockid_t)];
	struct l_timespec *	tp;	char tp_[PAD_(struct l_timespec *)];
};
struct	linux_clock_getres_args {
#ifdef _KERNEL
	struct sysmsg sysmsg;
#endif
	clockid_t	which;	char which_[PAD_(clockid_t)];
	struct l_timespec *	tp;	char tp_[PAD_(struct l_timespec *)];
};
struct	linux_clock_nanosleep_args {
#ifdef _KERNEL
	struct sysmsg sysmsg;
#endif
	clockid_t	which;	char which_[PAD_(clockid_t)];
	int	flags;	char flags_[PAD_(int)];
	struct l_timespec *	rqtp;	char rqtp_[PAD_(struct l_timespec *)];
	struct l_timespec *	rmtp;	char rmtp_[PAD_(struct l_timespec *)];
};
struct	linux_statfs64_args {
#ifdef _KERNEL
	struct sysmsg sysmsg;
#endif
	register_t dummy;
};
struct	linux_fstatfs64_args {
#ifdef _KERNEL
	struct sysmsg sysmsg;
#endif
	register_t dummy;
};
struct	linux_tgkill_args {
#ifdef _KERNEL
	struct sysmsg sysmsg;
#endif
	int	tgid;	char tgid_[PAD_(int)];
	int	pid;	char pid_[PAD_(int)];
	int	sig;	char sig_[PAD_(int)];
};
struct	linux_utimes_args {
#ifdef _KERNEL
	struct sysmsg sysmsg;
#endif
	char *	fname;	char fname_[PAD_(char *)];
	struct l_timeval *	tptr;	char tptr_[PAD_(struct l_timeval *)];
};
struct	linux_fadvise64_64_args {
#ifdef _KERNEL
	struct sysmsg sysmsg;
#endif
	register_t dummy;
};
struct	linux_mbind_args {
#ifdef _KERNEL
	struct sysmsg sysmsg;
#endif
	register_t dummy;
};
struct	linux_get_mempolicy_args {
#ifdef _KERNEL
	struct sysmsg sysmsg;
#endif
	register_t dummy;
};
struct	linux_set_mempolicy_args {
#ifdef _KERNEL
	struct sysmsg sysmsg;
#endif
	register_t dummy;
};
struct	linux_mq_open_args {
#ifdef _KERNEL
	struct sysmsg sysmsg;
#endif
	const char *	name;	char name_[PAD_(const char *)];
	int	oflag;	char oflag_[PAD_(int)];
	mode_t	mode;	char mode_[PAD_(mode_t)];
	struct mq_attr *	attr;	char attr_[PAD_(struct mq_attr *)];
};
struct	linux_mq_getsetattr_args {
#ifdef _KERNEL
	struct sysmsg sysmsg;
#endif
	l_mqd_t	mqd;	char mqd_[PAD_(l_mqd_t)];
	const struct mq_attr *	attr;	char attr_[PAD_(const struct mq_attr *)];
	struct mq_attr *	oattr;	char oattr_[PAD_(struct mq_attr *)];
};
struct	linux_kexec_load_args {
#ifdef _KERNEL
	struct sysmsg sysmsg;
#endif
	register_t dummy;
};
struct	linux_waitid_args {
#ifdef _KERNEL
	struct sysmsg sysmsg;
#endif
	register_t dummy;
};
struct	linux_add_key_args {
#ifdef _KERNEL
	struct sysmsg sysmsg;
#endif
	register_t dummy;
};
struct	linux_request_key_args {
#ifdef _KERNEL
	struct sysmsg sysmsg;
#endif
	register_t dummy;
};
struct	linux_keyctl_args {
#ifdef _KERNEL
	struct sysmsg sysmsg;
#endif
	register_t dummy;
};
struct	linux_ioprio_set_args {
#ifdef _KERNEL
	struct sysmsg sysmsg;
#endif
	register_t dummy;
};
struct	linux_ioprio_get_args {
#ifdef _KERNEL
	struct sysmsg sysmsg;
#endif
	register_t dummy;
};
struct	linux_inotify_init_args {
#ifdef _KERNEL
	struct sysmsg sysmsg;
#endif
	register_t dummy;
};
struct	linux_inotify_add_watch_args {
#ifdef _KERNEL
	struct sysmsg sysmsg;
#endif
	register_t dummy;
};
struct	linux_inotify_rm_watch_args {
#ifdef _KERNEL
	struct sysmsg sysmsg;
#endif
	register_t dummy;
};
struct	linux_migrate_pages_args {
#ifdef _KERNEL
	struct sysmsg sysmsg;
#endif
	register_t dummy;
};
struct	linux_openat_args {
#ifdef _KERNEL
	struct sysmsg sysmsg;
#endif
	l_int	dfd;	char dfd_[PAD_(l_int)];
	char *	path;	char path_[PAD_(char *)];
	l_int	flags;	char flags_[PAD_(l_int)];
	l_int	mode;	char mode_[PAD_(l_int)];
};
struct	linux_mkdirat_args {
#ifdef _KERNEL
	struct sysmsg sysmsg;
#endif
	l_int	dfd;	char dfd_[PAD_(l_int)];
	char *	path;	char path_[PAD_(char *)];
	l_int	mode;	char mode_[PAD_(l_int)];
};
struct	linux_mknodat_args {
#ifdef _KERNEL
	struct sysmsg sysmsg;
#endif
	l_int	dfd;	char dfd_[PAD_(l_int)];
	char *	path;	char path_[PAD_(char *)];
	l_int	mode;	char mode_[PAD_(l_int)];
	l_uint	dev;	char dev_[PAD_(l_uint)];
};
struct	linux_fchownat_args {
#ifdef _KERNEL
	struct sysmsg sysmsg;
#endif
	l_int	dfd;	char dfd_[PAD_(l_int)];
	char *	filename;	char filename_[PAD_(char *)];
	l_uid16_t	uid;	char uid_[PAD_(l_uid16_t)];
	l_gid16_t	gid;	char gid_[PAD_(l_gid16_t)];
	l_int	flag;	char flag_[PAD_(l_int)];
};
struct	linux_futimesat_args {
#ifdef _KERNEL
	struct sysmsg sysmsg;
#endif
	l_int	dfd;	char dfd_[PAD_(l_int)];
	char *	fname;	char fname_[PAD_(char *)];
	struct l_timeval *	tptr;	char tptr_[PAD_(struct l_timeval *)];
};
struct	linux_fstatat64_args {
#ifdef _KERNEL
	struct sysmsg sysmsg;
#endif
	l_int	dfd;	char dfd_[PAD_(l_int)];
	char *	path;	char path_[PAD_(char *)];
	struct l_stat64 *	statbuf;	char statbuf_[PAD_(struct l_stat64 *)];
	l_int	flag;	char flag_[PAD_(l_int)];
};
struct	linux_unlinkat_args {
#ifdef _KERNEL
	struct sysmsg sysmsg;
#endif
	l_int	dfd;	char dfd_[PAD_(l_int)];
	char *	path;	char path_[PAD_(char *)];
	l_int	flag;	char flag_[PAD_(l_int)];
};
struct	linux_renameat_args {
#ifdef _KERNEL
	struct sysmsg sysmsg;
#endif
	l_int	olddfd;	char olddfd_[PAD_(l_int)];
	char *	from;	char from_[PAD_(char *)];
	l_int	newdfd;	char newdfd_[PAD_(l_int)];
	char *	to;	char to_[PAD_(char *)];
};
struct	linux_linkat_args {
#ifdef _KERNEL
	struct sysmsg sysmsg;
#endif
	l_int	olddfd;	char olddfd_[PAD_(l_int)];
	char *	path;	char path_[PAD_(char *)];
	l_int	newdfd;	char newdfd_[PAD_(l_int)];
	char *	to;	char to_[PAD_(char *)];
	l_int	flags;	char flags_[PAD_(l_int)];
};
struct	linux_symlinkat_args {
#ifdef _KERNEL
	struct sysmsg sysmsg;
#endif
	char *	path;	char path_[PAD_(char *)];
	l_int	newdfd;	char newdfd_[PAD_(l_int)];
	char *	to;	char to_[PAD_(char *)];
};
struct	linux_readlinkat_args {
#ifdef _KERNEL
	struct sysmsg sysmsg;
#endif
	l_int	dfd;	char dfd_[PAD_(l_int)];
	char *	path;	char path_[PAD_(char *)];
	char *	buf;	char buf_[PAD_(char *)];
	l_int	count;	char count_[PAD_(l_int)];
};
struct	linux_fchmodat_args {
#ifdef _KERNEL
	struct sysmsg sysmsg;
#endif
	l_int	dfd;	char dfd_[PAD_(l_int)];
	char *	filename;	char filename_[PAD_(char *)];
	l_mode_t	mode;	char mode_[PAD_(l_mode_t)];
};
struct	linux_faccessat_args {
#ifdef _KERNEL
	struct sysmsg sysmsg;
#endif
	l_int	dfd;	char dfd_[PAD_(l_int)];
	char *	filename;	char filename_[PAD_(char *)];
	l_int	mode;	char mode_[PAD_(l_int)];
};
struct	linux_pselect6_args {
#ifdef _KERNEL
	struct sysmsg sysmsg;
#endif
	register_t dummy;
};
struct	linux_ppoll_args {
#ifdef _KERNEL
	struct sysmsg sysmsg;
#endif
	register_t dummy;
};
struct	linux_unshare_args {
#ifdef _KERNEL
	struct sysmsg sysmsg;
#endif
	register_t dummy;
};
struct	linux_set_robust_list_args {
#ifdef _KERNEL
	struct sysmsg sysmsg;
#endif
	struct linux_robust_list_head *	head;	char head_[PAD_(struct linux_robust_list_head *)];
	l_size_t	len;	char len_[PAD_(l_size_t)];
};
struct	linux_get_robust_list_args {
#ifdef _KERNEL
	struct sysmsg sysmsg;
#endif
	l_int	pid;	char pid_[PAD_(l_int)];
	struct linux_robust_list_head **	head;	char head_[PAD_(struct linux_robust_list_head **)];
	l_size_t *	len;	char len_[PAD_(l_size_t *)];
};
struct	linux_splice_args {
#ifdef _KERNEL
	struct sysmsg sysmsg;
#endif
	register_t dummy;
};
struct	linux_sync_file_range_args {
#ifdef _KERNEL
	struct sysmsg sysmsg;
#endif
	register_t dummy;
};
struct	linux_tee_args {
#ifdef _KERNEL
	struct sysmsg sysmsg;
#endif
	register_t dummy;
};
struct	linux_vmsplice_args {
#ifdef _KERNEL
	struct sysmsg sysmsg;
#endif
	register_t dummy;
};
struct	linux_move_pages_args {
#ifdef _KERNEL
	struct sysmsg sysmsg;
#endif
	register_t dummy;
};
struct	linux_getcpu_args {
#ifdef _KERNEL
	struct sysmsg sysmsg;
#endif
	l_uint *	pcpu;	char pcpu_[PAD_(l_uint *)];
	l_uint *	pnode;	char pnode_[PAD_(l_uint *)];
	void *	ptcache;	char ptcache_[PAD_(void *)];
};
struct	linux_epoll_pwait_args {
#ifdef _KERNEL
	struct sysmsg sysmsg;
#endif
	register_t dummy;
};
struct	linux_utimensat_args {
#ifdef _KERNEL
	struct sysmsg sysmsg;
#endif
	l_int	dfd;	char dfd_[PAD_(l_int)];
	char *	fname;	char fname_[PAD_(char *)];
	struct l_timespec *	tptr;	char tptr_[PAD_(struct l_timespec *)];
	l_int	flag;	char flag_[PAD_(l_int)];
};
struct	linux_signalfd_args {
#ifdef _KERNEL
	struct sysmsg sysmsg;
#endif
	register_t dummy;
};
struct	linux_timerfd_args {
#ifdef _KERNEL
	struct sysmsg sysmsg;
#endif
	register_t dummy;
};
struct	linux_eventfd_args {
#ifdef _KERNEL
	struct sysmsg sysmsg;
#endif
	register_t dummy;
};
struct	linux_pipe2_args {
#ifdef _KERNEL
	struct sysmsg sysmsg;
#endif
	l_ulong *	pipefds;	char pipefds_[PAD_(l_ulong *)];
	l_int	flags;	char flags_[PAD_(l_int)];
};

#ifdef COMPAT_43

#define	nosys	linux_nosys

#ifdef _KERNEL


#endif /* _KERNEL */

#endif /* COMPAT_43 */


#ifdef COMPAT_DF12

#define	nosys	linux_nosys

#ifdef _KERNEL


#endif /* _KERNEL */

#endif /* COMPAT_DF12 */


#ifdef _KERNEL

#define	nosys	linux_nosys
int	sys_linux_fork (struct linux_fork_args *);
int	sys_linux_open (struct linux_open_args *);
int	sys_linux_waitpid (struct linux_waitpid_args *);
int	sys_linux_creat (struct linux_creat_args *);
int	sys_linux_link (struct linux_link_args *);
int	sys_linux_unlink (struct linux_unlink_args *);
int	sys_linux_execve (struct linux_execve_args *);
int	sys_linux_chdir (struct linux_chdir_args *);
int	sys_linux_time (struct linux_time_args *);
int	sys_linux_mknod (struct linux_mknod_args *);
int	sys_linux_chmod (struct linux_chmod_args *);
int	sys_linux_lchown16 (struct linux_lchown16_args *);
int	sys_linux_stat (struct linux_stat_args *);
int	sys_linux_lseek (struct linux_lseek_args *);
int	sys_linux_getpid (struct linux_getpid_args *);
int	sys_linux_mount (struct linux_mount_args *);
int	sys_linux_oldumount (struct linux_oldumount_args *);
int	sys_linux_setuid16 (struct linux_setuid16_args *);
int	sys_linux_getuid16 (struct linux_getuid16_args *);
int	sys_linux_stime (struct linux_stime_args *);
int	sys_linux_ptrace (struct linux_ptrace_args *);
int	sys_linux_alarm (struct linux_alarm_args *);
int	sys_linux_fstat (struct linux_fstat_args *);
int	sys_linux_pause (struct linux_pause_args *);
int	sys_linux_utime (struct linux_utime_args *);
int	sys_linux_access (struct linux_access_args *);
int	sys_linux_nice (struct linux_nice_args *);
int	sys_linux_kill (struct linux_kill_args *);
int	sys_linux_rename (struct linux_rename_args *);
int	sys_linux_mkdir (struct linux_mkdir_args *);
int	sys_linux_rmdir (struct linux_rmdir_args *);
int	sys_linux_pipe (struct linux_pipe_args *);
int	sys_linux_times (struct linux_times_args *);
int	sys_linux_brk (struct linux_brk_args *);
int	sys_linux_setgid16 (struct linux_setgid16_args *);
int	sys_linux_getgid16 (struct linux_getgid16_args *);
int	sys_linux_signal (struct linux_signal_args *);
int	sys_linux_geteuid16 (struct linux_geteuid16_args *);
int	sys_linux_getegid16 (struct linux_getegid16_args *);
int	sys_linux_umount (struct linux_umount_args *);
int	sys_linux_ioctl (struct linux_ioctl_args *);
int	sys_linux_fcntl (struct linux_fcntl_args *);
int	sys_linux_olduname (struct linux_olduname_args *);
int	sys_linux_ustat (struct linux_ustat_args *);
int	sys_linux_getppid (struct linux_getppid_args *);
int	sys_linux_sigaction (struct linux_sigaction_args *);
int	sys_linux_sgetmask (struct linux_sgetmask_args *);
int	sys_linux_ssetmask (struct linux_ssetmask_args *);
int	sys_linux_setreuid16 (struct linux_setreuid16_args *);
int	sys_linux_setregid16 (struct linux_setregid16_args *);
int	sys_linux_sigsuspend (struct linux_sigsuspend_args *);
int	sys_linux_sigpending (struct linux_sigpending_args *);
int	sys_linux_sethostname (struct linux_sethostname_args *);
int	sys_linux_setrlimit (struct linux_setrlimit_args *);
int	sys_linux_old_getrlimit (struct linux_old_getrlimit_args *);
int	sys_linux_getgroups16 (struct linux_getgroups16_args *);
int	sys_linux_setgroups16 (struct linux_setgroups16_args *);
int	sys_linux_old_select (struct linux_old_select_args *);
int	sys_linux_symlink (struct linux_symlink_args *);
int	sys_linux_ostat (struct linux_ostat_args *);
int	sys_linux_readlink (struct linux_readlink_args *);
int	sys_linux_uselib (struct linux_uselib_args *);
int	sys_linux_reboot (struct linux_reboot_args *);
int	sys_linux_readdir (struct linux_readdir_args *);
int	sys_linux_mmap (struct linux_mmap_args *);
int	sys_linux_truncate (struct linux_truncate_args *);
int	sys_linux_ftruncate (struct linux_ftruncate_args *);
int	sys_linux_statfs (struct linux_statfs_args *);
int	sys_linux_fstatfs (struct linux_fstatfs_args *);
int	sys_linux_ioperm (struct linux_ioperm_args *);
int	sys_linux_socketcall (struct linux_socketcall_args *);
int	sys_linux_syslog (struct linux_syslog_args *);
int	sys_linux_setitimer (struct linux_setitimer_args *);
int	sys_linux_getitimer (struct linux_getitimer_args *);
int	sys_linux_newstat (struct linux_newstat_args *);
int	sys_linux_newlstat (struct linux_newlstat_args *);
int	sys_linux_newfstat (struct linux_newfstat_args *);
int	sys_linux_uname (struct linux_uname_args *);
int	sys_linux_iopl (struct linux_iopl_args *);
int	sys_linux_vhangup (struct linux_vhangup_args *);
int	sys_linux_vm86old (struct linux_vm86old_args *);
int	sys_linux_wait4 (struct linux_wait4_args *);
int	sys_linux_swapoff (struct linux_swapoff_args *);
int	sys_linux_sysinfo (struct linux_sysinfo_args *);
int	sys_linux_ipc (struct linux_ipc_args *);
int	sys_linux_sigreturn (struct linux_sigreturn_args *);
int	sys_linux_clone (struct linux_clone_args *);
int	sys_linux_newuname (struct linux_newuname_args *);
int	sys_linux_modify_ldt (struct linux_modify_ldt_args *);
int	sys_linux_adjtimex (struct linux_adjtimex_args *);
int	sys_linux_sigprocmask (struct linux_sigprocmask_args *);
int	sys_linux_create_module (struct linux_create_module_args *);
int	sys_linux_init_module (struct linux_init_module_args *);
int	sys_linux_delete_module (struct linux_delete_module_args *);
int	sys_linux_get_kernel_syms (struct linux_get_kernel_syms_args *);
int	sys_linux_quotactl (struct linux_quotactl_args *);
int	sys_linux_bdflush (struct linux_bdflush_args *);
int	sys_linux_sysfs (struct linux_sysfs_args *);
int	sys_linux_personality (struct linux_personality_args *);
int	sys_linux_setfsuid16 (struct linux_setfsuid16_args *);
int	sys_linux_setfsgid16 (struct linux_setfsgid16_args *);
int	sys_linux_llseek (struct linux_llseek_args *);
int	sys_linux_getdents (struct linux_getdents_args *);
int	sys_linux_select (struct linux_select_args *);
int	sys_linux_msync (struct linux_msync_args *);
int	sys_linux_getsid (struct linux_getsid_args *);
int	sys_linux_fdatasync (struct linux_fdatasync_args *);
int	sys_linux_sysctl (struct linux_sysctl_args *);
int	sys_linux_sched_setscheduler (struct linux_sched_setscheduler_args *);
int	sys_linux_sched_getscheduler (struct linux_sched_getscheduler_args *);
int	sys_linux_sched_get_priority_max (struct linux_sched_get_priority_max_args *);
int	sys_linux_sched_get_priority_min (struct linux_sched_get_priority_min_args *);
int	sys_linux_mremap (struct linux_mremap_args *);
int	sys_linux_setresuid16 (struct linux_setresuid16_args *);
int	sys_linux_getresuid16 (struct linux_getresuid16_args *);
int	sys_linux_vm86 (struct linux_vm86_args *);
int	sys_linux_query_module (struct linux_query_module_args *);
int	sys_linux_nfsservctl (struct linux_nfsservctl_args *);
int	sys_linux_setresgid16 (struct linux_setresgid16_args *);
int	sys_linux_getresgid16 (struct linux_getresgid16_args *);
int	sys_linux_prctl (struct linux_prctl_args *);
int	sys_linux_rt_sigreturn (struct linux_rt_sigreturn_args *);
int	sys_linux_rt_sigaction (struct linux_rt_sigaction_args *);
int	sys_linux_rt_sigprocmask (struct linux_rt_sigprocmask_args *);
int	sys_linux_rt_sigpending (struct linux_rt_sigpending_args *);
int	sys_linux_rt_sigtimedwait (struct linux_rt_sigtimedwait_args *);
int	sys_linux_rt_sigqueueinfo (struct linux_rt_sigqueueinfo_args *);
int	sys_linux_rt_sigsuspend (struct linux_rt_sigsuspend_args *);
int	sys_linux_pread (struct linux_pread_args *);
int	sys_linux_pwrite (struct linux_pwrite_args *);
int	sys_linux_chown16 (struct linux_chown16_args *);
int	sys_linux_getcwd (struct linux_getcwd_args *);
int	sys_linux_capget (struct linux_capget_args *);
int	sys_linux_capset (struct linux_capset_args *);
int	sys_linux_sigaltstack (struct linux_sigaltstack_args *);
int	sys_linux_sendfile (struct linux_sendfile_args *);
int	sys_linux_vfork (struct linux_vfork_args *);
int	sys_linux_getrlimit (struct linux_getrlimit_args *);
int	sys_linux_mmap2 (struct linux_mmap2_args *);
int	sys_linux_truncate64 (struct linux_truncate64_args *);
int	sys_linux_ftruncate64 (struct linux_ftruncate64_args *);
int	sys_linux_stat64 (struct linux_stat64_args *);
int	sys_linux_lstat64 (struct linux_lstat64_args *);
int	sys_linux_fstat64 (struct linux_fstat64_args *);
int	sys_linux_lchown (struct linux_lchown_args *);
int	sys_linux_getuid (struct linux_getuid_args *);
int	sys_linux_getgid (struct linux_getgid_args *);
int	sys_linux_getgroups (struct linux_getgroups_args *);
int	sys_linux_setgroups (struct linux_setgroups_args *);
int	sys_linux_chown (struct linux_chown_args *);
int	sys_linux_setfsuid (struct linux_setfsuid_args *);
int	sys_linux_setfsgid (struct linux_setfsgid_args *);
int	sys_linux_pivot_root (struct linux_pivot_root_args *);
int	sys_linux_mincore (struct linux_mincore_args *);
int	sys_linux_madvise (struct linux_madvise_args *);
int	sys_linux_getdents64 (struct linux_getdents64_args *);
int	sys_linux_fcntl64 (struct linux_fcntl64_args *);
int	sys_linux_gettid (struct linux_gettid_args *);
int	sys_linux_setxattr (struct linux_setxattr_args *);
int	sys_linux_lsetxattr (struct linux_lsetxattr_args *);
int	sys_linux_fsetxattr (struct linux_fsetxattr_args *);
int	sys_linux_getxattr (struct linux_getxattr_args *);
int	sys_linux_lgetxattr (struct linux_lgetxattr_args *);
int	sys_linux_fgetxattr (struct linux_fgetxattr_args *);
int	sys_linux_listxattr (struct linux_listxattr_args *);
int	sys_linux_llistxattr (struct linux_llistxattr_args *);
int	sys_linux_flistxattr (struct linux_flistxattr_args *);
int	sys_linux_removexattr (struct linux_removexattr_args *);
int	sys_linux_lremovexattr (struct linux_lremovexattr_args *);
int	sys_linux_fremovexattr (struct linux_fremovexattr_args *);
int	sys_linux_tkill (struct linux_tkill_args *);
int	sys_linux_sys_futex (struct linux_sys_futex_args *);
int	sys_linux_sched_setaffinity (struct linux_sched_setaffinity_args *);
int	sys_linux_sched_getaffinity (struct linux_sched_getaffinity_args *);
int	sys_linux_set_thread_area (struct linux_set_thread_area_args *);
int	sys_linux_get_thread_area (struct linux_get_thread_area_args *);
int	sys_linux_fadvise64 (struct linux_fadvise64_args *);
int	sys_linux_exit_group (struct linux_exit_group_args *);
int	sys_linux_epoll_create (struct linux_epoll_create_args *);
int	sys_linux_epoll_ctl (struct linux_epoll_ctl_args *);
int	sys_linux_epoll_wait (struct linux_epoll_wait_args *);
int	sys_linux_set_tid_address (struct linux_set_tid_address_args *);
int	sys_linux_clock_settime (struct linux_clock_settime_args *);
int	sys_linux_clock_gettime (struct linux_clock_gettime_args *);
int	sys_linux_clock_getres (struct linux_clock_getres_args *);
int	sys_linux_clock_nanosleep (struct linux_clock_nanosleep_args *);
int	sys_linux_statfs64 (struct linux_statfs64_args *);
int	sys_linux_fstatfs64 (struct linux_fstatfs64_args *);
int	sys_linux_tgkill (struct linux_tgkill_args *);
int	sys_linux_utimes (struct linux_utimes_args *);
int	sys_linux_fadvise64_64 (struct linux_fadvise64_64_args *);
int	sys_linux_mbind (struct linux_mbind_args *);
int	sys_linux_get_mempolicy (struct linux_get_mempolicy_args *);
int	sys_linux_set_mempolicy (struct linux_set_mempolicy_args *);
int	sys_linux_mq_open (struct linux_mq_open_args *);
int	sys_linux_mq_getsetattr (struct linux_mq_getsetattr_args *);
int	sys_linux_kexec_load (struct linux_kexec_load_args *);
int	sys_linux_waitid (struct linux_waitid_args *);
int	sys_linux_add_key (struct linux_add_key_args *);
int	sys_linux_request_key (struct linux_request_key_args *);
int	sys_linux_keyctl (struct linux_keyctl_args *);
int	sys_linux_ioprio_set (struct linux_ioprio_set_args *);
int	sys_linux_ioprio_get (struct linux_ioprio_get_args *);
int	sys_linux_inotify_init (struct linux_inotify_init_args *);
int	sys_linux_inotify_add_watch (struct linux_inotify_add_watch_args *);
int	sys_linux_inotify_rm_watch (struct linux_inotify_rm_watch_args *);
int	sys_linux_migrate_pages (struct linux_migrate_pages_args *);
int	sys_linux_openat (struct linux_openat_args *);
int	sys_linux_mkdirat (struct linux_mkdirat_args *);
int	sys_linux_mknodat (struct linux_mknodat_args *);
int	sys_linux_fchownat (struct linux_fchownat_args *);
int	sys_linux_futimesat (struct linux_futimesat_args *);
int	sys_linux_fstatat64 (struct linux_fstatat64_args *);
int	sys_linux_unlinkat (struct linux_unlinkat_args *);
int	sys_linux_renameat (struct linux_renameat_args *);
int	sys_linux_linkat (struct linux_linkat_args *);
int	sys_linux_symlinkat (struct linux_symlinkat_args *);
int	sys_linux_readlinkat (struct linux_readlinkat_args *);
int	sys_linux_fchmodat (struct linux_fchmodat_args *);
int	sys_linux_faccessat (struct linux_faccessat_args *);
int	sys_linux_pselect6 (struct linux_pselect6_args *);
int	sys_linux_ppoll (struct linux_ppoll_args *);
int	sys_linux_unshare (struct linux_unshare_args *);
int	sys_linux_set_robust_list (struct linux_set_robust_list_args *);
int	sys_linux_get_robust_list (struct linux_get_robust_list_args *);
int	sys_linux_splice (struct linux_splice_args *);
int	sys_linux_sync_file_range (struct linux_sync_file_range_args *);
int	sys_linux_tee (struct linux_tee_args *);
int	sys_linux_vmsplice (struct linux_vmsplice_args *);
int	sys_linux_move_pages (struct linux_move_pages_args *);
int	sys_linux_getcpu (struct linux_getcpu_args *);
int	sys_linux_epoll_pwait (struct linux_epoll_pwait_args *);
int	sys_linux_utimensat (struct linux_utimensat_args *);
int	sys_linux_signalfd (struct linux_signalfd_args *);
int	sys_linux_timerfd (struct linux_timerfd_args *);
int	sys_linux_eventfd (struct linux_eventfd_args *);
int	sys_linux_pipe2 (struct linux_pipe2_args *);

#endif /* !_LINUX_SYSPROTO_H_ */
#undef PAD_

#endif /* _KERNEL */
