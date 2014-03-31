/* ISC license. */

#include <errno.h>
#include "djbunix.h"
#include "bufalloc.h"
#include "tai.h"
#include "webipc.h"
#include "skaclient.h"

int skaclient_start (skaclient_t_ref a, char const *path, char const *banner, unsigned int bannerlen, struct taia const *deadline, struct taia *stamp)
{
  int fd = ipc_stream() ;
  if (fd < 0) return 0 ;
  if ((coe(fd) < 0) || !ipc_deadlineconnstamp(fd, path, deadline, stamp))
  {
    register int e = errno ;
    fd_close(fd) ;
    errno = e ;
    return 0 ;
  }
  skaclientin_init(&a->in, fd) ;
  bufalloc_init(&a->out, &fd_write, fd) ;
  if (!skaclient_readbanner(a, banner, bannerlen, deadline, stamp))
  {
    skaclient_end(a) ;
    return 0 ;
  }
  return 1 ;
}
