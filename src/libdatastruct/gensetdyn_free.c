/* ISC license. */

#include "stralloc.h"
#include "genalloc.h"
#include "gensetdyn.h"

void gensetdyn_free (gensetdyn_ref g)
{
  stralloc_free(&g->storage) ;
  genalloc_free(unsigned int, &g->freelist) ;
  *g = gensetdyn_zero ;
}
