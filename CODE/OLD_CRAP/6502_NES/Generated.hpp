       
       
    void adcIMM(Operands p)
    {
        _adc(p.b[0]);
    }

    void adcZP(Operands p)
    {
        _adc(mem[p.b[0]]);
    }

    void adcZPX(Operands p)
    {
        _adc(mem[(p.b[0] + regX) & 0xFF]);
    }

    void adcABS(Operands p)
    {
        _adc(mem[p.w]);
    }

    void adcABSX(Operands p)
    {
        _adc(mem[(p.w + regX) & 0xFFFF]);
    }

    void adcABSY(Operands p)
    {
        _adc(mem[(p.w + regY) & 0xFFFF]);
    }

    void adcINDX(Operands p)
    {
        _adc(mem[mem[(p.b[0] + regX) & 0xFF] | (mem[(p.b[0] + regX + 1) & 0xFF] << 8)]);
    }

    void adcINDY(Operands p)
    {
        _adc(mem[ ((mem[p.b[0]] | (mem[(p.b[0] + 1) & 0xFF] << 8)) + regY) & 0xFFFF ]);
    }

    void andIMM(Operands p)
    {
        _and(p.b[0]);
    }

    void andZP(Operands p)
    {
        _and(mem[p.b[0]]);
    }

    void andZPX(Operands p)
    {
        _and(mem[(p.b[0] + regX) & 0xFF]);
    }

    void andABS(Operands p)
    {
        _and(mem[p.w]);
    }

    void andABSX(Operands p)
    {
        _and(mem[(p.w + regX) & 0xFFFF]);
    }

    void andABSY(Operands p)
    {
        _and(mem[(p.w + regY) & 0xFFFF]);
    }

    void andINDX(Operands p)
    {
        _and(mem[mem[(p.b[0] + regX) & 0xFF] | (mem[(p.b[0] + regX + 1) & 0xFF] << 8)]);
    }

    void andINDY(Operands p)
    {
        _and(mem[ ((mem[p.b[0]] | (mem[(p.b[0] + 1) & 0xFF] << 8)) + regY) & 0xFFFF ]);
    }

    void aslACCM(Operands p)
    {
        _asl(regA);
    }

    void aslZP(Operands p)
    {
        _asl(mem[p.b[0]]);
    }

    void aslZPX(Operands p)
    {
        _asl(mem[(p.b[0] + regX) & 0xFF]);
    }

    void aslABS(Operands p)
    {
        _asl(mem[p.w]);
    }

    void aslABSX(Operands p)
    {
        _asl(mem[(p.w + regX) & 0xFFFF]);
    }

    void bccREL(Operands p)
    {
        _bcc(p.b[0]);
    }

    void bcsREL(Operands p)
    {
        _bcs(p.b[0]);
    }

    void beqREL(Operands p)
    {
        _beq(p.b[0]);
    }

    void bitZP(Operands p)
    {
        _bit(mem[p.b[0]]);
    }

    void bitABS(Operands p)
    {
        _bit(mem[p.w]);
    }

    void bmiREL(Operands p)
    {
        _bmi(p.b[0]);
    }

    void bneREL(Operands p)
    {
        _bne(p.b[0]);
    }

    void bplREL(Operands p)
    {
        _bpl(p.b[0]);
    }

    void brkIMP(Operands p)
    {
        _brk();
    }

    void bvcREL(Operands p)
    {
        _bvc(p.b[0]);
    }

    void bvsREL(Operands p)
    {
        _bvs(p.b[0]);
    }

    void clcIMP(Operands p)
    {
        _clc();
    }

    void cldIMP(Operands p)
    {
        _cld();
    }

    void cliIMP(Operands p)
    {
        _cli();
    }

    void clvIMP(Operands p)
    {
        _clv();
    }

    void cmpIMM(Operands p)
    {
        _cmp(p.b[0]);
    }

    void cmpZP(Operands p)
    {
        _cmp(mem[p.b[0]]);
    }

    void cmpZPX(Operands p)
    {
        _cmp(mem[(p.b[0] + regX) & 0xFF]);
    }

    void cmpABS(Operands p)
    {
        _cmp(mem[p.w]);
    }

    void cmpABSX(Operands p)
    {
        _cmp(mem[(p.w + regX) & 0xFFFF]);
    }

    void cmpABSY(Operands p)
    {
        _cmp(mem[(p.w + regY) & 0xFFFF]);
    }

    void cmpINDX(Operands p)
    {
        _cmp(mem[mem[(p.b[0] + regX) & 0xFF] | (mem[(p.b[0] + regX + 1) & 0xFF] << 8)]);
    }

    void cmpINDY(Operands p)
    {
        _cmp(mem[ ((mem[p.b[0]] | (mem[(p.b[0] + 1) & 0xFF] << 8)) + regY) & 0xFFFF ]);
    }

    void cpxIMM(Operands p)
    {
        _cpx(p.b[0]);
    }

    void cpxZP(Operands p)
    {
        _cpx(mem[p.b[0]]);
    }

    void cpxABS(Operands p)
    {
        _cpx(mem[p.w]);
    }

    void cpyIMM(Operands p)
    {
        _cpy(p.b[0]);
    }

    void cpyZP(Operands p)
    {
        _cpy(mem[p.b[0]]);
    }

    void cpyABS(Operands p)
    {
        _cpy(mem[p.w]);
    }

    void decZP(Operands p)
    {
        _dec(mem[p.b[0]]);
    }

    void decZPX(Operands p)
    {
        _dec(mem[(p.b[0] + regX) & 0xFF]);
    }

    void decABS(Operands p)
    {
        _dec(mem[p.w]);
    }

    void decABSX(Operands p)
    {
        _dec(mem[(p.w + regX) & 0xFFFF]);
    }

    void dexIMP(Operands p)
    {
        _dex();
    }

    void deyIMP(Operands p)
    {
        _dey();
    }

    void eorIMM(Operands p)
    {
        _eor(p.b[0]);
    }

    void eorZP(Operands p)
    {
        _eor(mem[p.b[0]]);
    }

    void eorZPX(Operands p)
    {
        _eor(mem[(p.b[0] + regX) & 0xFF]);
    }

    void eorABS(Operands p)
    {
        _eor(mem[p.w]);
    }

    void eorABSX(Operands p)
    {
        _eor(mem[(p.w + regX) & 0xFFFF]);
    }

    void eorABSY(Operands p)
    {
        _eor(mem[(p.w + regY) & 0xFFFF]);
    }

    void eorINDX(Operands p)
    {
         uint8_t low = mem[(p.b[0] + regX) & 0xFF];
         uint8_t hi = mem[(p.b[0] + regX + 1) & 0xFF];
         
        _eor(mem[low | (hi << 8)]);
    }

    void eorINDY(Operands p)
    {
        _eor(mem[ ((mem[p.b[0]] | (mem[(p.b[0] + 1) & 0xFF] << 8)) + regY) & 0xFFFF ]);
    }

    void incZP(Operands p)
    {
        _inc(mem[p.b[0]]);
    }

    void incZPX(Operands p)
    {
        _inc(mem[(p.b[0] + regX) & 0xFF]);
    }

    void incABS(Operands p)
    {
        _inc(mem[p.w]);
    }

    void incABSX(Operands p)
    {
        _inc(mem[(p.w + regX) & 0xFFFF]);
    }

    void inxIMP(Operands p)
    {
        _inx();
    }

    void inyIMP(Operands p)
    {
        _iny();
    }

    void jmpABS(Operands p)
    {
        _jmp(p.w);
    }

    void jmpIND(Operands p)
    {
        _jmp(mem[p.w] | (mem[p.w + 1] << 8));
    }

    void jsrABS(Operands p)
    {
        _jsr(p.w);
    }

    void ldaIMM(Operands p)
    {
        _lda(p.b[0]);
    }

    void ldaZP(Operands p)
    {
        _lda(mem[p.b[0]]);
    }

    void ldaZPX(Operands p)
    {
        _lda(mem[(p.b[0] + regX) & 0xFF]);
    }

    void ldaABS(Operands p)
    {
        _lda(mem[p.w]);
    }

    void ldaABSX(Operands p)
    {
        _lda(mem[(p.w + regX) & 0xFFFF]);
    }

    void ldaABSY(Operands p)
    {
        _lda(mem[(p.w + regY) & 0xFFFF]);
    }

    void ldaINDX(Operands p)
    {
        _lda(mem[mem[(p.b[0] + regX) & 0xFF] | (mem[(p.b[0] + regX + 1) & 0xFF] << 8)]);
    }

    void ldaINDY(Operands p)
    {
        _lda(mem[ ((mem[p.b[0]] | (mem[(p.b[0] + 1) & 0xFF] << 8)) + regY) & 0xFFFF ]);
    }

    void ldxIMM(Operands p)
    {
        _ldx(p.b[0]);
    }

    void ldxZP(Operands p)
    {
        _ldx(mem[p.b[0]]);
    }

    void ldxZPY(Operands p)
    {
        _ldx(mem[(p.b[0] + regY) & 0xFF]);
    }

    void ldxABS(Operands p)
    {
        _ldx(mem[p.w]);
    }

    void ldxABSY(Operands p)
    {
        _ldx(mem[(p.w + regY) & 0xFFFF]);
    }

    void ldyIMM(Operands p)
    {
        _ldy(p.b[0]);
    }

    void ldyZP(Operands p)
    {
        _ldy(mem[p.b[0]]);
    }

    void ldyZPX(Operands p)
    {
        _ldy(mem[(p.b[0] + regX) & 0xFF]);
    }

    void ldyABS(Operands p)
    {
        _ldy(mem[p.w]);
    }

    void ldyABSX(Operands p)
    {
        _ldy(mem[(p.w + regX) & 0xFFFF]);
    }

    void lsrACCM(Operands p)
    {
        _lsr(regA);
    }

    void lsrZP(Operands p)
    {
        _lsr(mem[p.b[0]]);
    }

    void lsrZPX(Operands p)
    {
        _lsr(mem[(p.b[0] + regX) & 0xFF]);
    }

    void lsrABS(Operands p)
    {
        _lsr(mem[p.w]);
    }

    void lsrABSX(Operands p)
    {
        _lsr(mem[(p.w + regX) & 0xFFFF]);
    }

    void nopIMP(Operands p)
    {
        _nop();
    }

    void oraIMM(Operands p)
    {
        _ora(p.b[0]);
    }

    void oraZP(Operands p)
    {
        _ora(mem[p.b[0]]);
    }

    void oraZPX(Operands p)
    {
        _ora(mem[(p.b[0] + regX) & 0xFF]);
    }

    void oraABS(Operands p)
    {
        _ora(mem[p.w]);
    }

    void oraABSX(Operands p)
    {
        _ora(mem[(p.w + regX) & 0xFFFF]);
    }

    void oraABSY(Operands p)
    {
        _ora(mem[(p.w + regY) & 0xFFFF]);
    }

    void oraINDX(Operands p)
    {
        _ora(mem[mem[(p.b[0] + regX) & 0xFF] | (mem[(p.b[0] + regX + 1) & 0xFF] << 8)]);
    }

    void oraINDY(Operands p)
    {
        _ora(mem[ ((mem[p.b[0]] | (mem[(p.b[0] + 1) & 0xFF] << 8)) + regY) & 0xFFFF ]);
    }

    void phaIMP(Operands p)
    {
        _pha();
    }

    void phpIMP(Operands p)
    {
        _php();
    }

    void plaIMP(Operands p)
    {
        _pla();
    }

    void plpIMP(Operands p)
    {
        _plp();
    }

    void rolACCM(Operands p)
    {
        _rol(regA);
    }

    void rolZP(Operands p)
    {
        _rol(mem[p.b[0]]);
    }

    void rolZPX(Operands p)
    {
        _rol(mem[(p.b[0] + regX) & 0xFF]);
    }

    void rolABS(Operands p)
    {
        _rol(mem[p.w]);
    }

    void rolABSX(Operands p)
    {
        _rol(mem[(p.w + regX) & 0xFFFF]);
    }

    void rorACCM(Operands p)
    {
        _ror(regA);
    }

    void rorZP(Operands p)
    {
        _ror(mem[p.b[0]]);
    }

    void rorZPX(Operands p)
    {
        _ror(mem[(p.b[0] + regX) & 0xFF]);
    }

    void rorABS(Operands p)
    {
        _ror(mem[p.w]);
    }

    void rorABSX(Operands p)
    {
        _ror(mem[(p.w + regX) & 0xFFFF]);
    }

    void rtiIMP(Operands p)
    {
        _rti();
    }

    void rtsIMP(Operands p)
    {
        _rts();
    }

    void sbcIMM(Operands p)
    {
        _sbc(p.b[0]);
    }

    void sbcZP(Operands p)
    {
        _sbc(mem[p.b[0]]);
    }

    void sbcZPX(Operands p)
    {
        _sbc(mem[(p.b[0] + regX) & 0xFF]);
    }

    void sbcABS(Operands p)
    {
        _sbc(mem[p.w]);
    }

    void sbcABSX(Operands p)
    {
        _sbc(mem[(p.w + regX) & 0xFFFF]);
    }

    void sbcABSY(Operands p)
    {
        _sbc(mem[(p.w + regY) & 0xFFFF]);
    }

    void sbcINDX(Operands p)
    {
        _sbc(mem[mem[(p.b[0] + regX) & 0xFF] | (mem[(p.b[0] + regX + 1) & 0xFF] << 8)]);
    }

    void sbcINDY(Operands p)
    {
        _sbc(mem[ ((mem[p.b[0]] | (mem[(p.b[0] + 1) & 0xFF] << 8)) + regY) & 0xFFFF ]);
    }

    void secIMP(Operands p)
    {
        _sec();
    }

    void sedIMP(Operands p)
    {
        _sed();
    }

    void seiIMP(Operands p)
    {
        _sei();
    }

    void staZP(Operands p)
    {
        _sta(mem[p.b[0]]);
    }

    void staZPX(Operands p)
    {
        _sta(mem[(p.b[0] + regX) & 0xFF]);
    }

    void staABS(Operands p)
    {
        _sta(mem[p.w]);
    }

    void staABSX(Operands p)
    {
        _sta(mem[(p.w + regX) & 0xFFFF]);
    }

    void staABSY(Operands p)
    {
        _sta(mem[(p.w + regY) & 0xFFFF]);
    }

    void staINDX(Operands p)
    {
        _sta(mem[mem[(p.b[0] + regX) & 0xFF] | (mem[(p.b[0] + regX + 1) & 0xFF] << 8)]);
    }

    void staINDY(Operands p)
    {
        _sta(mem[ ((mem[p.b[0]] | (mem[(p.b[0] + 1) & 0xFF] << 8)) + regY) & 0xFFFF ]);
    }

    void stxZP(Operands p)
    {
        _stx(mem[p.b[0]]);
    }

    void stxZPY(Operands p)
    {
        _stx(mem[(p.b[0] + regY) & 0xFF]);
    }

    void stxABS(Operands p)
    {
        _stx(mem[p.w]);
    }

    void styZP(Operands p)
    {
        _sty(mem[p.b[0]]);
    }

    void styZPX(Operands p)
    {
        _sty(mem[(p.b[0] + regX) & 0xFF]);
    }

    void styABS(Operands p)
    {
        _sty(mem[p.w]);
    }

    void taxIMP(Operands p)
    {
        _tax();
    }

    void tayIMP(Operands p)
    {
        _tay();
    }

    void tsxIMP(Operands p)
    {
        _tsx();
    }

    void txaIMP(Operands p)
    {
        _txa();
    }

    void txsIMP(Operands p)
    {
        _txs();
    }

    void tyaIMP(Operands p)
    {
        _tya();
    }




    void initOpTables()
    {

opTable[ 0x69] = &CPU::adcIMM;
opSizeTable[ 0x69] = 2;
opTable[ 0x65] = &CPU::adcZP;
opSizeTable[ 0x65] = 2;
opTable[ 0x75] = &CPU::adcZPX;
opSizeTable[ 0x75] = 2;
opTable[ 0x6D] = &CPU::adcABS;
opSizeTable[ 0x6D] = 3;
opTable[ 0x7D] = &CPU::adcABSX;
opSizeTable[ 0x7D] = 3;
opTable[ 0x79] = &CPU::adcABSY;
opSizeTable[ 0x79] = 3;
opTable[ 0x61] = &CPU::adcINDX;
opSizeTable[ 0x61] = 2;
opTable[ 0x71] = &CPU::adcINDY;
opSizeTable[ 0x71] = 2;
opTable[ 0x29] = &CPU::andIMM;
opSizeTable[ 0x29] = 2;
opTable[ 0x25] = &CPU::andZP;
opSizeTable[ 0x25] = 2;
opTable[ 0x35] = &CPU::andZPX;
opSizeTable[ 0x35] = 2;
opTable[ 0x2D] = &CPU::andABS;
opSizeTable[ 0x2D] = 3;
opTable[ 0x3D] = &CPU::andABSX;
opSizeTable[ 0x3D] = 3;
opTable[ 0x39] = &CPU::andABSY;
opSizeTable[ 0x39] = 3;
opTable[ 0x21] = &CPU::andINDX;
opSizeTable[ 0x21] = 2;
opTable[ 0x31] = &CPU::andINDY;
opSizeTable[ 0x31] = 2;
opTable[ 0x0A] = &CPU::aslACCM;
opSizeTable[ 0x0A] = 1;
opTable[ 0x06] = &CPU::aslZP;
opSizeTable[ 0x06] = 2;
opTable[ 0x16] = &CPU::aslZPX;
opSizeTable[ 0x16] = 2;
opTable[ 0x0E] = &CPU::aslABS;
opSizeTable[ 0x0E] = 3;
opTable[ 0x1E] = &CPU::aslABSX;
opSizeTable[ 0x1E] = 3;
opTable[ 0x90] = &CPU::bccREL;
opSizeTable[ 0x90] = 2;
opTable[ 0xB0] = &CPU::bcsREL;
opSizeTable[ 0xB0] = 2;
opTable[ 0xF0] = &CPU::beqREL;
opSizeTable[ 0xF0] = 2;
opTable[ 0x24] = &CPU::bitZP;
opSizeTable[ 0x24] = 2;
opTable[ 0x2C] = &CPU::bitABS;
opSizeTable[ 0x2C] = 3;
opTable[ 0x30] = &CPU::bmiREL;
opSizeTable[ 0x30] = 2;
opTable[ 0xD0] = &CPU::bneREL;
opSizeTable[ 0xD0] = 2;
opTable[ 0x10] = &CPU::bplREL;
opSizeTable[ 0x10] = 2;
opTable[ 0x00] = &CPU::brkIMP;
opSizeTable[ 0x00] = 1;
opTable[ 0x50] = &CPU::bvcREL;
opSizeTable[ 0x50] = 2;
opTable[ 0x70] = &CPU::bvsREL;
opSizeTable[ 0x70] = 2;
opTable[ 0x18] = &CPU::clcIMP;
opSizeTable[ 0x18] = 1;
opTable[ 0xD8] = &CPU::cldIMP;
opSizeTable[ 0xD8] = 1;
opTable[ 0x58] = &CPU::cliIMP;
opSizeTable[ 0x58] = 1;
opTable[ 0xB8] = &CPU::clvIMP;
opSizeTable[ 0xB8] = 1;
opTable[ 0xC9] = &CPU::cmpIMM;
opSizeTable[ 0xC9] = 2;
opTable[ 0xC5] = &CPU::cmpZP;
opSizeTable[ 0xC5] = 2;
opTable[ 0xD5] = &CPU::cmpZPX;
opSizeTable[ 0xD5] = 2;
opTable[ 0xCD] = &CPU::cmpABS;
opSizeTable[ 0xCD] = 3;
opTable[ 0xDD] = &CPU::cmpABSX;
opSizeTable[ 0xDD] = 3;
opTable[ 0xD9] = &CPU::cmpABSY;
opSizeTable[ 0xD9] = 3;
opTable[ 0xC1] = &CPU::cmpINDX;
opSizeTable[ 0xC1] = 2;
opTable[ 0xD1] = &CPU::cmpINDY;
opSizeTable[ 0xD1] = 2;
opTable[ 0xE0] = &CPU::cpxIMM;
opSizeTable[ 0xE0] = 2;
opTable[ 0xE4] = &CPU::cpxZP;
opSizeTable[ 0xE4] = 2;
opTable[ 0xEC] = &CPU::cpxABS;
opSizeTable[ 0xEC] = 3;
opTable[ 0xC0] = &CPU::cpyIMM;
opSizeTable[ 0xC0] = 2;
opTable[ 0xC4] = &CPU::cpyZP;
opSizeTable[ 0xC4] = 2;
opTable[ 0xCC] = &CPU::cpyABS;
opSizeTable[ 0xCC] = 3;
opTable[ 0xC6] = &CPU::decZP;
opSizeTable[ 0xC6] = 2;
opTable[ 0xD6] = &CPU::decZPX;
opSizeTable[ 0xD6] = 2;
opTable[ 0xCE] = &CPU::decABS;
opSizeTable[ 0xCE] = 3;
opTable[ 0xDE] = &CPU::decABSX;
opSizeTable[ 0xDE] = 3;
opTable[ 0xCA] = &CPU::dexIMP;
opSizeTable[ 0xCA] = 1;
opTable[ 0x88] = &CPU::deyIMP;
opSizeTable[ 0x88] = 1;
opTable[ 0x49] = &CPU::eorIMM;
opSizeTable[ 0x49] = 2;
opTable[ 0x45] = &CPU::eorZP;
opSizeTable[ 0x45] = 2;
opTable[ 0x55] = &CPU::eorZPX;
opSizeTable[ 0x55] = 2;
opTable[ 0x4D] = &CPU::eorABS;
opSizeTable[ 0x4D] = 3;
opTable[ 0x5D] = &CPU::eorABSX;
opSizeTable[ 0x5D] = 3;
opTable[ 0x59] = &CPU::eorABSY;
opSizeTable[ 0x59] = 3;
opTable[ 0x41] = &CPU::eorINDX;
opSizeTable[ 0x41] = 2;
opTable[ 0x51] = &CPU::eorINDY;
opSizeTable[ 0x51] = 2;
opTable[ 0xE6] = &CPU::incZP;
opSizeTable[ 0xE6] = 2;
opTable[ 0xF6] = &CPU::incZPX;
opSizeTable[ 0xF6] = 2;
opTable[ 0xEE] = &CPU::incABS;
opSizeTable[ 0xEE] = 3;
opTable[ 0xFE] = &CPU::incABSX;
opSizeTable[ 0xFE] = 3;
opTable[ 0xE8] = &CPU::inxIMP;
opSizeTable[ 0xE8] = 1;
opTable[ 0xC8] = &CPU::inyIMP;
opSizeTable[ 0xC8] = 1;
opTable[ 0x4C] = &CPU::jmpABS;
opSizeTable[ 0x4C] = 3;
opTable[ 0x6C] = &CPU::jmpIND;
opSizeTable[ 0x6C] = 3;
opTable[ 0x20] = &CPU::jsrABS;
opSizeTable[ 0x20] = 3;
opTable[ 0xA9] = &CPU::ldaIMM;
opSizeTable[ 0xA9] = 2;
opTable[ 0xA5] = &CPU::ldaZP;
opSizeTable[ 0xA5] = 2;
opTable[ 0xB5] = &CPU::ldaZPX;
opSizeTable[ 0xB5] = 2;
opTable[ 0xAD] = &CPU::ldaABS;
opSizeTable[ 0xAD] = 3;
opTable[ 0xBD] = &CPU::ldaABSX;
opSizeTable[ 0xBD] = 3;
opTable[ 0xB9] = &CPU::ldaABSY;
opSizeTable[ 0xB9] = 3;
opTable[ 0xA1] = &CPU::ldaINDX;
opSizeTable[ 0xA1] = 2;
opTable[ 0xB1] = &CPU::ldaINDY;
opSizeTable[ 0xB1] = 2;
opTable[ 0xA2] = &CPU::ldxIMM;
opSizeTable[ 0xA2] = 2;
opTable[ 0xA6] = &CPU::ldxZP;
opSizeTable[ 0xA6] = 2;
opTable[ 0xB6] = &CPU::ldxZPY;
opSizeTable[ 0xB6] = 2;
opTable[ 0xAE] = &CPU::ldxABS;
opSizeTable[ 0xAE] = 3;
opTable[ 0xBE] = &CPU::ldxABSY;
opSizeTable[ 0xBE] = 3;
opTable[ 0xA0] = &CPU::ldyIMM;
opSizeTable[ 0xA0] = 2;
opTable[ 0xA4] = &CPU::ldyZP;
opSizeTable[ 0xA4] = 2;
opTable[ 0xB4] = &CPU::ldyZPX;
opSizeTable[ 0xB4] = 2;
opTable[ 0xAC] = &CPU::ldyABS;
opSizeTable[ 0xAC] = 3;
opTable[ 0xBC] = &CPU::ldyABSX;
opSizeTable[ 0xBC] = 3;
opTable[ 0x4A] = &CPU::lsrACCM;
opSizeTable[ 0x4A] = 1;
opTable[ 0x46] = &CPU::lsrZP;
opSizeTable[ 0x46] = 2;
opTable[ 0x56] = &CPU::lsrZPX;
opSizeTable[ 0x56] = 2;
opTable[ 0x4E] = &CPU::lsrABS;
opSizeTable[ 0x4E] = 3;
opTable[ 0x5E] = &CPU::lsrABSX;
opSizeTable[ 0x5E] = 3;
opTable[ 0xEA] = &CPU::nopIMP;
opSizeTable[ 0xEA] = 1;
opTable[ 0x09] = &CPU::oraIMM;
opSizeTable[ 0x09] = 2;
opTable[ 0x05] = &CPU::oraZP;
opSizeTable[ 0x05] = 2;
opTable[ 0x15] = &CPU::oraZPX;
opSizeTable[ 0x15] = 2;
opTable[ 0x0D] = &CPU::oraABS;
opSizeTable[ 0x0D] = 3;
opTable[ 0x1D] = &CPU::oraABSX;
opSizeTable[ 0x1D] = 3;
opTable[ 0x19] = &CPU::oraABSY;
opSizeTable[ 0x19] = 3;
opTable[ 0x01] = &CPU::oraINDX;
opSizeTable[ 0x01] = 2;
opTable[ 0x11] = &CPU::oraINDY;
opSizeTable[ 0x11] = 2;
opTable[ 0x48] = &CPU::phaIMP;
opSizeTable[ 0x48] = 1;
opTable[ 0x08] = &CPU::phpIMP;
opSizeTable[ 0x08] = 1;
opTable[ 0x68] = &CPU::plaIMP;
opSizeTable[ 0x68] = 1;
opTable[ 0x28] = &CPU::plpIMP;
opSizeTable[ 0x28] = 1;
opTable[ 0x2A] = &CPU::rolACCM;
opSizeTable[ 0x2A] = 1;
opTable[ 0x26] = &CPU::rolZP;
opSizeTable[ 0x26] = 2;
opTable[ 0x36] = &CPU::rolZPX;
opSizeTable[ 0x36] = 2;
opTable[ 0x2E] = &CPU::rolABS;
opSizeTable[ 0x2E] = 3;
opTable[ 0x3E] = &CPU::rolABSX;
opSizeTable[ 0x3E] = 3;
opTable[ 0x6A] = &CPU::rorACCM;
opSizeTable[ 0x6A] = 1;
opTable[ 0x66] = &CPU::rorZP;
opSizeTable[ 0x66] = 2;
opTable[ 0x76] = &CPU::rorZPX;
opSizeTable[ 0x76] = 2;
opTable[ 0x6E] = &CPU::rorABS;
opSizeTable[ 0x6E] = 3;
opTable[ 0x7E] = &CPU::rorABSX;
opSizeTable[ 0x7E] = 3;
opTable[ 0x40] = &CPU::rtiIMP;
opSizeTable[ 0x40] = 1;
opTable[ 0x60] = &CPU::rtsIMP;
opSizeTable[ 0x60] = 1;
opTable[ 0xE9] = &CPU::sbcIMM;
opSizeTable[ 0xE9] = 2;
opTable[ 0xE5] = &CPU::sbcZP;
opSizeTable[ 0xE5] = 2;
opTable[ 0xF5] = &CPU::sbcZPX;
opSizeTable[ 0xF5] = 2;
opTable[ 0xED] = &CPU::sbcABS;
opSizeTable[ 0xED] = 3;
opTable[ 0xFD] = &CPU::sbcABSX;
opSizeTable[ 0xFD] = 3;
opTable[ 0xF9] = &CPU::sbcABSY;
opSizeTable[ 0xF9] = 3;
opTable[ 0xE1] = &CPU::sbcINDX;
opSizeTable[ 0xE1] = 2;
opTable[ 0xF1] = &CPU::sbcINDY;
opSizeTable[ 0xF1] = 2;
opTable[ 0x38] = &CPU::secIMP;
opSizeTable[ 0x38] = 1;
opTable[ 0xF8] = &CPU::sedIMP;
opSizeTable[ 0xF8] = 1;
opTable[ 0x78] = &CPU::seiIMP;
opSizeTable[ 0x78] = 1;
opTable[ 0x85] = &CPU::staZP;
opSizeTable[ 0x85] = 2;
opTable[ 0x95] = &CPU::staZPX;
opSizeTable[ 0x95] = 2;
opTable[ 0x8D] = &CPU::staABS;
opSizeTable[ 0x8D] = 3;
opTable[ 0x9D] = &CPU::staABSX;
opSizeTable[ 0x9D] = 3;
opTable[ 0x99] = &CPU::staABSY;
opSizeTable[ 0x99] = 3;
opTable[ 0x81] = &CPU::staINDX;
opSizeTable[ 0x81] = 2;
opTable[ 0x91] = &CPU::staINDY;
opSizeTable[ 0x91] = 2;
opTable[ 0x86] = &CPU::stxZP;
opSizeTable[ 0x86] = 2;
opTable[ 0x96] = &CPU::stxZPY;
opSizeTable[ 0x96] = 2;
opTable[ 0x8E] = &CPU::stxABS;
opSizeTable[ 0x8E] = 3;
opTable[ 0x84] = &CPU::styZP;
opSizeTable[ 0x84] = 2;
opTable[ 0x94] = &CPU::styZPX;
opSizeTable[ 0x94] = 2;
opTable[ 0x8C] = &CPU::styABS;
opSizeTable[ 0x8C] = 3;
opTable[ 0xAA] = &CPU::taxIMP;
opSizeTable[ 0xAA] = 1;
opTable[ 0xA8] = &CPU::tayIMP;
opSizeTable[ 0xA8] = 1;
opTable[ 0xBA] = &CPU::tsxIMP;
opSizeTable[ 0xBA] = 1;
opTable[ 0x8A] = &CPU::txaIMP;
opSizeTable[ 0x8A] = 1;
opTable[ 0x9A] = &CPU::txsIMP;
opSizeTable[ 0x9A] = 1;
opTable[ 0x98] = &CPU::tyaIMP;
opSizeTable[ 0x98] = 1;
    }
