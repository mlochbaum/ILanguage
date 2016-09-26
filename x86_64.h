// 16 integer registers:
// 0 rax  (result)
// 1 rcx  (arg 3)
// 2 rdx  (arg 2)
// 3 rbx
// 4 rsp
// 5 rbp
// 6 rsi  (arg 1)
// 7 rdi  (arg 0)
// 8 r8   (arg 4)
// 9 r9   (arg 5)
// ...
// 15 r15

typedef unsigned char UC;
typedef unsigned char Reg;
#define REG_RES 0
#define REG_ARG0 7
#define REG_ARG1 6
#define REG_ARG2 2
#define REG_ARG3 1
#define REG_ARG4 8
#define REG_ARG5 9
#define NO_REG 16
#define NO_REG_NM 17
typedef unsigned short RegM;
#define REG_MASK 48 // Don't modify rsp or rbp
#define REG_SAVE 4087 // Registers which function calls may modify

// Instructions
#define A_0REG(O,I) (((I)<<3) + (O))
#define A_REG(O,I) (0xC0 + A_0REG(O,I)) // TODO REX

#define MOV(O,I) {0x48,0x89,A_REG(O,I)}
#define ADD(O,I) {0x48,0x01,A_REG(O,I)}
#define SUB(O,I) {0x48,0x29,A_REG(O,I)}
#define IMUL(O,I) {0x48,0x0F,0xAF,A_REG(I,O)}
#define CMP(O,I) {0x48,0x39,A_REG(O,I)}
#define CMOVLE(O,I) {0x48,0x0F,0x4E,A_REG(I,O)}
#define NEG(O,I) {0x48,0xF7,A_REG(I,3)}

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
