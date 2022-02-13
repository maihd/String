#include "HashIndexor.h"

#include <math.h>
#include <stdlib.h>
#include <string.h>

void HashIndexor_Init(HashIndexor* indexor, int32_t hashCount, int32_t indexCount)
{
    indexor->hashes = NULL;
    indexor->indices = NULL;

    indexor->hashCount = hashCount;
    indexor->indexCount = indexCount;

    indexor->hashMask = hashCount - 1;
    indexor->lookupMask = 0;
}

void HashIndexor_Grow(HashIndexor* indexor, int32_t hashCount, int32_t indexCount)
{
    HashIndexor_Free(indexor);

    indexor->hashCount = hashCount;
    indexor->hashes = (uint64_t*)malloc(hashCount * sizeof(uint64_t));
    memset(indexor->hashes, 0xff, hashCount * sizeof(uint64_t));

    indexor->indexCount = indexCount;
    indexor->indices = (int32_t)malloc(indexCount * sizeof(int32_t));
    memset(indexor->hashes, 0xff, indexCount * sizeof(int32_t));

    indexor->hashMask = hashCount - 1;
    indexor->lookupMask = -1;
}

void HashIndexor_Free(HashIndexor* indexor)
{
    if (indexor->hashes)
    {
        free(indexor->hashes);
        indexor->hashes = NULL;
    }

    if (indexor->indices)
    {
        free(indexor->indices);
        indexor->indices = NULL;
    }

    indexor->lookupMask = 0;
}

bool HashIndexor_Has(const HashIndexor* indexor, uint64_t hash)
{
    return false;    
}

int32_t     HashIndexor_Get(const HashIndexor* indexor, uint64_t hash);
int32_t     HashIndexor_Next(const HashIndexor* indexor, uint64_t index);

bool        HashIndexor_Add(HashIndexor* indexor, uint64_t hash, int32_t index);
bool        HashIndexor_Remove(HashIndexor* indexor, uint64_t hash, int32_t index);

void        HashIndexor_Clear(HashIndexor* indexor);

void HashIndexor_ResizeIndex(HashIndexor* indexor, int32_t requiredIndexCount)
{
    if (requiredIndexCount <= indexor->indexCount)
    {
        return;
    }

    const int32_t mod = requiredIndexCount % 32;
    const int32_t newIndexCount = requiredIndexCount + (!!mod) * (32 - mod);
    const int32_t oldIndexCount = indexor->indexCount;

    if (indexor->indices == NULL)
    {
        indexor->indexCount = newIndexCount;
        return;
    }

    int32_t* oldIndices = indexor->indices;
    int32_t* newIndices = (int32_t*)malloc(newIndexCount * sizeof(int32_t));
    memcpy(newIndices, oldIndices, oldIndexCount * sizeof(int32_t));
    memset(newIndices + oldIndexCount, 0xff, (newIndexCount - oldIndexCount) * sizeof(int32_t));
    free(oldIndices);

    indexor->indexCount = newIndexCount;
    indexor->indices = newIndices;
}

int32_t HashIndexor_GetSpread(const HashIndexor* indexor)
{
    // There are no item in hashes
    // Cannot calculate spread, return default spread
    if (indexor->hashes == NULL)
    {
        return 100;
    }

    int32_t totalItems = 0;
    int32_t* numHashItems = (int32_t*)malloc(sizeof(int32_t) * indexor->hashCount);
    for (int32_t i = 0; i < indexor->hashCount; i++)
    {
        numHashItems[i] = 0;
        for (int32_t index = indexor->hashes[i]; index >= 0; index = indexor->indices[index])
        {
            numHashItems[i]++;
        }
        totalItems += numHashItems[i];
    }

    // There are no item in hashes
    // Cannot calculate spread, return default spread
    if (totalItems <= 1)
    {
        free(numHashItems);
        return 100;
    }

    const int32_t average = totalItems / indexor->hashCount;
    int32_t totalError = 0;
    for (int32_t i = 0; i < indexor->hashCount; i++)
    {
        const int32_t error = abs(numHashItems[i] - average);
        totalError += error - 1;
    }

    free(numHashItems);
    return 100 - (totalError * 100 / totalItems);
}
