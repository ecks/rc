#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>

/* AFI and SAFI type. */
typedef u_int16_t afi_t;
typedef u_int8_t safi_t;

#include "prefix.h"
#include "table.h"
#include "algorithms.h"

#include "r_lsa.h"
#include "r_lsdb.h"

struct r_lsdb * table;

static void
usage(void)
{
  fprintf(stderr, "USAGE: rc ");
}

int
main(int argc, char * argv[])
{
  int arg;

  int exitval = 0;
  struct r_lsdb * host1_table;
  struct r_lsdb * host2_table;
  struct prefix * host1_code;
  struct prefix * host2_code;
  int host1_len;
  int host2_len;

  host1_table = r_lsdb_extract("routes1.csv", 0);
  host2_table = r_lsdb_extract("routes2.csv", 0);
//  route_table_show(host1_table->db);
//  printf("-------------------------\n");
//  route_table_show(host2_table->db);
  host1_code = encode(host1_table->db, &host1_len);
  host2_code = encode(host2_table->db, &host2_len);

  float res = levenshtein(host1_code, host1_len, host2_code, host2_len, host1_table->db, host2_table->db);
  printf("Levenshtein distance: %0.04f of len %d and %d\n", res, host1_len, host2_len);

  r_lsdb_free(host1_table);
  r_lsdb_free(host2_table);
  free(host1_code);
  free(host2_code);

  return exitval;
}
