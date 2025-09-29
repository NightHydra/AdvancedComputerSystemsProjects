# Project 1 Report
## Alek Krupka

## 1. Baseline vs Auto-Vectorized Outputs

The plots below show the achieved GFLOP/s for each of the selected kernels
when the program is run with auto-vectorization vs when the progam
is run with just scalars.  The plots show how performance varies sweeping
across memory (ie. the initial array is small enough to fit in L1 cache vs
needing to be stored in DRAM).  Note that the error bars shown represent 1 standard
deviation away from the mean.

![Baseline Vs Autovectorized Performace For Elementwise Multiplication](data_analysis/plots/BaselineVsVectorizedMULTIPLY.png)
![Baseline Vs Autovectorized Performace For Reduction](data_analysis/plots/BaselineVsVectorizedREDUCE.png)
![Baseline Vs Autovectorized Performace For Streaming](data_analysis/plots/BaselineVsVectorizedSTREAM.png)

The next plots outlines the speedup achieved through vectorization on each selected kernel
using a line graph.  Note that Speedup is calculated as
the SIMD average / Baseline Average for each kernel.

![Scalar vs Autovectorized Speedup](data_analysis/plots/Part1SpeedUp.png)

## Locality Sweep

Below, we show plots for the multiply kernel sweeping across cache levels.
The first plot shows performance in GFLOP/s while the second
plot shows performance in CPE.  Different memory levels were tested by using
different test for the operations.  The transitions between locality
are shown through the different bar graphs.  Note that the streaming
kernel operation was used in the locality sweep test results.

![Locality Sweep in GFLOP/s](data_analysis/plots/LocalitySweepGFLOPS.png)
![Locality Sweep in (Cycles per Element : CPE)](data_analysis/plots/LocalitySweepCPE.png)

## Alignment and Tail Handling

Below, shows the performance differences between align and purposely misaligned
inputs for the multiply kernel.  As seen by the plot below
the aligned inputs experience and slight speedup across the board when sweeping
across cache levels.

![Alignment Comparison in GFLOP/s](data_analysis/plots/Part3Graph.png)

## Stride vs Gather effects on performance

The below plot shows the effects of how accessing index arrays of distance 8 apart
causes performance issues within the code.

![Stride vs Gather Effects](data_analysis/plots/Part4Plot.png)

As shown above, stride vs gather patterns do not cause major performance issues
for the scalar runs.  However, the stride length causes a major performance decrease for 
the vectorized executables.

## Data Type Comparison

The below plot shows a comparison in performance between 32-bit floating
type operations and 64-bit floating type operations.

![Float32 vs 64 Effects](data_analysis/plots/Part5Plot.png)

The next plot below shows the speedup across test sizes.

![Float32 vs Float64 Speedup](data_analysis/plots/Float32Float64SpeedUp.png)

As shown lane width intensity has a large effect on vectorization speedup
especially for low arithmetic intensities.  At low arithmetic intensities the
speedup is far greater than for tests with larger arithmetic intensity.
This is most likely due to the fact that as arithmetic instensity gets larger
the memory access latency time dominates the difference in
lane width.

## Vectorization verification

This section of the report has pictures showing the assembly code
that uses just scalar operations vs asssembly code that uses
SIMD instructions.  The first image of every set will contain the
non-vectorized code while the second will contain the vectorized code.
Following that, the instructions used by the vectorized code
will be listed.

### Float32 Multiplication

![F32MultNoVect](SIMD_VerificationPictures/Float32_NoVectorize/MultiplyNoVectorizeF32.png)

![F32MultVect](SIMD_VerificationPictures/Float32_Vectorized/MultiplyVectorizedF32.png)

The vectorized code uses the following vectorized instructions as shown above.

1. vmovups
2. vmulps

### Float32 Reduction

![F32ReduceNoVect](SIMD_VerificationPictures/Float32_NoVectorize/ReduceNoVectorizeF32.png)
![F32ReduceVect](SIMD_VerificationPictures/Float32_Vectorized/ReduceVectorizedF32.png)

The vectorized code uses the following vectorized instructions as shown above.

1. vmovups
2. vmulps
3. vaddss
4. vshufps
5. vunpckhps
6. vextractf
7. valignd

### Float32 Streaming

![F32StreamNoVect](SIMD_VerificationPictures/Float32_NoVectorize/StreamNoVectorizeF32.png)
![F32StreamVect](SIMD_VerificationPictures/Float32_Vectorized/StreamVectorizedF32.png)

The vectorized code uses the following vectorized instructions as shown above.

1. vcvrss2sd
2. vfmadd231sd

### Float64 Multiplication

![F64MultNoVect](SIMD_VerificationPictures/Float64_NoVectorize/MultiplyNoVectorizeF64.png)

![F64MultVect](SIMD_VerificationPictures/Float64_Vectorized/MultiplyVectorizedF64.png)

The vectorized code uses the following vectorized instructions as shown above.

1. vmovupd
2. vmulpd

### Float64 Reduction

![F64ReduceNoVect](SIMD_VerificationPictures/Float64_NoVectorize/ReduceNoVectorizeF64.png)
![F64ReduceVect](SIMD_VerificationPictures/Float64_Vectorized/ReduceVectorizedF64.png)

The vectorized code uses the following vectorized instructions as shown above.

1. vmovupd
2. vmulpd
3. vaddsd
4. vunpckhpd
5. valignq
6. vextractf

### Float64 Streaming

![F64StreamNoVect](SIMD_VerificationPictures/Float64_NoVectorize/StreamNoVectorizeF64.png)
![F64StreamVect](SIMD_VerificationPictures/Float64_Vectorized/StreamVectorizedF64.png)

The vectorized code uses the following vectorized instructions as shown above.

1. vmulpd
2. vaddsd
3. vunpckhpd
4. vextractf64x2
5. valignq

Below is a table that outlines all the vectorized instructions
used and which vectorized kernels use them.  One interesting note is that the double floating
type does not use an FMA SIMD instruction which may cause decreased speedup when switching
from single floating point to double floating point.

| Instruction    | F32 Mult. | F32 Reduce | F32 Stream | F64 Mult. | F64 Reduce | F64 Stream |
|----------------|-----------|------------|------------|-----------|------------|------------|
 | vmovups        | x         | x          |            |           |            |            |
 | vmulps         | x         | x          |            |           |            |            |
 | vaddss         |           | x          |            |           |            |            |
 | vshufps        |           | x          |            |           |            |            |
 | vunpckhps      |           | x          |            |           |            |            |
 | vextractf      |           | x          |            |           | x          |            |
 | valignd        |           | x          |            |           |            |            |
  | vcvrss2sd      |           |            | x          |           |            |            | 
 | vfmadd231sd    |           |            | x          |           |            |            |
 | vmovupd        |           |            |            | x         | x          |            |
 | vmulpd         |           |            |            | x         | x          | x          |
 | vaddsd         |           |            |            |           | x          | x          |
 | vunpckhpd      |           |            |            |           | x          | x          |
 | valignq        |           |            |            |           | x          | x          |
| vextractf64x2  |           |            |            |           |            | x          |

