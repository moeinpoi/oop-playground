# Numerical Methods in C++

Numerical method implementations written from scratch in object-oriented C++.
The emphasis is on clean design through abstractions (grid, field, boundary
condition, solver, time integrator).

**Status:** early work in progress.

## Implemented

- **1D finite difference** — central-difference derivatives via an abstract `Function` interface
- **1D heat equation** — explicit Euler time integration
- **2D heat equation** — explicit finite difference:
  - `2d_heat.cpp` — CPU baseline
  - `2d_heat.cu` — CUDA implementation

## Roadmap

- Additional physics: fluid flow, structural mechanics
- Lattice Boltzmann (D2Q9) solver
- Kokkos backends for hardware-agnostic parallelism
- Long-term: an open-source interface for FSI problems in microfluidics

## Structure

src/         # Source code
examples/    # Example output and plotting scripts

## Requirements

- C++17, CMake ≥ 3.18
- CUDA Toolkit (for the GPU build)

## Build & Run

mkdir build && cd build
cmake ..
make
./heat2d_cuda output_folder
