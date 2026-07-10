cmake_minimum_required(VERSION 3.18)
project(heat2d CUDA CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CUDA_STANDARD 17)

# Set GPU architecture — sm_86 for RTX 3090
set(CMAKE_CUDA_ARCHITECTURES 86)

# CPU baseline executable
add_executable(heat2d_cpu src/2d_heat/2d_heat.cpp)

# CUDA executable
add_executable(heat2d_cuda src/2d_heat/2d_heat.cu)
target_compile_options(heat2d_cuda PRIVATE
    $<$<COMPILE_LANGUAGE:CUDA>:-O2 --lineinfo>
)
