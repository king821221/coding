#include <math.h>
#include <assert.h>
#include <stdlib.h>
#define INT_BITS (8*sizeof(int))
#pragma once

class bitset
{
private:
       size_t* bitmask;
       size_t numbits;
       size_t bitmask_size;
public:
       bitset(size_t s) : numbits(s)
       {
           bitmask_size = (size_t)ceil((double)s/INT_BITS);
           bitmask = new size_t[bitmask_size];
           memset(bitmask, 0, sizeof(size_t) * bitmask_size);
           assert(bitmask);
       }
       ~bitset()
       {
           if (bitmask)
           {
               delete []bitmask;
           }
       }
       void set(size_t p)
       {
           assert(p < numbits);
           bitmask[p / INT_BITS] |= (1 << (p % INT_BITS));
       }
       void unset(size_t p)
       {
           assert(p < numbits);
           bitmask[p / INT_BITS] &= ~(1 << (p % INT_BITS));
       }
       bool test(size_t p) const
       {
           assert(p < numbits);
           return bitmask[p / INT_BITS] & (1 << (p % INT_BITS));
       }
       size_t num_bits_set() const
       {
           size_t numset = 0;
           for (size_t i = 0; i < bitmask_size; i++)
           {
               size_t bit_elem = bitmask[i];
               while (bit_elem)
               {
                   numset++;
                   bit_elem &= (bit_elem - 1);
               }
           }
           return numset;
       }
       size_t size() const
       {
           return numbits;
       }
};
