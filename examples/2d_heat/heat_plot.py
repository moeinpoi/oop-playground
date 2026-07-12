import numpy as np
import matplotlib.pyplot as plt

data = np.loadtxt("./2d_heat_output.csv", delimiter=',')

print(data.shape)

for step in [0, 20, 100, 1000]:
    plt.plot(data[step - 1 if step else 0, :], label=f"timestep = {step}")

plt.xlabel("grid index")
plt.ylabel("temperature")
plt.legend()
plt.legend(loc="lower center")
plt.savefig("profiles.png", dpi=150, bbox_inches="tight")
plt.show()
