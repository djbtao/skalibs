/* ISC license. */

#include <errno.h>
#include "allreadwrite.h"
#include "tai.h"
#include "iopause.h"
#include "ancillary.h"
#include "unix-timed.h"

int timed_ancil_fds_recv (int fdsocket, int *fds, unsigned int n, struct taia const *deadline, struct taia *stamp)
{
  iopause_fd x = { fdsocket, IOPAUSE_READ, 0 } ;
  for (;;)
  {
    register int r = iopause_stamp(&x, 1, deadline, stamp) ;
    if (r < 0) return 0 ;
    else if (!r) return (errno = ETIMEDOUT, 0) ;
    else if (x.revents & IOPAUSE_READ)
    {
      r = sanitize_read(ancil_recv_fds(fdsocket, fds, n)) ;
      if (r < 0) return 0 ;
      else if (r > 0) break ;
    }
  }
  return 1 ;
}
