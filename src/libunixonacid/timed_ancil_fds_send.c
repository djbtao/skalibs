/* ISC license. */

#include <errno.h>
#include "error.h"
#include "tai.h"
#include "iopause.h"
#include "ancillary.h"
#include "unix-timed.h"

int timed_ancil_fds_send (int fdsocket, int const *fds, unsigned int n, struct taia const *deadline, struct taia *stamp)
{
  iopause_fd x = { fdsocket, IOPAUSE_WRITE, 0 } ;
  for (;;)
  {
    register int r = iopause_stamp(&x, 1, deadline, stamp) ;
    if (r < 0) return 0 ;
    else if (!r) return (errno = ETIMEDOUT, 0) ;
    else if (x.revents & IOPAUSE_WRITE)
    {
      if (ancil_send_fds(fdsocket, fds, n) < 1)
      {
        if (!error_isagain(errno)) return 0 ;
      }
      else break ;
    }
  }
  return 1 ;
}
