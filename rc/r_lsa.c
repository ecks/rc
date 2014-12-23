#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "prefix.h"
#include "table.h"

#include "r_lsa.h"
#include "r_lsdb.h"

struct r_lsa *
r_lsa_new(void)
{
  struct r_lsa *  new = calloc(1, sizeof(struct r_lsa));
  new->lock = 1;
  return new;
}

void
r_lsa_free(struct r_lsa * lsa)
{
  assert(lsa->lock == 0);

  if(lsa->data != NULL)
    r_lsa_data_free(lsa->data);

  memset(lsa, 0, sizeof(struct r_lsa));
  free(lsa); 
}

struct r_lsa *
r_lsa_lock(struct r_lsa * lsa)
{
  lsa->lock++;
  return lsa;
}

void
r_lsa_unlock(struct r_lsa * lsa)
{
  lsa->lock--;

  if(lsa->lock == 0)
  {
    r_lsa_free(lsa); 
    lsa = NULL;
  }
}

struct r_lsa_header *
r_lsa_data_new(size_t size, int type)
{
  if(type == 0)
    return calloc(size, sizeof(struct r_lsa_header));
  else
  {
    struct r_as_external_lsa * lsa = calloc(size, sizeof(struct r_as_external_lsa));
    lsa->txt = calloc(40, sizeof(char));
    return (struct r_lsa_header *)lsa;
  }
}

void r_lsa_data_free(struct r_lsa_header * lsah)
{
  free(lsah);
}
