import numpy as np
from pathlib import Path

here = Path(__file__).parent
cpu = np.loadtxt(here / "out_cpu" / "2d_heat_output_cpu.csv", delimiter=",")
gpu = np.loadtxt(here / "out_gpu" / "2d_heat_output.csv", delimiter=",")

diff = np.abs(cpu - gpu)
rel_diff = diff / np.maximum(np.abs(cpu), 1e-300)

print("Max abs= ", diff.max(), "Max rel= ", rel_diff[cpu != 0].max())