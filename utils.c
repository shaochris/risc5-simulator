#include "utils.h"

#define ALLMASK    ((uint64_t)-1)

#define WORDLENGTH 32

uint32_t get_bits(uint32_t instruction, uint8_t low, uint8_t high) {
    instruction <<= (WORDLENGTH - 1 - high);
    instruction >>= (WORDLENGTH - (high - low + 1));
    return instruction;
}


uint32_t get_opcode(uint32_t instruction) {
    return get_bits(instruction, 0, 6);
}

uint32_t get_rd(uint32_t instruction) {
    return get_bits(instruction, 7, 11);
}

uint32_t get_funct3(uint32_t instruction) {
    return get_bits(instruction, 12, 14);
}

uint32_t get_rs1(uint32_t instruction) {
    return get_bits(instruction, 15, 19);
}

uint32_t get_rs2(uint32_t instruction) {
    return get_bits(instruction, 20, 24);
}

uint32_t get_funct7(uint32_t instruction) {
    return get_bits(instruction, 25, 31);
}

uint64_t get_imm_i(uint32_t instruction) {
    uint32_t imm = get_bits(instruction, 20, 31);
    return sign_expand(imm, 11);
}

uint64_t get_imm_s(uint32_t instruction) {
    uint32_t low_bits = get_bits(instruction, 7, 11);
    uint32_t high_bits = get_bits(instruction, 25, 31);
    uint32_t imm = (high_bits << 5) | low_bits;
    return sign_expand(imm, 11);
}

uint64_t get_imm_sb(uint32_t instruction) {
    uint32_t imm1 = get_bits(instruction, 31, 31) << 12;
    uint32_t imm2 = get_bits(instruction, 25, 30) << 5;
    uint32_t imm3 = get_bits(instruction, 8, 11) << 1;
    uint32_t imm4 = get_bits(instruction, 7, 7) << 11;
    uint32_t imm = imm1 | imm2 | imm3 | imm4;
    return sign_expand(imm, 12);
}

uint64_t get_imm_u(uint32_t instruction) {
    uint32_t imm = get_bits(instruction, 12, 31);
    imm <<= 12;
    return sign_expand(imm, 31);

}

uint64_t get_imm_uj(uint32_t instruction) {
//    uint32_t imm = get_bits(instruction, 12, 31);
    uint32_t imm1 = get_bits(instruction, 31, 31) << 20;
    uint32_t imm2 = get_bits(instruction, 21, 30) << 1;
    uint32_t imm3 = get_bits(instruction, 20, 20) << 11;
    uint32_t imm4 = get_bits(instruction, 12, 19) << 12;
    uint32_t imm = imm1 | imm2 | imm3 | imm4;
    return sign_expand(imm, 20);
}

uint64_t get_imm(uint32_t instruction) {
    uint32_t opcode = get_opcode(instruction);
    if (is_i(opcode)) {
        return get_imm_i(instruction);
    }
    if (is_s(opcode)) {
        return get_imm_s(instruction);
    }
    if (is_sb(opcode)) {
        return get_imm_sb(instruction);
    }
    if (is_u(opcode)) {
        return get_imm_u(instruction);
    }
    if (is_uj(opcode)) {
        return get_imm_uj(instruction);
    }

    return 0;

}


uint64_t sign_expand(uint32_t imm, uint8_t bit_num) {
    uint32_t MSB = imm & (1 << bit_num);
    if (MSB == 0) {
        return (uint64_t) imm;
    } else {
        // construct mask
        uint64_t mask = ALLMASK << (bit_num + 1);
        return mask | (uint64_t) imm;
    }
}

uint64_t unsign_expand(uint64_t imm, uint8_t bit_num) {
    uint64_t MSB = imm & (1 << bit_num);
    if (MSB == 0) {
        return imm;
    } else {
        // construct mask
        uint64_t mask = ALLMASK >> (63 - bit_num);
        return mask & imm;
    }
}

bool is_r(uint32_t opcode) {
    return opcode == 0x33 || opcode == 0x3B;
}

bool is_i(uint32_t opcode) {
    return opcode == 0x03 || opcode == 0x13 || opcode == 0x1B || opcode == 0x67 || opcode == 0x73;
}

bool is_s(uint32_t opcode) {
    return opcode == 0x23;
}

bool is_sb(uint32_t opcode) {
    return opcode == 0x63;
}

bool is_u(uint32_t opcode) {
    return opcode == 0x37 || opcode == 0x17;
}

bool is_uj(uint32_t opcode) {
    return opcode == 0x6F;
}

bool is_branch(uint32_t opcode) {
    return is_sb(opcode) || opcode == JALR || opcode == JAL;
}

bool has_rs1(uint32_t opcode) {
    return is_r(opcode) || is_i(opcode) || is_s(opcode) || is_sb(opcode);
}

bool has_rs2(uint32_t opcode) {
    return is_r(opcode) || is_s(opcode) || is_sb(opcode);
}

bool has_rd(uint32_t opcode) {
    return is_r(opcode) || is_i(opcode) || is_u(opcode) || is_uj(opcode);
}

bool has_imm(uint32_t opcode) {
    return is_i(opcode) || is_s(opcode) || is_sb(opcode) || is_uj(opcode) || is_u(opcode);
}