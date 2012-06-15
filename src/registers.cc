#include "registers.h"

#define CASE_REGGP(XX) \
    case UD_R_##XX##L  : \
    case UD_R_##XX##H  : \
    case UD_R_##XX##X  : \
    case UD_R_E##XX##X : \
    case UD_R_R##XX##X : \
        return registers[REG_R##XX##X - REG_OFFSET];

#define CASE_REGGP2(XX) \
    case UD_R_##XX##L  : \
    case UD_R_##XX     : \
    case UD_R_E##XX    : \
    case UD_R_R##XX    : \
        return registers[REG_R##XX - REG_OFFSET];

#define CASE_REGNUM(XX) \
    case UD_R_R##XX##B: \
    case UD_R_R##XX##W: \
    case UD_R_R##XX##D: \
    case UD_R_R##XX   : \
        return registers[REG_R##XX - REG_OFFSET];

#define CASE_REGBITSGP(XX) \
    case UD_R_##XX##L  : return 8; \
    case UD_R_##XX##H  : return 8; \
    case UD_R_##XX##X  : return 12; \
    case UD_R_E##XX##X : return 32; \
    case UD_R_R##XX##X : return 64;

#define CASE_REGBITSGP2(XX) \
    case UD_R_##XX##L  : return 8; \
    case UD_R_##XX     : return 16; \
    case UD_R_E##XX    : return 32; \
    case UD_R_R##XX    : return 64;

#define CASE_REGBITSNUM(XX) \
    case UD_R_R##XX##B: return 8; \
    case UD_R_R##XX##W: return 16; \
    case UD_R_R##XX##D: return 32; \
    case UD_R_R##XX   : return 64;

Registers :: Registers ()
{
    this->registers.resize(REG_SIZE);
    
    for (size_t i = 0; i < registers.size(); i++) {
        registers[i] = 0;
    }
}

size_t Registers :: get_register_index (int reg)
{
    switch (reg) {
    CASE_REGGP(A)
    CASE_REGGP(B)
    CASE_REGGP(C)
    CASE_REGGP(D)
    CASE_REGNUM(8)
    CASE_REGNUM(9)
    CASE_REGNUM(10)
    CASE_REGNUM(11)
    CASE_REGNUM(12)
    CASE_REGNUM(13)
    CASE_REGNUM(14)
    CASE_REGNUM(15)
    CASE_REGGP2(SI)
    CASE_REGGP2(DI)
    CASE_REGGP2(SP)
    CASE_REGGP2(BP)
    case REG_CF : return registers[REG_CF - REG_OFFSET];
    case REG_PF : return registers[REG_PF - REG_OFFSET];
    case REG_AF : return registers[REG_AF - REG_OFFSET];
    case REG_ZF : return registers[REG_ZF - REG_OFFSET];
    case REG_SF : return registers[REG_SF - REG_OFFSET];
    case REG_OF : return registers[REG_OF - REG_OFFSET];
    }
    
    return 0;
}

int Registers :: bits (int reg)
{
    switch (reg) {
    CASE_REGBITSGP(A)
    CASE_REGBITSGP(B)
    CASE_REGBITSGP(C)
    CASE_REGBITSGP(D)
    CASE_REGBITSNUM(8)
    CASE_REGBITSNUM(9)
    CASE_REGBITSNUM(10)
    CASE_REGBITSNUM(11)
    CASE_REGBITSNUM(12)
    CASE_REGBITSNUM(13)
    CASE_REGBITSNUM(14)
    CASE_REGBITSNUM(15)
    CASE_REGBITSGP2(SI)
    CASE_REGBITSGP2(DI)
    CASE_REGBITSGP2(SP)
    CASE_REGBITSGP2(BP)
    case REG_CF :
    case REG_PF :
    case REG_AF :
    case REG_ZF :
    case REG_SF :
    case REG_OF :
        return 1;
    }
    
    return 0;
}

int Registers :: left_shift (int reg)
{
    switch (reg) {
    case UD_R_AH :
    case UD_R_BH :
    case UD_R_CH :
    case UD_R_DH :
        return 8;
    }
    
    return 0;
}
