/* ISC license. */

#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include "bytestr.h"
#include "strerr2.h"
#include "uint.h"
#include "allreadwrite.h"
#include "env.h"
#include "sig.h"
#include "djbunix.h"

unsigned int child_spawn2 (char const *prog, char const *const *argv, char const *const *envp, unsigned int uid, unsigned int gid, int *fds)
{
  int p[3][2] ;
  int syncpipe[2] ;
  int pid ;
  int e ;
  if (pipe(p[0]) < 0) return 0 ;
  if (pipe(p[1]) < 0) { e = errno ; goto errp0 ; }
  if (pipe(p[2]) < 0) { e = errno ; goto errp1 ; }
  if (pipe(syncpipe) < 0) { e = errno ; goto errp2 ; }
  if (coe(syncpipe[1]) < 0) { e = errno ; goto errsp ; }
  pid = fork() ;
  if (pid < 0) { e = errno ; goto errsp ; }
  else if (!pid)
  {
    unsigned int m = 25 ;
    unsigned int n = str_len(PROG) ;
    char fmt[25 + UINT_FMT] = "SKACLIENT_ADDITIONAL_FDS=" ;
    char name[n + 9] ;
    byte_copy(name, n, PROG) ;
    byte_copy(name + n, 9, " (child)") ;
    PROG = name ;
    m += uint_fmt(fmt + m, p[2][1]) ;
    fmt[m++] = 0 ;
    fd_close(syncpipe[0]) ;
    fd_close(p[2][0]) ;
    fd_close(p[1][1]) ;
    fd_close(p[0][0]) ;
    if ((fd_move(0, p[1][0]) < 0) || (fd_move(1, p[0][1]) < 0)) goto syncdie ;
    if (gid && (prot_gid(gid) < 0)) goto syncdie ;
    if (uid && (prot_uid(uid) < 0)) goto syncdie ;
    sig_blocknone() ;
    pathexec_r_name(prog, argv, envp, env_len(envp), fmt, m) ;

   syncdie:
    {
      char c = errno ;
      fd_write(syncpipe[1], &c, 1) ;
    }
    _exit(111) ;
  }
  fd_close(syncpipe[1]) ;
  fd_close(p[2][1]) ;
  fd_close(p[1][0]) ;
  fd_close(p[0][1]) ;
  {
    char c ;
    p[1][0] = fd_read(syncpipe[0], &c, 1) ;
    if (p[1][0] < 0) goto killclosewait ;
    else if (p[1][0]) { e = c ; goto closewait ; }
  }
  fd_close(syncpipe[0]) ;
  if ((ndelay_on(p[0][0]) < 0) || (coe(p[0][0]) < 0)
   || (ndelay_on(p[1][1]) < 0) || (coe(p[1][1]) < 0)
   || (ndelay_on(p[2][0]) < 0) || (coe(p[2][0]) < 0)) goto killclosewait ;
  fds[2] = p[2][0] ;
  fds[0] = p[0][0] ;
  fds[1] = p[1][1] ;
  return (unsigned int)pid ;

 errsp:
  fd_close(syncpipe[1]) ;
  fd_close(syncpipe[0]) ;  
 errp2:
  fd_close(p[2][1]) ;
  fd_close(p[2][0]) ;
 errp1:
  fd_close(p[1][1]) ;
  fd_close(p[1][0]) ;  
 errp0:
  fd_close(p[0][1]) ;
  fd_close(p[0][0]) ;  
  errno = e ;
  return 0 ;

 killclosewait:
  e = errno ;
  kill(pid, SIGKILL) ;
 closewait:
  fd_close(p[2][0]) ;
  fd_close(p[1][1]) ;
  fd_close(p[0][0]) ;
  wait_pid(&syncpipe[1], pid) ;
  errno = e ;
  return 0 ;
}
