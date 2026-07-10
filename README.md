# Numerical Methods in C++ & CUDA

Numerical method implementation from scratch in object-oriented C++.
Each method gets CUDA and Kokkos versions.

**Current focus:** Heat transfer, fluid flow, and structural mechanics. 

**Long-term goal:** grow this into an open-source interface for researchers working on FSI (fluid–structure interaction) problems in microfluidics.

## Structure

```
src/        # Source code 
build*/     # Build directories
bench/      # Benchmark examples
```

## Requirements

- C++17 
- CMake
- CUDA Toolkit (for GPU builds)

## Build & Run (ex: heat2d gpu build)

```bash
mkdir build_gpu && cd build_gpu
cmake ..
make
./heat2d output_folder
```

## About
The emphasis is on clean OOP design through abstractions, and on benchmarking CPU vs GPU performance.
