#ifndef _sadconf_h
#define _satconf_h

bool satconf(char *filename);
void equiv_print(FILE *out, int vars, int old);
int verify_cl(FILE *out, int vars, vector<vector<int> > part_verify);
#endif
