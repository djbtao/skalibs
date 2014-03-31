/* ISC license. */

#include <errno.h>
#include "kolbak.h"

int kolbak_enqueue (kolbak_queue_t *q, kolbak_func_t_ref f, void *data)
{
  register unsigned int newtail = (q->tail + 1) % q->n ;
  if (newtail == q->head) return (errno = ENOBUFS, 0) ;
  q->tab[q->tail].f = f ;
  q->tab[q->tail].data = data ;
  q->tail = newtail ;
  return 1 ;
}
