#include <string.h>
#include "builtin.h"
#include "arith.h"

D_P1(itemize) { setL(p, wrapL(T(l),1,1,0,P(cpy1(l)))); }
D_P2(cross) {
  T tl=T(l), tr=T(r), t=tl|tr; U s=t_sizeof(t); P v=MALLOC(2*s);
  if (PURE(t)) { memcpy(v,P(l),s); memcpy(v+s,P(r),s); }
  else { ((V*)v)[0]=cpy1(l); ((V*)v)[1]=cpy1(r); }
  setL(p, wrapL(t,2,2,0,v));
}

void resize(L l, U s, U c) { // Assume resizing up
  if (l->c>=c) return; l->p=realloc(l->p, s*c);
  if (l->l+l->o > l->c) memcpy(LP(l)+s*l->c, LP(l), s*(l->l+l->o-l->c));
  l->c=c;
}

void listcpy1(L d, L s, U i) {
#define DS s1-s0
#define DD d1-d0
  U ss=t_sizeof(d->t);
  P d0=LIST_PTR_ATS(d,i,ss), d1=LP(d)+d->c*ss;
  P s0=LIST_PTR_ATS(s,0,ss), s1=LP(s)+s->c*ss;
  I l=s->l*ss;
  if (DS <= DD) {
    memcpy(d0,s0,min(l,DS)); if((l-=DS)<=0)return; s0=LP(s); d0+=DS;
    memcpy(d0,s0,min(l,DD)); if((l-=DD)<=0)return; s0+=DD; d0=LP(d);
  } else {
    memcpy(d0,s0,min(l,DD)); if((l-=DD)<=0)return; s0+=DD; d0=LP(d);
    memcpy(d0,s0,min(l,DS)); if((l-=DS)<=0)return; s0=LP(s); d0+=DS;
  }
  memcpy(d0,s0,l);
#undef DS
#undef DD
}
void listcpy(L d, L s, U i) {
  s->r--; if (s->l) {
    d->l += s->l;
    if (s->r && s->t&COMP_t) {
      // Increase reference count of source's children
      U ss=t_sizeof(s->t);
      DO(j,s->l) (**(U**)(LIST_PTR_ATS(s,j,ss)))++;
    }
    if (PURE(d->t)==PURE(s->t)) listcpy1(d,s,i);
    else { DO(j,s->l) LIST_AT(d,i+j) = cpy1(list_at(s,j)); }
  }
  if (!s->r) { FREEL(s); }
}
/* Assume (^l->t)|r->t and l->c>=l->l+r->l.
 * Copy the contents of r into l. */
void append(L l, L r) { listcpy(l, r, l->l); }
/* Assume l->t|(^r->t) and r->c>=l->l+r->l.
 * Copy the contents of l into r. */
void prepend(L l, L r) { r->o += r->c-l->l; r->o%=r->c; listcpy(r,l,0); }

#define REL(v) return setL(p,v);
D_P2(concat) {
  if (T(l)==L_t) { L lv=L(l);
    if (T(r)==L_t) { L rv=L(r);
      T t=lv->t|rv->t; U ll=lv->l+rv->l, c=next_pow_2(ll);
      U s=t_sizeof(t);
      B ifl = lv->r==1 && PURE(lv->t)==PURE(t);
      B ifr = rv->r==1 && PURE(rv->t)==PURE(t);
      if (ifl  &&  lv->l >= ifr*rv->l) {
        resize(lv,s,c); append(lv,rv); REL(lv);
      } else if (ifr) {
        resize(rv,s,c); prepend(lv,rv); REL(rv);
      } else {
        L v=wrapL(t,c,0,0,MALLOC(s*c));
        append(v,lv); append(v,rv); REL(v);
      }
    } else {
      T t=lv->t|T(r); U ll=lv->l+1, c=next_pow_2(ll); U s=t_sizeof(t);
      L v; if (lv->r==1 && PURE(lv->t)==PURE(t)) {
        resize(lv,s,c); v=lv;
      } else {
        v=wrapL(t,c,0,0,MALLOC(s*c)); append(v,lv);
      }
      mv_P(list_at(v,v->l), r); v->l=ll; REL(v);
    }
  } else {
    return cross_p2(p,l,r);
  }
}
#undef REL

D_L1(list) { return !!(l&L_t); }
D_D1(list) { return list_l1(T(l)); }
D_P1(first) { mv_Pd(p, cpy(listV_at(l,0))); del(l); }
D_P1(last) { mv_Pd(p, cpy(listV_at(l,L(l)->l-1))); del(l); }

D_L2(select) { return 2*!!(r&ARITH_t) + !!(l&L_t); }
D_D2(select) { return select_l2(T(l),T(r)); }
D_P2(select) {
  Z i=Z(r), ll=L(l)->l; if (i<0 && i>-ll) i+=ll;
  mv_Pd(p, cpy(listV_at(l,i))); del(l);del(r);
}
// drop a elements from the beginning and b from the end of l
void cropList(V p, V l, U a, U b) {
  L lv=L(l);
  if (lv->r > 1) {
    U nl=lv->l-a-b;
    T t=lv->t; U s=t_sizeof(t); P v=MALLOC(s*next_pow_2(nl));
    DO(i,nl) valcpy(v+s*i, LIST_PTR_ATS(lv,i+a,s), t);
    del(l); setL(p, wrapArray(t, nl, v));
  } else {
    if (a>0) { DO(j,a) del(list_at(lv,a)); }
    if (b>0) { DO(j,b) del(list_at(lv,lv->l-b+j)); }
    lv->o = (lv->o+a) % lv->c; lv->l -= a+b;
    mv_P(p, l);
  }
}
D_P2(take) {
  Z i=Z(r), ll=L(l)->l; del(r);
  if (i>=0) cropList(p, l, 0, ll-i); else cropList(p, l, ll+i, 0);
}
D_P2(drop) {
  Z i=Z(r); del(r);
  if (i>=0) cropList(p, l, i, 0); else cropList(p, l, 0, -i);
}

D_L2(list) { return 2*!!(r&L_t) + !!(l&L_t); }
D_D2(list) { return list_l2(T(l),T(r)); }
// Turn v into a modifiable list including type t.
void addtype(V v, T t) {
  L l=L(v); T lt=l->t;
  if (lt&t || IMPURE(lt)) get(v);
  else {
    U n=l->l, c=next_pow_2(n), s=t_sizeof(lt);
    DECL_ARR(V,vs,c); DO(i,n) { vs[i] = cpy1(list_ats(l,i,s)); }
    if (l->r<=1) {
      FREE(l->p); l->r=1; l->t|=t; l->c=c; l->o=0; l->p=(P)vs;
    } else {
      l->r--; L(v) = wrapL(lt|t, c, n, 0, vs);
    }
  }
}
D_P2(amend) {
  L rv=L(r);
#define ERR1(m) del(l); del(r); ERR(m)
  if (rv->l != 2) { ERR1("Argument to { must be a two-element list"); }
  V vi=list_at(rv,0); PURIFY(vi);
  if (T(vi)&CONST_t) {
    if (T(vi)!=Z_t) { ERR1("Index in { must be an integer"); }
    get(r); rv=L(r); V v=list_at(rv,1); PURIFY(v);
    addtype(l,T(v));
    V dst=listV_at(l,Z(vi)); del(dst);
    mv_Pd(dst, cpy(v)); del(r); mv_P(p, l);
  } else {
    ERR1("Form of argument to { is invalid");
  }
#undef ERR1
}

D_P1(length) { U n; if (T(l)&L_t) n=L(l)->l; else n=1; del(l); setZ(p,n); }
D_L2(copy) { return 2*!!(r&ARITH_t) + 1; }
D_D2(copy) { return copy_l2(T(l),T(r)); }
D_P2(copy) {
  Z ll;
  if (T(r)==R_t) {
    R rl=getR(r); ll=rl;
    if (ll!=rl) { err=strdup("Argument to # is not an integer"); }
  } else {
    ll=getZ(r);
  }
  del(r);
  if (!err && ll<0) { err=strdup("Argument to # is negative"); }
  if (err) { del(l); return; }
  T t=T(l); U s=t_sizeof(T(l)), lu=ll;
  P v=MALLOC(next_pow_2(lu*s)); DO(i,lu) valcpy(v+i*s,P(l),t);
  del(l); setL(p, wrapArray(t, lu, v));
}

D_P1(iota) {
  Z lz=getCeilZ(l); del(l);
  B w=(lz<0); U ll=w?-lz:lz;
  U c=next_pow_2(ll); DECL_ARR(Z,v,c);
  if (w) { DO(i,ll)v[i]=ll-1-i; } else { DO(i,ll)v[i]=i; }
  setL(p, wrapL(Z_t, c, ll, 0, v));
}
D_P2(iota) {
  switch (T(l)) {
    case Z_t: { Z lz=getZ(l), ll=getCeilZ(r)-lz; del(l); del(r);
                I s=sign(ll); U lu=ll*s; U c=next_pow_2(lu); DECL_ARR(Z,v,c);
                DO(i,lu)v[i]=lz+s*i; return setL(p, wrapL(Z_t, c, lu, 0, v));
              }
    case R_t: { R lr=getR(l), llr=getR(r)-lr; del(l); del(r);
                I s=sign(llr); U ll=ceiling(s*llr);
                U c=next_pow_2(ll); DECL_ARR(R,v,c);
                DO(i,ll)v[i]=lr+(R)s*i; return setL(p, wrapL(R_t, c, ll, 0, v));
              }
  }
}

D_L11(reduce) { return !!(ll&(L_t|CONST_t)); }
D_D11(reduce) { return reduce_l11(l,T(ll)); }
D_L12(reduce) { return 2 + !!(ll&(L_t|CONST_t)); }
D_D12(reduce) { return reduce_l12(l,T(ll),T(rr)); }
D_T1(identity_of) { return E_t; }
D_P1(identity_of) { del(l); ERR("Identity unknown"); }
static void reduce_sub(V p, V l, V ll, U i, V v) {
  U len=L(ll)->l;
  if (len<i) { del(ll); return identity_of_p1(p,l); }
  T t[3]; t[1]=t[2]=L(ll)->t; U s=t_sizeof(t[1]);
  get(ll); V vt;
  if (i) v=listV_ats(ll,0,s); else t[1]=T(v); v=cpy1(v);
  for(;i<len&&(!err);i++) {
    t[0]=t[2]; t[2]=apply_T(l,2,t); if(t[0]==t[2]) break;
    vt=v; T(v)=t[2]; P(v)=MALLOC(t_sizeof(T(v)));
    apply2_P(v, l, vt, listV_ats(ll,i,s)); FREE(P(vt));
  }
  if (i<len && !err) {
    AS as; A a=&as;
    if (len-i>=ASM_MIN_ITER && apply_R_full(a,l,2,t)) {
      RegM u=1<<REG_ARG0|1<<REG_ARG1; Reg ai[2]; a->i=ai;
      a->o=ai[0]=REG_RES; ai[1]=get_reg(a->u|u|1<<ai[0]);
      RegM pop=start_A(a,2,u);
      ASM(a, PUSH,REG_ARG2,-);
      asm_load(a,t[0],ai[0],REG_ARG2); U label=a->l;
      asm_load(a,t[1],ai[1],REG_ARG0);
      apply_A_full(a,l,2,t);
      ASM(a, ADDI1,REG_ARG0,s);
      ASM(a, CMP,REG_ARG0,REG_ARG1);
      asm_jump(a, C_B, label);
      U c=L(ll)->c, o=L(ll)->o;
      P lp=LP(L(ll)), end = lp+s*((o+len-1)%c+1);
      if (o+len > c && o+i < c) {
        ASM(a, MOV,REG_ARG2,REG_ARG1);
        ASM(a, MOV_RI,REG_ARG1, (Z)(end));
        ASM(a, SUBI4,REG_ARG0, s*c);
        ASM(a, CMP,REG_ARG1,REG_ARG2);
        asm_jump(a, C_NE, label);
        end = lp+s*c;
      }
      ASM(a, POP,REG_ARG2,-);
      asm_write(a,t[0],REG_ARG2,REG_RES);
      void (*f)(P,P,P) = finish_A(a,pop);
      f(lp+s*(o+i), end, v.p); asm_unmap(a,f);
    } else {
      do { apply2_P(v, l, v, listV_ats(ll,i,s)); } while (++i<len&&(!err));
    }
  }
  if (err) for(;i<len;i++) del(listV_ats(ll,i,s)); else mv_P(p,v);
  FREEL(L(ll)); FREE(P(v));
}
D_P11(reduce) {
  if (!(T(ll)&L_t)) { return mv_P(p, ll); }
  reduce_sub(p, l, ll, 1, ll);
}
D_P12(reduce) {
  if (!(T(ll)&L_t)) { V(p)=apply2(l,ll,rr); return; }
  reduce_sub(p, l, ll, 0, rr);
}

D_P1(reverse) {
  get(l); L v=L(l); U n=v->l; T t=v->t;
  if (!PURE(t)) {
    DO(i,n/2) {
      V vt=LIST_AT(v,i); LIST_AT(v,i)=LIST_AT(v,n-i-1);
      LIST_AT(v,n-i-1)=vt;
    }
  } else {
#define AT(i) LIST_PTR_ATS(v,i,s)
    U s=t_sizeof(t); P p=MALLOC(s);
    DO(i,n/2) {
      memcpy(p, AT(i), s); memcpy(AT(i), AT(n-i-1), s);
      memcpy(AT(n-i-1), p, s);
    }
    FREE(p);
#undef AT
  }
  setL(p,L(l));
}

D_P2(rotate) {
  I rot;
  if (T(r)==R_t) {
    R rr=getR(r); rot=rr;
    if (rot!=rr) { del(r); del(l); ERR("Rotation amount is not an integer"); }
  } else {
    rot=getZ(r);
  }
  del(r);
  get(l); L v=L(l); setL(p,v);
  U n=v->l, o=v->o, c=v->c, a, b; // Move rot elts from a to b
  if (n==c) { v->o=(o+rot)%c; return; }
  U rtu = rot%n;
  if (rtu <= n/2) { a=o; b=n; o=rtu; }
  else { a=o+rtu; if(a>=c)a-=c; rtu=n-rtu; o=b=c-n; }
  b+=a; if (b>=c) b-=c;
  o+=a; if (o>=c) o-=c; v->o=o;

  U s=t_sizeof(v->t); rtu*=s; a*=s; b*=s; c*=s;
  P q=v->p, stop=q+rtu, s1=q+c-a, s2=q+c-b;
  if (s1<stop) {
    if (s2<s1) { memmove(q+b,q+a,s2-q); q=s2; s2=stop; b-=c; }
    memmove(q+b,q+a,s1-q); q=s1; a-=c;
  }
  if (s2<stop) { memmove(q+b,q+a,s2-q); q=s2; b-=c; }
  memmove(q+b,q+a,stop-q);
}

U count_indices(L l, U n, U* over) {
  if (l->t!=Z_t) {ERR("Argument must be a list of integers")0;}
  U o=0, s=0; DO(i,n) {
    Z e=LIST_AT_T(Z,l,i);
    if (e<0) {ERR("Numbers in argument must not be negative")0;}
    s+=e; if (s>++o) o=s;
  }
  *over=o-n; return s;
}

D_P1(indices) {
  L il=L(l); U n=il->l;
  U o=0, s=count_indices(il, n, &o); if (err) { del(l); return; }
  U c=il->c, oi=il->o, ie=min(c-oi,n);
  Z *v0=LP(il), *v=v0+oi, *d, *ds=0;
  if (il->r==1 && o<=c-n) {
    ds=v0+c; d=v0 + (il->o = o>oi ? oi+n : c>=oi+n ? 0 : oi+n-c);
    il->l=s; setL(p, il);
  } else {
    U rc=next_pow_2(s); d=MALLOC(rc*sizeof(Z));
    il->r--; setL(p, wrapL(Z_t, rc, s, 0, d));
  }
  U i=0; Z *de=d;
  do {
    for (; i<ie; i++) {
      de+=v[i]; while (d<de) { *d++=i; if(d==ds){d-=c;de-=c;} }
    }
    v-=c; ie=n;
  } while (i<n);
  if (!il->r) { il->r++; del(l); }
}

D_P2(replicate) {
  L il=L(r), ll=L(l); U n=il->l;
  if (n!=ll->l) { del(l); del(r); ERR("Lengths must match"); }
  U o=0, s=count_indices(il, n, &o); if (err) { del(l); del(r); return; }
  T t=ll->t; U ls=t_sizeof(t), c=ll->c;
  P l0=ll->p, src=l0+ll->o*ls, dst, stop=l0+c*ls;
  if (ll->r==1 && o<=c-n) {
    U oi=ll->o;
    dst=l0+ls*(ll->o = o>oi ? oi+n : c>=oi+n ? 0 : oi+n-c);
    ll->l=s; setL(p, ll);
  } else {
    U rc=next_pow_2(s); dst=MALLOC(rc*ls);
    ll->r--; setL(p, wrapL(t, rc, s, 0, dst));
  }
  Z *v=LP(il)+il->o; U i=0, ie=min(il->c-il->o,n);
  do {
    for (; i<ie; i++) {
      for (U j=v[i]; j--; ) {
        memcpy(dst, src, ls);
        dst+=ls; if(dst==stop)dst=l0;
      }
      src+=ls; if(src==stop)src=l0;
    }
    v-=il->c; ie=n;
  } while (i<n);
  del(r);
  if (!ll->r) { ll->r++; del(l); }
}


void list_init() {
  DB(t1,';',L); DB(p1,';',itemize);
  DB(t2,';',L); DB(p2,';',cross);

  DB(t2,',',L); DB(p2,',',concat);

  B_u1['g']=DB(l1,'g',list); DB(d1,'g',list);
  DB(t1,'g',V); DB(p1,'g',first);
  B_u1['G']=DB(l1,'G',list); DB(d1,'G',list);
  DB(t1,'G',V); DB(p1,'G',last);

  B_u2['}']=DB(l2,'}',select); DB(d2,'}',select);
  B_u2['g']=DB(l2,'g',select); DB(d2,'g',select);
  B_u2['G']=DB(l2,'G',select); DB(d2,'G',select);
  DB(t2,'}',V); DB(p2,'}',select);
  DB(t2,'g',V); DB(p2,'g',take);
  DB(t2,'G',V); DB(p2,'G',drop);
  B_u2['{']=DB(l2,'{',list); DB(d2,'{',list);
  DB(t2,'{',L); DB(p2,'{',amend);

  DB(t1,'#',Z); DB(p1,'#',length);
  B_u2['#']=DB(l2,'#',copy); DB(d2,'#',copy);
  DB(t2,'#',L); DB(p2,'#',copy);

  B_u1['i']=DB(l1,'i',arith); DB(d1,'i',arith);
  DB(t1,'i',L); DB(p1,'i',iota);
  B_u2['i']=DB(l2,'i',arith); DB(d2,'i',arith);
  DB(t2,'i',L); DB(p2,'i',iota);

  B_u11['r']=DB(l11,'r',reduce); DB(d11,'r',reduce);
  DB(t11,'r',V); DB(p11,'r',reduce);
  B_u12['r']=DB(l12,'r',reduce); DB(d12,'r',reduce);
  DB(t12,'r',V); DB(p12,'r',reduce);

  B_u1['z']=DB(l1,'z',list); DB(d1,'z',list);
  DB(t1,'z',L); DB(p1,'z',reverse);
  B_u2['z']=DB(l2,'z',select); DB(d2,'z',select);
  DB(t2,'z',L); DB(p2,'z',rotate);

  B_u1['\\']=DB(l1,'\\',list); DB(d1,'\\',list);
  DB(t1,'\\',L); DB(p1,'\\',indices);
  B_u2['\\']=DB(l2,'\\',list); DB(d2,'\\',list);
  DB(t2,'\\',L); DB(p2,'\\',replicate);
}
