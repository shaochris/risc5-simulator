#ifndef ARCHITECTURE_UTILS_H
#define ARCHITECTURE_UTILS_H

#define LOAD 0x03
#define SAVE 0x23
#define JALR 0x67
#define JAL  0x6F

#include <stdint.h>
#include <stdbool.h>

uint32_t get_opcode(uint32_t instruction);

uint32_t get_rd(uint32_t instruction);

uint32_t get_funct3(uint32_t instruction);

uint32_t get_rs1(uint32_t instruction);

uint32_t get_rs2(uint32_t instruction);

uint32_t get_funct7(uint32_t instruction);

uint64_t get_imm(uint32_t instruction);

uint64_t sign_expand(uint32_t imm, uint8_t bit_num);

uint64_t unsign_expand(uint64_t imm, uint8_t bit_num);

bool is_r(uint32_t opcode);

bool is_i(uint32_t opcode);

bool is_s(uint32_t opcode);

bool is_sb(uint32_t opcode);

bool is_u(uint32_t opcode);

bool is_uj(uint32_t opcode);

bool is_branch(uint32_t opcode);

bool has_rs1(uint32_t opcode);

bool has_rs2(uint32_t opcode);

bool has_rd(uint32_t opcode);

bool has_imm(uint32_t opcode);

#endif
