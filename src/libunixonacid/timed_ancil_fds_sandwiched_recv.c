/* ISC license. */

#include <errno.h>
#include "uint32.h"
#include "bytestr.h"
#include "error.h"
#include "allreadwrite.h"
#include "buffer.h"
#include "tai.h"
#include "djbunix.h"
#include "unix-timed.h"

int timed_ancil_fds_sandwiched_recv (int fdsock, int *fds, unsigned int n, char const *before, unsigned int beforelen, char const *after, unsigned int afterlen, struct taia const *deadline, struct taia *stamp)
{
  unsigned int maxlen = (beforelen + 4) > afterlen ? beforelen + 4 : afterlen ;
  char buf[maxlen] ;
  buffer b = BUFFER_INIT(&fd_read, fdsock, buf, maxlen) ;
  uint32 got ;
  {
    char gotbefore[beforelen + 4] ;
    if (timed_buffer_get(&b, gotbefore, beforelen + 4, deadline, stamp) < beforelen + 4) return -1 ;
    if (byte_diff(gotbefore, beforelen, before)) return (errno = EPROTO, -1) ;
    uint32_unpack_big(gotbefore + beforelen, &got) ;
    if (got > n) return (errno = ERANGE, -1) ;
  }
  if (!timed_ancil_fds_recv(fdsock, fds, got, deadline, stamp)) return -1 ;
  {
    char gotafter[afterlen] ;
    if (timed_buffer_get(&b, gotafter, afterlen, deadline, stamp) < afterlen)
      goto err ;
    if (byte_diff(gotafter, afterlen, after))
    {
      errno = EPROTO ;
      goto err ;
    }
  }
  {
    register unsigned int i = 0 ;
    for (; i < got ; i++) if (ndelay_on(fds[i]) < 0) goto err ;
  }
  return (int)got ;
 err:
  {
    register unsigned int e = errno ;
    register unsigned int i = 0 ;
    for (; i < got ; i++) fd_close(fds[i]) ;
    errno = e ;
  }
  return -1 ;
}
