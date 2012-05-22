#include <string.h>

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

Str Zfmt(Zv z) {
  DECL_STR(s, 20); sprintf(s, "%lld", z); return s;
}
Str Rfmt(Rv r) {
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
  DECL_STR(s, 1); s[0]=' '; Str st; I len;
  if (ll==1) {
    st=toString(l->v[0]); len=1+strlen(st);
    s=realloc(s,len+3); strcpy(s+1,st); FREE(st);
    strcpy(s+len,".; "); return s;
  }
  I e=1; DDO(i, ll) {
    st=toString(LIST_AT(l,i)); len=1+strlen(st);
    s=realloc(s,e+len+(i==ll-1)); strcpy(s+e,st); FREE(st); e+=len;
    if(i==0) s[e-1]=';';
    else if(i==ll-1) {s[e-1]=' '; s[e]='\0';}
    else s[e-1]=',';
  }
  return s;
}
Str Afmt(A a) {
#define GETSTRING(st, i) v->v = ((char*)a->v)+size*i; st=toString(v)
  I l=a->l; I size=t_sizeof(a->t);
  if (l==0) return strdup("N");
  DECL_STR(s, 1); s[0]=' '; Str st; I len;
  DECL(V,v); v->t=a->t;
  if (l==1) {
    GETSTRING(st, 0); len=1+strlen(st);
    s=realloc(s,len+4); strcpy(s+1,st); FREE(st);
    strcpy(s+len,".; "); return s;
  }
  I e=1; DDO(i, l) {
    GETSTRING(st, i); len=1+strlen(st);
    s=realloc(s,e+len+(i==l-1)); strcpy(s+e,st); FREE(st); e+=len;
    if(i==0) s[e-1]=';';
    else if(i==l-1) {s[e-1]=' '; s[e]='\0';}
    else s[e-1]=',';
  }
  FREE(v); return s;
#undef GETSTRING
}

Str toString(V v) {
  switch (v->t) {
    case E_t: return *(E)v->v;
    case B_t: return chartostr(*(B)v->v);
    case F_t: return Ffmt((F)v->v);
    case O_t: return Ofmt((O)v->v);
    case Q_t: return quote('`', *(Q)v->v);
    case N_t: return quote('\'', *(N)v->v);
    case S_t: return quote('"', chartostr(*(S)v->v));
    case Z_t: return Zfmt(*(Z)v->v);
    case R_t: return Rfmt(*(R)v->v);
    case C_t: return strdup("Dude, that's complex."); //TODO
    case L_t: return Lfmt((L)v->v);
    case A_t: return Afmt((A)v->v);
  }
}
