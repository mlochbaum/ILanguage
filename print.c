#include <stdio.h>
#include <string.h>

#include "type.h"

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
  DECL_STR(s, 20); sprintf(s, "%lld", z); return s;
}
Str Rfmt(R r) {
  DECL_STR(s, 20); sprintf(s, "%f", r); return s;
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
    DDO(i,ll) s[i+1] = ((C*)l->p)[(i+l->o)%l->c];
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
Str TShow(T t) {
  switch (t) {
    case E_t: return strdup("Error");
    case S_t: return strdup("Compiled");
    case B_t: return strdup("Builtin");
    case F_t: return strdup("Composition");
    case O_t: return strdup("Partial application");
    case Q_t: return strdup("Quasiquote");
    case N_t: return strdup("Name");
    case C_t: return strdup("Char");
    case Z_t: return strdup("Integer");
    case R_t: return strdup("Real");
    case K_t: return strdup("Complex");
    case L_t: return strdup("List");
    default: { DECL_STR(s, 28); sprintf(s, "Multitype (%llx)", t);
               return s; }
  }
}
Str Lshow(L l, I indent) {
  I ll=l->l;
  DECL_STR(sp, 0); Str st,si; I len;
  I e=1,et; DDO(i, ll) {
    st=PShowI(l->t, LIST_PTR_AT(l,i), indent+2); len=strlen(st);
    et=e; sp=realloc(sp, e = et+3+indent+len); si=sp+et-1;
    *(si++)='\n'; DDO(j,indent+2) *(si++)=' ';
    strcpy(si,st); FREE(st);
  }
  st = TShow(l->t);
  DECL_STR(s, strlen(st)+23+4*20+e);
  sprintf(s, "%s, ref %lld, %lld[%lld] (%lld allocated)%s",
              st,     l->r, l->o,l->l,  l->c,          sp);
  FREE(st); FREE(sp);
  return s;
}
Str PShowI(T t, P p, I indent) {
  switch (t) {
    case F_t: return Ffmt(*(F*)p); //TODO
    case O_t: return Ofmt(*(O*)p); //TODO
    case L_t: return Lshow(*(L*)p, indent);
    default: return PToString(t,p);
  }
}
Str Show(V v) {
  Str st=TShow(T(v)), sp=PShowI(T(v), P(v), 0);
  DECL_STR(s, strlen(st)+2+strlen(sp));
  sprintf(s, "%s: %s", st, sp); FREE(st); FREE(sp); return s;
}
