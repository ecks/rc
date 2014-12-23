#ifndef ALGORITHMS_H
#define ALGORITHMS_H

struct prefix * encode(struct route_table * rt, int * len);

float levenshtein(struct prefix * s, int s_size, struct prefix * t, int t_size, struct route_table * srt, struct route_table * trt);

float ngram(char * s_txt, char * t_txt);
#endif
