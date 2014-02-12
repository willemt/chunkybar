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

void *sc_init(const unsigned int max)
{
    sparsecounter_t *me;

    me = calloc(1, sizeof(sparsecounter_t));
    me->max = max;
    me->first_block = NULL;

    assert(me);
    return me;
}

void sc_free(
    void *ra
)
{
    sparsecounter_t *me = ra;
    var_block_t *b, *prev;

    for (b = me->first_block; b; )
    {
        prev = b;
        b=b->next;
        free(prev);
    }

    free(me);
}

void sc_set_max(sparsecounter_t* me, const unsigned int max)
{
    me->max = max;
}

void sc_mark_all_incomplete(sparsecounter_t * me)
{
    sc_mark_incomplete(me,0,me->max);
}

int sc_get_num_blocks(const sparsecounter_t * me)
{
    const var_block_t *b;
    int num;

    for (num=0, b = me->first_block; b; num++, b = b->next);

    return num;
}

void sc_mark_complete(
    sparsecounter_t * me,
    const unsigned int offset,
    const unsigned int len
)
{
    var_block_t *this = NULL, *prev;

    /* initialise first block */
    if (!me->first_block)
    {
        var_block_t *b;

        b = malloc(sizeof(var_block_t));
        b->len = len;
        b->offset = offset;
        b->next = NULL;

        /* set the node in one moment */
        me->first_block = b;
        return;
    }

    /* get 1st block that has a higher offset than us */

    prev = me->first_block;

    /* we are at the beginning of the blocks */
    if (offset < prev->offset)
    {
        var_block_t *b;

        b = malloc(sizeof(var_block_t));
        b->len = len;
        b->offset = offset;
        b->next = this = prev;
        prev = b;

        /* set the node in one moment */
        me->first_block = b;
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
            var_block_t *b;

            b = malloc(sizeof(var_block_t));
            b->offset = offset;
            b->len = len;
            b->next = this;
            prev->next = b;
            prev = b;
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
    sparsecounter_t * me,
    const unsigned int offset,
    const unsigned int len
)
{
    var_block_t *this = NULL, *prev;

    if (!me->first_block)
        return;

    /* get 1st block that has a higher offset than us */

    prev = NULL;
    this = me->first_block;

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
                me->first_block = this->next;
                free(this);
                this = me->first_block;
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
            var_block_t *b;

            b = malloc(sizeof(var_block_t));
            b->offset = offset + len;
            b->len = this->len - len - (offset - this->offset);
            b->next = this->next;

            this->len = offset - this->offset;
            this->next = b;
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

int sc_is_complete(const sparsecounter_t * me)
{
    const var_block_t *b;

    b = me->first_block;
    return b && !b->next && b->len == me->max;
}

void sc_get_incomplete(
    const sparsecounter_t * me,
    unsigned int *offset,
    unsigned int *len,
    const unsigned int max
)
{
    const var_block_t *b;

    *offset = *len = 0;
    b = me->first_block;

    if (!b)
    {
        *offset = 0;
        *len = max;
    }
    else
    {
        if (b->offset != 0)
        {
            *offset = 0;

            if (b->next)
            {
                *len = b->next->offset - b->offset;
            }
            else
            {
                *len = b->offset;
            }
        }
        else if (!b->next)
        {
            *offset = b->len;
            *len = max;
        }
        else
        {
            *offset = 0 + b->len;
            *len = b->next->offset - b->len;
        }
    }

    *len = __capmax(*len, max);

//    printf("%d %d %d %d\n", me->max, *len, max, *offset + *len);

    /*  make sure we aren't going over the boundary */
    if (me->max < *offset + *len)
    {
        *len = me->max - *offset;
    }
}

unsigned int sc_get_nbytes_completed(
    const sparsecounter_t * me
)
{
    const var_block_t *b;
    unsigned int nbytes;

    for (b = me->first_block, nbytes=0; b; b = b->next)
    {
        nbytes += b->len;
    }

    return nbytes;
}

int sc_have(
    const sparsecounter_t * me,
    const unsigned int offset,
    const unsigned int len
)
{
    const var_block_t *b;

    for (b = me->first_block; b; b = b->next)
    {
        if (offset < b->offset)
        {

        }
        else if (b->offset <= offset)
        {
            if (offset + len <= b->offset + b->len)
            {
                return 1;
            }
        }
        else
        {
            return 0;
        }
    }

    return 0;
}

void sc_print_contents(const sparsecounter_t * me)
{
    const var_block_t *b;

    for (b = me->first_block; b; b = b->next)
    {
        printf("%d to %d\n", b->offset, b->offset + b->len);
    }
}
