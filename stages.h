#ifndef ARCHITECTURE_DECODE_H
#define ARCHITECTURE_DECODE_H

#include <stdint.h>
#include <stdbool.h>
#include "utils.h"
#include "executor.h"
#include "cache.h"


extern void execute_single_instruction(uint64_t pc, uint64_t *new_pc);

bool instruction_fetch_stage(uint64_t pc, uint32_t *instruction);

void
decode_stage(uint32_t instruction, uint64_t pc, uint64_t *reg1, uint64_t *reg2, uint64_t *imm, uint64_t *extend_pc);

// execute stage and branch stage are together
void execute_stage(uint32_t instruction, uint64_t reg1, uint64_t reg2, uint64_t imm,
                   uint64_t pc, uint64_t *res);

bool branch_stage(uint32_t instruction, uint64_t reg1, uint64_t reg2);

bool memory_stage(uint32_t instruction, uint64_t res, uint64_t reg2, uint64_t *val);

void write_back_stage(uint32_t instruction, uint64_t res);


#endif //ARCHITECTURE_DECODE_H
