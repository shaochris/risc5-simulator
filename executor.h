#ifndef ARCHITECTURE_EXECUTOR_H
#define ARCHITECTURE_EXECUTOR_H

#include <stdint.h>
#include <stdbool.h>
#include "utils.h"

// R doesn't have imm
uint64_t exec_r(uint32_t instruction, uint64_t reg1, uint64_t reg2);

// I instruction
uint64_t exec_i(uint32_t instruction, uint64_t reg1, uint64_t imm);

uint64_t jalr(uint64_t pc);

// S
uint64_t exec_s(uint32_t instruction, uint64_t reg1, uint64_t reg2, uint64_t imm);

// SB
bool exec_sb(uint32_t instruction, uint64_t reg1, uint64_t reg2);

// U   auipc lui
uint64_t exec_u(uint32_t instruction, uint64_t imm, uint64_t pc);

// UJ
uint64_t exec_uj(uint32_t instruction, uint64_t pc);

#endif //ARCHITECTURE_EXECUTOR_H
