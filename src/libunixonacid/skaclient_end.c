/* ISC license. */

#include <errno.h>
#include "djbunix.h"
#include "skaclient.h"

void skaclient_end (skaclient_t_ref a)
{
  unsigned int pid = 0 ;
  register int e = errno ;
  if (a->out.fd != skaclientin_fd(&a->in))
  {
    fd_close(a->out.fd) ;
    if (a->options & SKACLIENT_OPTION_WAITPID) pid = a->pid ;
  }
  bufalloc_free(&a->out) ;
  skaclientin_end(&a->in) ;
  *a = skaclient_zero ;
  if (pid)
  {
    int wstat ;
    wait_pid(&wstat, (int)pid) ;
  }
  errno = e ;
}
