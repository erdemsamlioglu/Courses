import numpy as np


A=np.array([[np.sqrt(i**2+j**2) for j in range(1,4)] for i in range(1,6)])
b=np.array([i for i in range(1,6)])

U, S, Vt=np.linalg.svd(A, full_matrices=False)

non_zero_elements= S>1e-10  
basis_B= U[:,non_zero_elements]

print("Matrix A:")
print(A)
print("\nVector b:")
print(b)
print("\nBasis for B:")
print(basis_B)