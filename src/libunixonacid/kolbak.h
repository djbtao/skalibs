/* ISC license. */

#ifndef KOLBAK_H
#define KOLBAK_H

#include "buffer.h"

typedef int kolbak_func_t (buffer *, void *) ;
typedef kolbak_func_t *kolbak_func_t_ref ;

typedef struct kolbak_closure_s kolbak_closure_t, *kolbak_closure_t_ref ;
struct kolbak_closure_s
{
  kolbak_func_t_ref f ;
  void *data ;
} ;
#define KOLBACK_CLOSURE_ZERO { .f = 0, .data = 0 }

typedef struct kolbak_queue_s kolbak_queue_t, *kolbak_queue_t_ref ;
struct kolbak_queue_s
{
  kolbak_closure_t *tab ;
  unsigned int n ;
  unsigned int head ;
  unsigned int tail ;
} ;
#define KOLBAK_QUEUE_ZERO { .tab = 0, .n = 0, .head = 0, .tail = 0 }
#define KOLBAK_QUEUE_INIT(s, len) { .tab = (s), .n = (len), .head = 0, .tail = 0 }

extern void kolbak_queue_init (kolbak_queue_t *, kolbak_closure_t *, unsigned int) ;
extern int kolbak_enqueue (kolbak_queue_t *, kolbak_func_t_ref, void *) ;
extern int kolbak_unenqueue (kolbak_queue_t *) ;
extern int kolbak_call (kolbak_queue_t *, buffer *) ; 

#endif
