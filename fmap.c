#include <string.h>
#include "type.h"
#include "asm.h"

T mapclass_T(T t) {
  return ((t&CONST_t)?CONST_X:0) | ((t&FUNC_t)?FUNC_X:0)
    | ((t&L_t)?LIST_X:0);
}
X mapclass(V v) {
  X x; switch (T(v)) {
#define CASE(T) case T##_t:
    ON_TYPES(CONST, CASE) x.t=CONST_X; break;
    ON_TYPES(FUNC , CASE) x.t=FUNC_X; break;
    case L_t: x.t=LIST_X; x.v=&(L(v)->l); break;
#undef CASE
  }
  return x;
}

U mapclasseq(X x, X y) {
  return x.t==y.t && (x.t!=LIST_X || *((U*)x.v)==*((U*)y.v));
}

// TODO move/simplify
V constant(V v) {
  if (T(v)&CONST_t) return v;
  DECL(V*, vv); *vv=v; return newF(wrapF(newB('k'),1,vv));
}

V fmap(V f, U n, V* x, U d) {
  T t[n]; DO(i,n) t[i]=T(x[i]);
  V v; T(v)=fmap_T(f,n,t,d,d); P(v)=MALLOC(t_sizeof(T(v)));
  fmap_P(v,f,n,x,d); return v;
}
T fmap_T(V f, U n, T* x, U dl, U du) { return fmap_TT(T(f),n,x,dl,du); }
T fmap_TT(T f, U n, T* x, U dl, U du) {
  T tl=~0, tu=~0;
  DO(i, n) {
    if (! (dl&1<<i)) { tl &= mapclass_T(x[i]);
      if (! (du&1<<i)) tu &= mapclass_T(x[i]); }
  }
  if (!tu) return E_t;
  T t=0;
  if (tu & FUNC_X) t |= O_t;
  if (tu & LIST_X) t |= L_t;
  if (t==0 && (IMPURE(tu) || tu & CONST_X)) t = E_t;
  return t;
}

void fmap_LIST_P(V, V, U, V*, U, U);
void fmap_P(V v, V f, U n, V* x, U d) {
  X m={0,NULL};
  DO(i, n) if (! (d&1<<i)) {
    X mt=mapclass(x[i]);
    if (!m.t) m=mt;
    else if (!mapclasseq(m,mt)) {
      DO(j,n)del(x[j]); ERR("Incompatible mapclasses");
    }
  }
  switch (m.t) {
    case CONST_X: DO(i,n)del(x[i]); ERR("Domain error");
    case FUNC_X:  { DECL_ARR(V,xx,n); DO(i,n) {
                    if (d&1<<i) xx[i]=constant(cpy1(x[i]));
                    else xx[i]=cpy1(x[i]);
                  } setO(v, wrapO(cpy(f),n,xx)); return; }
    case LIST_X:  return fmap_LIST_P(v, f, n, x, d, *(U*)m.v);
  }
}


void fmap_LIST_P(V v, V f, U n, V* x, U d, U l) {
  U i; T ts[n]; U ss[n];
  DO(j,n) {
    ts[j] = d&1<<j ? T(x[j]) : L(x[j])->t; ss[j]=t_sizeof(ts[j]);
    if (!(d&1<<j)) get(x[j]);
  }
  T t = apply_T(f, n, ts); U s=t_sizeof(t);
  U c=next_pow_2(l); L ll = wrapL(t,c,l,0,MALLOC(c*s));
  // n_wrap lists need to wrap during traversal
  // The list to wrap is given by order[i] and the length
  // afterwards is wrap[order[i]].
  V xi[n]; U n_wrap=0; U order[n+1]; U wrap[n+1]; DO(j,n) {
    V *u=xi+j; U sj=ss[j]; T(*u)=ts[j];
    if (d&1<<j) P(*u)=MALLOC(sj);
    else {
      L lj=L(x[j]); U len0=lj->c-lj->o;
      P(*u)=LP(lj) + sj*lj->o;
      if (len0<l) {
        wrap[j]=len0;
        U k; for (k=n_wrap++; k>0&&len0<wrap[order[k-1]]; k--) {
          order[k]=order[k-1];
        } order[k]=j;
      }
    }
  } order[n_wrap]=n; wrap[n]=l;
  U len0=wrap[order[0]];

  AS as; A a=&as;
  if (l>=ASM_MIN_ITER && apply_R_full(a,f,n,ts)) {
    Reg rx[n], ri, rxi[n]; RegM u=0;
    DO(j,n+2) u|=1<<reg_args[j]; DO(j,n) rx[j]=reg_args[j+2];
    DO(j,n) rxi[j]=(d&1<<j)?rx[j]:NO_REG; a->i=rxi;
    u|=1<<(ri=REG_RES);

    RegM pop=start_A(a,n,u); a->o=NO_REG; choose_regn(a,n);
    DO(j,n) if (d&1<<j) asm_load(a,ts[j],rxi[j],rx[j]);
    ASM(a, XOR4,ri,ri); U label=a->l;
    DO(j,n) if (!(d&1<<j)) asm_load_at(a,ts[j],rxi[j],rx[j],ri);
    apply_A_full(a,f,n,ts);
    asm_write_at(a,t,REG_ARG0,a->o,ri);
    ASM(a, ADDI1,ri,1);
    ASM(a, CMP,ri,REG_ARG1);
    asm_jump(a, C_B,label);
    U wo,w=len0; i=0; while (w<l) {
      wo=w; U k=0; do { k++; w=wrap[order[i+k]]; } while (wo==w);
      ASM(a, MOV_RI,REG_ARG1,(Z)(w));
      U sc[k]; DO(j,k) { U o=order[i+j]; sc[j]=ss[o]*L(x[o])->c; }
      DO(j,k) ASM(a, SUBI4,rx[order[i+j]],sc[j]);
      ASM(a, CMP,ri,REG_ARG1);
      asm_jump(a, C_B,label);
      if (w<l) DO(j,k) ASM(a, ADDI4,rx[order[i+j]],sc[j]);
      i+=k;
    }
    P ff = finish_A(a,pop);
#define PI(i) (d&1<<i) ? P(x[i]) : LP(L(x[i])) + ss[i]*L(x[i])->o
    switch (n) {
      case 0: i = ((U(*)(P,Z))ff)(LP(ll), len0); break;
      case 1: i = ((U(*)(P,Z,P))ff)(LP(ll), len0, PI(0)); break;
      case 2: i = ((U(*)(P,Z,P,P))ff)(LP(ll), len0, PI(0), PI(1)); break;
      case 3: i = ((U(*)(P,Z,P,P,P))ff)(LP(ll), len0, PI(0), PI(1), PI(2)); break;
      case 4: i = ((U(*)(P,Z,P,P,P,P))ff)(LP(ll), len0, PI(0), PI(1), PI(2), PI(3)); break;
    }
    asm_unmap(a,ff);
#undef PI
  } else {
    if (n==1) {
      P px0=P(*xi); i=0; do { for (;i<len0;i++) {
        P(*xi)=px0+ss[0]*i;
        apply_P(TP(t, LP(ll) + s*i), f, n, xi);
        if (err) { DO(k,i) del(TP(t, LP(ll) + s*k)); FREEL(ll); i=l; break; }
      } px0-=ss[0]*L(x[0])->c; len0=l; } while (i<l);
    } else {
      P px[n]; DO(j,n) px[j]=P(xi[j]);
      U k=0; i=0; do { len0=wrap[order[k]]; for (;i<len0;i++) {
        DO(j,n) { V *u=xi+j; if (d&1<<j) cp_P(*u,x[j]); else P(*u)=px[j]+ss[j]*i; }
        apply_P(TP(t, LP(ll) + s*i), f, n, xi);
        if (err) { DO(k,i) del(TP(t, LP(ll) + s*k)); FREEL(ll); i=l; break; }
      } k++; } while (i<l);
    }
  }
  if (err) { DO(j,n) { if (!(d&1<<j)) {
    for(U k=i+1;k<l;k++) del(listV_ats(x[j],k,ss[j]));
  } } }
  DO(i, n) {
    if (d&1<<i) { del(x[i]); FREE(P(xi[i])); }
    else { FREEL(L(x[i])); }
  } if (!err) setL(v,ll);
}
