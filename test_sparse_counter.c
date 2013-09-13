#include <stdbool.h>
#include <assert.h>
#include <setjmp.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "CuTest.h"

#include "sparse_counter.h"

void Testsparsecounter_Init(
    CuTest * tc
)
{
    void *prog;

    prog = sc_init(100);

    CuAssertTrue(tc, prog != NULL);
    sc_free(prog);
}

void Testsparsecounter_IsNotComplete(
    CuTest * tc
)
{
    void *prog;

    prog = sc_init(100);

    CuAssertTrue(tc, !sc_is_complete(prog));
    sc_free(prog);
}

void Testsparsecounter_Mark(
    CuTest * tc
)
{
    void *prog;

    prog = sc_init(100);

    sc_mark_complete(prog, 0, 100);

    CuAssertTrue(tc, sc_is_complete(prog));
    sc_free(prog);
}

void Testsparsecounter_MarkAddsBlocks(
    CuTest * tc
)
{
    void *prog;

    prog = sc_init(100);

    CuAssertTrue(tc, 0 == sc_get_num_blocks(prog));

    sc_mark_complete(prog, 0, 10);

    CuAssertTrue(tc, 1 == sc_get_num_blocks(prog));

    sc_mark_complete(prog, 20, 10);

//    printf("%d\n", sc_get_num_blocks(prog));

    CuAssertTrue(tc, 2 == sc_get_num_blocks(prog));

    sc_mark_complete(prog, 40, 10);

//    printf("%d\n", sc_get_num_blocks(prog));

    CuAssertTrue(tc, 3 == sc_get_num_blocks(prog));
    sc_free(prog);
}

void Testsparsecounter_MarkHalves(
    CuTest * tc
)
{
    void *prog;

    prog = sc_init(100);

    sc_mark_complete(prog, 0, 50);

    CuAssertTrue(tc, !sc_is_complete(prog));

    sc_mark_complete(prog, 50, 50);

    CuAssertTrue(tc, sc_is_complete(prog));
    sc_free(prog);
}

void Testsparsecounter_MergingLeftHappens(
    CuTest * tc
)
{
    void *prog;

    prog = sc_init(100);

    CuAssertTrue(tc, 0 == sc_get_num_blocks(prog));

    sc_mark_complete(prog, 25, 25);

    CuAssertTrue(tc, 1 == sc_get_num_blocks(prog));

    sc_mark_complete(prog, 0, 25);

    CuAssertTrue(tc, 1 == sc_get_num_blocks(prog));
    sc_free(prog);
}

void Testsparsecounter_MergingRightHappens(
    CuTest * tc
)
{
    void *prog;

    prog = sc_init(100);

    CuAssertTrue(tc, 0 == sc_get_num_blocks(prog));

    sc_mark_complete(prog, 0, 25);

    CuAssertTrue(tc, 1 == sc_get_num_blocks(prog));

    sc_mark_complete(prog, 25, 25);

    CuAssertTrue(tc, 1 == sc_get_num_blocks(prog));
    sc_free(prog);
}

void Testsparsecounter_MergingEaten(
    CuTest * tc
)
{
    void *prog;

    prog = sc_init(100);

    CuAssertTrue(tc, 0 == sc_get_num_blocks(prog));

    sc_mark_complete(prog, 0, 40);

    CuAssertTrue(tc, 1 == sc_get_num_blocks(prog));

    sc_mark_complete(prog, 10, 30);

    CuAssertTrue(tc, 1 == sc_get_num_blocks(prog));
    sc_free(prog);
}

void Testsparsecounter_MergingMiddleHappens(
    CuTest * tc
)
{
    void *prog;

    prog = sc_init(100);

    CuAssertTrue(tc, 0 == sc_get_num_blocks(prog));

    sc_mark_complete(prog, 0, 40);

    CuAssertTrue(tc, 1 == sc_get_num_blocks(prog));

    sc_mark_complete(prog, 60, 40);

    CuAssertTrue(tc, 2 == sc_get_num_blocks(prog));

    sc_mark_complete(prog, 40, 20);

    CuAssertTrue(tc, 1 == sc_get_num_blocks(prog));
    sc_free(prog);
}

void Testsparsecounter_MergingHappensAndDoesnt(
    CuTest * tc
)
{
    void *prog;

    prog = sc_init(100);

    CuAssertTrue(tc, 0 == sc_get_num_blocks(prog));

    sc_mark_complete(prog, 0, 25);

    CuAssertTrue(tc, 1 == sc_get_num_blocks(prog));

    sc_mark_complete(prog, 25, 25);

    CuAssertTrue(tc, 1 == sc_get_num_blocks(prog));

    sc_mark_complete(prog, 51, 25);

    CuAssertTrue(tc, 2 == sc_get_num_blocks(prog));
    sc_free(prog);
}

void Testsparsecounter_MarkQuarters(
    CuTest * tc
)
{
    void *prog;

    prog = sc_init(100);

    sc_mark_complete(prog, 0, 25);

    CuAssertTrue(tc, !sc_is_complete(prog));

    sc_mark_complete(prog, 50, 25);

    CuAssertTrue(tc, !sc_is_complete(prog));

    sc_mark_complete(prog, 75, 25);

    CuAssertTrue(tc, !sc_is_complete(prog));

    sc_mark_complete(prog, 25, 25);

    CuAssertTrue(tc, sc_is_complete(prog));
    sc_free(prog);
}

void Testsparsecounter_MarkHugeSwallow(
    CuTest * tc
)
{
    void *prog;

    prog = sc_init(100);

    sc_mark_complete(prog, 0, 9);
    sc_mark_complete(prog, 10, 9);
    sc_mark_complete(prog, 20, 9);
    sc_mark_complete(prog, 30, 9);
    sc_mark_complete(prog, 40, 9);
    sc_mark_complete(prog, 50, 9);
    sc_mark_complete(prog, 5, 95);

    CuAssertTrue(tc, 1 == sc_get_num_blocks(prog));
    CuAssertTrue(tc, sc_is_complete(prog));
    sc_free(prog);
}

void Testsparsecounter_Mark2(
    CuTest * tc
)
{
    void *prog;

    prog = sc_init(100);

    sc_mark_complete(prog, 25, 50);
    sc_mark_complete(prog, 0, 10);
    CuAssertTrue(tc, 2 == sc_get_num_blocks(prog));
    sc_free(prog);
}

void Testsparsecounter_GetIncomplete(
    CuTest * tc
)
{
    void *prog;

    prog = sc_init(100);

    unsigned int len, offset;

    sc_get_incomplete(prog, &offset, &len, 32);

    CuAssertTrue(tc, offset == 0);
    CuAssertTrue(tc, len == 32);
    sc_free(prog);
}

void Testsparsecounter_GetIncomplete2(
    CuTest * tc
)
{
    void *prog;

    prog = sc_init(100);

    unsigned int len, offset;

    sc_mark_complete(prog, 0, 50);

    CuAssertTrue(tc, 1 == sc_get_num_blocks(prog));

    sc_get_incomplete(prog, &offset, &len, 32);

    CuAssertTrue(tc, offset == 50);
    CuAssertTrue(tc, len == 32);
    sc_free(prog);
}

void Testsparsecounter_GetIncomplete3(
    CuTest * tc
)
{
    void *prog;

    prog = sc_init(100);

    unsigned int len, offset;

    sc_mark_complete(prog, 25, 50);

    CuAssertTrue(tc, 1 == sc_get_num_blocks(prog));

    sc_get_incomplete(prog, &offset, &len, 32);

    CuAssertTrue(tc, offset == 0);
    CuAssertTrue(tc, len == 25);
    sc_free(prog);
}

void Testsparsecounter_GetIncomplete4(
    CuTest * tc
)
{
    void *prog;

    prog = sc_init(100);

    unsigned int len, offset;

    sc_mark_complete(prog, 25, 50);
    sc_mark_complete(prog, 0, 10);

    CuAssertTrue(tc, 2 == sc_get_num_blocks(prog));

    sc_get_incomplete(prog, &offset, &len, 32);

    CuAssertTrue(tc, offset == 10);
    CuAssertTrue(tc, len == 15);
    sc_free(prog);
}

void Testsparsecounter_GetNBytesCompleted(
    CuTest * tc
)
{
    void *prog;

    prog = sc_init(100);

    CuAssertTrue(tc, 0 == sc_get_nbytes_completed(prog));

    sc_mark_complete(prog, 25, 50);
    CuAssertTrue(tc, 50 == sc_get_nbytes_completed(prog));

    sc_mark_complete(prog, 0, 10);
    CuAssertTrue(tc, 60 == sc_get_nbytes_completed(prog));
    sc_free(prog);
}

void Testsparsecounter_GetIncompleteWithLenUnderBoundary(
    CuTest * tc
)
{
    void *prog = sc_init(100);

    unsigned int len, offset;

    sc_get_incomplete(prog, &offset, &len, 200);
    CuAssertTrue(tc, 100 == len);
    sc_free(prog);
}

void Testsparsecounter_no_mark_thus_dont_have(
    CuTest * tc
)
{
    void *prog;

    prog = sc_init(100);

    CuAssertTrue(tc, !sc_have(prog, 0, 10));
    sc_free(prog);
}

void Testsparsecounter_mark_thus_do_have(
    CuTest * tc
)
{
    void *prog;

    prog = sc_init(100);

    sc_mark_complete(prog, 0, 10);
    CuAssertTrue(tc, sc_have(prog, 0, 10));
    sc_free(prog);
}

void Testsparsecounter_mark_thus_do_have_inside(
    CuTest * tc
)
{
    void *prog;

    prog = sc_init(100);

    sc_mark_complete(prog, 0, 100);
    CuAssertTrue(tc, sc_have(prog, 0, 10));
    sc_free(prog);
}

void Testsparsecounter_mark_thus_do_have2(
    CuTest * tc
)
{
    void *prog;

    prog = sc_init(100);

    sc_mark_complete(prog, 0, 10);
    sc_mark_complete(prog, 11, 15);
    sc_mark_complete(prog, 26, 10);
    CuAssertTrue(tc, sc_have(prog, 15, 10));
    sc_free(prog);
}

void Testsparsecounter_unmark(
    CuTest * tc
)
{
    void *prog;

    prog = sc_init(100);

    sc_mark_complete(prog, 0, 100);
    CuAssertTrue(tc, sc_is_complete(prog));

    sc_mark_incomplete(prog, 0, 100);
    CuAssertTrue(tc, !sc_is_complete(prog));
    CuAssertTrue(tc, !sc_have(prog,0,100));
    CuAssertTrue(tc, 0 == sc_get_num_blocks(prog));
    sc_free(prog);
}

void Testsparsecounter_unmark2(
    CuTest * tc
)
{
    void *prog;

    prog = sc_init(100);

    sc_mark_complete(prog, 0, 10);
    sc_mark_complete(prog, 10, 10);
    sc_mark_complete(prog, 20, 10);
    sc_mark_complete(prog, 30, 10);
    sc_mark_incomplete(prog, 0, 100);

    CuAssertTrue(tc, !sc_is_complete(prog));
    CuAssertTrue(tc, !sc_have(prog,0,10));
    CuAssertTrue(tc, !sc_have(prog,10,10));
    CuAssertTrue(tc, !sc_have(prog,20,10));
    CuAssertTrue(tc, !sc_have(prog,30,10));
    CuAssertTrue(tc, 0 == sc_get_nbytes_completed(prog));
    CuAssertTrue(tc, 0 == sc_get_num_blocks(prog));

    sc_free(prog);
}

void Testsparsecounter_unmark_swallow_left(
    CuTest * tc
)
{
    void *prog;

    prog = sc_init(100);

    sc_mark_complete(prog, 20, 30); /*  20 to 50 */
    sc_mark_incomplete(prog, 0, 40); /*  0 to 40 */
    CuAssertTrue(tc, !sc_is_complete(prog));
    CuAssertTrue(tc, !sc_have(prog,0,30));
    CuAssertTrue(tc, sc_have(prog,40,10));
    CuAssertTrue(tc, 10 == sc_get_nbytes_completed(prog));
    CuAssertTrue(tc, 1 == sc_get_num_blocks(prog));

    sc_free(prog);
}

void Testsparsecounter_unmark_swallow_right(
    CuTest * tc
)
{
    void *prog;

    prog = sc_init(100);

    sc_mark_complete(prog, 20, 30); /*  20 to 50 */
    sc_mark_incomplete(prog, 30, 30); /*  30 to 60  */
    CuAssertTrue(tc, !sc_is_complete(prog));
    CuAssertTrue(tc, !sc_have(prog,30,30));
    CuAssertTrue(tc, sc_have(prog,20,10));
    CuAssertTrue(tc, 10 == sc_get_nbytes_completed(prog));
    CuAssertTrue(tc, 1 == sc_get_num_blocks(prog));

    sc_free(prog);
}

void Testsparsecounter_unmarkHalves(
    CuTest * tc
)
{
    void *prog;

    prog = sc_init(100);

    sc_mark_complete(prog, 0, 100);
    CuAssertTrue(tc, sc_is_complete(prog));
    sc_mark_incomplete(prog, 25, 50);

    CuAssertTrue(tc, !sc_is_complete(prog));
    CuAssertTrue(tc, sc_have(prog,0,25));
    CuAssertTrue(tc, !sc_have(prog,25,50));
    CuAssertTrue(tc, sc_have(prog,75,25));
    CuAssertTrue(tc, 50 == sc_get_nbytes_completed(prog));
    CuAssertTrue(tc, 2 == sc_get_num_blocks(prog));

    sc_free(prog);
}

void Testsparsecounter_dontUnmarkHalvesAtEdge(
    CuTest * tc
)
{
    void *prog;

    prog = sc_init(100);

    sc_mark_complete(prog, 0, 100);
    CuAssertTrue(tc, sc_is_complete(prog));
    sc_mark_incomplete(prog, 50, 50);

    CuAssertTrue(tc, !sc_is_complete(prog));
    CuAssertTrue(tc, sc_have(prog,0,50));
    CuAssertTrue(tc, !sc_have(prog,50,50));
    CuAssertTrue(tc, 50 == sc_get_nbytes_completed(prog));
    CuAssertTrue(tc, 1 == sc_get_num_blocks(prog));

    sc_free(prog);
}

void Testsparsecounter_dontUnmark3(
    CuTest * tc
)
{
    void *prog;

    prog = sc_init(262144);

    sc_mark_complete(prog, 0, 49152);
    sc_mark_complete(prog, 65536,196608);
    sc_mark_incomplete(prog, 114688, 16384);

    CuAssertTrue(tc, sc_have(prog,0,49152));
    CuAssertTrue(tc, sc_have(prog,65536,49152));
    CuAssertTrue(tc, !sc_have(prog,114688, 16384));
    CuAssertTrue(tc, sc_have(prog,131072,131072));
    CuAssertTrue(tc, 3 == sc_get_num_blocks(prog));

    sc_free(prog);
}
