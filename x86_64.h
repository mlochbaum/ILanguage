// 16 integer registers:
// 0 rax  (result)
// 1 rcx  (arg 4)
// 2 rdx  (arg 3)
// 3 rbx
// 4 rsp
// 5 rbp
// 6 rsi  (arg 2)
// 7 rdi  (arg 1)
// 8 r8   (arg 5)
// 9 r9   (arg 6)
// ...
// 15 r15

typedef unsigned char UC;
typedef unsigned char Reg;
#define NO_REG 16
#define NO_REG_NM 17
typedef unsigned short RegM;

// Instructions
#define MOV(O,I) {0x48,0x89,A_REG(O,I)}
#define ADD(O,I) {0x48,0x01,A_REG(O,I)}
#define SUB(O,I) {0x48,0x29,A_REG(O,I)}
#define IMUL(O,I) {0x48,0x0F,0xAF,A_REG(I,O)}
#define CMP(O,I) {0x48,0x39,A_REG(O,I)}
#define CMOVLE(O,I) {0x48,0x0F,0x4E,A_REG(I,O)}
#define NEG(O,I) {0x48,0xF7,A_REG(I,3)}
#define A_REG(O,I) (0xC0 + ((I)<<3) + (O)) // TODO REX

// Frame to convert from A to S
// Args: P rdi, V {rsi rdx}, I ecx, V* r8

// push  rdx
// mov   rdi,QWORD PTR [r8+0x8]
// mov   rdi,QWORD PTR [rdi]
#define PRE1 {0x52                 \
             ,0x49,0x8b,0x78,0x08  \
             ,0x48,0x8b,0x3f}

// push  rdx
// mov   rdi,QWORD PTR [r8+0x8]
// mov   rsi,QWORD PTR [r8+0x18]
// mov   rdi,QWORD PTR [rdi]
// mov   rsi,QWORD PTR [rsi]
#define PRE2 {0x52                 \
             ,0x49,0x8b,0x78,0x08  \
             ,0x49,0x8b,0x70,0x18  \
             ,0x48,0x8b,0x3f       \
             ,0x48,0x8b,0x36}

// TODO test output type rsi; change size based on type
// pop rdx
// mov QWORD PTR [rdx],rax
// ret
#define POST {0x5a            \
             ,0x48,0x89,0x02  \
             ,0xc3}
