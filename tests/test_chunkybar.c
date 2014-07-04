#include <stdbool.h>
#include <assert.h>
#include <setjmp.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "CuTest.h"

#include "chunkybar.h"

void Test_chunkybar_new(
    CuTest * tc
)
{
    void *prog;

    prog = chunky_new(100);

    CuAssertTrue(tc, prog != NULL);
    chunky_free(prog);
}

void Test_chunkybar_IsNotComplete(
    CuTest * tc
)
{
    void *prog;

    prog = chunky_new(100);

    CuAssertTrue(tc, !chunky_is_complete(prog));
    chunky_free(prog);
}

void Test_chunkybar_Mark(
    CuTest * tc
)
{
    void *prog;

    prog = chunky_new(100);

    chunky_mark_complete(prog, 0, 100);

    CuAssertTrue(tc, chunky_is_complete(prog));
    chunky_free(prog);
}

void Test_chunkybar_MarkAddchunkyhunks(
    CuTest * tc
)
{
    void *prog;

    prog = chunky_new(100);

    CuAssertTrue(tc, 0 == chunky_get_num_chunks(prog));

    chunky_mark_complete(prog, 0, 10);

    CuAssertTrue(tc, 1 == chunky_get_num_chunks(prog));

    chunky_mark_complete(prog, 20, 10);

    CuAssertTrue(tc, 2 == chunky_get_num_chunks(prog));

    chunky_mark_complete(prog, 40, 10);

    CuAssertTrue(tc, 3 == chunky_get_num_chunks(prog));
    chunky_free(prog);
}

void Test_chunkybar_MarkHalves(
    CuTest * tc
)
{
    void *prog;

    prog = chunky_new(100);

    chunky_mark_complete(prog, 0, 50);

    CuAssertTrue(tc, !chunky_is_complete(prog));

    chunky_mark_complete(prog, 50, 50);

    CuAssertTrue(tc, chunky_is_complete(prog));
    chunky_free(prog);
}

void Test_chunkybar_MergingLeftHappens(
    CuTest * tc
)
{
    void *prog;

    prog = chunky_new(100);

    CuAssertTrue(tc, 0 == chunky_get_num_chunks(prog));

    chunky_mark_complete(prog, 25, 25);

    CuAssertTrue(tc, 1 == chunky_get_num_chunks(prog));

    chunky_mark_complete(prog, 0, 25);

    CuAssertTrue(tc, 1 == chunky_get_num_chunks(prog));
    chunky_free(prog);
}

void Test_chunkybar_MergingRightHappens(
    CuTest * tc
)
{
    void *prog;

    prog = chunky_new(100);

    CuAssertTrue(tc, 0 == chunky_get_num_chunks(prog));

    chunky_mark_complete(prog, 0, 25);

    CuAssertTrue(tc, 1 == chunky_get_num_chunks(prog));

    chunky_mark_complete(prog, 25, 25);

    CuAssertTrue(tc, 1 == chunky_get_num_chunks(prog));
    chunky_free(prog);
}

void Test_chunkybar_MergingEaten(
    CuTest * tc
)
{
    void *prog;

    prog = chunky_new(100);

    CuAssertTrue(tc, 0 == chunky_get_num_chunks(prog));

    chunky_mark_complete(prog, 0, 40);

    CuAssertTrue(tc, 1 == chunky_get_num_chunks(prog));

    chunky_mark_complete(prog, 10, 30);

    CuAssertTrue(tc, 1 == chunky_get_num_chunks(prog));
    chunky_free(prog);
}

void Test_chunkybar_MergingMiddleHappens(
    CuTest * tc
)
{
    void *prog;

    prog = chunky_new(100);

    CuAssertTrue(tc, 0 == chunky_get_num_chunks(prog));

    chunky_mark_complete(prog, 0, 40);

    CuAssertTrue(tc, 1 == chunky_get_num_chunks(prog));

    chunky_mark_complete(prog, 60, 40);

    CuAssertTrue(tc, 2 == chunky_get_num_chunks(prog));

    chunky_mark_complete(prog, 40, 20);

    CuAssertTrue(tc, 1 == chunky_get_num_chunks(prog));
    chunky_free(prog);
}

void Test_chunkybar_MergingHappensAndDoesnt(
    CuTest * tc
)
{
    void *prog;

    prog = chunky_new(100);

    CuAssertTrue(tc, 0 == chunky_get_num_chunks(prog));

    chunky_mark_complete(prog, 0, 25);

    CuAssertTrue(tc, 1 == chunky_get_num_chunks(prog));

    chunky_mark_complete(prog, 25, 25);

    CuAssertTrue(tc, 1 == chunky_get_num_chunks(prog));

    chunky_mark_complete(prog, 51, 25);

    CuAssertTrue(tc, 2 == chunky_get_num_chunks(prog));
    chunky_free(prog);
}

void Test_chunkybar_MarkQuarters(
    CuTest * tc
)
{
    void *prog;

    prog = chunky_new(100);

    chunky_mark_complete(prog, 0, 25);

    CuAssertTrue(tc, !chunky_is_complete(prog));

    chunky_mark_complete(prog, 50, 25);

    CuAssertTrue(tc, !chunky_is_complete(prog));

    chunky_mark_complete(prog, 75, 25);

    CuAssertTrue(tc, !chunky_is_complete(prog));

    chunky_mark_complete(prog, 25, 25);

    CuAssertTrue(tc, chunky_is_complete(prog));
    chunky_free(prog);
}

void Test_chunkybar_MarkHugeSwallow(
    CuTest * tc
)
{
    void *prog;

    prog = chunky_new(100);

    chunky_mark_complete(prog, 0, 9);
    chunky_mark_complete(prog, 10, 9);
    chunky_mark_complete(prog, 20, 9);
    chunky_mark_complete(prog, 30, 9);
    chunky_mark_complete(prog, 40, 9);
    chunky_mark_complete(prog, 50, 9);
    chunky_mark_complete(prog, 5, 95);

    CuAssertTrue(tc, 1 == chunky_get_num_chunks(prog));
    CuAssertTrue(tc, chunky_is_complete(prog));
    chunky_free(prog);
}

void Test_chunkybar_Mark2(
    CuTest * tc
)
{
    void *prog;

    prog = chunky_new(100);

    chunky_mark_complete(prog, 25, 50);
    chunky_mark_complete(prog, 0, 10);
    CuAssertTrue(tc, 2 == chunky_get_num_chunks(prog));
    chunky_free(prog);
}

void Test_chunkybar_GetIncomplete(
    CuTest * tc
)
{
    void *prog;

    prog = chunky_new(100);

    unsigned int len, offset;

    chunky_get_incomplete(prog, &offset, &len, 32);

    CuAssertTrue(tc, offset == 0);
    CuAssertTrue(tc, len == 32);
    chunky_free(prog);
}

void Test_chunkybar_GetIncomplete2(
    CuTest * tc
)
{
    void *prog;

    prog = chunky_new(100);

    unsigned int len, offset;

    chunky_mark_complete(prog, 0, 50);

    CuAssertTrue(tc, 1 == chunky_get_num_chunks(prog));

    chunky_get_incomplete(prog, &offset, &len, 32);

    CuAssertTrue(tc, offset == 50);
    CuAssertTrue(tc, len == 32);
    chunky_free(prog);
}

void Test_chunkybar_GetIncomplete3(
    CuTest * tc
)
{
    void *prog;

    prog = chunky_new(100);

    unsigned int len, offset;

    chunky_mark_complete(prog, 25, 50);

    CuAssertTrue(tc, 1 == chunky_get_num_chunks(prog));

    chunky_get_incomplete(prog, &offset, &len, 32);

    CuAssertTrue(tc, offset == 0);
    CuAssertTrue(tc, len == 25);
    chunky_free(prog);
}

void Test_chunkybar_GetIncomplete4(
    CuTest * tc
)
{
    void *prog;

    prog = chunky_new(100);

    unsigned int len, offset;

    chunky_mark_complete(prog, 25, 50);
    chunky_mark_complete(prog, 0, 10);

    CuAssertTrue(tc, 2 == chunky_get_num_chunks(prog));

    chunky_get_incomplete(prog, &offset, &len, 32);

    CuAssertTrue(tc, offset == 10);
    CuAssertTrue(tc, len == 15);
    chunky_free(prog);
}

void Test_chunkybar_GetNBytechunkyompleted(
    CuTest * tc
)
{
    void *prog;

    prog = chunky_new(100);

    CuAssertTrue(tc, 0 == chunky_get_nbytes_completed(prog));

    chunky_mark_complete(prog, 25, 50);
    CuAssertTrue(tc, 50 == chunky_get_nbytes_completed(prog));

    chunky_mark_complete(prog, 0, 10);
    CuAssertTrue(tc, 60 == chunky_get_nbytes_completed(prog));
    chunky_free(prog);
}

void Test_chunkybar_GetIncompleteWithLenUnderBoundary(
    CuTest * tc
)
{
    void *prog = chunky_new(100);

    unsigned int len, offset;

    chunky_get_incomplete(prog, &offset, &len, 200);
    CuAssertTrue(tc, 100 == len);
    chunky_free(prog);
}

void Test_chunkybar_no_mark_thus_dont_have(
    CuTest * tc
)
{
    void *prog;

    prog = chunky_new(100);

    CuAssertTrue(tc, !chunky_have(prog, 0, 10));
    chunky_free(prog);
}

void Test_chunkybar_mark_thus_do_have(
    CuTest * tc
)
{
    void *prog;

    prog = chunky_new(100);

    chunky_mark_complete(prog, 0, 10);
    CuAssertTrue(tc, chunky_have(prog, 0, 10));
    chunky_free(prog);
}

void Test_chunkybar_mark_thus_do_have_inside(
    CuTest * tc
)
{
    void *prog;

    prog = chunky_new(100);

    chunky_mark_complete(prog, 0, 100);
    CuAssertTrue(tc, chunky_have(prog, 0, 10));
    chunky_free(prog);
}

void Test_chunkybar_mark_thus_do_have2(
    CuTest * tc
)
{
    void *prog;

    prog = chunky_new(100);

    chunky_mark_complete(prog, 0, 10);
    chunky_mark_complete(prog, 11, 15);
    chunky_mark_complete(prog, 26, 10);
    CuAssertTrue(tc, chunky_have(prog, 15, 10));
    chunky_free(prog);
}

void Test_chunkybar_unmark(
    CuTest * tc
)
{
    void *prog;

    prog = chunky_new(100);

    chunky_mark_complete(prog, 0, 100);
    CuAssertTrue(tc, chunky_is_complete(prog));

    chunky_mark_incomplete(prog, 0, 100);
    CuAssertTrue(tc, !chunky_is_complete(prog));
    CuAssertTrue(tc, !chunky_have(prog,0,100));
    CuAssertTrue(tc, 0 == chunky_get_num_chunks(prog));
    chunky_free(prog);
}

void Test_chunkybar_unmark2(
    CuTest * tc
)
{
    void *prog;

    prog = chunky_new(100);

    chunky_mark_complete(prog, 0, 10);
    chunky_mark_complete(prog, 10, 10);
    chunky_mark_complete(prog, 20, 10);
    chunky_mark_complete(prog, 30, 10);
    chunky_mark_incomplete(prog, 0, 100);

    CuAssertTrue(tc, !chunky_is_complete(prog));
    CuAssertTrue(tc, !chunky_have(prog,0,10));
    CuAssertTrue(tc, !chunky_have(prog,10,10));
    CuAssertTrue(tc, !chunky_have(prog,20,10));
    CuAssertTrue(tc, !chunky_have(prog,30,10));
    CuAssertTrue(tc, 0 == chunky_get_nbytes_completed(prog));
    CuAssertTrue(tc, 0 == chunky_get_num_chunks(prog));

    chunky_free(prog);
}

void Test_chunkybar_unmark_swallow_left(
    CuTest * tc
)
{
    void *prog;

    prog = chunky_new(100);

    chunky_mark_complete(prog, 20, 30); /*  20 to 50 */
    chunky_mark_incomplete(prog, 0, 40); /*  0 to 40 */
    CuAssertTrue(tc, !chunky_is_complete(prog));
    CuAssertTrue(tc, !chunky_have(prog,0,30));
    CuAssertTrue(tc, chunky_have(prog,40,10));
    CuAssertTrue(tc, 10 == chunky_get_nbytes_completed(prog));
    CuAssertTrue(tc, 1 == chunky_get_num_chunks(prog));

    chunky_free(prog);
}

void Test_chunkybar_unmark_swallow_right(
    CuTest * tc
)
{
    void *prog;

    prog = chunky_new(100);

    chunky_mark_complete(prog, 20, 30); /*  20 to 50 */
    chunky_mark_incomplete(prog, 30, 30); /*  30 to 60  */
    CuAssertTrue(tc, !chunky_is_complete(prog));
    CuAssertTrue(tc, !chunky_have(prog,30,30));
    CuAssertTrue(tc, chunky_have(prog,20,10));
    CuAssertTrue(tc, 10 == chunky_get_nbytes_completed(prog));
    CuAssertTrue(tc, 1 == chunky_get_num_chunks(prog));

    chunky_free(prog);
}

void Test_chunkybar_unmarkHalves(
    CuTest * tc
)
{
    void *prog;

    prog = chunky_new(100);

    chunky_mark_complete(prog, 0, 100);
    CuAssertTrue(tc, chunky_is_complete(prog));
    chunky_mark_incomplete(prog, 25, 50);

    CuAssertTrue(tc, !chunky_is_complete(prog));
    CuAssertTrue(tc, chunky_have(prog,0,25));
    CuAssertTrue(tc, !chunky_have(prog,25,50));
    CuAssertTrue(tc, chunky_have(prog,75,25));
    CuAssertTrue(tc, 50 == chunky_get_nbytes_completed(prog));
    CuAssertTrue(tc, 2 == chunky_get_num_chunks(prog));

    chunky_free(prog);
}

void Test_chunkybar_dontUnmarkHalvesAtEdge(
    CuTest * tc
)
{
    void *prog;

    prog = chunky_new(100);

    chunky_mark_complete(prog, 0, 100);
    CuAssertTrue(tc, chunky_is_complete(prog));
    chunky_mark_incomplete(prog, 50, 50);

    CuAssertTrue(tc, !chunky_is_complete(prog));
    CuAssertTrue(tc, chunky_have(prog,0,50));
    CuAssertTrue(tc, !chunky_have(prog,50,50));
    CuAssertTrue(tc, 50 == chunky_get_nbytes_completed(prog));
    CuAssertTrue(tc, 1 == chunky_get_num_chunks(prog));

    chunky_free(prog);
}

void Test_chunkybar_dontUnmark3(
    CuTest * tc
)
{
    void *prog;

    prog = chunky_new(262144);

    chunky_mark_complete(prog, 0, 49152);
    chunky_mark_complete(prog, 65536,196608);
    chunky_mark_incomplete(prog, 114688, 16384);

    CuAssertTrue(tc, chunky_have(prog,0,49152));
    CuAssertTrue(tc, chunky_have(prog,65536,49152));
    CuAssertTrue(tc, !chunky_have(prog,114688, 16384));
    CuAssertTrue(tc, chunky_have(prog,131072,131072));
    CuAssertTrue(tc, 3 == chunky_get_num_chunks(prog));

    chunky_free(prog);
}
