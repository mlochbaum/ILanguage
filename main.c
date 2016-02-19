#include <stdio.h>
#include <string.h>

#include "type.h"
#include "name.h"
#include "scan.h"

int main(int argc, char** argv) {
  builtin_init();
  size_t c=1<<7;
  DECL_STR(in, c);
  while (1) {
    V v; Str s;

    if (getline(&in, &c, stdin) == -1) { break; }
    I l = strlen(in);
    if (l <= 1) break;
    puts(s = toString(v = scan(in))); ddel(v); FREE(s);
  }
  FREE(in);
  StrVfree(names);
  return 0;
}
