#include "executor.h"

// Type R

uint64_t add(uint64_t reg1, uint64_t reg2) {
    return reg1 + reg2;
}

uint64_t sub(uint64_t reg1, uint64_t reg2) {
    return reg1 - reg2;
}


uint64_t sll(uint64_t reg1, uint64_t reg2) {
    return reg1 << reg2;
}

uint64_t slt(uint64_t reg1, uint64_t reg2) {
    if ((int64_t) reg1 < (int64_t) reg2) {
        return 1;
    } else {
        return 0;
    }
}

uint64_t sltu(uint64_t reg1, uint64_t reg2) {
    if (reg1 < reg2) {
        return 1;
    } else {
        return 0;
    }
}

uint64_t xor(uint64_t reg1, uint64_t reg2) {
    return reg1 ^ reg2;
}

uint64_t srl(uint64_t reg1, uint64_t reg2) {
    return reg1 >> reg2;
}

uint64_t sra(uint64_t reg1, uint64_t reg2) {
    int64_t res = (int64_t) reg1 >> reg2;
    return (uint64_t) res;
}

uint64_t or(uint64_t reg1, uint64_t reg2) {
    return reg1 | reg2;
}

uint64_t and(uint64_t reg1, uint64_t reg2) {
    return reg1 & reg2;
}

uint64_t addw(uint64_t reg1, uint64_t reg2) {
    uint32_t w1 = (uint32_t) reg1;
    uint32_t w2 = (uint32_t) reg2;
    uint32_t res = w1 + w2;
    return (uint64_t) res;
}

uint64_t subw(uint64_t reg1, uint64_t reg2) {
    uint32_t w1 = (uint32_t) reg1;
    uint32_t w2 = (uint32_t) reg2;
    uint32_t res = w1 - w2;
    return (uint64_t) res;
}

uint64_t sllw(uint64_t reg1, uint64_t reg2) {
    uint32_t w1 = (uint32_t) reg1;
    uint32_t w2 = (uint32_t) reg2;
    uint32_t res = w1 << w2;
    return (uint64_t) res;
}

uint64_t srlw(uint64_t reg1, uint64_t reg2) {
    uint32_t w1 = (uint32_t) reg1;
    uint32_t w2 = (uint32_t) reg2;
    uint32_t res = w1 >> w2;
    return (uint64_t) res;
}

uint64_t sraw(uint64_t reg1, uint64_t reg2) {
    uint32_t w1 = (uint32_t) reg1;
    uint32_t w2 = (uint32_t) reg2;
    int32_t res = (int32_t) w1 >> w2;
    return (uint64_t) res;
}

// R doesn't have imm
uint64_t exec_r(uint32_t instruction, uint64_t reg1, uint64_t reg2) {
    uint32_t opcode = get_opcode(instruction);
    uint32_t funct3 = get_funct3(instruction);
    uint32_t funct7 = get_funct7(instruction);
    if (opcode == 0x33) {
        if (funct7 == 0x00) {
            switch (funct3) {
                case 0:
                    return add(reg1, reg2);
                case 1:
                    return sll(reg1, reg2);
                case 2:
                    return slt(reg1, reg2);
                case 3:
                    return sltu(reg1, reg2);
                case 4:
                    return xor(reg1, reg2);
                case 5:
                    return srl(reg1, reg2);
                case 6:
                    return or(reg1, reg2);
                case 7:
                    return and(reg1, reg2);
                default:
                    // debugging
                    return 0;
            }

        }
        if (funct7 == 0x20) {
            switch (funct3) {
                case 0:
                    return sub(reg1, reg2);
                case 5:
                    return sra(reg1, reg2);
                default:
                    // debugging
                    return 0;
            }
        }
    }

    if (opcode == 0x3B) {
        if (funct7 == 0x00) {
            switch (funct3) {
                case 0:
                    return addw(reg1, reg2);
                case 1:
                    return sllw(reg1, reg2);
                case 5:
                    return srlw(reg1, reg2);
                default:
                    // debugging
                    return 0;
            }
        }
        if (funct7 == 0x20) {
            switch (funct3) {
                case 0:
                    subw(reg1, reg2);
                case 5:
                    sraw(reg1, reg2);
                default:
                    // debugging
                    return 0;
            }
        }

    }
    // debugging
    return 0;
}


// I instruction
uint64_t lx(uint64_t reg1, uint64_t imm) {  // lb, lh, lw, ld, lbu, lhu, lwu
    return reg1 + imm;
}

uint64_t addi(uint64_t reg1, uint64_t imm) {
    return reg1 + imm;
}

uint64_t slli(uint64_t reg1, uint64_t imm) {
    return reg1 << imm;
}

uint64_t slti(uint64_t reg1, uint64_t imm) {
    if ((int64_t) reg1 < (int64_t) imm) {
        return 1;
    } else {
        return 0;
    }
}

uint64_t sltiu(uint64_t reg1, uint64_t imm) {
    if (reg1 < unsign_expand(imm, 11)) {
        return 1;
    } else {
        return 0;
    }
}

uint64_t xori(uint64_t reg1, uint64_t imm) {
    return reg1 ^ imm;
}

uint64_t srli(uint64_t reg1, uint64_t imm) {
    return reg1 >> imm;
}

uint64_t srai(uint64_t reg1, uint64_t imm) {
    int64_t res = (int64_t) reg1 >> imm;
    return (uint64_t) res;
}

uint64_t ori(uint64_t reg1, uint64_t imm) {
    return reg1 | imm;
}

uint64_t andi(uint64_t reg1, uint64_t imm) {
    return reg1 & imm;
}

uint64_t addiw(uint64_t reg1, uint64_t imm) {
    uint32_t w1 = (uint32_t) reg1;
    uint32_t w2 = (uint32_t) imm;
    uint32_t res = w1 + w2;
    return (uint64_t) res;
}

uint64_t slliw(uint64_t reg1, uint64_t imm) {
    uint32_t w1 = (uint32_t) reg1;
    uint32_t w2 = (uint32_t) imm;
    uint32_t res = w1 << w2;
    return (uint64_t) res;
}

uint64_t srliw(uint64_t reg1, uint64_t imm) {
    uint32_t w1 = (uint32_t) reg1;
    uint32_t w2 = (uint32_t) imm;
    uint32_t res = w1 >> w2;
    return (uint64_t) res;
}

uint64_t sraiw(uint64_t reg1, uint64_t imm) {
    uint32_t w1 = (uint32_t) reg1;
    uint32_t w2 = (uint32_t) imm;
    int32_t res = (int32_t) w1 >> w2;
    return (uint64_t) res;
}

uint64_t jalr(uint64_t pc) {
    return pc + 4;
}

uint64_t csrrw(uint64_t reg1, uint64_t imm) {
    uint64_t res = 0;
    return (uint64_t) res;
}

uint64_t csrrs(uint64_t reg1, uint64_t imm) {
    uint64_t res = 0;
    return (uint64_t) res;
}

uint64_t csrrc(uint64_t reg1, uint64_t imm) {
    uint64_t res = 0;
    return (uint64_t) res;
}

uint64_t csrrwi(uint64_t reg1, uint64_t imm) {
    uint64_t res = 0;
    return (uint64_t) res;
}

uint64_t csrrsi(uint64_t reg1, uint64_t imm) {
    uint64_t res = 0;
    return (uint64_t) res;
}

uint64_t csrrci(uint64_t reg1, uint64_t imm) {
    uint64_t res = 0;
    return (uint64_t) res;
}

// I
uint64_t exec_i(uint32_t instruction, uint64_t reg1, uint64_t imm) {
    uint32_t opcode = get_opcode(instruction);
    uint32_t funct3 = get_funct3(instruction);
    if (opcode == LOAD) {
        return lx(reg1, imm);   // load instructions, return the address
    }

    uint32_t funct7 = 0;
    if (opcode == 0x13) {
        switch (funct3) {
            case 0:
                return addi(reg1, imm);
            case 1:
                funct7 = get_funct7(instruction);
                if (funct7 == 0) {
                    slli(reg1, imm);
                } else {
                    // debugging
                }
            case 2:
                return slti(reg1, imm);
            case 3:
                return sltiu(reg1, imm);
            case 4:
                return xori(reg1, imm);
            case 5:
                funct7 = get_funct7(instruction);
                if (funct7 == 0x00) {
                    return srli(reg1, imm);
                } else if (funct7 == 0x20) {
                    return srai(reg1, imm);
                } else {
                    // debugging
                    return 0;
                }
            case 6:
                return ori(reg1, imm);
            case 7:
                return andi(reg1, imm);
            default:
                // debugging
                return 0;
        }
    }

    if (opcode == 0x1B) {
        switch (funct3) {
            case 1:
                funct7 = get_funct7(instruction);
                if (funct7 == 0) {
                    return slliw(reg1, imm);
                } else {
                    // debugging
                    return 0;
                }
            case 5:
                funct7 = get_funct7(instruction);
                if (funct7 == 0x00) {
                    return srliw(reg1, imm);
                } else if (funct7 == 0x20) {
                    return sraiw(reg1, imm);
                } else {
                    // debugging
                    return 0;
                }
            default:
                // debugging
                return 0;
        }
    }

    if (opcode == 0x73) {
        switch (funct3) {
            case 0:
                // ecall ebreak
                return 0;
            case 1:
                return csrrw(reg1, imm);
            case 2:
                return csrrs(reg1, imm);
            case 3:
                return csrrc(reg1, imm);
            case 5:
                return csrrwi(reg1, imm);
            case 6:
                return csrrsi(reg1, imm);
            case 7:
                return csrrci(reg1, imm);
            default:
                // debugging
                return 0;
        }
    }

    return 0;
}

// S
uint64_t sx(uint64_t reg1, uint64_t imm) {  // sb, sd, sh
    return reg1 + imm;
}

uint64_t exec_s(uint32_t instruction, uint64_t reg1, uint64_t reg2, uint64_t imm) {
    uint32_t opcode = get_opcode(instruction);
    if (opcode == 0x23) {
        return sx(reg1, imm);  // return address
    }
    // debugging
    return 0;
}


// SB
bool beq(uint64_t reg1, uint64_t reg2) {
    return reg1 == reg2;
}

bool bne(uint64_t reg1, uint64_t reg2) {
    return reg1 != reg2;
}


bool bge(uint64_t reg1, uint64_t reg2) {
    return (int64_t) reg1 == (int64_t) reg2;
}

bool bgeu(uint64_t reg1, uint64_t reg2) {
    return reg1 > reg2;
}

bool blt(uint64_t reg1, uint64_t reg2) {
    return (int64_t) reg1 < (int64_t) reg2;
}

bool bltu(uint64_t reg1, uint64_t reg2) {
    return reg1 < reg2;
}

// executor sb operations, return true of falses
bool exec_sb(uint32_t instruction, uint64_t reg1, uint64_t reg2) {
    uint32_t opcode = get_opcode(instruction);
    uint32_t funct3 = get_funct3(instruction);
    if (opcode == 0x63) {
        switch (funct3) {
            case 0:
                return beq(reg1, reg2);
            case 1:
                return bne(reg1, reg2);
            case 4:
                return blt(reg1, reg2);
            case 5:
                return bge(reg1, reg2);
            case 6:
                return bltu(reg1, reg2);
            case 7:
                return bgeu(reg1, reg2);
            default:
                // debugging
                return 0;
        }
    }
    return 0;
}

// U
uint64_t auipc(uint64_t imm, const uint64_t pc) {
    return pc + imm;
}

uint64_t lui(uint64_t imm) {
    return imm;
}

uint64_t exec_u(uint32_t instruction, uint64_t imm, uint64_t pc) {
    uint32_t opcode = get_opcode(instruction);
    if (opcode == 0x17) {
        return auipc(imm, pc);
    }
    if (opcode == 0x37) {
        return lui(imm);
    }
    return 0;
}

// UJ
uint64_t jal(uint64_t pc) {
    return pc + 4;
}

uint64_t exec_uj(uint32_t instruction, uint64_t pc) {
    uint32_t opcode = get_opcode(instruction);
    if (opcode == 0x6F) {
        return jal(pc);
    }
    // debugging
    return 0;
}

