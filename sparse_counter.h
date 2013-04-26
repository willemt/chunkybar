
typedef struct
{
    void *first_block;
    int max;
} sparsecounter_t;

void *sparsecounter_init(const int max);

void sparsecounter_free( void *ra);

int sparsecounter_get_num_blocks( sparsecounter_t * prog);

void sparsecounter_mark_complete( sparsecounter_t * prog, const int offset, const int len);

bool sparsecounter_is_complete( sparsecounter_t * prog);

void sparsecounter_get_incomplete( const sparsecounter_t * prog, int *offset, int *len, const int max);

int sparsecounter_get_nbytes_completed( const sparsecounter_t * prog);

int sparsecounter_have( sparsecounter_t * prog, const int offset, const int len);
