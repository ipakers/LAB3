/*************************************************************************
* Copyright 2012 Altera Corporation, San Jose, California, USA.          *
* All rights reserved. All use of this software and documentation is     *
* subject to the License Agreement located at the end of this file below.*
*************************************************************************/
/*
 * The functions in this file perform floating-point operations with software emulation.
 * 
 * Uses #pragma statements to instruct GCC to not use the custom instructions
 * even though the -mcustom-<operation> command-line arguments are specified.
 * 
 * Section SW_SECTION (#define in float2_performance.h) of the performance counter 
 * is used to measure the time for each operation. The PERF_BEGIN and
 * PERF_END statements start and stop the counter for measuring the time.
 */

#include <math.h>
#include "float2_performance.h" 

/* 
 * These pragmas override the -mcustom-<operation> GCC command-line
 * options and force GCC to use the software emulation implementation
 * of floating-point operations instead of the custom instructions.
 */
#pragma GCC target("no-custom-fabss")
#pragma GCC target("no-custom-fnegs")
#pragma GCC target("no-custom-fcmpnes")
#pragma GCC target("no-custom-fcmpeqs")
#pragma GCC target("no-custom-fcmpges")
#pragma GCC target("no-custom-fcmpgts")
#pragma GCC target("no-custom-fcmples")
#pragma GCC target("no-custom-fcmplts")
#pragma GCC target("no-custom-fixsi")
#pragma GCC target("no-custom-floatis")
#pragma GCC target("no-custom-fmuls")
#pragma GCC target("no-custom-fadds")
#pragma GCC target("no-custom-fsubs")
#pragma GCC target("no-custom-fdivs")

INT_INT_INT(overhead, operand_a + operand_b, SW, NUM_SW_ITERATIONS, SW_SECTION);

FLT_FLT_FLT(add, operand_a + operand_b, SW, NUM_SW_ITERATIONS, SW_SECTION);
FLT_FLT_FLT(sub, operand_a - operand_b, SW, NUM_SW_ITERATIONS, SW_SECTION);
FLT_FLT_FLT(mul, operand_a * operand_b, SW, NUM_SW_ITERATIONS, SW_SECTION);
FLT_FLT_FLT(div, operand_a / operand_b, SW, NUM_SW_ITERATIONS, SW_SECTION);
FLT_FLT_FLT(min, fminf(operand_a, operand_b), SW, NUM_SW_ITERATIONS, SW_SECTION);
FLT_FLT_FLT(max, fmaxf(operand_a, operand_b), SW, NUM_SW_ITERATIONS, SW_SECTION);

FLT_FLT(abs, fabsf(operand_a), SW, NUM_SW_ITERATIONS, SW_SECTION);
FLT_FLT(neg, -operand_a, SW, NUM_SW_ITERATIONS, SW_SECTION);
FLT_FLT(sqrt, sqrtf(operand_a), SW, NUM_SW_ITERATIONS, SW_SECTION);

INT_FLT(round, lroundf(operand_a), SW, NUM_SW_ITERATIONS, SW_SECTION);

INT_FLT(float2int, (int)operand_a, SW, NUM_SW_ITERATIONS, SW_SECTION);

FLT_INT(int2float, (float)operand_a, SW, NUM_SW_ITERATIONS, SW_SECTION);

INT_FLT_FLT(cmplt, operand_a < operand_b, SW, NUM_SW_ITERATIONS, SW_SECTION);
INT_FLT_FLT(cmple, operand_a <= operand_b, SW, NUM_SW_ITERATIONS, SW_SECTION);
INT_FLT_FLT(cmpgt, operand_a > operand_b, SW, NUM_SW_ITERATIONS, SW_SECTION);
INT_FLT_FLT(cmpge, operand_a >= operand_b, SW, NUM_SW_ITERATIONS, SW_SECTION);
INT_FLT_FLT(cmpeq, operand_a == operand_b, SW, NUM_SW_ITERATIONS, SW_SECTION);
INT_FLT_FLT(cmpne, operand_a != operand_b, SW, NUM_SW_ITERATIONS, SW_SECTION);
