#include "cache.h"

#define I_CACHE_LINES 512
#define D_CACHE_LINES 2048
#define DOUBLEWORD 64

struct i_cache_line {
    uint64_t sig; //51 bits
    uint32_t block[4];
    bool valid;
};

struct _i_cache {
    int idx;
    struct i_cache_line caches[I_CACHE_LINES];
} i_cache;

struct d_cache_line {
    uint64_t sig;  // 50 bits
    uint32_t block[2];
    bool valid;
};

struct _d_cache {
    int idx;
    struct d_cache_line caches[D_CACHE_LINES];
} d_cache;

uint64_t get_64bits(uint64_t addr, uint8_t low, uint8_t high) {
    addr <<= (DOUBLEWORD - 1 - high);
    addr >>= (DOUBLEWORD - (high - low + 1));
    return addr;
}


// if addr was found in cache, transfer it to instruction
bool found_i_cache(uint64_t addr, uint32_t *instruction) {
    uint64_t line_num = get_64bits(addr, 4, 12);
    uint64_t sig = get_64bits(addr, 13, 63);
    if (i_cache.caches[line_num].valid && i_cache.caches[line_num].sig == sig) {
        uint64_t offset = get_64bits(addr, 0, 3);
        *instruction = i_cache.caches[line_num].block[offset / 4];
        printf("found addr in i cache line %llx\n", line_num);
        return true;
    }
    return false;
}

// use update to flush the op of read and write
bool update_i_cache(uint64_t addr) {
    uint64_t block_addr = get_64bits(addr, 4, 63) << 4;

    uint32_t value[4];
    bool status = memory_status(block_addr, &value);

    if (status) {
        printf("previous read op at %llx has finished.\n", block_addr);
        copy_i_cache(addr, value);
        return true;
    }
    return false;
}

// copy the block into cache
void copy_i_cache(uint64_t addr, uint32_t block[4]) {
    uint64_t line_num = get_64bits(addr, 4, 12);
    memcpy(i_cache.caches[line_num].block, block, 16);
    i_cache.caches[line_num].valid = true;
    printf("copy the value of block to cache line %llx, the value is %lx %lx %lx %lx\n", line_num, block[0], block[1],
           block[2], block[3]);
}


// found and read d cache
bool found_d_cache(uint64_t addr, uint64_t *value) {
    uint64_t line_num = get_64bits(addr, 3, 13);
    uint64_t sig = get_64bits(addr, 14, 63);
    if (d_cache.caches[line_num].valid && d_cache.caches[line_num].sig == sig) {
        uint64_t offset = get_64bits(addr, 0, 2);
        *value = d_cache.caches[line_num].block[offset / 4];
        return true;
    }
    return false;
}

// write cache with the value of register 2, but need to declare the size of data in bytes
bool write_d_cache(uint64_t addr, uint64_t val, uint8_t size_of_bytes) {
    uint64_t line_num = get_64bits(addr, 3, 13);

    uint64_t high = (uint64_t) d_cache.caches[line_num].block[0];
    uint64_t low = (uint64_t) d_cache.caches[line_num].block[1];

    uint64_t block = 0;
    block |= (high << 32);
    block |= (low);

    val <<= (DOUBLEWORD - (size_of_bytes * 8));
    val >>= (DOUBLEWORD - (size_of_bytes * 8)); // do the same thing, but reverse

    block |= val;   // then we can change the last k-byte value in cache

    d_cache.caches[line_num].block[0] = (uint32_t) block;
    d_cache.caches[line_num].block[1] = (uint32_t) (block >> 32);
    return true;
}


//
bool update_d_cache(uint64_t addr) {
    uint64_t block_addr = get_64bits(addr, 3, 63) << 3;

    uint64_t value;
    bool status = memory_status(block_addr, &value);
    if (status) {
        copy_d_cache(addr, value);
        return true;
    }
    return false;
}

void copy_d_cache(uint64_t addr, uint64_t block) {
    uint64_t line_num = get_64bits(addr, 3, 13);
    d_cache.caches[line_num].block[0] = (uint32_t) (block);
    d_cache.caches[line_num].block[1] = (uint32_t) (block >> 32);
    d_cache.caches[line_num].valid = true;
}