/*************************************************************************
 * Copyright 2012 Altera Corporation, San Jose, California, USA.          *
 * All rights reserved. All use of this software and documentation is     *
 * subject to the License Agreement located at the end of this file below.*
 *************************************************************************/
/******************************************************************************
 *
 * Description
 * ************* 
 * This program provides a simple sanity test of the 
 * Floating Point Hardware 2 component functionality.
 * This program runs quickly so can run on RTL simulation or a board.
 * The program is self-checking and displays a pass/fail message.
 *
 * System Requirements
 * *******************
 * The following peripherals must be connected to the Nios II:
 *   - Some character device (e.g. JTAG UART or serial UART)
 */

#include <stdio.h>
#include <math.h>
#include <limits.h>
#include "altera_nios_custom_instr_floating_point_2.h"

static int check_flt_one_legal_result(const char* expr, 
  float expected, float actual);
static int check_flt_two_legal_results(const char* expr, 
  float expected1, float expected2, float actual);
static int check_int(const char* expr, int expected, int actual);

/*
 * These macros use the stringify feature of the C preprocessor (#Expr)
 * to convert the expression into a string. This just saves typing.
 */
#define CHECK_FLT1(Expected, Expr) \
    fail |= check_flt_one_legal_result(#Expr, (Expected), (Expr))
#define CHECK_FLT2(Expected1, Expected2, Expr) \
    fail |= check_flt_two_legal_results(#Expr, (Expected1), (Expected2), (Expr))
#define CHECK_INT(Expected, Expr) \
    fail |= check_int(#Expr, (Expected), (Expr))

int 
main(void)
{
    /* Set to non-zero if failure detected. */
    int fail = 0;

    /* These are volatile to prevent GCC from optimizing out code. */
    volatile float pos = 1.5f;
    volatile float bigger_pos = 4.8f;
    volatile float neg = -1.25f;
    volatile float smaller_neg = -1.75e20f;
    volatile int integer = 1234;

    printf("Common Input Operands\n");
    printf("  pos=%g (0x%08x)\n", pos, *(unsigned int*)&pos);
    printf("  bigger_pos=%g (0x%08x)\n", bigger_pos, *(unsigned int*)&bigger_pos);
    printf("  neg=%g (0x%08x)\n", neg, *(unsigned int*)&neg);
    printf("  smaller_neg=%g (0x%08x)\n", smaller_neg, *(unsigned int*)&smaller_neg);
    printf("  integer=%d\n", integer);
    printf("\n");

    /* absolute */
    CHECK_FLT1(1.25f, fabsf(neg));

    /* negate */
    CHECK_FLT1(-1.5f, -pos);
    CHECK_FLT1(1.25f, -neg);

    /* equal-to */
    CHECK_INT(1, pos == pos);
    CHECK_INT(1, neg == neg);
    CHECK_INT(0, bigger_pos == pos);

    /* not-equal-to */
    CHECK_INT(0, pos != pos);
    CHECK_INT(1, pos != neg);
    CHECK_INT(1, bigger_pos != pos);

    /* less-than */
    CHECK_INT(1, pos < bigger_pos);
    CHECK_INT(0, bigger_pos < pos);
    CHECK_INT(1, neg < pos);

    /* less-than-or-equal-to */
    CHECK_INT(1, pos <= bigger_pos);
    CHECK_INT(0, bigger_pos <= pos);
    CHECK_INT(1, pos <= pos);

    /* greater-than */
    CHECK_INT(0, pos > bigger_pos);
    CHECK_INT(1, bigger_pos > pos);
    CHECK_INT(0, neg > pos);

    /* greater-than-or-equal-to */
    CHECK_INT(0, pos >= bigger_pos);
    CHECK_INT(1, bigger_pos >= pos);
    CHECK_INT(1, neg >= neg);

    /* minimum */
    CHECK_FLT1(pos, fminf(pos, bigger_pos));
    CHECK_FLT1(pos, fminf(bigger_pos, pos));
    CHECK_FLT1(neg, fminf(neg, pos));
    CHECK_FLT1(smaller_neg, fminf(neg, smaller_neg));

    /* maximum */
    CHECK_FLT1(bigger_pos, fmaxf(pos, bigger_pos));
    CHECK_FLT1(bigger_pos, fmaxf(bigger_pos, pos));
    CHECK_FLT1(pos, fmaxf(neg, pos));
    CHECK_FLT1(neg, fmaxf(neg, smaller_neg));

    /* float2int */
    CHECK_INT(1, (int)pos);
    CHECK_INT(4, (int)bigger_pos);
    CHECK_INT(-1, (int)neg);
    CHECK_INT(INT_MIN, (int)smaller_neg);

    /* round */
    CHECK_INT(2, lroundf(pos));
    CHECK_INT(4, lroundf(4.4f));
    CHECK_INT(5, lroundf(bigger_pos));
    CHECK_INT(-1, lroundf(neg));
    CHECK_INT(-2, lroundf(-1.5f));
    CHECK_INT(INT_MIN, lroundf(smaller_neg));

    /* int2float */
    CHECK_FLT1(1234.0f, (float)integer);

    /* square root - faithful rounding, so 2 legal results */
    const float expected_sqrt_1 = 1.22474493f; // 0x3F9C_C471
    const float expected_sqrt_2 = 1.22474480f; // 0x3F9C_C470
    CHECK_FLT2(expected_sqrt_1, expected_sqrt_2, sqrtf(pos));

    /* multiply - faithful rounding, so 2 legal results */
    const float expected_mult_1 = 7.20000029f; // 0x40E6_6667
    const float expected_mult_2 = 7.19999980f; // 0x40E6_6666
    CHECK_FLT2(expected_mult_1, expected_mult_2, pos * bigger_pos);

    /* add - faithful rounding, so 2 legal results */
    const float expected_add_1 = 6.30000019f; // 0x40C9_999A
    const float expected_add_2 = 6.30000067f; // 0x40C9_999B
    CHECK_FLT2(expected_add_1, expected_add_2, pos + bigger_pos);

    /* subtract - faithful rounding, so 2 legal results */
    const float expected_sub_1 =-3.30000019f; // 0xC053_3334
    const float expected_sub_2 =-3.30000043f; // 0xC053_3335
    CHECK_FLT2(expected_sub_1, expected_sub_2, pos - bigger_pos);

    /* divide - nearest rounding, so 1 legal result */
    const float expected_div = 0.31250000f;
    CHECK_FLT1(expected_div, pos / bigger_pos);

    if (fail) {
        printf("\nTEST: FAIL\n");
    } else {
        printf("\nTEST: PASS\n");
    }

    return 0; /* Return value is ignored. */
}

/* 
 * Tells GCC to use the software emulation for equality comparison
 * for all code located after this line in the source file.
 * This is done so that the equality check in the comparison
 * doesn't use the Floating Point Hardware 2 component, 
 * because we are supposed to be test that.
 */
#pragma no_custom_fcmpeqs

/* Return zero if actual == expected, non-zero otherwise. */
static int
check_flt_one_legal_result(const char* expr, float expected, float actual)
{
    if (actual == expected) {
        printf("pass: %s\n", expr);
        return 0;
    }

    printf("fail: %s, expected=%g (0x%08x), actual=%g (0x%08x)\n",
      expr,
      expected,
      *(unsigned int*)&expected,
      actual,
      *(unsigned int*)&actual
    );
   
    return 1;
}

/* Return zero if actual == expected1 or expected2, non-zero otherwise. */
static int
check_flt_two_legal_results(const char* expr, float expected1, float expected2, float actual)
{
    if ((actual == expected1) || (actual == expected2)) {
        printf("pass: %s\n", expr);
        return 0;
    }

    printf("fail: %s, expected=%g (0x%08x) or %g (0x%08x), actual=%g (0x%08x)\n",
      expr,
      expected1,
      *(unsigned int*)&expected1,
      expected2,
      *(unsigned int*)&expected2,
      actual,
      *(unsigned int*)&actual
    );

    return 1;
}

/* Return zero if actual == expected, non-zero otherwise. */
static int
check_int(const char* expr, int expected, int actual)
{
    if (expected == actual) {
        printf("pass: %s\n", expr);
        return 0;
    }

    printf("fail: %s, expected=%d, actual=%d\n",
      expr,
      expected,
      actual
    );

    return 1;
}
