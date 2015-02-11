Float2 Performance compares the performance of floating-point
operations using the Floating Point Hardware 2 custom instructions vs. 
software emulation.  After each operation is performed, a table is printed
showing the performance of the custom instructions and software emulation.

This code must be compiled with GCC optimization level -O2.

The Floating Point Hardware 2 component is the 2nd generation of floating-point
custom instructions for Nios II.  It offers improved performance, 
hardware acceleration of more operations, and reduced resource usage relative
to the 1st generation.  Results are not fully IEEE 754 compliant due to the
implementation of simplified rounding.
      
System Requirements
*******************
The following peripherals must be connected to the Nios II:
  - 'Floating Point Hardware 2' component
  - 'Performance Counter Unit' component named "performance_counter_0"
      - Must have at least 2 sections
     - Must be connected to the same clock as the Nios II
  - Some character device (e.g. JTAG UART or serial UART)

Software Files
****************
float2_performance.c    ==> Main source file
float2_performance_sw.c ==> Operations using software emulation
float2_performance_hw.c ==> Operations using hardware
float2_performance.h    ==> #defines and function prototypes
