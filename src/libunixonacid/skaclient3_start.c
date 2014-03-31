/* ISC license. */

#include <errno.h>
#include "error.h"
#include "djbunix.h"
#include "bufalloc.h"
#include "webipc.h"
#include "unix-timed.h"
#include "skaclient.h"

int skaclient3_start (skaclient3_t_ref a, char const *path, char const *before, unsigned int beforelen, char const *after, unsigned int afterlen, struct taia const *deadline, struct taia *stamp)
{
  int fds[2] ;
  int r ;
  int fdsock = ipc_stream() ;
  if (fdsock < 0) return 0 ;
  if ((coe(fdsock) < 0) || !ipc_deadlineconnstamp(fdsock, path, deadline, stamp)) goto fail ;
  r = timed_ancil_fds_sandwiched_recv(fdsock, fds, 2, before, beforelen, after, afterlen, deadline, stamp) ;
  if (r < 0) goto fail ;
  if (r != 2)
  {
    if (r) fd_close(fds[0]) ;
    errno = EPROTO ;
    goto fail ;
  }
  skaclientin_init(&a->sync.in, fdsock) ;
  bufalloc_init(&a->sync.out, &fd_write, fdsock) ;
  skaclientin_init(&a->asyncin, fds[0]) ;
  bufalloc_init(&a->asyncout, &fd_write, fds[1]) ;
  return 1 ;
 fail:
  {
    register int e = errno ;
    fd_close(fdsock) ;
    errno = e ;
  }
  return 0 ;
}
