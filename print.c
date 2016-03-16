#include <stdio.h>
#include <string.h>

#include "type.h"

// Size of I, Z, R under sprintf
#define SI 21
#define SZ 21
#define SR 29

Str PToString(T t, P p);
Str toString(V v);

Str chartostr(char c) {
  DECL_NSTR(s, 1); s[0]=c; return s;
}
Str quote(const char q, Str s) {
  I l = strlen(s);
  DECL_NSTR(r, l+2);
  strcpy(r+1, s); FREE(s);
  r[0]=r[l+1]=q; return r;
}

Str Zfmt(Z z) {
  DECL_STR(s, SZ);
  snprintf(s, SZ, "%ld", z); return s;
}
Str Rfmt(R r) {
  DECL_STR(s, SR);
  snprintf(s, SR, "%f", r); return s;
}

Str Ffmt(F f) {
  if (f->l==0) return strdup("System error: function with no args");
  Str ss = toString(f->x[0]); I l=1+strlen(ss);
  DECL_STR(s, l); s[0]=' '; strcpy(s+1, ss); FREE(ss);
  if (f->l==1) {
    ss = toString(f->f); I e=strlen(ss); s=realloc(s,l+3+e);
    s[l]='.'; strcpy(s+l+1, ss); s[l+1+e]=' '; s[l+2+e]='\0';
    FREE(ss); return s;
  } else if (f->l==2) {
    ss = toString(f->f); I lf=strlen(ss);
    Str sr = toString(f->x[1]); I lr=strlen(sr);
    s=realloc(s,l+lf+lr+2);
    strcpy(s+l, ss); strcpy(s+l+lf, sr); strcpy(s+l+lf+lr, " ");
    FREE(ss); FREE(sr); return s;
  }
}
Str Ofmt(O o) {
  if (o->l==0) return strdup("System error: compositon with no args");
  Str ss = toString(o->x[0]); I l=1+strlen(ss);
  DECL_STR(s, l); s[0]=' '; strcpy(s+1, ss); FREE(ss);
  if (o->l==1) {
    ss = toString(o->f); I e=strlen(ss); s=realloc(s,l+3+e);
    s[l]='o'; strcpy(s+l+1, ss); s[l+1+e]=' '; s[l+2+e]='\0';
    FREE(ss); return s;
  } else if (o->l==2) {
    ss = toString(o->f); I lf=strlen(ss);
    Str sr = toString(o->x[1]); I lr=strlen(sr);
    s=realloc(s,l+lf+lr+2);
    strcpy(s+l, ss); strcpy(s+l+lf, sr); strcpy(s+l+lf+lr, " ");
    FREE(ss); FREE(sr); return s;
  }
}

Str Lfmt(L l) {
  I ll=l->l;
  if (ll==0) return strdup("N");
  if (l->t == C_t) {
    DECL_NSTR(s, ll+2); s[0]=s[ll+1]='"';
    DDO(i,ll) s[i+1] = ((C*)LP(l))[(i+l->o)%l->c];
    return s;
  }
  DECL_STR(s, 1); s[0]=' '; Str st; I len;
  if (ll==1) {
    st=PToString(l->t, LIST_PTR_AT(l,0)); len=1+strlen(st);
    s=realloc(s,len+4); strcpy(s+1,st); FREE(st);
    strcpy(s+len,".; "); return s;
  }
  I e=1; DDO(i, ll) {
    st=PToString(l->t, LIST_PTR_AT(l,i)); len=1+strlen(st);
    s=realloc(s,e+len+(i==ll-1)); strcpy(s+e,st); FREE(st); e+=len;
    if(i==0) s[e-1]=';';
    else if(i==ll-1) {s[e-1]=' '; s[e]='\0';}
    else s[e-1]=',';
  }
  return s;
}

Str PToString(T t, P p) {
  switch (t) {
    case E_t: return *(E*)p;
    case B_t: return chartostr(*(B*)p);
    case F_t: return Ffmt(*(F*)p);
    case O_t: return Ofmt(*(O*)p);
    case Q_t: return quote('`', strdup(*(Q*)p));
    case N_t: return quote('\'', strdup(*(N*)p));
    case C_t: return quote('"', chartostr(*(C*)p));
    case Z_t: return Zfmt(*(Z*)p);
    case R_t: return Rfmt(*(R*)p);
    case K_t: return strdup("Dude, that's complex."); //TODO
    case L_t: return Lfmt(*(L*)p);
    default: return toString(*(V*)p);
  }
}
Str toString(V v) { return PToString(T(v), P(v)); }


// Show the exact state of an object (including reference count)
Str PShowI(T t, P p, I indent);
Str ShowI(V v, I indent);
Str TShow(T t) {
  switch (t) {
    case E_t: return strdup("Error");
    case S_t: return strdup("Compiled");
    case B_t: return strdup("Builtin");
    case O_t: return strdup("Composition");
    case F_t: return strdup("Partial application");
    case Q_t: return strdup("Quasiquote");
    case N_t: return strdup("Name");
    case C_t: return strdup("Char");
    case Z_t: return strdup("Integer");
    case R_t: return strdup("Real");
    case K_t: return strdup("Complex");
    case L_t: return strdup("List");
    default: { DECL_STR(s, 28); sprintf(s, "Multitype (%x)", t);
               return s; }
  }
}
// Append s to d with indent i and update e to point to the end of d.
Str appendI(Str d, Str s, I* e, I indent) {
  I et=*e; d=realloc(d, *e = et+1+indent+strlen(s)); Str di=d+et-1;
  *(di++)='\n'; DDO(j,indent) *(di++)=' ';
  strcpy(di,s); FREE(s); return d;
}
Str Lshow(L l, I indent) {
  DECL_STR(sp, 0); I in=indent+2, e=1;
  DDO(i, l->l) sp=appendI(sp, PShowI(l->t, LIST_PTR_AT(l,i), in), &e, in);
  Str st = TShow(l->t);
  DECL_STR(s, strlen(st)+23+4*SI+e);
  sprintf(s, "%s, ref %d, %d[%d] (%d allocated)%s",
              st, l->r,l->o,l->l,l->c,         sp);
  FREE(st); FREE(sp);
  return s;
}
Str Fshow(F f, I indent) {
  DECL_STR(sp, 0); I in=indent+2, e=1;
  sp=appendI(sp, ShowI(f->f, in), &e, in);
  DDO(i, f->l) sp=appendI(sp, ShowI(f->x[i], in), &e, in);
  DECL_STR(s, 11+2*SI+e);
  sprintf(s, "ref %d, %d args%s",
                f->r,f->l,   sp);
  FREE(sp);
  return s;
}
Str PShowI(T t, P p, I indent) {
  switch (t) {
    case O_t: case F_t: return Fshow(*(F*)p, indent);
    case L_t: return Lshow(*(L*)p, indent);
    default: { if (PURE(t)) return PToString(t,p);
               else return ShowI(*(V*)p, indent); }
  }
}
Str ShowI(V v, I indent) {
  Str st=TShow(T(v)), sp=PShowI(T(v), P(v), indent);
  DECL_STR(s, strlen(st)+2+strlen(sp));
  sprintf(s, "%s: %s", st, sp); FREE(st); FREE(sp); return s;
}
Str Show(V v) { return ShowI(v,0); }
