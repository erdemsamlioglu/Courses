
import numpy as np
import matplotlib.pyplot as plt
import sys

n_values = np.arange(1, 1001)
epsilon = sys.float_info.epsilon

def f(n):
    return (n * (((n + 1) / n) - 1) - 1)

def g(n, epsilon):
    return f(n) / epsilon




g_values = [g(n, epsilon) for n in n_values]

#Plotting part of the question
plt.scatter(n_values, g_values, s=5)  
plt.xlabel('n')
plt.ylabel('g(n)')
plt.title('Plot of g(n) for n in [1, 1000] and n is integer')
plt.grid(True)

plt.show()
valueforthis=f(1000)
#Finding zero values part
zero_values = [n for n, i in zip(n_values, g_values) if i == 0]
print("Values of n for g(n) is 0:", zero_values)




