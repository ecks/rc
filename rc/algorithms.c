#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "prefix.h"
#include "table.h"
#include "r_lsa.h"
#include "r_lsdb.h"

#include "algorithms.h"

float min_f(float a, float b)
{
  if (a < b)
    return a;
  else
    return b;
}

int min_i(int a, int b)
{
  if (a < b)
    return a;
  else
    return b;
}

int max_i(int a, int b)
{
  if (a > b)
    return a;
  else
    return b;
}

float minimum_f(float a, float b, float c)
{
  float temp = min_f(a,b);
  return min_f(temp, c);
}

float ngram(char * s_txt, char * t_txt)
{
  // set the number of grams to two
  int n = 2;
  int ni;
  int i;
  int j;
  int s_len = strlen(s_txt);
  int t_len = strlen(t_txt);

//  printf("%s\n", s_txt);
//  printf("%s\n", t_txt);
  if(s_len == 0 || t_len == 0)
  {
    if(s_len == t_len)
      return 0;
    else
      return 1;
  }

  int cost = 0;
  if(s_len < n || t_len < n)
  {
    for(i = 0, ni<min_i(s_len, t_len);i<ni;i++)
    {
      if(s_txt[i] == t_txt[i])
        cost++;
    }
    return 1 - ((float) cost/max_i(s_len, t_len));
  }

  int sa_len = s_len+n-1;
  char sa[sa_len];
  for(i = 0; i < sa_len; i++)
  {
    if(i < (n-1))
    {
      sa[i] = 0;
    }
    else
    {
      sa[i] = s_txt[i-n+1];
    }
  } 
  
  float p[s_len+1];
  float d[s_len+1];
  float _d[s_len+1];
  char t_j[n];

  for(i = 0; i <= s_len; i++)
  {
    p[i] = i;
  } 

  for(j = 1; j <= t_len;  j++)
  {
    if(j < n)
    {
      int ti;
      for(ti = 0; ti < (n-j); ti++)
      {
        t_j[ti] = 0;
      }
      for(ti = (n-j); ti < n; ti++)
      {
        t_j[ti] = t_txt[ti-(n-j)];
      }
    }
    else
    {
      strncpy(t_j, t_txt+(j-n), n);
//      printf("%s\n", t_j);
    }
    d[0] = j;
    for(i = 1; i <= s_len; i++)
    {
      cost = 0;
      int tn = n;
      // compare sa to t_j
      int ni;
      for(ni = 0; ni < n; ni++)
      {
        if(sa[i-1+ni] != t_j[ni])
        {
          cost++;
        }
        else if(sa[i-1+ni] == 0)
        {
          tn--;
        }
      }
      float ec = (float) cost/tn;
//      printf("%f\n", ec);
      d[i] = min_f(min_f(d[i-1]+1, p[i]+1), p[i-1] + ec);
    }
//    printf("d: %f %f %f %f %f\n", d[0],d[1],d[2],d[3],d[4]);
//    printf("p: %f %f %f %f %f\n", p[0],p[1],p[2],p[3],p[4]);
    memcpy(_d,  p, sizeof(float)*(s_len+1));
    memcpy(p, d, sizeof(float)*(s_len+1));
    memcpy(d, _d, sizeof(float)*(s_len+1));
  }
  return p[s_len]/max_i(t_len, s_len);
}

void
swap_int(int * a, int * b)
{
  int c = *a;
  *a = *b;
  *b = c;
}

void
swap_prefix(struct prefix * a, struct prefix * b)
{
  struct prefix c = *a;
  *a = *b;
  *b = c;
}

void
reverse_int(int * first, int * last)
{
  while((first != last) && (first != --last)) 
    swap_int(first++, last);
}

void
reverse_prefix(struct prefix * first, struct prefix * last)
{
  while((first != last) && (first != --last))
    swap_prefix(first++, last);
}

void 
label(struct route_table * rt)
{
  struct route_node * rn;
  int i = 1;

  for(rn = route_top(rt); rn; rn = route_next(rn))
  {
    rn->label = i;
    rn->valid = 1;
    i++;
  }
}

int
is_leaf(struct route_node * rt)
{
//  struct r_as_external_lsa * lsa_ex_l = rt->info;
  if(rt->l_left == NULL && rt->l_right == NULL)
    return 1;
  if(rt->l_left != NULL && !rt->l_left->valid && rt->l_right == NULL)
    return 1;
  if(rt->l_right != NULL && !rt->l_right->valid && rt->l_left == NULL)
    return 1;
  if(rt->l_left != NULL && rt->l_right != NULL && !rt->l_left->valid && !rt->l_right->valid)
    return 1;
  return 0;
}

struct prefix *
encode(struct route_table * rt, int * len)
{
  struct route_node * rn;
  struct route_node * rn_p;
  struct route_node * rn_pp;
  int * inps = calloc(8000, sizeof(int));
  struct prefix * ips = calloc(8000, sizeof(struct prefix));
  int i = 1;
  int count = 0;

  label(rt);

  for(rn = route_top(rt); rn; rn = route_next(rn))
  {
    if(is_leaf(rn) && rn->parent != NULL)
    {
//      printf("Leaf node...\t");
//      if(rn->info != NULL) 
//        printf("Not null: %s/%d %d\n", inet_ntoa(rn->p.u.prefix4), rn->p.prefixlen, rn->label);
//      else
//        printf("Null: %s/%d %d\n", inet_ntoa(rn->p.u.prefix4), rn->p.prefixlen, rn->label);

      inps[count] = rn->label;
      ips[count] = rn->p;
      count++;
      rn_p = rn->parent;
  
//      printf("Parent...\t");
//      if(rn_p->info != NULL) 
//        printf("Not null: %s/%d %d\n", inet_ntoa(rn_p->p.u.prefix4), rn_p->p.prefixlen, rn_p->label);
//      else
//        printf("Null: %s/%d %d\n", inet_ntoa(rn_p->p.u.prefix4), rn_p->p.prefixlen, rn_p->label);

      inps[count] = rn_p->label;
      ips[count] = rn_p->p;
      count++;
      rn->valid = 0;
      while(is_leaf(rn_p) && rn_p->label < rn->label && rn_p->parent != NULL)
      {
//        printf("Backtracking...\t");
        rn_pp = rn_p->parent;
//        if(rn_pp->info != NULL) 
//          printf("Not null: %s/%d %d\n", inet_ntoa(rn_pp->p.u.prefix4), rn_pp->p.prefixlen, rn_pp->label);
//        else
//          printf("Null: %s/%d %d\n", inet_ntoa(rn_pp->p.u.prefix4), rn_pp->p.prefixlen, rn_pp->label);
        inps[count] = rn_pp->label;
        ips[count] = rn_pp->p;
        count++;
        rn_p->valid = 0;
        rn_p = rn_pp;
      }  
    }
    i++;    
  }

  if(count == 0 && (rn = route_top(rt)) != NULL)
  {
    inps[count] = rn->label;
    ips[count] = rn->p;
    count++;
  } 
 
  int j;
  for(j = 0; j < count; j++)
  {
//    if((j+1) == count)
//      printf("%d", inps[j]);
//    else
//      printf("%d,", inps[j]);
  }
//  printf("\n");
  reverse_int(inps, (inps+j));
  reverse_prefix(ips, (ips+j));
  for(j = 0; j < count; j++)
  {
//    if((j+1) == count)
//      printf("%d", inps[j]);
//    else
//      printf("%d,", inps[j]);
  }
//  printf("\n");

  *len = j; 
  return ips;
}

float levenshtein(struct prefix * s, int s_size, struct prefix * t, int t_size, struct route_table * srt, struct route_table * trt)
{
  float d[s_size+1][t_size+1];
  float r;
  float norm_r;
  struct route_node * s_rn = NULL;
  struct route_node * t_rn = NULL;
  struct r_lsa * s_lsa;
  struct r_lsa * t_lsa;
  

  int i;
  for(i = 0; i <= s_size; i++)
  {
    d[i][0] = i;
  } int j;
  for(j = 0; j <= t_size; j++)
  {
    d[0][j] = j;
  }

  for(j = 0; j < t_size; j++)
  {
    for(i = 0; i < s_size; i++)
    {
      s_rn = NULL;
      t_rn = NULL;
//      printf("Before: d[%d][%d]: %f\n", i, j, d[i][j]);

      if(prefix_same(&s[i], &t[j]) && ((s_rn = route_node_lookup(srt, &s[i])) != NULL) && ((t_rn = route_node_lookup(trt, &t[j])) != NULL))
      {
        s_lsa = s_rn->info;
        t_lsa = t_rn->info;
        float ngr = ngram(((struct r_as_external_lsa *)s_lsa->data)->txt,((struct r_as_external_lsa *)t_lsa->data)->txt);
//        printf("ngram: %.10f\n", ngr);
        d[i+1][j+1] = d[i][j] + ngr;
//        route_node_show(s_rn);
//        route_node_show(t_rn);
//        printf("After <prefix same>: d[%d][%d]: %f\n", i+1, j+1, d[i+1][j+1]);
      }
      // if the nodes themselves are empty
      // the edit distance is 0
      else if(prefix_same(&s[i], &t[j]) && ((s_rn = route_node_lookup(srt, &s[i])) == NULL) && ((t_rn = route_node_lookup(trt, &t[j])) == NULL))
      {
        d[i+1][j+1] = d[i][j] + 0.0;
//        printf("After <no data>: d[%d][%d]: %f\n", i+1, j+1, d[i+1][j+1]);
      }
      else
      {
//        if(s_rn != NULL)
//          route_node_show(s_rn);
//        if(s_rn != NULL)
//          route_node_show(t_rn);

//        printf("Before <prefix dif>: d[%d][%d]: %f\n", i, j+1, d[i][j+1]);
//        printf("Before <prefix dif>: d[%d][%d]: %f\n", i+1, j, d[i+1][j]);
//        printf("Before <prefix dif>: d[%d][%d]: %f\n", i, j, d[i][j]);
        d[i+1][j+1] = minimum_f
                  (
                    d[i][j+1] + 1.0,
                    d[i+1][j] + 1.0,
                    d[i][j] + 1.0
                  );
//        printf("After <prefix dif>: d[%d][%d]: %f\n", i+1, j+1, d[i+1][j+1]);
      }
    }
  }

  r = d[s_size][t_size];
  norm_r = (float)r/(max_i(s_size, t_size));
  return norm_r; 
}
