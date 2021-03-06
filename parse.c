#include <string.h>
#include "type.h"
#include "scan.h"

/////////////// parseChar //////////////////
V parseChar(Str* s) { return newB(*(*s)++); }

/////////////// parseNum ///////////////////
I isDigit(char c) { return '0'<=c && c<='9'; }
I parseDig(char c) { return (I)c - (I)'0'; }
R parseDec(Str* s);
// **s is a digit or underscore
V parseNum(Str* s) {
  Z i=0;
  I mult=1;
  if (**s=='_') { mult=-1; (*s)++; }
  while (isDigit(**s)) {
    i*=10; i+=parseDig(**s); (*s)++;
  }
  if (**s=='.' && isDigit((*s)[1])) {
    (*s)++; return newR(mult* (i+parseDec(s)));
  }
  else return newZ(mult*i);
}
R parseDec(Str* s) {
  R d=0; Str st;
  while (isDigit(**s)) (*s)++;
  st = *s-1;
  while (*st!='.') {
    d+=parseDig(*st); d/=10; st--;
  }
  return d;
}

//////////////// parseQuote /////////////////////
V parseQuote(Str* s) {
  char q=**s;
  I l=0; (*s)++;
  while ((*s)[l]!=q) l++;
  DECL_NSTR(ss,l);
  strncpy(ss, *s, l); (*s)+=l+1;
  switch(q) {
    case '`': return newQ(ss);
    case '\'': return newN(ss);
    case '"': return makeStr(ss);
  }
}

V parseVal(Str* s) {
  V v;
  switch (**s) {
    case '(':
      (*s)++; v = parse(s); break;
    case '\'': case '`': case '"':
      v = parseQuote(s); break;
    case'_':
    case'0': case'1': case'2': case'3': case'4':
    case'5': case'6': case'7': case'8': case'9': 
      v = parseNum(s); break;
    default:
      v = parseChar(s);
  }
  return v;
}
