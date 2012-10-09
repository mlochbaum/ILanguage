#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

/*
Verify the given string, checking that nesting symbols ()'`"
are correct. If they are, return NULL; otherwise, return
a string with an error message.

Rules:
  `"' must be paired with themselves, and between them all other
  characters have no special meaning.
  "" between quotes indicates a literal quote character. This does
  not change the verification algorithm.
  () must be paired, and characters between them retain their meanings.
*/
SET_STACK(char);

I isquote(char c) {
  switch (c) {
    case '\'':
    case '`':
    case '"': return 1;
    default: return 0;
  }
}
Str verify(Str s) {
  charStack stack = charnew();
  charpush(stack, '\0');
  while (*s) {
    if (*s == '\t') { charfree(stack); return "Tab in input"; }
    char last=charpeek(stack);
    if (isquote(last)) {
      if (last==*s) charpop(stack);
    } else {
      if (isquote(*s) || *s=='(') charpush(stack,*s);
      if (*s==')') {
        if (last=='(') { charpop(stack); last=charpeek(stack); }
        else { charfree(stack); return "Closed quote with no open quote"; }
      }
    }
    s++;
  }
  char last=charpeek(stack);
  charfree(stack);
  switch (last) {
    case '\'': return "Unterminated name";
    case '`': return "Unterminated backquote";
    case '"': return "Open quote";
    case '(': return "Open parenthesis";
    default: return NULL;
  }
}

/*
************* Scanning **************
At this point we know that quotes and parentheses are balanced,
so we can ignore all kinds of testing.
*/
typedef struct { V v; I p; } *W; // p is precedence
W newW(V v, I p) {
  DECL(W, w); w->p=p; w->v=v; return w;
}
SET_STACK(W);

void stackApplyStep(WStack stack) {
  W r=Wpop(stack), f=Wpop(stack), l=Wpop(stack);
  V v = apply2dd(f->v, l->v, r->v);
  Wpush(stack, newW(v, max(l->p, r->p)));
  FREE(f); FREE(l); FREE(r);
}
void stackApply(WStack stack, I p) {
  while (stack->l >= 3 &&
      stack->h->v->p >= stack->h->n->v->p &&
      p >= stack->h->v->p)
    stackApplyStep(stack);
}
void push(WStack stack, W w) {
  stackApply(stack, w->p);
  Wpush(stack, w);
}

W next(Str* s, I* lp) {
  I rp=0; I lf=0;
  V v = parseVal(s); W w;
  while (**s==' ' || **s=='\n') {
    rp++; if (**s=='\n') { lf++; rp=0; } (*s)++;
  }
  if (lf) { rp = lf*(1<<25) - rp; }
  w = newW(v, max(*lp,rp));
  *lp=rp;
  return w;
}

V parse(Str* s) {
  if (**s == ')') return Err("Empty parens"); 
  WStack stack = Wnew();
  I *p=MALLOC(sizeof(p)); *p=0;
  while (**s && **s!=')') {
    push(stack, next(s,p));
  }
  stack->h->v->p = INT_MAX;
  stackApply(stack, INT_MAX);
  W w = Wpop(stack); V v = w->v; FREE(w);
  (*s)++;
  if (stack->l) {
    while (stack->l) {
      W w = Wpop(stack); V v = w->v; FREE(w); del(v);
    } FREE(stack); FREE(p); del(v);
    return Err("Incorrect number of functions");
  }
  FREE(p);
  FREE(stack);
  return v;
}



V scan(Str c) {
  Str e = verify(c); if (e) return Err(e);
  return parse(&c);
}
