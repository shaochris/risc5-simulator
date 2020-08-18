#include "stages.h"
#include "cache.h"

// return is_stalled
bool instruction_fetch_stage(uint64_t pc, uint32_t *instruction) {
    // first update the status of cache
    bool is_found = found_i_cache(pc, instruction);
    if (is_found) {
        return false;
    } else {
        // execute a memory op
        uint64_t block_addr = get_64bits(pc, 4, 63) << 4;
        uint32_t block[4];
        // if memory op is right, copy it to cache, otherwise it will be pended
        bool status = memory_read(block_addr, block, 16);
        if (status) {
            copy_i_cache(block_addr, block);
        }
        return true;
    }
}

// decode state, get the value of register
// read rs1, rs2 and extend imm to 64 bits
void
decode_stage(uint32_t instruction, uint64_t pc, uint64_t *reg1, uint64_t *reg2, uint64_t *imm, uint64_t *extend_pc) {
    uint32_t opcode = get_opcode(instruction);

    uint32_t rs1, rs2;
    rs1 = rs2 = 0;
    if (has_rs1(opcode)) {
        rs1 = get_rs1(instruction);
    }
    if (has_rs2(opcode)) {
        rs2 = get_rs2(instruction);
    }

    register_read(rs1, rs2, reg1, reg2);

    if (has_imm(opcode)) {
        *imm = get_imm(instruction);
    } else {
        *imm = 0;
    }

    if (is_branch(opcode)) {
        *extend_pc = pc + *imm;
    }
}

// return res and change the new_pc by pointer
void execute_stage(uint32_t instruction, uint64_t reg1, uint64_t reg2, uint64_t imm,
                   uint64_t pc, uint64_t *res) {
    uint32_t opcode = get_opcode(instruction);
    if (is_r(opcode)) {
        *res = exec_r(instruction, reg1, reg2);

    } else if (is_i(opcode)) {
        *res = exec_i(instruction, reg1, imm);
        if (opcode == JALR) {
            *res = jalr(pc);
        }

    } else if (is_s(opcode)) {
        *res = exec_s(instruction, reg1, reg2, imm);

    } else if (is_sb(opcode)) {
        //;

    } else if (is_u(opcode)) {
        *res = exec_u(instruction, imm, pc);

    } else if (is_uj(opcode)) {
        *res = exec_uj(instruction, pc);

    } else {
        //debugging;
    }
}

// the branch state is used for branch operations, return true or false, the normal ops will return false
bool branch_stage(uint32_t instruction, uint64_t reg1, uint64_t reg2) {
    uint32_t opcode = get_opcode(instruction);
    if (is_sb(opcode)) {
        return exec_sb(instruction, reg1, reg2);
    }
    if (opcode == JALR) {
        return true;
    }
    if (opcode == JAL) {
        return true;
    }
    return false;
}

// if opcode is Load, result of execution stage is address, return data loaded
// else, just pass the res
// if opcode is save, it need reg2
bool memory_stage(uint32_t instruction, uint64_t res, uint64_t reg2, uint64_t *val) {
    uint32_t opcode = get_opcode(instruction);

    *val = res;
    uint64_t block;

    if (opcode != LOAD && opcode != SAVE) {
        return false;
    }

    bool is_found;
    if (opcode == LOAD) {
        printf("Load at %lld\n", res);
        is_found = found_d_cache(res, &block);
        if (!is_found) {  // if not found, the stall is true, so return true
            uint64_t block_addr = get_64bits(res, 3, 63) << 3;
//            uint64_t line_num = get_64bits(res, 3, 13);

            printf("load and caches missed, add read op and stall ");
            bool status = memory_read(block_addr, &block, 16);
            if (status) {
                copy_d_cache(res, block);
                printf("copy data to %llx block: %llx,", block_addr, block);
            }
            printf("\n");
            return true;
        }

        uint8_t b = (uint8_t) (block);
        uint16_t hw = (uint16_t) (block);
        uint32_t w = (uint32_t) (block);
        uint64_t dl = (uint64_t) (block);

        uint32_t funct3 = get_funct3(instruction);

        switch (funct3) {
            case 0x0:    // lb
                *val = (uint64_t) (int8_t) b;
                break;
            case 0x1:    // lh
                *val = (uint64_t) (int16_t) hw;
                break;
            case 0x2:    // lw
                *val = (uint64_t) (int32_t) w;
                break;
            case 0x3:    // ld
                *val = (uint64_t) (int64_t) dl;
                break;
            case 0x4:    // lbu
                *val = (uint64_t) b;
                break;
            case 0x5:    // lhu
                *val = (uint64_t) hw;
                break;
            case 0x6:    // lwu
                *val = (uint64_t) w;
                break;
            default:
                // debugging
                break;
        }
        printf("Load from cache, %lld\n", *val);
    }

    // store
    if (opcode == SAVE) {
        printf("Store at %lld\n", res);
        is_found = found_d_cache(res, &block);
        uint32_t funct3 = get_funct3(instruction);

        if (is_found) {   // found in cache, need to update
            printf("save and found cache, write into cache\n");
            switch (funct3) {
                case 0x0:   // sb
                    write_d_cache(res, reg2, 1);
                    break;
                case 0x1:   // sh
                    write_d_cache(res, reg2, 2);
                    break;
                case 0x2:
                    write_d_cache(res, reg2, 4);
                    break;
                case 0x3:
                    write_d_cache(res, reg2, 8);
                default:
                    //debugging
                    break;
            }
        }
        // write to memory
        printf("write into memory %lld \n", res);
        switch (funct3) {
            case 0x0:   // sb
                memory_write(res, (uint8_t) (reg2), 1);
                break;
            case 0x1:   // sh
                memory_write(res, (uint16_t) (reg2), 2);
                break;
            case 0x2:
                memory_write(res, (uint32_t) (reg2), 4);
                break;
            case 0x3:
                memory_write(res, (uint64_t) (reg2), 8);
            default:
                //debugging
                break;
        }
    }
    return false;
}


void write_back_stage(uint32_t instruction, uint64_t val) {
    uint32_t opcode = get_opcode(instruction);

    if (has_rd(opcode)) {
        uint64_t rd = get_rd(instruction);
        register_write(rd, val);
        printf("write %d at register %d\n", val, rd);
    }

}

void execute_single_instruction(uint64_t pc, uint64_t *new_pc) {
    uint32_t instruction;

    instruction_fetch_stage(pc, &instruction);

//    instruction = 0b00000000000100000000000010010011;

    uint64_t reg1, reg2, imm, extend_pc;
    decode_stage(instruction, pc, &reg1, &reg2, &imm, &extend_pc);

    uint64_t res;
    execute_stage(instruction, reg1, reg2, imm, pc, &res);
    bool branch = branch_stage(instruction, reg1, reg2);
    if (branch) {
        *new_pc = extend_pc;
    } else {
        *new_pc = pc + 4;
    }

    uint64_t val;
    memory_stage(instruction, res, reg2, &val);

    write_back_stage(instruction, val);
}

#include <stdio.h>

extern int unit_test() {
    return 0;
}