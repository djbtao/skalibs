/* ISC license. */

#include <errno.h>
#include "bufalloc.h"
#include "djbunix.h"
#include "skaclient.h"

void skaclient3_end (skaclient3_t_ref a)
{
  skaclient_end(&a->sync) ;
  skaclientin_end(&a->asyncin) ;
  {
    register int e = errno ;
    fd_close(bufalloc_fd(&a->asyncout)) ;
    errno = e ;
  }
  bufalloc_free(&a->asyncout) ;
}
