/* ISC license. */

#include "kolbak.h"
#include "skaclient.h"

int skaclient4_put (skaclient4_t *a, char const *s, unsigned int len, kolbak_func_t_ref f, void *data)
{
  if (!kolbak_enqueue(&a->qinfo, f, data)) return 0 ;
  if (!skaclient2_put(&a->connection, s, len))
  {
    kolbak_unenqueue(&a->qinfo) ;
    return 0 ;
  }
  return 1 ;
}
