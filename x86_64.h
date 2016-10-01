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
#define A_0REG(O,I) ((((I)&7)<<3) + ((O)&7))
#define A_REG(O,I) (0xC0 + A_0REG(O,I))
#define REX8(O,I) (0x48 + ((O)>7)+(((I)>7)<<2))

#define MOV(O,I)  {REX8(O,I),0x89,A_REG(O,I)}
#define ADD(O,I)  {REX8(O,I),0x01,A_REG(O,I)}
#define SUB(O,I)  {REX8(O,I),0x29,A_REG(O,I)}
#define IMUL(I,O) {REX8(O,I),0x0F,0xAF,A_REG(O,I)}
#define CMP(O,I)  {REX8(O,I),0x39,A_REG(O,I)}
#define CMOVLE(I,O) {REX8(O,I),0x0F,0x4E,A_REG(O,I)}
#define NEG(I,O)  {REX8(O,0),0xF7,A_REG(O,3)}

#define ADDI(O,I) {REX8(O,0),0x83,A_REG(O,0),I}

#define PUSH(O,I) {0x50+O}
#define POP(O,I)  {0x58+O}

#define BYTES4(I) ((UC)(I)),((UC)((I)>>8)),((UC)((I)>>16)),((UC)((I)>>24))

#define MOV_MR(O,I,OFF) {REX8(O,I),0x89,0x40+A_0REG(O,I),OFF}
#define MOV_MR0(O,I)    {REX8(O,I),0x89,A_0REG(O,I)}
#define MOV_MI(O,I,OFF) {0xC7,0x40+A_0REG(O,0),OFF,BYTES4(I)}
#define MOV_RI(O,I)     {0xB8+(O) , BYTES4(I)}
#define MOV_RM(I,O,OFF) {REX8(O,I),0x8B,0x40+A_0REG(O,I),OFF}
#define MOV_RM0(I,O)    {REX8(O,I),0x8B,A_0REG(O,I)}

#define BYTES8(I) ((UC)(I)),((UC)((I)>>8)),((UC)((I)>>16)),((UC)((I)>>24)) \
          ,((UC)((I)>>32)),((UC)((I)>>40)),((UC)((I)>>48)),((UC)((I)>>56))
#define MOV_RI8(O,I) {REX8(O,0),0xB8+(O) , BYTES8(I)}

#define CALL(O,I) {0xFF,A_REG(O,2)}

#define JO(O,I)  {0x70,((UC)(O)-2)}
#define JNO(O,I) {0x71,((UC)(O)-2)}
#define JB(O,I)  {0x72,((UC)(O)-2)}
#define JAE(O,I) {0x73,((UC)(O)-2)}
#define JE(O,I)  {0x74,((UC)(O)-2)}
#define JNE(O,I) {0x75,((UC)(O)-2)}
#define JBE(O,I) {0x76,((UC)(O)-2)}
#define JA(O,I)  {0x77,((UC)(O)-2)}
#define JS(O,I)  {0x78,((UC)(O)-2)}
#define JNS(O,I) {0x79,((UC)(O)-2)}
#define JP(O,I)  {0x7A,((UC)(O)-2)}
#define JNP(O,I) {0x7B,((UC)(O)-2)}
#define JL(O,I)  {0x7C,((UC)(O)-2)}
#define JGE(O,I) {0x7D,((UC)(O)-2)}
#define JLE(O,I) {0x7E,((UC)(O)-2)}
#define JG(O,I)  {0x7F,((UC)(O)-2)}

#define SUBI4(O,I) {REX8(O,0),0x81,A_REG(O,5),BYTES4(I)}

#define RET {0xC3}
