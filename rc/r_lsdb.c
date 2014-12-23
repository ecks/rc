#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "libcsv/csv.h"

#include "prefix.h"
#include "table.h"

#include "r_lsa.h"
#include "r_lsdb.h"

static int put_comma;

void cb1 (void *s, size_t i, void *buf) 
{
  char * buf_str = (char *)buf;
  strncpy(buf_str, s, i);
  buf_str[i] = '\0';
  put_comma = 1;
}

void cb2 (int c, void * buf) 
{
  put_comma = 0;
}

struct r_lsdb * r_lsdb_extract(char * filename, int error)
{
  FILE * lsdb;
  struct csv_parser p;
  int i;
  int col = 0;
  char c;
  char buf[20];
  struct r_lsdb * table;
  struct r_lsa * lsa;
  struct r_as_external_lsa * lsa_ex;

  table = r_lsdb_new(error);
  csv_init(&p, 0);

  lsdb = fopen(filename, "r");
  while((i = fgetc(lsdb)) != EOF)
  {
    c = i; 
    if(csv_parse(&p, &c, 1, cb1, cb2, buf) != 1)
    {
      fprintf(stderr, "Error: %s\n", csv_strerror(csv_error(&p)));
      exit(EXIT_FAILURE);
    }

    if(c == ',')
    {
      if(col == 0)
      {
        lsa = r_lsa_new();
        lsa->data = r_lsa_data_new(1, 1);
        lsa->lsdb = table;
        inet_pton(AF_INET, buf, &(lsa->data->id));   
//        printf("first column: buffer is %s\n", buf);
        col++;
      }
      else if(col == 1)
      {
        lsa_ex = (struct r_as_external_lsa *)lsa->data;
        inet_pton(AF_INET, buf, &(lsa_ex->mask));   
//        printf("second column: buffer is %s\n", buf);
        col++;
      }
      else if(col == 2)
      {
        lsa_ex = (struct r_as_external_lsa *)lsa->data;
        strcat(lsa_ex->txt,buf);
//        printf("third column: buffer is %s\n", buf);
        col++;
      }
      else if(col == 3)
      {
        lsa_ex = (struct r_as_external_lsa *)lsa->data;
        strcat(lsa_ex->txt,buf);
//        printf("fourth column: buffer is %s\n", buf);
        col++;
      }
    }

    if(c == '\n')
    {
      lsa_ex = (struct r_as_external_lsa *)lsa->data;
      strcat(lsa_ex->txt,buf);
//      printf("txt buffer is %s\n", lsa_ex->txt);
      r_lsdb_add(table, lsa);
      // we have just parsed whole line
      // so set col back to 0
      col = 0;
//      printf("last column: buffer is %s\n", buf);
    }
  }

  csv_fini(&p, cb1, cb2, NULL);
  csv_free(&p);

  return table;
}

struct r_lsdb * r_lsdb_new(int error)
{
  struct r_lsdb  * new;
  new = calloc(1, sizeof(struct r_lsdb));
  new->db = route_table_init(error);
  
  return new;
}

void
r_lsdb_free(struct r_lsdb * lsdb)
{
  r_lsdb_cleanup(lsdb);
  free(lsdb);
}

void
r_lsdb_cleanup(struct r_lsdb * lsdb)
{
  r_lsdb_delete_all(lsdb);

  route_table_finish(lsdb->db);
}

static void 
r_lsdb_prefix_set(struct prefix_ls * lp, struct r_lsa * lsa)
{
  lp->family = AF_INET;
  lp->prefixlen = 64;
  lp->id = lsa->data->id;
  inet_pton(AF_INET, "0.0.0.0", &(lp->adv_router));
}

static void
r_lsdb_delete_entry(struct r_lsdb * lsdb, struct route_node *rn)
{
  struct r_lsa * lsa = rn->info;

  if(!lsa)
    return;

  lsdb->count--;
  rn->info = NULL;
  route_unlock_node(rn);
  r_lsa_unlock(lsa);
  return;
}

void
r_lsdb_add(struct r_lsdb * lsdb, struct r_lsa * lsa)
{
  struct route_table * table;
  struct prefix_ipv4 p;
  struct route_node * rn;
  struct r_as_external_lsa * lsa_ex;

  table = lsdb->db;
  lsa_ex = (struct r_as_external_lsa *) lsa->data;
  p.family = AF_INET;
  p.prefix = lsa->data->id;
  p.prefixlen = ip_masklen(lsa_ex->mask);
  apply_mask_ipv4(&p);

//  r_lsdb_prefix_set(&lp, lsa);
  rn = route_node_get(table, (struct prefix *)&p);
 
  if(rn->info && rn->info == lsa)
    return;

  if(rn->info)
    r_lsdb_delete_entry(lsdb, rn);

  lsdb->count++;
//  lsdb->checksum += ntohs(lsa->data->checksum);  
  rn->info = r_lsa_lock(lsa);
}

void
r_lsdb_delete(struct r_lsdb * lsdb, struct route_node * rn)
{
  r_lsdb_delete_entry(lsdb, rn);
} 

void
r_lsdb_delete_all(struct r_lsdb * lsdb)
{
  struct route_table * table;
  struct route_node * rn;

  table = lsdb->db;
  for(rn = route_top(table); rn; rn = route_next(rn))
    if(rn->info != NULL)
      r_lsdb_delete_entry(lsdb, rn);
}

struct r_lsa *
r_lsdb_lookup(struct r_lsdb * lsdb, struct r_lsa * lsa)
{
  struct route_table * table;
  struct prefix_ls lp;
  struct route_node * rn;
  struct r_lsa * find;

  table = lsdb->db;
  r_lsdb_prefix_set(&lp, lsa);
  rn = route_node_lookup(table, (struct prefix *)&lp);
  if(rn)
  {
    find = rn->info;
    return find;
  }
  return NULL; 
}

unsigned long
r_lsdb_count_all(struct r_lsdb * lsdb)
{
  return lsdb->count;
}

unsigned int
r_lsdb_checksum(struct r_lsdb * lsdb)
{
  return lsdb->checksum;
}

unsigned long
r_lsdb_isempty(struct r_lsdb * lsdb)
{
  return (lsdb->count == 0);
} 

