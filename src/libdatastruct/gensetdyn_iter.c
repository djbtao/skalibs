/* ISC license. */

#include "bitarray.h"
#include "functypes.h"
#include "gensetdyn.h"
#include "lolstdio.h"

unsigned int gensetdyn_iter (gensetdyn_ref g, iterfunc_t_ref f, void *stuff)
{
 /*
    XXX: we may be called by a freeing function, so we cannot alloc -
    XXX: so pray that the bitarray fits in the stack.
 */
  unsigned char bits[bitarray_div8(g->storage.len)] ;
  unsigned int i = 0, j = 0, n = 0, m = gensetdyn_n(g) ;
  register unsigned int *fl = genalloc_s(unsigned int, &g->freelist) ;
  register unsigned int sp = genalloc_len(unsigned int, &g->freelist) ;
  bitarray_setn(bits, 0, g->storage.len) ;
  LOLDEBUG("gensetdyn_iter:  storage.len = %u, freelist.len = %u, m = %u", g->storage.len, sp, m) ;
  
  for (; i < sp ; i++)
  {
    bitarray_clear(bits, fl[i]) ;
    LOLDEBUG("gensetdyn_iter: cleared bit %u (freelist item %u)", fl[i], i) ;
  }
  for (i = 0 ; (i < g->storage.len) && (j < m) ; i++)
  {
    LOLDEBUG("gensetdyn_iter: bit %u, j = %u, n = %u", i, j, n) ;
    if (bitarray_peek(bits, i))
    {
      j++ ;
      if ((*f)(gensetdyn_p(g, i), stuff)) n++ ;
      LOLDEBUG("gensetdyn_iter: bit %u set, after iteration j = %u, n = %u", i, j, n) ;
    }
    else LOLDEBUG("gensetdyn_iter: bit %u cleared", i) ;
  }
  return n ;
}
