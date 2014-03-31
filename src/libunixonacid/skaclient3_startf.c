/* ISC license. */

#include <errno.h>
#include "error.h"
#include "allreadwrite.h"
#include "bytestr.h"
#include "bufalloc.h"
#include "tai.h"
#include "djbunix.h"
#include "skaclient.h"

int skaclient3_startf (skaclient3_t_ref a, char const *prog, char const *const *argv, char const *const *envp, char const *before, unsigned int beforelen, char const *after, unsigned int afterlen, uint32 options, struct taia const *deadline, struct taia *stamp)
{
  int fds[4] ;
  unsigned int pid = child_spawn3(prog, argv, envp, 0, 0, fds) ;
  if (!pid) return 0 ;
  a->sync.pid = pid ;
  a->sync.options = options ;
  skaclientin_init(&a->sync.in, fds[0]) ;
  bufalloc_init(&a->sync.out, &fd_write, fds[1]) ;
  skaclientin_init(&a->asyncin, fds[2]) ;
  bufalloc_init(&a->asyncout, &fd_write, fds[3]) ;
  {
    char tmp[beforelen] ;
    if (skaclientin_read(&a->sync.in, tmp, beforelen, deadline, stamp) < beforelen) goto err ;
    if (byte_diff(tmp, beforelen, before)) { errno = EPROTO ; goto err ; }
  }
  {
    char tmp[afterlen] ;
    if (skaclientin_read(&a->asyncin, tmp, afterlen, deadline, stamp) < afterlen) goto err ;
    if (byte_diff(tmp, afterlen, after)) { errno = EPROTO ; goto err ; }
  }
  return 1 ;

 err:
  skaclient3_end(a) ;
  return 0 ;
}
