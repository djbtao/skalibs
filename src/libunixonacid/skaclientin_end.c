/* ISC license. */

#include <errno.h>
#include "djbunix.h"
#include "skaclient.h"

void skaclientin_end (skaclientin_t_ref in)
{
  register int e = errno ;
  fd_close(in->b.fd) ;
  stralloc_free(&in->data) ;
  *in = skaclientin_zero ;
  errno = e ;
}
