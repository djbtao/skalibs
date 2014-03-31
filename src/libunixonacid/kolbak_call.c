/* ISC license. */

#include <errno.h>
#include "kolbak.h"

int kolbak_call (kolbak_queue_t *q, buffer *b)
{
  register int r ;
  if (q->head == q->tail) return (errno = EILSEQ, -1) ;
  r = (*q->tab[q->head].f)(b, q->tab[q->head].data) ;
  if (r > 0) q->head = (q->head + 1) % q->n ;
  return r ;
}
