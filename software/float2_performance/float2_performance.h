#ifndef __FLOAT2_PERFORMANCE_H__
#define __FLOAT2_PERFORMANCE_H__

#include "system.h"
#include "altera_avalon_performance_counter.h"

/* 
 * Performance counter sections start at 1, not 0. 
 */
#define HW_SECTION 1  /* section for HW operations */
#define SW_SECTION 2  /* section for SW operations */

/*
 * Measurement function prototypes for HW.
 */
int fp_overhead_HW(int, int);
float fp_add_HW(float, float);
float fp_sub_HW(float, float);
float fp_mul_HW(float, float);
float fp_div_HW(float, float);
float fp_sqrt_HW(float);
float fp_min_HW(float, float);
float fp_max_HW(float, float);
float fp_abs_HW(float);
float fp_neg_HW(float);
int fp_round_HW(float);
int fp_float2int_HW(float);
float fp_int2float_HW(int);
int fp_cmplt_HW(float, float);
int fp_cmple_HW(float, float);
int fp_cmpgt_HW(float, float);
int fp_cmpge_HW(float, float);
int fp_cmpeq_HW(float, float);
int fp_cmpne_HW(float, float);

/*
 * Measurement function prototypes for SW.
 */
int fp_overhead_SW(int, int);
float fp_div_SW(float, float);
float fp_sub_SW(float, float);
float fp_add_SW(float, float);
float fp_mul_SW(float, float);
float fp_sqrt_SW(float);
float fp_min_SW(float, float);
float fp_max_SW(float, float);
float fp_abs_SW(float);
float fp_neg_SW(float);
int fp_round_SW(float);
int fp_float2int_SW(float);
float fp_int2float_SW(int);
int fp_cmplt_SW(float, float);
int fp_cmple_SW(float, float);
int fp_cmpgt_SW(float, float);
int fp_cmpge_SW(float, float);
int fp_cmpeq_SW(float, float);
int fp_cmpne_SW(float, float);

/*
 * Macros that generate code to measure performance of operators.
 * These are designed to be compiled with GCC optimizatin level -O2.
 * The volatile "result" variable is present to prevent GCC
 * from performing unwanted optimizations that introduce measurement error.
 */

/* int output, 2 int inputs */
#define INT_INT_INT(OPERATION, CODE, OPTYPE, NUM_ITERATIONS, SECTION) \
int                                                     \
fp_##OPERATION##_##OPTYPE(                              \
  int operand_a,                                        \
  int operand_b)                                        \
{                                                       \
  volatile int result;                                  \
  register int r;                                       \
                                                        \
  PERF_BEGIN (PERFORMANCE_COUNTER_0_BASE, SECTION);     \
  r = CODE;                                             \
  PERF_END (PERFORMANCE_COUNTER_0_BASE, SECTION);       \
  result = r;                                           \
                                                        \
  return result;                                        \
}

/* float output, 2 float inputs */
#define FLT_FLT_FLT(OPERATION, CODE, OPTYPE, NUM_ITERATIONS, SECTION) \
float                                                   \
fp_##OPERATION##_##OPTYPE(                              \
  float operand_a,                                      \
  float operand_b)                                      \
{                                                       \
  volatile float result;                                \
  register float r;                                     \
                                                        \
  PERF_BEGIN (PERFORMANCE_COUNTER_0_BASE, SECTION);     \
  r = CODE;                                             \
  PERF_END (PERFORMANCE_COUNTER_0_BASE, SECTION);       \
  result = r;                                           \
                                                        \
  return result;                                        \
}

/* float output, 1 float input */
#define FLT_FLT(OPERATION, CODE, OPTYPE, NUM_ITERATIONS, SECTION) \
float                                                   \
fp_##OPERATION##_##OPTYPE(                              \
  float operand_a)                                      \
{                                                       \
  volatile float result;                                \
  register float r;                                     \
                                                        \
  PERF_BEGIN (PERFORMANCE_COUNTER_0_BASE, SECTION);     \
  r = CODE;                                      \
  PERF_END (PERFORMANCE_COUNTER_0_BASE, SECTION);       \
  result = r;                                           \
                                                        \
  return result;                                        \
}

/* float output, 1 int input */
#define FLT_INT(OPERATION, CODE, OPTYPE, NUM_ITERATIONS, SECTION) \
float                                                   \
fp_##OPERATION##_##OPTYPE(                              \
  int operand_a)                                        \
{                                                       \
  volatile float result;                                \
  register float r;                                     \
                                                        \
  PERF_BEGIN (PERFORMANCE_COUNTER_0_BASE, SECTION);     \
  r = CODE;                                      \
  PERF_END (PERFORMANCE_COUNTER_0_BASE, SECTION);       \
  result = r;                                           \
                                                        \
  return result;                                        \
}

/* int output, 1 float input */
#define INT_FLT(OPERATION, CODE, OPTYPE, NUM_ITERATIONS, SECTION) \
int                                                     \
fp_##OPERATION##_##OPTYPE(                              \
  float operand_a)                                      \
{                                                       \
  volatile int result;                                  \
  register int r;                                       \
                                                        \
  PERF_BEGIN (PERFORMANCE_COUNTER_0_BASE, SECTION);     \
  r = CODE;                                      \
  PERF_END (PERFORMANCE_COUNTER_0_BASE, SECTION);       \
  result = r;                                           \
                                                        \
  return result;                                        \
}

/* int output, 2 float inputs */
#define INT_FLT_FLT(OPERATION, CODE, OPTYPE, NUM_ITERATIONS, SECTION) \
int                                                     \
fp_##OPERATION##_##OPTYPE(                              \
  float operand_a,                                      \
  float operand_b)                                      \
{                                                       \
  volatile int result;                                  \
  register int r;                                       \
                                                        \
  PERF_BEGIN (PERFORMANCE_COUNTER_0_BASE, SECTION);     \
  r = CODE;                                      \
  PERF_END (PERFORMANCE_COUNTER_0_BASE, SECTION);       \
  result = r;                                           \
                                                        \
  return result;                                        \
}

#endif /* __FLOAT2_PERFORMANCE_H__ */
