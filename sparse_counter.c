#include <assert.h>
#include <setjmp.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "sparse_counter.h"

typedef struct var_block_s var_block_t;

struct var_block_s
{
    unsigned int offset;
    unsigned int len;
    var_block_t *next;
};

static unsigned int __capmax(
    unsigned int val,
    unsigned int max
)
{
    if (max < val)
    {
        return max;
    }
    else
    {
        return val;
    }
}

/**
 * @param max The amount we are expecting to count to
 * @return initialised counter */
void *sc_init(const unsigned int max)
{
    sparsecounter_t *prog;

    prog = calloc(1, sizeof(sparsecounter_t));
    prog->max = max;
    prog->first_block = NULL;
    return prog;
}

void sc_free(
    void *ra
)
{
    sparsecounter_t *prog = ra;
    var_block_t *blk, *prev;

    blk = prog->first_block;

    while (blk)
    {
        prev = blk;
        blk = blk->next;
        free(prev);
    }

    free(prog);
}

/**
 * @return number of non-contiginous blocks */
int sc_get_num_blocks(
    sparsecounter_t * prog
)
{
    var_block_t *block;
    int num;

    for (num=0, block = prog->first_block; block; num++, block = block->next);

    return num;
}

/**
 * Mark this block as complete */
void sc_mark_complete(
    sparsecounter_t * prog,
    const unsigned int offset,
    const unsigned int len
)
{
    var_block_t *this = NULL, *prev;

    /* initialise first block */
    if (!prog->first_block)
    {
        var_block_t *block;

        block = prog->first_block = malloc(sizeof(var_block_t));
        block->len = len;
        block->offset = offset;
        block->next = NULL;
        return;
    }

    /* get 1st block that has a higher offset than us */

    prev = prog->first_block;

    /* we are at the beginning of the blocks */
    if (offset < prev->offset)
    {
        var_block_t *block;

        block = prog->first_block = malloc(sizeof(var_block_t));
        block->len = len;
        block->offset = offset;
        block->next = this = prev;
        prev = block;
    }
    else
    {
        /*  find a place where our offset is equal or greater */
        while (prev->next && (prev->next->offset <= offset))
        {
            prev = prev->next;
        }

        this = prev->next;

        /* The left block eats/touches this new one...
         * |00000LLN00000|;
         * |00000LLNL0000|
         * Combine the old with new */
        if (offset <= prev->offset + prev->len)
        {
            /* completely ate this one!
             * |00000LLNL0000| */
            if (offset + len <= prev->offset + prev->len)
            {

            }
            else
            {
                /* increase coverage */
                prev->len = (offset + len) - prev->offset;
                /* might eat another... */
            }
        }
        else
        {
            var_block_t *blk;

            blk = malloc(sizeof(var_block_t));
            blk->offset = offset;
            blk->len = len;
            blk->next = this;
            prev->next = blk;
            prev = blk;
        }
    }

    /* prev will feast! */
    if (this && this->offset <= prev->offset + prev->len)
    {
        do
        {
            /* not eaten */
            if (!(this->offset + this->len <= prev->offset + prev->len))
            {
                prev->len = (this->offset + this->len) - prev->offset;
            }

            prev->next = this->next;
            free(this);
            this = prev->next;
        }
        while (this && this->offset <= prev->offset + prev->len);
    }
}

void sc_mark_incomplete(
    sparsecounter_t * prog,
    const unsigned int offset,
    const unsigned int len
)
{
    var_block_t *this = NULL, *prev;

    if (!prog->first_block)
        return;

    /* get 1st block that has a higher offset than us */

    prev = NULL;
    this = prog->first_block;

    while (this)
    {
        /*  whole chunk gets eaten */
        if (offset <= this->offset &&
                this->offset + this->len <= offset + len)
        {
            if (prev)
            {
                prev->next = this->next;
                free(this);
                this = prev;
            }
            else
            {
                prog->first_block = this->next;
                free(this);
                this = prog->first_block;
                if (!this) break;
            }
        }
        /*
         * In the middle
         * |00000LXL00000|
         */
        else if (this->offset < offset &&
                offset + len < this->offset + this->len)
        {
            var_block_t *blk;

            blk = malloc(sizeof(var_block_t));
            blk->offset = offset + len;
            blk->len = this->len - len - (offset - this->offset);
            blk->next = this->next;

            this->len = offset - this->offset;
            this->next = blk;
        }
        /*
         * swallow left
         * |00000XLL00000|
         */
        else if (this->offset < offset + len &&
                offset + len < this->offset + this->len)
        {
            this->len -= offset+len - this->offset;
            this->offset = offset+len;
        }
        /*
         * swallow right
         * |00000LLX00000|
         */
        else if (this->offset < offset &&
                offset < this->offset + this->len &&
                this->offset + this->len <= offset + len)
        {
            this->len = offset - this->offset;
        }

        prev = this;
        this = this->next;
    }
}

int sc_is_complete(
    sparsecounter_t * prog
)
{
    var_block_t *block;

    block = prog->first_block;

    return block && !block->next && block->len == prog->max;
}

/**
 * Get an incompleted block  */
void sc_get_incomplete(
    const sparsecounter_t * prog,
    unsigned int *offset,
    unsigned int *len,
    const unsigned int max
)
{
    const var_block_t *block;

    *offset = *len = 0;
    block = prog->first_block;

    if (!block)
    {
        *offset = 0;
        *len = max;
    }
    else
    {
        if (block->offset != 0)
        {
            *offset = 0;

            if (block->next)
            {
                *len = block->next->offset - block->offset;
            }
            else
            {
                *len = block->offset;
            }
        }
        else if (!block->next)
        {
            *offset = block->len;
            *len = max;
        }
        else
        {
            *offset = 0 + block->len;
            *len = block->next->offset - block->len;
        }
    }

    *len = __capmax(*len, max);

//    printf("%d %d %d %d\n", prog->max, *len, max, *offset + *len);

    /*  make sure we aren't going over the boundary */
    if (prog->max < *offset + *len)
    {
        *len = prog->max - *offset;
    }
}

unsigned int sc_get_nbytes_completed(
    const sparsecounter_t * prog
)
{
    const var_block_t *block;

    block = prog->first_block;

    unsigned int nbytes = 0;

    while (block)
    {
        nbytes += block->len;
        block = block->next;
    }

    return nbytes;
}

/**
 * @return 1 if we have this block, 0 otherwise */
int sc_have(
    sparsecounter_t * prog,
    const unsigned int offset,
    const unsigned int len
)
{
    const var_block_t *block;

    block = prog->first_block;

    while (block)
    {
        if (offset < block->offset)
        {

        }
        else if (block->offset <= offset)
        {
            if (offset + len <= block->offset + block->len)
            {
                return 1;
            }
        }
        else
        {
            return 0;
        }

        block = block->next;
    }

    return 0;
}

void sc_print_contents(sparsecounter_t * prog)
{
    const var_block_t *block;

    block = prog->first_block;

    while (block)
    {
        printf("%d to %d\n", block->offset, block->offset + block->len);
        block = block->next;
    }
}
