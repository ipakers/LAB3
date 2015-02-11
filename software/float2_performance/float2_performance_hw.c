/*************************************************************************
* Copyright 2012 Altera Corporation, San Jose, California, USA.          *
* All rights reserved. All use of this software and documentation is     *
* subject to the License Agreement located at the end of this file below.*
*************************************************************************/
/*
 * The functions in this file perform floating-point using hardware.
 * 
 * Section SECTION (#define in float2_performance.h) of the performance counter 
 * is used to measure the time for each operation.  The PERF_BEGIN and
 * PERF_END statements start and stop the counter for measuring the time.
 */

#include <math.h>
#include "float2_performance.h"
#include "altera_nios_custom_instr_floating_point_2.h" /* For fminf()/fmaxf() macros */

INT_INT_INT(overhead, operand_a + operand_b, HW, NUM_HW_ITERATIONS, HW_SECTION);

FLT_FLT_FLT(add, operand_a + operand_b, HW, NUM_HW_ITERATIONS, HW_SECTION);
FLT_FLT_FLT(sub, operand_a - operand_b, HW, NUM_HW_ITERATIONS, HW_SECTION);
FLT_FLT_FLT(mul, operand_a * operand_b, HW, NUM_HW_ITERATIONS, HW_SECTION);
FLT_FLT_FLT(div, operand_a / operand_b, HW, NUM_HW_ITERATIONS, HW_SECTION);
FLT_FLT_FLT(min, fminf(operand_a, operand_b), HW, NUM_HW_ITERATIONS, HW_SECTION);
FLT_FLT_FLT(max, fmaxf(operand_a, operand_b), HW, NUM_HW_ITERATIONS, HW_SECTION);

FLT_FLT(abs, fabsf(operand_a), HW, NUM_HW_ITERATIONS, HW_SECTION);
FLT_FLT(neg, -operand_a, HW, NUM_HW_ITERATIONS, HW_SECTION);
FLT_FLT(sqrt, sqrtf(operand_a), HW, NUM_HW_ITERATIONS, HW_SECTION);

INT_FLT(round, lroundf(operand_a), HW, NUM_HW_ITERATIONS, HW_SECTION);
INT_FLT(float2int, (int)operand_a, HW, NUM_HW_ITERATIONS, HW_SECTION);

FLT_INT(int2float, (float)operand_a, HW, NUM_HW_ITERATIONS, HW_SECTION);

INT_FLT_FLT(cmplt, operand_a < operand_b, HW, NUM_HW_ITERATIONS, HW_SECTION);
INT_FLT_FLT(cmple, operand_a <= operand_b, HW, NUM_HW_ITERATIONS, HW_SECTION);
INT_FLT_FLT(cmpgt, operand_a > operand_b, HW, NUM_HW_ITERATIONS, HW_SECTION);
INT_FLT_FLT(cmpge, operand_a >= operand_b, HW, NUM_HW_ITERATIONS, HW_SECTION);
INT_FLT_FLT(cmpeq, operand_a == operand_b, HW, NUM_HW_ITERATIONS, HW_SECTION);
INT_FLT_FLT(cmpne, operand_a != operand_b, HW, NUM_HW_ITERATIONS, HW_SECTION);
