#include <stdio.h>
#include <conio.h>

#include <stdexcept>

#include "Int.hpp"

#define SBYTE(x) int8_t(x)
#define UBYTE(x) uint8_t(x)

#define CALL_METHOD(object, methodPtr)  ((object)->*(methodPtr))

#define BIT0 1
#define BIT1 2
#define BIT2 4
#define BIT3 8
#define BIT4 16
#define BIT5 32
#define BIT6 64
#define BIT7 128

#define GET_BIT(x, y) bool((x) & BIT##y)
#define SET_BIT(x, y)   x |= BIT##y
#define CLEAR_BIT(x, y) x &= ~BIT##y
#define SET_BIT_VAL(x, y, z) x = ((-bool(z)) & (x | BIT##y)) | (x & ~BIT##y)

#define ADDIF(a, b, f) a += (b) & -bool(f)

#define BYTE(v, n) (((uint8_t*)&v)[n])

struct CPU {
       static const int MEM_SIZE = 0x10000;
       static const int OP_TABLE_SIZE = 0x100;
       
       static const int STACK_START = 0x100;
       static const int STACK_LEN = 0x100;
       
       static const int NMI_ADDR = 0xFFFA;
       static const int RESET_ADDR = 0xFFFC;
       static const int IRQ_ADDR = 0xFFFE;
       
       static const uint8_t CARRY = 1;
       static const uint8_t ZERO  = 2;
       static const uint8_t NO_IRQ = 4;
       static const uint8_t DECIMAL = 8;
       static const uint8_t BRK_CMD = 16;
       static const uint8_t OVERFLOW = 64;
       static const uint8_t NEGATIVE = 128;
       
       typedef union Operands {
               uint16_t w;
               uint8_t b[2];
       };
       
       typedef void (CPU::*Op)(Operands p);
       
       uint8_t regA, regX, regY, regS, regF;
       uint8_t mem[MEM_SIZE];
       Op opTable[OP_TABLE_SIZE];
       int opSizeTable[OP_TABLE_SIZE];
       int pc;
       
       bool getFlag(uint8_t mask) const
       {
           return regF & mask;
       }
       
       void setFlag(uint8_t mask)
       {
           regF |= mask;
       }   
       
       void clearFlag(uint8_t mask)
       {
           regF &= ~mask;
       }
       
       void setFlagVal(uint8_t mask, bool v)
       {
           regF = ((-v) & (regF | mask)) | (regF & ~mask);
       }
       
       void updateZeroFlag(uint8_t n)
       {
            setFlagVal(ZERO, !n);
       }
       
       void updateNegativeFlag(uint8_t n)
       {
           setFlagVal(NEGATIVE, GET_BIT(n, 7));
       }
       
       void push(uint8_t n)
       {
            mem[STACK_START + regS] = n;
            --regS;
       }
       
       void pop()
       {
            ++regS;
       }
       
       uint8_t peek() const
       {
           return mem[STACK_START + regS];
       }
       
       void die(Operands p)
       {
           throw std::runtime_error("Invalid instruction!");
       }
       
       // *****************
       // * Op prototypes *
       // *****************
       
       void _adc(uint8_t n)
       {
           int ures = regA + n + getFlag(CARRY);
           int sres = SBYTE(regA) + SBYTE(n) + getFlag(CARRY);
           
           regA = ures & 0xFF;
           
           setFlagVal(CARRY, ures > 255);
           setFlagVal(OVERFLOW, sres < -128 || sres > 127);
           updateZeroFlag(regA);
           updateNegativeFlag(regA);
       }
       
       void _and(uint8_t n)
       {
           regA &= n;
           updateZeroFlag(regA);
           updateNegativeFlag(regA);
       }
       
       void _asl(uint8_t& n)
       {            
            setFlagVal(CARRY, GET_BIT(n, 7));
            n <<= 1;
            updateZeroFlag(n);
            updateNegativeFlag(n);
       }
       
       void _bcc(uint8_t n)
       {
            ADDIF(pc, SBYTE(n), !getFlag(CARRY));
       }
       
       void _bcs(uint8_t n)
       {
            ADDIF(pc, SBYTE(n), getFlag(CARRY));
       }
       
       void _beq(uint8_t n)
       {
            ADDIF(pc, SBYTE(n), getFlag(ZERO));
       }
       
       void _bit(uint8_t n)
       {
            updateZeroFlag(regA & n);
            updateNegativeFlag(n);
            setFlagVal(OVERFLOW, GET_BIT(n, 6));
       }
       
       void _bmi(uint8_t n)
       {
            if(getFlag(NEGATIVE))
               pc += SBYTE(n);
       }
       
       void _bne(uint8_t n)
       {
             ADDIF(pc, SBYTE(n), !getFlag(ZERO));
       }
       
       void _bpl(uint8_t n)
       {
            ADDIF(pc, SBYTE(n), !getFlag(NEGATIVE));
       }
       
// TODO:
       void _brk()
       {
            setFlag(BRK_CMD);
       }

       void _bvc(uint8_t n)
       {
            ADDIF(pc, SBYTE(n), !getFlag(OVERFLOW));
       }
       
       void _bvs(uint8_t n)
       {
            ADDIF(pc, SBYTE(n), getFlag(OVERFLOW));
       }
       
       void _clc()
       {
            clearFlag(CARRY);
       }
       
       void _cld()
       {
            clearFlag(DECIMAL);
       }
       
       void _cli()
       {
            clearFlag(NO_IRQ);
       }
       
       void _clv()
       {
            clearFlag(OVERFLOW);
       }
       
       void _cmp(uint8_t n)
       {
            uint8_t res = regA - n;
            
            if(!res)
            {
               setFlag(ZERO);
               clearFlag(CARRY);
               clearFlag(NEGATIVE);
            }
            
            else
            {
                setFlagVal(CARRY, regA > n);
                updateNegativeFlag(res);
            }
       }
       
       void _cpx(uint8_t n)
       {
            uint8_t res = regX - n;
            
            if(!res)
            {
               setFlag(ZERO);
               clearFlag(CARRY);
               clearFlag(NEGATIVE);
            }
            
            else
            {
                setFlagVal(CARRY, regX > n);
                updateNegativeFlag(res);
            }
       }
       
       void _cpy(uint8_t n)
       {
            uint8_t res = regY - n;
            
            if(!res)
            {
               setFlag(ZERO);
               clearFlag(CARRY);
               clearFlag(NEGATIVE);
            }
            
            else
            {
                setFlagVal(CARRY, regY > n);
                updateNegativeFlag(res);
            }
       }
       
       void _dec(uint8_t& n)
       {
            --n;
            updateZeroFlag(n);
            updateNegativeFlag(n);
       }
       
       void _dex()
       {
            --regX;
            updateZeroFlag(regX);
            updateNegativeFlag(regX);
       }
       
       void _dey()
       {
            --regY;
            updateZeroFlag(regY);
            updateNegativeFlag(regY);
       }
       
       void _eor(uint8_t n)
       {
            regA ^= n;
            updateZeroFlag(regA);
            updateNegativeFlag(regA);
       }
       
       void _inc(uint8_t& n)
       {
            ++n;
            updateZeroFlag(n);
            updateNegativeFlag(n);
       }
       
       void _inx()
       {
            ++regX;
            updateZeroFlag(regX);
            updateNegativeFlag(regX);
       }
       
       void _iny()
       {
            ++regY;
            updateZeroFlag(regY);
            updateNegativeFlag(regY);
       }
       
       void _jmp(uint16_t n)
       {
            pc = n;
       }
       
       void _jsr(uint16_t n)
       {
            int t = pc - 1;
            push(BYTE(t, 1));
            push(BYTE(t, 0));
            pc = n;
       }
       
       void _lda(uint8_t n)
       {
            regA = n;
            updateZeroFlag(regA);
            updateNegativeFlag(regA);
       }

       void _ldx(uint8_t n)
       {
            regX = n;
            updateZeroFlag(regX);
            updateNegativeFlag(regX);
       }
       
       void _ldy(uint8_t n)
       {
            regY = n;
            updateZeroFlag(regY);
            updateNegativeFlag(regY);
       }
       
       void _lsr(uint8_t& n)
       {
            setFlagVal(CARRY, GET_BIT(n, 0));
            
            n >>= 1;
            updateZeroFlag(n);
            updateNegativeFlag(n);
       }
       
       void _nop()
       {
       }
       
       void _ora(uint8_t n)
       {
           regA |= n;
           updateZeroFlag(regA);
           updateNegativeFlag(regA);
       }
       
       void _pha()
       {
            push(regA);
       }
       
       void _php()
       {
            push(regF);
       }
       
       void _pla()
       {
            regA = peek();
            pop();
            updateZeroFlag(regA);
            updateNegativeFlag(regA);
       }
       
       void _plp()
       {
            regF = peek();
            pop();
       }
       
       void _rol(uint8_t& n)
       {
            uint8_t t = n;
            
            n <<= 1;
            
            SET_BIT_VAL(n, 0, getFlag(CARRY));
            setFlagVal(CARRY, GET_BIT(t, 7));
       }
       
       void _ror(uint8_t& n)
       {
            uint8_t t = n;
            
            n >>= 1;
            
            SET_BIT_VAL(n, 7, getFlag(CARRY));
            setFlagVal(CARRY, GET_BIT(t, 0));
       }
       
       void _rti()
       {
            regF = peek();
            pop();
            
            uint8_t low = peek();
            pop();
            
            uint8_t hi = peek() << 8;
            pop();
            
            pc = low | hi;
       }
       
       void _rts()
       {
            uint8_t low = peek();
            pop();
            
            uint8_t hi = peek() << 8;
            pop();
            
            pc = low | hi + 1;
       }
       
       void _sbc(uint8_t n)
       {
           int ures = regA - n - !getFlag(CARRY);
           int sres = SBYTE(regA) - SBYTE(n) - !getFlag(CARRY);
           
           regA = ures & 0xFF;
           
           setFlagVal(CARRY, !(ures > 255));
           setFlagVal(OVERFLOW, sres < -128 || sres > 127);
           updateZeroFlag(regA);
           updateNegativeFlag(regA);
       }
       
       void _sec()
       {
            setFlag(CARRY);
       }
       
       void _sed()
       {
            setFlag(DECIMAL);
       }
       
       void _sei()
       {
            setFlag(NO_IRQ);
       }
       
       void _sta(uint8_t& n)
       {
            n = regA;
       }
       
       void _stx(uint8_t& n)
       {
            n = regX;
       }
       
       void _sty(uint8_t& n)
       {
            n = regY;
       }
       
       void _tax()
       {
            regX = regA;
            updateZeroFlag(regX);
            updateNegativeFlag(regX);
       }
       
       void _tay()
       {
            regY = regA;
            updateZeroFlag(regY);
            updateNegativeFlag(regY);
       }
       
       void _tsx()
       {
            regX = regS;
            updateZeroFlag(regX);
            updateNegativeFlag(regX);
       }
       
       void _txa()
       {
            regA = regX;
            updateZeroFlag(regA);
            updateNegativeFlag(regA);
       }
       
       void _txs()
       {
            regS = regX;
       }
       
       void _tya()
       {
            regA = regY;
            updateZeroFlag(regA);
            updateNegativeFlag(regA);
       }
       
       // *******************
       // * Generated stuff *
       // *******************
       
#include "Generated.hpp"
       
public:
       
       void reset()
       {
           regA = 0, regX = 0, regY = 0, regF = 0;
           regS = 0xFF;
           
           for(int i = 0; i < MEM_SIZE; i++)
              mem[i] = 0;
       }
       
       CPU()
       {
           for(int i = 0; i < OP_TABLE_SIZE; i++)
           {
              opTable[i] = &CPU::die;
              opSizeTable[i] = 0;
           }
           
           initOpTables();
           reset();
       }
       
       void load(char const* path)
       {
            FILE* file = fopen(path, "rb");
            
            if(!file)
               throw std::runtime_error("Unable to load file!");
            
            reset();
            
            char header[16];
            fread(header, 16, 1, file);
            fread(mem, 1, 0x800, file);
            
            pc = 0;//mem[mem[RESET_ADDR] | mem[RESET_ADDR + 1] << 8];
            
            for(int i = 0; i < 30; i++)
              printf("%x ", mem[i]);
            
            printf("\n\n\n\n");
            fclose(file);
       }
       
       void logic()
       {
            //printf("a=%d x=%d y=%d f=%d,    pc=%d\n", regA, regX, regY, regF, pc);
            
            Operands p;
            p.b[0] = mem[pc + 1];
            p.b[1] = mem[pc + 2];
            printf("%x, %x, %x\n", mem[pc], p.b[0], p.b[1]);
            
            CALL_METHOD(this, opTable[mem[pc]])(p);
            pc += opSizeTable[mem[pc]];
            //printf(" - %d\n", pc);
            printf("a=%d x=%d y=%d f=%d,    pc=%d\n", regA, regX, regY, regF, pc);
            
          //CALL_METHOD(this, opTable[0])(3, 4);
       }
};

int main()
{
   CPU cpu;
   
   cpu.load("test.nes");
   
   
   try {
       while(1)
       {
          cpu.logic();
       }
   } catch(std::runtime_error const& e)
   {
     printf("\nEnded!");
   }
   
   getch();
   return 0;
}
