#include "pipeline.h"
#include "cache.h"

#define ENTRY_NUM 32
#define READ 1
#define WRITE 2

struct btb_entry {
    uint64_t pc;
    uint64_t predict_pc;
    bool valid;
};

struct _BTB {
    struct btb_entry entries[ENTRY_NUM];
    uint8_t idx;
};

static struct _BTB BTB;

bool found_BTB(uint64_t pc, uint64_t *predicted_pc) {
    bool predict = false;
    for (int i = 0; i < ENTRY_NUM; i++) {
        if (BTB.entries[i].pc == pc && BTB.entries[i].valid) {
            predict = true;
            *predicted_pc = BTB.entries[i].predict_pc;
            break;
        }
    }
    return predict;
}

void update_BTB(uint64_t pc, uint64_t predicted_pc) {
    int idx = BTB.idx % ENTRY_NUM;
    BTB.entries[idx].pc = pc;
    BTB.entries[idx].predict_pc = predicted_pc;
    BTB.entries[idx].valid = true;
    BTB.idx++;
}

void delete_BTB(uint64_t pc) {
    for (int i = 0; i < ENTRY_NUM; i++) {
        if (BTB.entries[i].pc == pc) {
            BTB.entries[i].valid = false;
            break;
        }
    }
}

void stage_fetch(struct stage_reg_d *new_d_reg) {
    // fetch instruction
    printf("\nfetch stage....\n");

    uint32_t instruction;
    uint64_t pc;

    pc = get_pc();

    // if stalled, re-fetch the instruction
    if (current_stage_x_register->data_stalled == 1) {
        pc = current_stage_x_register->new_pc;
        printf("found data hazard, refetch instruction ");
    }

    if (current_stage_m_register->control_stalled == 1) {
        pc = current_stage_m_register->new_pc;
        printf("found control hazard refectch isntruction ");
    }

    if (current_stage_w_register->stalled == 1) {
        pc = current_stage_w_register->new_pc;
        printf("found data cache missed, re-fetch instruction ");
    }
    printf("fetch instruction at %lld\n", pc);
    // flush the memory ops
    update_i_cache(pc);
    update_d_cache(current_stage_w_register->memory);

    bool stalled = instruction_fetch_stage(pc, &instruction);
    if (stalled) {   // cache missed, stall the pipeline
        new_d_reg->instruction = NOP;
        new_d_reg->stalled = 1;
        printf("i cache missed, stall the line, and don't modify pc\n");
        return;
    }

    uint64_t new_pc;
    new_pc = pc;

    // if found entry ,change next pc
    uint64_t predicted_pc = 0;
    bool predict = found_BTB(pc, &predicted_pc);
    if (predict) {
        printf("found pc in BTB, set next pc = %lld\n", predicted_pc);
        set_pc(predicted_pc);
    } else {
        printf("didn't find pc in BTB, set next pc = pc + 4\n");
        set_pc(pc + 4);
    }

    new_d_reg->stalled = 0;
    new_d_reg->instruction = instruction;
    new_d_reg->new_pc = new_pc;
}

void stage_decode(struct stage_reg_x *new_x_reg) {
    uint32_t instruction = current_stage_d_register->instruction;

    uint64_t new_pc = current_stage_d_register->new_pc;
    new_x_reg->new_pc = new_pc;
    printf("\ndecode stage...\n");

    // if this instruction is NOP, do nothing
    if (instruction == NOP) {
        new_x_reg->instruction = NOP;
        new_x_reg->data_stalled = 0;
        return;
    }

    // read miss
    if (current_stage_d_register->stalled == 1) {
        printf("found data cache miss, stall\n");
        new_x_reg->instruction = NOP;
        new_x_reg->data_stalled = 0;
        return;
    }

    // data hazard
    if (current_stage_x_register->data_stalled == 1) {
        printf("found data hazard in last instruction, stall\n");
        new_x_reg->instruction = NOP;
        new_x_reg->data_stalled = 0;
        return;
    }

    // control hazard
    if (current_stage_m_register->control_stalled == 1) {
        printf("found control hazard in last instruction, stall\n");
        new_x_reg->instruction = NOP;
        new_x_reg->instruction = 0;
        return;
    }

    // data cache miss
    if (current_stage_w_register->stalled == 1) {
        printf("found data cache missed in last instruction, stall\n");
        new_x_reg->instruction = NOP;
        new_x_reg->instruction = 0;
        return;
    }


    uint64_t reg1, reg2, imm, extend_pc;
    decode_stage(instruction, new_pc, &reg1, &reg2, &imm, &extend_pc);

    uint32_t opcode = get_opcode(instruction);

    uint32_t pre_opcode;
    uint32_t pre_instruction;

    // previous one instruction
    pre_instruction = current_stage_x_register->instruction;
    pre_opcode = get_opcode(pre_instruction);
    if (has_rd(pre_opcode)) {
        uint32_t rd = get_rd(pre_instruction);
        if (has_rs1(opcode)) {
            uint32_t rs1 = get_rs1(instruction);
            if (rs1 == rd) {
                // stall, this instruction won't be finished
                printf("stall the pipeline\n");
                new_x_reg->instruction = NOP;
                new_x_reg->data_stalled = 1;
                return;
            }
        }

        if (has_rs2(opcode)) {
            uint32_t rs2 = get_rs2(instruction);
            if (rs2 == rd) {
                // stall, this instruction won't be finished
                printf("stall the pipeline\n");
                new_x_reg->instruction = NOP;
                new_x_reg->data_stalled = 1;
                return;
            }
        }

    }

    // the second previous instruction, if it's LOAD stall one cycle, otherwise don't stall
    pre_instruction = current_stage_m_register->instruction;
    pre_opcode = get_opcode(pre_instruction);
    if (has_rd(pre_opcode)) {
        uint32_t rd = get_rd(pre_instruction);
        if (has_rs1(opcode)) {
            uint32_t rs1 = get_rs1(instruction);
            if (rs1 == rd) {
                if (pre_opcode != LOAD) {
                    reg1 = current_stage_m_register->res;
                    printf("forwarding...\n");
                } else {
                    // stall
                    printf("stall the pipeline\n");
                    new_x_reg->instruction = NOP;
                    new_x_reg->data_stalled = 1;
                    return;
                }
            }
        }

        if (has_rs2(opcode)) {
            uint32_t rs2 = get_rs2(instruction);
            if (rs2 == rd) {
                if (pre_opcode != LOAD) {
                    printf("forwarding...\n");
                    reg2 = current_stage_m_register->res;
                } else {
                    new_x_reg->instruction = NOP;
                    new_x_reg->data_stalled = 1;
                    return;
                    // stall
                }
            }
        }

    }

    // the third previous instruction
    pre_instruction = current_stage_w_register->instruction;
    pre_opcode = get_opcode(pre_instruction);
    if (has_rd(pre_opcode)) {
        uint32_t rd = get_rd(pre_instruction);
        if (has_rs1(opcode)) {
            uint32_t rs1 = get_rs1(instruction);
            if (rs1 == rd) {
                reg1 = current_stage_w_register->val;
                printf("forwarding...\n");
            }
        }

        if (has_rs2(opcode)) {
            uint32_t rs2 = get_rs2(instruction);
            if (rs2 == rd) {
                reg2 = current_stage_w_register->val;
                printf("forwarding...\n");
            }
        }
    }

    // after forwarding, pass the value to next stage
    printf("reg1: %lld, reg2: %lld, imm: %lld, extend_pc：%lld\n", reg1, reg2, imm, extend_pc);
    new_x_reg->instruction = instruction;
    new_x_reg->reg1 = reg1;
    new_x_reg->reg2 = reg2;
    new_x_reg->imm = imm;
    new_x_reg->extend_pc = extend_pc;
    new_x_reg->data_stalled = 0;
}

extern void stage_execute(struct stage_reg_m *new_m_reg) {
    uint32_t instruction = current_stage_x_register->instruction;
    printf("\nexecute stage.....\n");
    // if this instruction is NOP, pass the instruction and cancel stall signals
    if (instruction == NOP) {
        new_m_reg->instruction = NOP;
        new_m_reg->control_stalled = 0;
        return;
    }

    // data hazard
    if (current_stage_x_register->data_stalled == 1) {
        printf("found data hazard, stall\n");
        new_m_reg->instruction = NOP;
        new_m_reg->control_stalled = 0;
        return;
    }

    // control hazard
    if (current_stage_m_register->control_stalled == 1) {
        printf("found control cache miss, stall\n");
        new_m_reg->instruction = NOP;
        new_m_reg->control_stalled = 0;
        return;
    }

    // data cache missed
    if (current_stage_w_register->stalled == 1) {
        printf("found data cache missed, stalled\n");
        new_m_reg->instruction = NOP;
        new_m_reg->control_stalled = 0;
        return;
    }

    uint64_t reg1 = current_stage_x_register->reg1;
    uint64_t reg2 = current_stage_x_register->reg2;
    uint64_t imm = current_stage_x_register->imm;
    uint64_t new_pc = current_stage_x_register->new_pc;
    uint64_t extend_pc = current_stage_x_register->extend_pc;

    uint64_t res;
    execute_stage(instruction, reg1, reg2, imm, new_pc, &res);
    bool branch = branch_stage(instruction, reg1, reg2);

    // get the predicted result
    uint64_t predicted_pc = 0;
    bool predict = found_BTB(new_pc, &predicted_pc);

    printf("predict: %d, branch: %d, res: %lld\n", predict, branch, res);
    // BTB part
    if (branch) {
        // instruction which branch successfully
        printf("branch : this pc %lld,  next pc %lld", new_pc, extend_pc);
        if (predict) {
            //do nothing;
            printf("predict right, don't need to stall\n");
            new_m_reg->control_stalled = 0;
        } else {  // didn't predict, update BTB and stall the pipeline
            printf("didn't predict ");
            if (extend_pc < new_pc) {     // backwards taken
                update_BTB(new_pc, extend_pc);
                printf("next pc < this pc, update BTB, ");
            }
            printf("stall pipleline\n");
            new_m_reg->control_stalled = 1;
            new_pc = extend_pc;
        }
    } else {
        // predict wrong, delete the information and stall the pipeline
        printf("didn't need to branch\n");
        if (predict) {
            printf("wrong prediction, delete in BTB, reset next pc, stall pipeline");
            delete_BTB(new_pc);
            new_pc = new_pc + 4;
            new_m_reg->control_stalled = 1;
        } else {
            printf("didn't predict, normal execution\n");
            new_m_reg->control_stalled = 0;
        }
    }

    new_m_reg->instruction = instruction;
    new_m_reg->reg2 = reg2;
    new_m_reg->res = res;
    new_m_reg->new_pc = new_pc;

}

extern void stage_memory(struct stage_reg_w *new_w_reg) {
    // get values
    printf("\nmemory stage....\n");
    uint32_t instruction = current_stage_m_register->instruction;
    new_w_reg->new_pc = current_stage_m_register->new_pc;
    // if this instruction is NOP, do nothing
    if (instruction == NOP) {
        new_w_reg->instruction = NOP;
        new_w_reg->stalled = 0;
        return;
    }

    // control hazard
    if (current_stage_m_register->control_stalled == 1) {
        printf("found control hazard, stalled\n");
        new_w_reg->instruction = NOP;
        new_w_reg->stalled = 0;
        return;
    }

    // data cache missed
    if (current_stage_w_register->stalled == 1) {
        printf("found data cache hazard, stalled\n");
        new_w_reg->instruction = NOP;
        new_w_reg->stalled = 0;
        return;
    }

    uint64_t res = current_stage_m_register->res;
    uint64_t reg2 = current_stage_m_register->reg2;

    // run
    uint64_t val;
    bool stalled = memory_stage(instruction, res, reg2, &val);

    if (stalled) {
        printf("data cache miss, stall\n");
        new_w_reg->stalled = 1;
        new_w_reg->instruction = NOP;
        return;
    } else {

        new_w_reg->stalled = 0;
    }

    uint32_t opcode = get_opcode(instruction);
    if (opcode == LOAD || opcode == SAVE) {
        new_w_reg->memory = res;
    }

    // pass value to next;
    new_w_reg->instruction = instruction;
    new_w_reg->val = val;
    printf("val: %lld, memory: %lld\n", val, new_w_reg->memory);
}

extern void stage_writeback(void) {
    printf("\nwrite back stage...\n");
    uint32_t instruction = current_stage_w_register->instruction;
    // if this instruction is NOP, do nothing
    if (instruction == NOP) {
        return;
    }

    if (current_stage_w_register->stalled == 1) {
        printf("data cache missed, stalled\n");
        return;
    }

    uint64_t val = current_stage_w_register->val;
    write_back_stage(instruction, val);
}

void unit_tests() {

}