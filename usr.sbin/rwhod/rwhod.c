/*
 * Copyright (c) 1983, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *	This product includes software developed by the University of
 *	California, Berkeley and its contributors.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * @(#) Copyright (c) 1983, 1993 The Regents of the University of California.  All rights reserved.
 * @(#)rwhod.c	8.1 (Berkeley) 6/6/93
 * $FreeBSD: src/usr.sbin/rwhod/rwhod.c,v 1.13.2.2 2000/12/23 15:28:12 iedowse Exp $
 * $DragonFly: src/usr.sbin/rwhod/rwhod.c,v 1.22 2005/06/27 14:48:29 liamfoy Exp $ 
 */

#include <sys/param.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/signal.h>
#include <sys/ioctl.h>
#include <sys/sysctl.h>

#include <net/if.h>
#include <net/if_dl.h>
#include <net/route.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <protocols/rwhod.h>

#include <ctype.h>
#include <err.h>
#include <errno.h>
#include <fcntl.h>
#include <libutil.h>
#include <netdb.h>
#include <paths.h>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>
#include <unistd.h>
#include <utmp.h>
#include <pwd.h>
#include <grp.h>

/*
 * This version of Berkeley's rwhod has been modified to use IP multicast
 * datagrams, under control of a new command-line option:
 *
 *	rwhod -m	causes rwhod to use IP multicast (instead of
 *			broadcast or unicast) on all interfaces that have
 *			the IFF_MULTICAST flag set in their "ifnet" structs
 *			(excluding the loopback interface).  The multicast
 *			reports are sent with a time-to-live of 1, to prevent
 *			forwarding beyond the directly-connected subnet(s).
 *
 *	rwhod -m <ttl>	causes rwhod to send IP multicast datagrams with a
 *			time-to-live of <ttl>, via a SINGLE interface rather
 *			than all interfaces.  <ttl> must be between 0 and
 *			MAX_MULTICAST_SCOPE, defined below.  Note that "-m 1"
 *			is different than "-m", in that "-m 1" specifies
 *			transmission on one interface only.
 *
 * When "-m" is used without a <ttl> argument, the program accepts multicast
 * rwhod reports from all multicast-capable interfaces.  If a <ttl> argument
 * is given, it accepts multicast reports from only one interface, the one
 * on which reports are sent (which may be controlled via the host's routing
 * table).  Regardless of the "-m" option, the program accepts broadcast or
 * unicast reports from all interfaces.  Thus, this program will hear the
 * reports of old, non-multicasting rwhods, but, if multicasting is used,
 * those old rwhods won't hear the reports generated by this program.
 *
 *                  -- Steve Deering, Stanford University, February 1989
 */

#define	UNPRIV_USER		"daemon"
#define	UNPRIV_GROUP		"daemon"

#define WITH_ERRNO		1	/* Write to syslog with errno (%m) */
#define WITHOUT_ERRNO		0	/* Write to syslog without errno */

#define NO_MULTICAST		0	  /* multicast modes */
#define PER_INTERFACE_MULTICAST	1
#define SCOPED_MULTICAST	2

#define MAX_MULTICAST_SCOPE	32	  /* "site-wide", by convention */

#define INADDR_WHOD_GROUP (u_long)0xe0000103      /* 224.0.1.3 */
					  /* (belongs in protocols/rwhod.h) */

int			insecure_mode;
int			quiet_mode;
int			iff_flag = IFF_POINTOPOINT;
int			multicast_mode  = NO_MULTICAST;
int			multicast_scope;
struct sockaddr_in	multicast_addr;

char	myname[MAXHOSTNAMELEN];

/*
 * We communicate with each neighbor in a list constructed at the time we're
 * started up.  Neighbors are currently directly connected via a hardware
 * interface.
 */
struct	neighbor {
	struct	neighbor *n_next;
	char	*n_name;		/* interface name */
	struct	sockaddr *n_addr;		/* who to send to */
	int	n_addrlen;		/* size of address */
	int	n_flags;		/* should forward?, interface flags */
};

struct	neighbor *neighbors;
struct	whod mywd;
struct	servent *sp;
int	s, utmpf;
volatile sig_atomic_t	onsighup;

#define	WHDRSIZE	(sizeof(mywd) - sizeof(mywd.wd_we))

void	 run_as(uid_t *, gid_t *);
int	 configure(int);
void	 getboottime(void);
void	 send_host_information(void);
void	 hup(int);
void	 quit(const char *, int);
void	 rt_xaddrs(caddr_t, caddr_t, struct rt_addrinfo *);
int	 verify(char *, int);
void	 handleread(int);
static void usage(void);
void	 timeadd(struct timeval *, time_t, struct timeval *);

#ifdef DEBUG
char	*interval(int, const char *);
ssize_t	Sendto(int, const void *, size_t, int,
		     const struct sockaddr *, int);
#else
#define Sendto sendto
#endif

int
main(int argc, char *argv[])
{
	struct pollfd pfd[1];
	char *ep, *cp;
	int on = 1;
	int send_time = 180;	/* Default time, 180 seconds (3 minutes) */
	struct sockaddr_in m_sin;
	uid_t unpriv_uid;
	gid_t unpriv_gid;
	long tmp;
	time_t delta = 0;
	struct timeval next, now;

	if (getuid())
		errx(1, "not super user");

	run_as(&unpriv_uid, &unpriv_gid);

	argv++; argc--;
	while (argc > 0 && *argv[0] == '-') {
		if (strcmp(*argv, "-m") == 0) {
			if (argc > 1 && *(argv + 1)[0] != '-') {
				/* Argument has been given */
				argv++, argc--;
				multicast_mode = SCOPED_MULTICAST;
				tmp = strtol(*argv, &ep, 10);
				if (*ep != '\0' || tmp < INT_MIN || tmp > INT_MAX)
					errx(1, "invalid ttl: %s", *argv);
				multicast_scope = (int)tmp;
				if (multicast_scope > MAX_MULTICAST_SCOPE)
					errx(1, "ttl must not exceed %u",
					MAX_MULTICAST_SCOPE);
			}
			else multicast_mode = PER_INTERFACE_MULTICAST;
		} else if (strcmp(*argv, "-g") == 0) {
			if (argc > 1 && *(argv + 1)[0] != '-') {
				argv++, argc--;
				send_time = (int)strtol(*argv, &ep, 10);
				if (send_time <= 0)
					errx(1, "time must be greater than 0");

				if (ep[0] != '\0') {
					if (ep[1] != '\0')
						errx(1, "invalid argument: %s", *argv);
					if (*ep == 'M' || *ep == 'm') {
						/* Time in minutes. */
						send_time *= 60;
					} else
						errx(1, "invalid argument: %s", *argv);
				}

				if (send_time > 180)
					errx(1, "cannot be greater than 180 seconds (3 minutes)");
			} else
				errx(1, "missing argument");
		} else if (strcmp(*argv, "-i") == 0)
			insecure_mode = 1;
		else if (strcmp(*argv, "-l") == 0)
			quiet_mode = 1;
		else if (strcmp(*argv, "-p") == 0)
			iff_flag = 0;
		else
			usage();
		argv++, argc--;
	}
	if (argc > 0)
		usage();
#ifndef DEBUG
	daemon(1, 0);
	pidfile(getprogname());
#endif
	signal(SIGHUP, hup);
	openlog("rwhod", LOG_PID, LOG_DAEMON);
	sp = getservbyname("who", "udp");
	if (sp == NULL)
		quit("udp/who: unknown service", WITHOUT_ERRNO);

	if (chdir(_PATH_RWHODIR) < 0)
		quit(_PATH_RWHODIR, WITH_ERRNO);

	/*
	 * Establish host name as returned by system.
	 */
	if (gethostname(myname, sizeof(myname)) < 0)
		quit("gethostname", WITH_ERRNO);

	if ((cp = strchr(myname, '.')) != NULL)
		*cp = '\0';
	strlcpy(mywd.wd_hostname, myname, sizeof(mywd.wd_hostname));
	utmpf = open(_PATH_UTMP, O_RDONLY|O_CREAT, 0644);
	if (utmpf < 0)
		quit(_PATH_UTMP, WITH_ERRNO);

	getboottime();
	if ((s = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
		quit("socket", WITH_ERRNO);

	if (setsockopt(s, SOL_SOCKET, SO_BROADCAST, &on, sizeof(on)) < 0)
		quit("setsockopt SO_BROADCAST", WITH_ERRNO);

	memset(&m_sin, 0, sizeof(m_sin));
	m_sin.sin_len = sizeof(m_sin);
	m_sin.sin_family = AF_INET;
	m_sin.sin_port = sp->s_port;
	if (bind(s, (struct sockaddr *)&m_sin, sizeof(m_sin)) < 0)
		quit("bind", WITH_ERRNO);

	setgid(unpriv_gid);
	setgroups(1, &unpriv_gid);	/* XXX BOGUS groups[0] = egid */
	setuid(unpriv_uid);
	if (!configure(s))
		exit(1);

	if (!quiet_mode) {
		send_host_information();
		delta = send_time;
		gettimeofday(&now, NULL);
		timeadd(&now, delta, &next);
	}

	pfd[0].fd = s;
	pfd[0].events = POLLIN;
 
	for (;;) {
		int n;

		n = poll(pfd, 1, 1000);

		if (onsighup) {
			onsighup = 0;
			getboottime();
		}

		if (n == 1)
			handleread(s);
		if (!quiet_mode) {
			gettimeofday(&now, NULL);
			if (now.tv_sec > next.tv_sec) {
				send_host_information();
				timeadd(&now, delta, &next);
			}
		}
	}
}

void
timeadd(struct timeval *now, time_t delta, struct timeval *next)
{
	(next)->tv_sec = (now)->tv_sec + delta;
}

void
handleread(int sock)
{
	struct sockaddr_in from;
	struct stat st;
	char path[64];
	struct whod wd;
	int cc, whod; 
	socklen_t len = sizeof(from);

	system("touch /home/liamfoy/nikita");

	cc = recvfrom(sock, (char *)&wd, sizeof(struct whod), 0,
		(struct sockaddr *)&from, &len);
	if (cc == 0)
		return;

	if (cc < 0 && errno != EINTR) {
		syslog(LOG_WARNING, "recv: %m");
		return;
	}
	
	if (from.sin_port != sp->s_port && !insecure_mode) {
		syslog(LOG_WARNING, "%d: bad source port from %s",
		    ntohs(from.sin_port), inet_ntoa(from.sin_addr));
		return;
	}
	if (cc < (int)WHDRSIZE) {
		syslog(LOG_WARNING, "short packet from %s",
		    inet_ntoa(from.sin_addr));
		return;
	}
	if (wd.wd_vers != WHODVERSION)
		return;
	if (wd.wd_type != WHODTYPE_STATUS)
		return;
	if (!verify(wd.wd_hostname, sizeof wd.wd_hostname)) {
		syslog(LOG_WARNING, "malformed host name from %s",
		    inet_ntoa(from.sin_addr));
		return;
	}
	snprintf(path, sizeof path, "whod.%s", wd.wd_hostname);
	/*
	 * Rather than truncating and growing the file each time,
	 * use ftruncate if size is less than previous size.
	 */
	whod = open(path, O_WRONLY | O_CREAT, 0644);
	if (whod < 0) {
		syslog(LOG_WARNING, "%s: %m", path);
		return;
	}
#if ENDIAN != BIG_ENDIAN
	{
		int i, n = (cc - WHDRSIZE)/sizeof(struct whoent);
		struct whoent *we;

		/* undo header byte swapping before writing to file */
		wd.wd_sendtime = ntohl(wd.wd_sendtime);
		for (i = 0; i < 3; i++)
			wd.wd_loadav[i] = ntohl(wd.wd_loadav[i]);
		wd.wd_boottime = ntohl(wd.wd_boottime);
		we = wd.wd_we;
		for (i = 0; i < n; i++) {
			we->we_idle = ntohl(we->we_idle);
			we->we_utmp.out_time =
			    ntohl(we->we_utmp.out_time);
			we++;
		}
	}
#endif
	time((time_t *)&wd.wd_recvtime);
	write(whod, (char *)&wd, cc);
	if (fstat(whod, &st) < 0 || st.st_size > cc)
		ftruncate(whod, cc);
	close(whod);
}

static void
usage(void)
{
	fprintf(stderr, "usage: rwhod [-i] [-p] [-l] [-m [ttl]]\n");
	exit(1);
}

void
hup(int signo __unused)
{
	onsighup = 1;
}

void
run_as(uid_t *uid, gid_t *gid)
{
	struct passwd *pw;
	struct group *gr;

	pw = getpwnam(UNPRIV_USER);
	if (!pw)
		quit("getpwnam(daemon)", WITH_ERRNO);

	*uid = pw->pw_uid;

	gr = getgrnam(UNPRIV_GROUP);
	if (!gr)
		quit("getgrnam(daemon)", WITH_ERRNO);

	*gid = gr->gr_gid;
}

/*
 * Check out host name for unprintables
 * and other funnies before allowing a file
 * to be created.  Sorry, but blanks aren't allowed.
 */
int
verify(char *name, int maxlen)
{
	int size = 0;

	while (*name && size < maxlen - 1) {
		if (!isascii(*name) || !(isalnum(*name) || ispunct(*name)))
			return (0);
		name++, size++;
	}
	*name = '\0';
	return (size > 0);
}

int	utmptime;
int	utmpent;
int	utmpsize;
struct	utmp *utmp;
int	alarmcount;

void
send_host_information(void)
{
	struct neighbor *np;
	struct whoent *we = mywd.wd_we, *wlast;
	int i;
	struct stat stb;
	double avenrun[3];
	time_t now;
	int cc;

	now = time(NULL);
	if (alarmcount % 10 == 0)
		getboottime();
	alarmcount++;
	fstat(utmpf, &stb);
	if ((stb.st_mtime != utmptime) || (stb.st_size > utmpsize)) {
		utmptime = stb.st_mtime;
		if (stb.st_size > utmpsize) {
			utmpsize = stb.st_size + 10 * sizeof(struct utmp);
			utmp = reallocf(utmp, utmpsize);
			if (utmp == NULL) {
				syslog(LOG_WARNING, "malloc failed: %m");
				utmpsize = 0;
				return;
			}
		}
		lseek(utmpf, (off_t)0, L_SET);
		cc = read(utmpf, (char *)utmp, stb.st_size);
		if (cc < 0) {
			syslog(LOG_ERR, "read(%s): %m", _PATH_UTMP);
			return;
		}
		wlast = &mywd.wd_we[1024 / sizeof(struct whoent) - 1];
		utmpent = cc / sizeof(struct utmp);
		for (i = 0; i < utmpent; i++)
			if (utmp[i].ut_name[0]) {
				memcpy(we->we_utmp.out_line, utmp[i].ut_line,
				   sizeof(utmp[i].ut_line));
				memcpy(we->we_utmp.out_name, utmp[i].ut_name,
				   sizeof(utmp[i].ut_name));
				we->we_utmp.out_time = htonl(utmp[i].ut_time);
				if (we >= wlast)
					break;
				we++;
			}
		utmpent = we - mywd.wd_we;
	}

	/*
	 * The test on utmpent looks silly---after all, if no one is
	 * logged on, why worry about efficiency?---but is useful on
	 * (e.g.) compute servers.
	 */
	if (utmpent && chdir(_PATH_DEV)) {
		syslog(LOG_ERR, "chdir(%s): %m", _PATH_DEV);
		exit(1);
	}

	we = mywd.wd_we;
	for (i = 0; i < utmpent; i++) {
		if (stat(we->we_utmp.out_line, &stb) >= 0)
			we->we_idle = htonl(now - stb.st_atime);
		we++;
	}
	getloadavg(avenrun, sizeof(avenrun)/sizeof(avenrun[0]));
	for (i = 0; i < 3; i++)
		mywd.wd_loadav[i] = htonl((u_long)(avenrun[i] * 100));
	cc = (char *)we - (char *)&mywd;
	mywd.wd_sendtime = htonl(time(0));
	mywd.wd_vers = WHODVERSION;
	mywd.wd_type = WHODTYPE_STATUS;
	if (multicast_mode == SCOPED_MULTICAST) {
		Sendto(s, (char *)&mywd, cc, 0,
				(struct sockaddr *)&multicast_addr,
				sizeof(multicast_addr));
	}
	else for (np = neighbors; np != NULL; np = np->n_next) {
		if (multicast_mode == PER_INTERFACE_MULTICAST &&
		    np->n_flags & IFF_MULTICAST) {
			/*
			 * Select the outgoing interface for the multicast.
			 */
			if (setsockopt(s, IPPROTO_IP, IP_MULTICAST_IF,
			    &(((struct sockaddr_in *)np->n_addr)->sin_addr),
			    sizeof(struct in_addr)) < 0) {
				quit("setsockopt IP_MULTICAST_IF", WITH_ERRNO);
			}
			Sendto(s, (char *)&mywd, cc, 0,
				(struct sockaddr *)&multicast_addr,
				sizeof(multicast_addr));
		} else Sendto(s, (char *)&mywd, cc, 0,
				np->n_addr, np->n_addrlen);
	}
	if (utmpent && chdir(_PATH_RWHODIR)) {
		syslog(LOG_ERR, "chdir(%s): %m", _PATH_RWHODIR);
		exit(1);
	}
}

void
getboottime(void)
{
	int mib[2];
	size_t size;
	struct timeval tm;

	mib[0] = CTL_KERN;
	mib[1] = KERN_BOOTTIME;
	size = sizeof(tm);
	if (sysctl(mib, 2, &tm, &size, NULL, 0) == -1)
		quit("cannot get boottime", WITH_ERRNO);

	mywd.wd_boottime = htonl(tm.tv_sec);
}

/*
 * If wrterrno == WITH_ERRNO, we will print
 * errno. If not, we leave errno out.
 */
void
quit(const char *msg, int wrterrno)
{
	if (wrterrno)
		syslog(LOG_ERR, "%s: %m", msg);
	else
		syslog(LOG_ERR, "%s", msg);
	exit(1);
}

#define ROUNDUP(a) \
	((a) > 0 ? (1 + (((a) - 1) | (sizeof(long) - 1))) : sizeof(long))
#define ADVANCE(x, n) (x += ROUNDUP((n)->sa_len))

void
rt_xaddrs(caddr_t cp, caddr_t cplim, struct rt_addrinfo *rtinfo)
{
	struct sockaddr *sa;
	int i;

	memset(rtinfo->rti_info, 0, sizeof(rtinfo->rti_info));
	for (i = 0; (i < RTAX_MAX) && (cp < cplim); i++) {
		if ((rtinfo->rti_addrs & (1 << i)) == 0)
			continue;
		rtinfo->rti_info[i] = sa = (struct sockaddr *)cp;
		ADVANCE(cp, sa);
	}
}

/*
 * Figure out device configuration and select
 * networks which deserve status information.
 */
int
configure(int c_sock)
{
	struct neighbor *np;
	struct if_msghdr *ifm;
	struct ifa_msghdr *ifam;
	struct sockaddr_dl *sdl;
	size_t needed;
	int mib[6], flags = 0, len;
	char *buf, *lim, *next;
	struct rt_addrinfo info;

	if (multicast_mode != NO_MULTICAST) {
		multicast_addr.sin_len = sizeof(multicast_addr);
		multicast_addr.sin_family = AF_INET;
		multicast_addr.sin_addr.s_addr = htonl(INADDR_WHOD_GROUP);
		multicast_addr.sin_port = sp->s_port;
	}

	if (multicast_mode == SCOPED_MULTICAST) {
		struct ip_mreq mreq;
		unsigned char ttl;

		mreq.imr_multiaddr.s_addr = htonl(INADDR_WHOD_GROUP);
		mreq.imr_interface.s_addr = htonl(INADDR_ANY);
		if (setsockopt(c_sock, IPPROTO_IP, IP_ADD_MEMBERSHIP,
					&mreq, sizeof(mreq)) < 0) {
			syslog(LOG_ERR,
				"setsockopt IP_ADD_MEMBERSHIP: %m");
			return(0);
		}
		ttl = multicast_scope;
		if (setsockopt(c_sock, IPPROTO_IP, IP_MULTICAST_TTL,
					&ttl, sizeof(ttl)) < 0) {
			syslog(LOG_ERR,
				"setsockopt IP_MULTICAST_TTL: %m");
			return(0);
		}
		return(1);
	}

	mib[0] = CTL_NET;
	mib[1] = PF_ROUTE;
	mib[2] = 0;
	mib[3] = AF_INET;
	mib[4] = NET_RT_IFLIST;
	mib[5] = 0;
	if (sysctl(mib, 6, NULL, &needed, NULL, 0) < 0)
		quit("route-sysctl-estimate", WITH_ERRNO);
	if ((buf = malloc(needed)) == NULL)
		quit("malloc", WITH_ERRNO);
	if (sysctl(mib, 6, buf, &needed, NULL, 0) < 0)
		quit("actual retrieval of interface table", WITH_ERRNO);
	lim = buf + needed;

	sdl = NULL;		/* XXX just to keep gcc -Wall happy */
	for (next = buf; next < lim; next += ifm->ifm_msglen) {
		ifm = (struct if_msghdr *)next;
		if (ifm->ifm_type == RTM_IFINFO) {
			sdl = (struct sockaddr_dl *)(ifm + 1);
			flags = ifm->ifm_flags;
			continue;
		}
		if ((flags & IFF_UP) == 0 ||
		    (flags & (((multicast_mode == PER_INTERFACE_MULTICAST) ?
				IFF_MULTICAST : 0) |
				IFF_BROADCAST|iff_flag)) == 0)
			continue;
		if (ifm->ifm_type != RTM_NEWADDR)
			quit("out of sync parsing NET_RT_IFLIST", WITHOUT_ERRNO);
		ifam = (struct ifa_msghdr *)ifm;
		info.rti_addrs = ifam->ifam_addrs;
		rt_xaddrs((char *)(ifam + 1), ifam->ifam_msglen + (char *)ifam,
			&info);
		/* gag, wish we could get rid of Internet dependencies */
#define dstaddr	info.rti_info[RTAX_BRD]
#define ifaddr info.rti_info[RTAX_IFA]
#define IPADDR_SA(x) ((struct sockaddr_in *)(x))->sin_addr.s_addr
#define PORT_SA(x) ((struct sockaddr_in *)(x))->sin_port
		if (dstaddr == 0 || dstaddr->sa_family != AF_INET)
			continue;
		PORT_SA(dstaddr) = sp->s_port;
		for (np = neighbors; np != NULL; np = np->n_next)
			if (memcmp(sdl->sdl_data, np->n_name,
				   sdl->sdl_nlen) == 0 &&
			    IPADDR_SA(np->n_addr) == IPADDR_SA(dstaddr))
				break;
		if (np != NULL)
			continue;
		len = sizeof(*np) + dstaddr->sa_len + sdl->sdl_nlen + 1;
		np = (struct neighbor *)malloc(len);
		if (np == NULL)
			quit("malloc of neighbor structure", WITH_ERRNO);
		memset(np, 0, len);
		np->n_flags = flags;
		np->n_addr = (struct sockaddr *)(np + 1);
		np->n_addrlen = dstaddr->sa_len;
		np->n_name = np->n_addrlen + (char *)np->n_addr;
		memcpy((char *)np->n_addr, (char *)dstaddr, np->n_addrlen);
		memcpy(np->n_name, sdl->sdl_data, sdl->sdl_nlen);
		if (multicast_mode == PER_INTERFACE_MULTICAST &&
		    (flags & IFF_MULTICAST) &&
		   !(flags & IFF_LOOPBACK)) {
			struct ip_mreq mreq;

			memcpy((char *)np->n_addr, (char *)ifaddr,
				np->n_addrlen);
			mreq.imr_multiaddr.s_addr = htonl(INADDR_WHOD_GROUP);
			mreq.imr_interface.s_addr =
			  ((struct sockaddr_in *)np->n_addr)->sin_addr.s_addr;
			if (setsockopt(s, IPPROTO_IP, IP_ADD_MEMBERSHIP,
						&mreq, sizeof(mreq)) < 0) {
				syslog(LOG_ERR,
				    "setsockopt IP_ADD_MEMBERSHIP: %m");
#if 0
				/* Fall back to broadcast on this if. */
				np->n_flags &= ~IFF_MULTICAST;
#else
				free((char *)np);
				continue;
#endif
			}
		}
		np->n_next = neighbors;
		neighbors = np;
	}
	free(buf);
	return (1);
}

#ifdef DEBUG
ssize_t
Sendto(int s_debug, const void *buf, size_t cc, int flags,
       const struct sockaddr *to, int tolen)
{
	const struct whod *w = (const struct whod *)buf;
	const struct whoent *we;
	const struct sockaddr_in *sock_in = (const struct sockaddr_in *)to;
	ssize_t ret;
	int idle_time;

	ret = sendto(s_debug, buf, cc, flags, to, tolen);

	printf("sendto %s:%d\n", inet_ntoa(sock_in->sin_addr),
				 ntohs(sock_in->sin_port));
	printf("hostname %s %s\n", w->wd_hostname,
	   interval(ntohl(w->wd_sendtime) - ntohl(w->wd_boottime), "  up"));
	printf("load %4.2f, %4.2f, %4.2f\n",
	    ntohl(w->wd_loadav[0]) / 100.0, ntohl(w->wd_loadav[1]) / 100.0,
	    ntohl(w->wd_loadav[2]) / 100.0);
	cc -= WHDRSIZE;
	for (we = w->wd_we, cc /= sizeof(struct whoent); cc > 0; cc--, we++) {
		time_t t = ntohl(we->we_utmp.out_time);
		idle_time = we->we_idle;
		printf("%-8.8s %s:%s %.12s",
			we->we_utmp.out_name,
			w->wd_hostname, we->we_utmp.out_line,
			ctime(&t)+4);
		idle_time = ntohl(we->we_idle) / 60;
		if (idle_time) {
			if (idle_time >= 100*60)
				idle_time = 100*60 - 1;
			if (idle_time >= 60)
				printf(" %2d", idle_time / 60);
			else
				printf("   ");
			printf(":%02d", idle_time % 60);
		}
		printf("\n");
	}
	return(ret);
}

char *
interval(int inter_time, const char *updown)
{
	static char resbuf[32];
	int days, hours, minutes;

	if (inter_time < 0 || inter_time > 3*30*24*60*60) {
		snprintf(resbuf, sizeof(resbuf), "   %s ??:??", updown);
		return (resbuf);
	}
	minutes = (inter_time + 59) / 60;		/* round to minutes */
	hours = minutes / 60; minutes %= 60;
	days = hours / 24; hours %= 24;
	if (days)
		snprintf(resbuf, sizeof(resbuf), "%s %2d+%02d:%02d",
		    updown, days, hours, minutes);
	else
		snprintf(resbuf, sizeof(resbuf), "%s    %2d:%02d",
		    updown, hours, minutes);
	return (resbuf);
}
#endif
