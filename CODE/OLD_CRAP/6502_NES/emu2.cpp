#include <stdio.h>
#include <conio.h>
#include <math.h>

#include <allegro.h>

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

#define PIXEL(bmp, x, y) ((long*)(bmp)->line[(y)])[(x)]

BITMAP* buffer;

void init()
{
     allegro_init();
     
     install_mouse();
     install_keyboard();
     set_color_depth(32);
     set_gfx_mode(GFX_AUTODETECT_WINDOWED, 800, 600, 0, 0);
     
     buffer = create_bitmap(SCREEN_W, SCREEN_H);
     
     srand(time(NULL));
}

void deinit()
{
     destroy_bitmap(buffer);
}


struct CPU {
       static const int MEM_SIZE = 0x10000;
       static const int OP_TABLE_SIZE = 0x100;
       
       static const int STACK_START = 0x100;
       static const int STACK_LEN = 0x100;
       
       static const int SCREEN_START = 0x1000;
       
       static const int NMI_ADDR = 0xFFFA;
       static const int RESET_ADDR = 0xFFFC;
       static const int IRQ_ADDR = 0xFFFE;
       
       static const uint8_t CARRY_MASK = 1;
       static const uint8_t ZERO_MASK  = 2;
       static const uint8_t NO_IRQ = 4;
       static const uint8_t DECIMAL = 8;
       static const uint8_t BRK_CMD = 16;
       static const uint8_t OVERFLOW_MASK = 64;
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
       
       void updateZERO_MASKFlag(uint8_t n)
       {
            setFlagVal(ZERO_MASK, !n);
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
           return mem[STACK_START + regS + 1];
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
           int ures = regA + n + getFlag(CARRY_MASK);
           int sres = SBYTE(regA) + SBYTE(n) + getFlag(CARRY_MASK);
           
           regA = ures & 0xFF;
           
           setFlagVal(CARRY_MASK, ures > 255);
           setFlagVal(OVERFLOW_MASK, sres < -128 || sres > 127);
           updateZERO_MASKFlag(regA);
           updateNegativeFlag(regA);
       }
       
       void _and(uint8_t n)
       {
           regA &= n;
           updateZERO_MASKFlag(regA);
           updateNegativeFlag(regA);
       }
       
       void _asl(uint8_t& n)
       {            
            setFlagVal(CARRY_MASK, GET_BIT(n, 7));
            n <<= 1;
            updateZERO_MASKFlag(n);
            updateNegativeFlag(n);
       }
       
       void _bcc(uint8_t n)
       {
            ADDIF(pc, SBYTE(n), !getFlag(CARRY_MASK));
       }
       
       void _bcs(uint8_t n)
       {
            ADDIF(pc, SBYTE(n), getFlag(CARRY_MASK));
       }
       
       void _beq(uint8_t n)
       {
            ADDIF(pc, SBYTE(n), getFlag(ZERO_MASK));
       }
       
       void _bit(uint8_t n)
       {
            updateZERO_MASKFlag(regA & n);
            updateNegativeFlag(n);
            setFlagVal(OVERFLOW_MASK, GET_BIT(n, 6));
       }
       
       void _bmi(uint8_t n)
       {
            if(getFlag(NEGATIVE))
               pc += SBYTE(n);
       }
       
       void _bne(uint8_t n)
       {
             ADDIF(pc, SBYTE(n), !getFlag(ZERO_MASK));
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
            ADDIF(pc, SBYTE(n), !getFlag(OVERFLOW_MASK));
       }
       
       void _bvs(uint8_t n)
       {
            ADDIF(pc, SBYTE(n), getFlag(OVERFLOW_MASK));
       }
       
       void _clc()
       {
            clearFlag(CARRY_MASK);
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
            clearFlag(OVERFLOW_MASK);
       }
       
       void _cmp(uint8_t n)
       {
            uint8_t res = regA - n;
            
            if(!res)
            {
               setFlag(ZERO_MASK);
               clearFlag(CARRY_MASK);
               clearFlag(NEGATIVE);
            }
            
            else
            {
                clearFlag(ZERO_MASK);
                setFlagVal(CARRY_MASK, regA > n);
                updateNegativeFlag(res);
            }
       }
       
       void _cpx(uint8_t n)
       {
            uint8_t res = regX - n;
            
            if(!res)
            {
               setFlag(ZERO_MASK);
               clearFlag(CARRY_MASK);
               clearFlag(NEGATIVE);
            }
            
            else
            {
                clearFlag(ZERO_MASK);
                setFlagVal(CARRY_MASK, regX > n);
                updateNegativeFlag(res);
            }
       }
       
       void _cpy(uint8_t n)
       {
            uint8_t res = regY - n;
            
            if(!res)
            {
               setFlag(ZERO_MASK);
               clearFlag(CARRY_MASK);
               clearFlag(NEGATIVE);
            }
            
            else
            {
                clearFlag(ZERO_MASK);
                setFlagVal(CARRY_MASK, regY > n);
                updateNegativeFlag(res);
            }
       }
       
       void _dec(uint8_t& n)
       {
            --n;
            updateZERO_MASKFlag(n);
            updateNegativeFlag(n);
       }
       
       void _dex()
       {
            --regX;
            updateZERO_MASKFlag(regX);
            updateNegativeFlag(regX);
       }
       
       void _dey()
       {
            --regY;
            updateZERO_MASKFlag(regY);
            updateNegativeFlag(regY);
       }
       
       void _eor(uint8_t n)
       {
            regA ^= n;
            updateZERO_MASKFlag(regA);
            updateNegativeFlag(regA);
       }
       
       void _inc(uint8_t& n)
       {
            ++n;
            updateZERO_MASKFlag(n);
            updateNegativeFlag(n);
       }
       
       void _inx()
       {
            ++regX;
            updateZERO_MASKFlag(regX);
            updateNegativeFlag(regX);
       }
       
       void _iny()
       {
            ++regY;
            updateZERO_MASKFlag(regY);
            updateNegativeFlag(regY);
       }
       
       void _jmp(uint16_t n)
       {
            pc = n;
       }
       
       void _jsr(uint16_t n)
       {
            int t = pc - 1;
            //printf("STK %x %x, %x\n", BYTE(t, 1), BYTE(t, 0), t);
            push(BYTE(t, 1));
            push(BYTE(t, 0));
            //printf("STK %x\n", peek());
            pc = n;
       }
       
       void _lda(uint8_t n)
       {
            regA = n;
            updateZERO_MASKFlag(regA);
            updateNegativeFlag(regA);
       }

       void _ldx(uint8_t n)
       {
            regX = n;
            updateZERO_MASKFlag(regX);
            updateNegativeFlag(regX);
       }
       
       void _ldy(uint8_t n)
       {
            regY = n;
            updateZERO_MASKFlag(regY);
            updateNegativeFlag(regY);
       }
       
       void _lsr(uint8_t& n)
       {
            setFlagVal(CARRY_MASK, GET_BIT(n, 0));
            
            n >>= 1;
            updateZERO_MASKFlag(n);
            updateNegativeFlag(n);
       }
       
       void _nop()
       {
       }
       
       void _ora(uint8_t n)
       {
           regA |= n;
           updateZERO_MASKFlag(regA);
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
            updateZERO_MASKFlag(regA);
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
            
            SET_BIT_VAL(n, 0, getFlag(CARRY_MASK));
            setFlagVal(CARRY_MASK, GET_BIT(t, 7));
       }
       
       void _ror(uint8_t& n)
       {
            uint8_t t = n;
            
            n >>= 1;
            
            SET_BIT_VAL(n, 7, getFlag(CARRY_MASK));
            setFlagVal(CARRY_MASK, GET_BIT(t, 0));
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
            
            
            pc = (low | hi) + 1;
            //printf("RTS    %x\n", pc);
            //getch();
       }
       
       void _sbc(uint8_t n)
       {
           int ures = regA - n - !getFlag(CARRY_MASK);
           int sres = SBYTE(regA) - SBYTE(n) - !getFlag(CARRY_MASK);
           
           regA = ures & 0xFF;
           
           setFlagVal(CARRY_MASK, !(ures > 255));
           setFlagVal(OVERFLOW_MASK, sres < -128 || sres > 127);
           updateZERO_MASKFlag(regA);
           updateNegativeFlag(regA);
       }
       
       void _sec()
       {
            setFlag(CARRY_MASK);
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
            updateZERO_MASKFlag(regX);
            updateNegativeFlag(regX);
       }
       
       void _tay()
       {
            regY = regA;
            updateZERO_MASKFlag(regY);
            updateNegativeFlag(regY);
       }
       
       void _tsx()
       {
            regX = regS;
            updateZERO_MASKFlag(regX);
            updateNegativeFlag(regX);
       }
       
       void _txa()
       {
            regA = regX;
            updateZERO_MASKFlag(regA);
            updateNegativeFlag(regA);
       }
       
       void _txs()
       {
            regS = regX;
       }
       
       void _tya()
       {
            regA = regY;
            updateZERO_MASKFlag(regA);
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
            fread(mem, 1, 0x2000, file);
            fread(mem + MEM_SIZE - 0x2000, 1, 0x2000, file);
            
            pc = mem[RESET_ADDR] | mem[RESET_ADDR + 1] << 8;
            printf("[[%d]]\n\n\n\n", pc);
            
            // Clear screen:
            for(int i = 0; i < 32 * 32; i++)
               mem[SCREEN_START + i] = 0;
            
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
            //printf("%x, %x, %x\n", mem[pc], p.b[0], p.b[1]);
            
            int oldPc = pc;
            pc += opSizeTable[mem[pc]];
            
            CALL_METHOD(this, opTable[mem[oldPc]])(p);
            
            //printf(" - %d\n", pc);
            printf("a=%d x=%d y=%d f=%d,    pc=%x\n", regA, regX, regY, regF, pc);
            
          //CALL_METHOD(this, opTable[0])(3, 4);
       }
};


int main()
{   
    bool exit = false;
    
    init();
    
    BITMAP* display = create_bitmap(32, 32);
    clear_to_color(display, makecol(128, 0, 0));
    
    int pal[16] = {0, makecol(255, 255, 255), makecol(255, 0, 0), makecol(0, 255, 255),
                   makecol(255, 0, 255), makecol(0, 255, 0), makecol(0, 0, 255), makecol(255, 255, 0),
                   makecol(255, 128, 0), makecol(64, 32, 0), makecol(255, 128, 128), makecol(64, 64, 64),
                   makecol(128, 128, 128), makecol(128, 255, 128), makecol(128, 128, 255), makecol(200, 200, 200)};
                   
    CPU cpu;
   
   cpu.load("test.nes");
   bool stop = false;
     
     
     
    while(!exit)
    {
     if(key[KEY_ESC]) exit = true;
  
     try {
         if(!stop)
         for(int i = 0; i < 1; i++)
          cpu.logic();
     } catch(std::runtime_error const& e)
     {
      stop = true;
       printf("\nEnded!");
     }
   
   int c = 0;
   for(int j = 0; j < 32; j++)
     for(int i = 0; i < 32; i++)
     {
       PIXEL(display, i, j) = pal[cpu.mem[CPU::SCREEN_START + c] % 16];
       c++;
     }
  
     clear_to_color(buffer, makecol(32, 32, 32));
     
     stretch_blit(display, buffer, 0, 0, display->w, display->h, 0, 0, display->w * 8, display->h * 8);
     
     draw_sprite(buffer, mouse_sprite, mouse_x, mouse_y);
     blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
    }
    
    deinit();
    return 0;
}END_OF_MAIN()
