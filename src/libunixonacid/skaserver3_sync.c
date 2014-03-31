/* ISC license. */

#include "sysdeps.h"
#include <unistd.h>
#include <errno.h>
#include "uint32.h"
#include "uint.h"
#include "env.h"
#include "bufalloc.h"
#include "tai.h"
#include "djbunix.h"
#include "unix-timed.h"
#include "skaclient.h"

int skaserver3_sync (bufalloc *b, char const *before, unsigned int beforelen, char const *after, unsigned int afterlen, struct taia const *deadline, struct taia *stamp)
{
  char const *x = env_get("SKACLIENT_ADDITIONAL_FDS") ;
  if (x)
  {
    unsigned int p[2] ;
    unsigned int pos = uint_scan(x, &p[0]) ;
    if (!pos) return (errno = EINVAL, -1) ;
    if (x[pos++] != ',') return (errno = EINVAL, -1) ;
    if (!uint0_scan(x + pos, &p[1])) return (errno = EINVAL, -1) ;
    if ((ndelay_on((int)p[0]) < 0) || (ndelay_on((int)p[1]) < 0)) return -1 ;
    if (!bufalloc_put(bufalloc_1, before, beforelen)) return -1 ;
    if (!timed_bufalloc_flush(bufalloc_1, deadline, stamp)) return -1 ;
    bufalloc_init(b, &fd_write, (int)p[1]) ;
    if (!bufalloc_put(b, after, afterlen)) return -1 ;
    if (!timed_bufalloc_flush(b, deadline, stamp)) return -1 ;
    return (int)p[0] ;
  }
  else
  {
    int p[2][2] ;
    register int e ;
    if (pipenb(p[0]) < 0) return -1 ;
    if (pipenb(p[1]) < 0) { e = errno ; goto err1 ; }
    e = p[0][0] ; p[0][0] = p[1][0] ; p[1][0] = e ;
    if (!bufalloc_put(bufalloc_1, before, beforelen)) goto fail ;
    {
      char pack[4] ;
      uint32_pack_big(pack, 2) ;
      if (!bufalloc_put(bufalloc_1, pack, 4)) goto fail ;
    }
    if (!timed_bufalloc_flush(bufalloc_1, deadline, stamp)
     || !timed_ancil_fds_send(1, p[0], 2, deadline, stamp)
     || !bufalloc_put(bufalloc_1, after, afterlen)
     || !timed_bufalloc_flush(bufalloc_1, deadline, stamp))
      goto fail ;
#ifndef HASANCILAUTOCLOSE
    fd_close(p[0][1]) ;
    fd_close(p[0][0]) ;
#endif
    bufalloc_init(b, &fd_write, p[1][1]) ;
    return p[1][0] ;

   fail:
    e = errno ;
    fd_close(p[1][1]) ;
    fd_close(p[1][0]) ;
   err1:
    fd_close(p[0][1]) ;
    fd_close(p[0][0]) ;
    errno = e ;
    return -1 ;
  }
}
