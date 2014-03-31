/* ISC license. */

#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include "bytestr.h"
#include "strerr2.h"
#include "allreadwrite.h"
#include "env.h"
#include "sig.h"
#include "djbunix.h"

unsigned int child_spawn (char const *prog, char const *const *argv, char const *const *envp, unsigned int uid, unsigned int gid, int *fds)
{
  int p[2][2] ;
  int syncpipe[2] ;
  int pid ;
  int e ;
  if (pipe(p[0]) < 0) return 0 ;
  if (pipe(p[1]) < 0) { e = errno ; goto errp0 ; }
  if (pipe(syncpipe) < 0) { e = errno ; goto errp1 ; }
  if (coe(syncpipe[1]) < 0) { e = errno ; goto errsp ; }
  pid = fork() ;
  if (pid < 0) { e = errno ; goto errsp ; }
  else if (!pid)
  {
    unsigned int n = str_len(PROG) ;
    char name[n + 9] ;
    byte_copy(name, n, PROG) ;
    byte_copy(name + n, 9, " (child)") ;
    PROG = name ;
    fd_close(syncpipe[0]) ;
    fd_close(p[1][1]) ;
    fd_close(p[0][0]) ;
    if ((fd_move(0, p[1][0]) < 0) || (fd_move(1, p[0][1]) < 0)) goto syncdie ;
    if (gid && (prot_gid(gid) < 0)) goto syncdie ;
    if (uid && (prot_uid(uid) < 0)) goto syncdie ;
    /*
       XXX: we should unignore signals here, but there's a compromise to be
       XXX: found between bloat, API complexity, and theoretical correctness.
       XXX: Not sure what The Right Thing is; just not unignoring atm.
       XXX: 20130326 edit: The Right Thing is probably to leave the ignore
       XXX: status alone, because programs that do actual signal handling
       XXX: are likely to use a selfpipe, and selfpipe_finish() restores
       XXX: old signal handlers - it knows what to do and we don't. So,
       XXX: leave that work to selfpipe_finish().
    */
    sig_blocknone() ; /* empty sigprocmask in the child, always */
    pathexec_r_name(prog, argv, envp, env_len(envp), "SKACLIENT_ADDITIONAL_FDS", 25) ;

   syncdie:
    {
      char c = errno ;
      fd_write(syncpipe[1], &c, 1) ;
    }
    _exit(111) ;
  }
  fd_close(syncpipe[1]) ;
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
   || (ndelay_on(p[1][1]) < 0) || (coe(p[1][1]) < 0)) goto killclosewait ;
  fds[0] = p[0][0] ;
  fds[1] = p[1][1] ;
  return (unsigned int)pid ;

 errsp:
  fd_close(syncpipe[1]) ;
  fd_close(syncpipe[0]) ;  
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
  fd_close(p[1][1]) ;
  fd_close(p[0][0]) ;
  wait_pid(&syncpipe[1], pid) ;
  errno = e ;
  return 0 ;
}
