/* ISC license. */

#include "siovec.h"
#include "kolbak.h"
#include "skaclient.h"

int skaclient4_putv (skaclient4_t *a, siovec_t *v, unsigned int n, kolbak_func_t_ref f, void *data)
{
  if (!kolbak_enqueue(&a->qinfo, f, data)) return 0 ;
  if (!skaclient2_putv(&a->connection, v, n))
  {
    kolbak_unenqueue(&a->qinfo) ;
    return 0 ;
  }
  return 1 ;
}
