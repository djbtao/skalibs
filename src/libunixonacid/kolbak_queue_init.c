/* ISC license. */

#include "kolbak.h"

void kolbak_queue_init (kolbak_queue_t *q, kolbak_closure_t *s, unsigned int len)
{
  q->tab = s ;
  q->n = len ;
  q->head = 0 ;
  q->tail = 0 ;
}
