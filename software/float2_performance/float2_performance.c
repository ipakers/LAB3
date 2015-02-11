/*************************************************************************
* Copyright 2012 Altera Corporation, San Jose, California, USA.          *
* All rights reserved. All use of this software and documentation is     *
* subject to the License Agreement located at the end of this file below.*
*************************************************************************/
/******************************************************************************
 *
 * Description
 * ************* 
 * This program compares the performance of floating-point operations
 * using the Floating Point Hardware 2 component vs. software emulation.
 * 
 * After each operation is performed, a table is printed showing the
 * performance of the hardware and software emulation.
 *
 * This code is designed to be compiled with GCC optimization level -O2.
 * 
 * System Requirements
 * *******************
 * The following components must be connected to the Nios II:
 *   - Floating Point Hardware 2 component
 *   - Performance counter named "performance_counter_0"
 *       - Must have at least 2 sections
 *       - Must be connected to the same clock as the Nios II
 *   - Some character device (e.g. JTAG UART or serial UART)
 *
 * Software Files
 * ****************
 * float2_performance.c    ==> This file
 * float2_performance_sw.c ==> Operations using software emulation
 * float2_performance_hw.c ==> Operations using hardware
 * float2_performance.h    ==> #defines and function prototypes
 */

#include <stdio.h> 
#include <stdlib.h>
#include <math.h>
#include <float.h>
#include <sys/alt_irq.h>
#include "float2_performance.h"

/* 
 * Number of times each floating-point operation is executed. 
 * These are selected to enable running on RTL simulation in
 * a reasonable period of time.
 * Perform more iterations for hardware because they run much faster.
 *
 * If you want higher accuracy of measurement, you can either
 * increase the number of iterations (and potentially run on
 * a board instead of RTL simulation if it takes too long) or
 * configure your Nios II to use tightly-coupled memories instead
 * of caches.
 */
#define NUM_HW_ITERATIONS 100
#define NUM_SW_ITERATIONS 10

/* 
 * If hardware cycles/operation is in error by this amount of a clock cycle,
 * the test fails.  If you have higher accuracy (see comment above
 * for number of iterations), you can reduce this value.
 */
#define MAX_ALLOWED_CYCLE_DIFF  0.25f

/* Expected number of cycles to execute hardware operations. */
#define add_HW_EXPECTED_CYCLES_PER_OPERATION          5
#define sub_HW_EXPECTED_CYCLES_PER_OPERATION          5
#define mul_HW_EXPECTED_CYCLES_PER_OPERATION          4
#define div_HW_EXPECTED_CYCLES_PER_OPERATION          16
#define min_HW_EXPECTED_CYCLES_PER_OPERATION          1
#define max_HW_EXPECTED_CYCLES_PER_OPERATION          1
#define abs_HW_EXPECTED_CYCLES_PER_OPERATION          1
#define neg_HW_EXPECTED_CYCLES_PER_OPERATION          1
#define sqrt_HW_EXPECTED_CYCLES_PER_OPERATION         8
#define round_HW_EXPECTED_CYCLES_PER_OPERATION        2
#define float2int_HW_EXPECTED_CYCLES_PER_OPERATION    2
#define int2float_HW_EXPECTED_CYCLES_PER_OPERATION    4
#define cmplt_HW_EXPECTED_CYCLES_PER_OPERATION        1
#define cmple_HW_EXPECTED_CYCLES_PER_OPERATION        1
#define cmpgt_HW_EXPECTED_CYCLES_PER_OPERATION        1
#define cmpge_HW_EXPECTED_CYCLES_PER_OPERATION        1
#define cmpeq_HW_EXPECTED_CYCLES_PER_OPERATION        1
#define cmpne_HW_EXPECTED_CYCLES_PER_OPERATION        1

/*
 * Macros that generate code to call measurement functions.
 */

/* float output, 2 float inputs */
#define CALL_FLT_FLT_FLT(OPERATION, FMT)                \
  pre();                                                \
  for (i = 0; i < NUM_HW_ITERATIONS; i++)               \
    flt_result += fp_##OPERATION##_HW(flt_a, flt_b);    \
  for (i = 0; i < NUM_SW_ITERATIONS; i++)               \
    flt_result += fp_##OPERATION##_SW(flt_a, flt_b);    \
  alt_irq_enable_all(context);                          \
  PERF_STOP_MEASURING(PERFORMANCE_COUNTER_0_BASE);      \
  fail |= report(hw_overhead, sw_overhead, #OPERATION,  \
    (float)OPERATION##_HW_EXPECTED_CYCLES_PER_OPERATION);

/* float output, 1 float input */
#define CALL_FLT_FLT(OPERATION, FMT)                    \
  pre();                                                \
  for (i = 0; i < NUM_HW_ITERATIONS; i++)               \
    flt_result += fp_##OPERATION##_HW(flt_a);           \
  for (i = 0; i < NUM_SW_ITERATIONS; i++)               \
    flt_result += fp_##OPERATION##_SW(flt_a);           \
  alt_irq_enable_all(context);                          \
  PERF_STOP_MEASURING(PERFORMANCE_COUNTER_0_BASE);      \
  fail |= report(hw_overhead, sw_overhead, #OPERATION,  \
    (float)OPERATION##_HW_EXPECTED_CYCLES_PER_OPERATION);

/* int output, 1 float input */
#define CALL_INT_FLT(OPERATION, FMT)                    \
  pre();                                                \
  for (i = 0; i < NUM_HW_ITERATIONS; i++)               \
    int_result += fp_##OPERATION##_HW(flt_a);           \
  for (i = 0; i < NUM_SW_ITERATIONS; i++)               \
    int_result += fp_##OPERATION##_SW(flt_a);           \
  alt_irq_enable_all(context);                          \
  PERF_STOP_MEASURING(PERFORMANCE_COUNTER_0_BASE);      \
  fail |= report(hw_overhead, sw_overhead, #OPERATION,  \
    (float)OPERATION##_HW_EXPECTED_CYCLES_PER_OPERATION);

/* float output, 1 int input */
#define CALL_FLT_INT(OPERATION, FMT)                    \
  pre();                                                \
  for (i = 0; i < NUM_HW_ITERATIONS; i++)               \
    flt_result += fp_##OPERATION##_HW(int_a);           \
  for (i = 0; i < NUM_SW_ITERATIONS; i++)               \
    flt_result += fp_##OPERATION##_SW(int_a);           \
  alt_irq_enable_all(context);                          \
  PERF_STOP_MEASURING(PERFORMANCE_COUNTER_0_BASE);      \
  fail |= report(hw_overhead, sw_overhead, #OPERATION,  \
    (float)OPERATION##_HW_EXPECTED_CYCLES_PER_OPERATION);

/* int output, 2 float inputs */
#define CALL_INT_FLT_FLT(OPERATION, FMT)                \
  pre();                                                \
  for (i = 0; i < NUM_HW_ITERATIONS; i++)               \
    int_result += fp_##OPERATION##_HW(flt_a, flt_b);    \
  for (i = 0; i < NUM_SW_ITERATIONS; i++)               \
    int_result += fp_##OPERATION##_SW(flt_a, flt_b);    \
  alt_irq_enable_all(context);                          \
  PERF_STOP_MEASURING(PERFORMANCE_COUNTER_0_BASE);      \
  fail |= report(hw_overhead, sw_overhead, #OPERATION,  \
    (float)OPERATION##_HW_EXPECTED_CYCLES_PER_OPERATION);

/*
 * Local prototypes
 */
static void pre(void);
static int report(int hw_overhead, int sw_overhead, 
  const char* operation, float hw_expected_cycles_per_operation);

/* 
 * Globals
 */

static alt_irq_context context;  /* Use when disabling interrupts. */

static const char separator[]     = 
  "+-----------+------+--------+---------+------------+\n";
static const char column_header[] = 
  "| Operation | Type | Cycles | Speedup | Iterations |\n";

int 
main(void)
{
  /* Overhead of measurement in cycles */
  int hw_overhead = 0;
  int sw_overhead = 0;

  /* Input operands. Values don't affect hardware performance (but might affect softwrare performance). */
  const float flt_a = 3.14f;
  const float flt_b = -10.0f;
  const int int_a = 10000;
  
  /* Results. Just recorded to prevent compiler optimizing out code. */
  float flt_result = 0.0f;
  int int_result = 0;

  /* Loop counter */
  int i;

  /* Set to non-zero if failure detected. */
  int fail = 0;

  /************************************************
   * Perf counter latency - Measuring the overhead
   ************************************************/ 
  pre(); 
  for (i = 0; i < NUM_HW_ITERATIONS; i++) {
    int_result += fp_overhead_HW(int_a, int_a);
  }
  for (i = 0; i < NUM_SW_ITERATIONS; i++) {
    int_result += fp_overhead_SW(int_a, int_a);
  }
  alt_irq_enable_all(context);  
  PERF_STOP_MEASURING (PERFORMANCE_COUNTER_0_BASE);
  
  /* 
   * The overhead is measured by timing how long it takes to time a instruction that executes in
   * one cycle (integer add). This measures all the overheads including the performance counter,
   * function call, and loop overhead.
   * Need to subtract out 1 cycle times the number of iterations to remove the cycles it takes
   * to execute the integer add.  What is left is a measurement of the number of cycles it takes
   * to execute the operation. 
   */
  hw_overhead = 
     (int)perf_get_section_time ((void*)PERFORMANCE_COUNTER_0_BASE, HW_SECTION) -
     1 * NUM_HW_ITERATIONS;    /* cycles to execute integer add */
  sw_overhead = 
     (int)perf_get_section_time ((void*)PERFORMANCE_COUNTER_0_BASE, SW_SECTION) -
     1 * NUM_SW_ITERATIONS;    /* cycles to execute integer add */

  /* 
   * Use macros to create code to measure performance. 
   */
  CALL_FLT_FLT_FLT(add, "%f + %f");
  CALL_FLT_FLT_FLT(sub, "%f - %f");
  CALL_FLT_FLT_FLT(mul, "%f * %f");
  CALL_FLT_FLT_FLT(div, "%f / %f");
  CALL_FLT_FLT_FLT(min, "min(%f, %f)");
  CALL_FLT_FLT_FLT(max, "max(%f, %f)");
  CALL_FLT_FLT(abs, "abs(%f)");
  CALL_FLT_FLT(neg, "-%f");
  CALL_FLT_FLT(sqrt, "sqrt(%f)");
  CALL_INT_FLT(round, "round(%f)");
  CALL_INT_FLT(float2int, "int(%f)");
  CALL_FLT_INT(int2float, "float(%d)");
  CALL_INT_FLT_FLT(cmplt, "%f < %f");
  CALL_INT_FLT_FLT(cmple, "%f <= %f");
  CALL_INT_FLT_FLT(cmpgt, "%f > %f");
  CALL_INT_FLT_FLT(cmpge, "%f >= %f");
  CALL_INT_FLT_FLT(cmpeq, "%f == %f");
  CALL_INT_FLT_FLT(cmpne, "%f != %f");

  if (fail) {
     printf("\nTEST: FAIL\n");
  } else {
     printf("\nTEST: PASS\n");
  }

  /* Return results just to prevent compiler from optimizing out code. */
  return int_result + (int)flt_result;
}

/* Called before measuring performance of an operation. */
static void
pre(void)
{
  PERF_RESET (PERFORMANCE_COUNTER_0_BASE);
  context = alt_irq_disable_all();
  PERF_START_MEASURING (PERFORMANCE_COUNTER_0_BASE);
}

/* Obtains cycle count from performance counter peripheral and returns cycles/operation. */
static float
calc_cycles_per_operation(int section_num, int overhead, float num_iterations_float)
{
  const int cycles_with_overhead = 
    (int)perf_get_section_time ((void*)PERFORMANCE_COUNTER_0_BASE, section_num);
  const int cycles_without_overhead = cycles_with_overhead - overhead;

  return (float)cycles_without_overhead / num_iterations_float;
}

/* Displays results for one performance counter section. */
static void
report_section(
   const char* operation,
   const char* fptype,
   int num_iterations_int,
   float cycles_per_operation,
   float ref_cycles_per_operation)
{
  const float speedup = ref_cycles_per_operation / cycles_per_operation;

  printf ("| %-9s | %-4s | %6.2f | %6.1fX | %10d |\n",
          operation,
          fptype,
          cycles_per_operation,
          speedup,
          num_iterations_int);
  printf ("%s", separator);
}

/* 
 * Displays results for all performance counter sections.
 * Also checks actual cycles/operation against expected cycles/operation
 * and returns non-zero if difference exceeds maximum.
 */
static int 
report(int hw_overhead, int sw_overhead, const char* operation,
  float hw_expected_cycles_per_operation)
{
  const float hw_cycles_per_operation = 
    calc_cycles_per_operation(HW_SECTION, hw_overhead, (float)NUM_HW_ITERATIONS);
  const float sw_cycles_per_operation = 
    calc_cycles_per_operation(SW_SECTION, sw_overhead, (float)NUM_SW_ITERATIONS);
  const float diff_cycles_per_operation = 
    fabsf(hw_expected_cycles_per_operation - hw_cycles_per_operation);
  
  printf ("\n");
  printf ("%s%s%s", separator, column_header, separator);

  report_section(
   operation,
   "HW",
   NUM_HW_ITERATIONS,
   hw_cycles_per_operation,
   sw_cycles_per_operation);

  report_section(
   operation,
   "SW",
   NUM_SW_ITERATIONS,
   sw_cycles_per_operation,
   sw_cycles_per_operation);

   if (diff_cycles_per_operation > MAX_ALLOWED_CYCLE_DIFF) {
      printf("ERROR: %s: cycles/operation actual=%.2f expected=%.0f diff=%.2f (max allowed is %.2f)\n",
        operation, 
        hw_cycles_per_operation,
        hw_expected_cycles_per_operation,
        diff_cycles_per_operation,
        MAX_ALLOWED_CYCLE_DIFF);

      return 1;
   }

   return 0; /* success */
}
