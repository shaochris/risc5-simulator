#ifndef ASS3_CACHE_H
#define ASS3_CACHE_H

#include "utils.h"
#include <stdint.h>
#include <stdbool.h>
#include "riscv_sim_pipeline_framework.h"
#include <memory.h>
#include "stdio.h"

bool found_i_cache(uint64_t addr, uint32_t *value);

bool found_d_cache(uint64_t addr, uint64_t *value);

uint64_t get_64bits(uint64_t addr, uint8_t low, uint8_t high);

bool update_i_cache(uint64_t);

bool update_d_cache(uint64_t);

bool write_d_cache(uint64_t addr, uint64_t val, uint8_t size_of_bytes);

void copy_i_cache(uint64_t addr, uint32_t block[4]);

void copy_d_cache(uint64_t addr, uint64_t block);

#endif //ASS3_CACHE_H
