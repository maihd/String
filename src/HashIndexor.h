#pragma once

#include <stdint.h>
#include <stdbool.h>

typedef struct HashIndexor
{
    uint64_t*   hashes;
    int32_t*    indices;
    
    int32_t     hashCount;
    int32_t     indexCount;

    int32_t     hashMask;
    int32_t     lookupMask;
} HashIndexor;

void        HashIndexor_Init(HashIndexor* indexor, int32_t hashCount, int32_t indexCount);
void        HashIndexor_Grow(HashIndexor* indexor, int32_t hashCount, int32_t indexCount);
void        HashIndexor_Free(HashIndexor* indexor);

bool        HashIndexor_Has(const HashIndexor* indexor, uint64_t hash);
int32_t     HashIndexor_Get(const HashIndexor* indexor, uint64_t hash);
int32_t     HashIndexor_Next(const HashIndexor* indexor, uint64_t index);

bool        HashIndexor_Add(HashIndexor* indexor, uint64_t hash, int32_t index);
bool        HashIndexor_Remove(HashIndexor* indexor, uint64_t hash, int32_t index);

void        HashIndexor_Clear(HashIndexor* indexor);

void        HashIndexor_ResizeIndex(HashIndexor* indexor, int32_t requiredIndexCount);
int32_t     HashIndexor_GetSpread(const HashIndexor* indexor);
