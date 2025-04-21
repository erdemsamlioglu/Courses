import pickle
import numpy as np
from sklearn.svm import SVC
import matplotlib.pyplot as plt


dataset, labels = pickle.load(open("../datasets/part2_dataset_1.data", "rb"))

C_values= [1,10]
kernels= ["linear", "rbf"]

x_min, x_max= dataset[:, 0].min()-1, dataset[:, 0].max()+1
y_min, y_max= dataset[:, 1].min()-1, dataset[:, 1].max()+1

h=0.02

xx, yy=np.meshgrid(np.arange(x_min, x_max, h), np.arange(y_min, y_max, h ))

for i in C_values:
    for j in kernels:
        model=SVC(C=i,kernel=j)
        model.fit(dataset, labels)

        Z=model.predict(np.c_[xx.ravel(), yy.ravel()])
        Z=Z.reshape(xx.shape)

        plt.figure(figsize=(8, 6))
        plt.contourf(xx, yy, Z, alpha=0.8, cmap=plt.cm.coolwarm)
        plt.scatter(dataset[:, 0], dataset[:, 1], c=labels, cmap=plt.cm.coolwarm, edgecolors='k')
        plt.title(f"SVM with C={i}, Kernel={j}")
        plt.xlabel("Feature 1")
        plt.ylabel("Feature 2")
        plt.tight_layout()
        plt.show()