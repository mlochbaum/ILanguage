#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "type.h"
#include "mem.h"
#include "apply.h"
#include "stack.h"
#include "mem.c"
#include "hash.c"
#include "parse.c"
#include "builtin.c"
#include "apply.c"
#include "dom.c"
#include "fmap.c"
#include "scan.c"
#include "print.c"

V scan(Str);
Str toString(V v);

int main() {
  builtin_init();
  size_t c=1<<7;
  DECL_STR(in, c);
  while (1) {
    V v; Str s;

    if (getline(&in, &c, stdin) == -1) { break; }
    I l = strlen(in);
    if (l <= 1) break;
    //Temporary measure because '\n' is not treated as whitespace
    if (in[l-1]=='\n') in[l-1]='\0';
    puts(s = toString(v = scan(in))); del(v); FREE(s);
  }
  FREE(in);
  StrVfree(names);
  return 0;
}
