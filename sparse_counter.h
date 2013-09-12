
typedef struct
{
    void *first_block;
    unsigned int max;
} sparsecounter_t;

void *sparsecounter_init(const unsigned int max);

void sparsecounter_free( void *ra);

int sparsecounter_get_num_blocks( sparsecounter_t * prog);

void sparsecounter_mark_complete( sparsecounter_t * prog,
        const unsigned int offset, const unsigned int len);

void sparsecounter_mark_incomplete( sparsecounter_t * prog,
        const unsigned int offset, const unsigned int len);

int sparsecounter_is_complete( sparsecounter_t * prog);

void sparsecounter_get_incomplete( const sparsecounter_t * prog,
        unsigned int *offset, unsigned int *len, const unsigned int max);

unsigned int sparsecounter_get_nbytes_completed( const sparsecounter_t * prog);

int sparsecounter_have( sparsecounter_t * prog,
        const unsigned int offset, const unsigned int len);

void sparsecounter_print_contents(sparsecounter_t * prog);
