import numpy as np
import matplotlib.pyplot as plt

data = np.loadtxt("./2d_heat_output.csv", delimiter=',')

print(data.shape)

for t in [0, 19,99, 999]:
    plt.plot(data[t,:])

plt.show()