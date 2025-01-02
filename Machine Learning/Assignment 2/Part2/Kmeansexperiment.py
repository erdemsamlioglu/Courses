import pickle
import numpy as np
import matplotlib.pyplot as plt
from sklearn.cluster import KMeans
from sklearn.metrics import silhouette_score

# The datasets are already preprocessed...
dataset1 = pickle.load(open("../datasets/part2_dataset_1.data", "rb"))
dataset2 = pickle.load(open("../datasets/part2_dataset_2.data", "rb"))

def calculate_confidence_interval(accuracies):
    mean=np.mean(accuracies)
    std=np.std(accuracies)
    confidence_interval=1.96*std /np.sqrt(len(accuracies))
    return (mean, confidence_interval)




def plot_results(K_values, loss_results, silhouette_results, dataset_name):
    #Elbow
    plt.figure(figsize=(10, 5))
    loss_means = [res[1] for res in loss_results]
    plt.plot(K_values, loss_means, marker='s', linestyle='-', markersize=4, linewidth=2, color='b')
    plt.xlabel("Number of Clusters (K)")
    plt.ylabel("Avg Loss")
    plt.title(f"Elbow Method for {dataset_name}")
    plt.grid(True)
    plt.savefig(f"{dataset_name.lower().replace(' ', '_')}_elbow.png", dpi=300)
    plt.close()


    #Silhouette
    plt.figure(figsize=(10, 5))
    silhouette_means = [res[1] for res in silhouette_results]
    plt.plot(K_values, silhouette_means, marker='s', linestyle='-', markersize=4, linewidth=2, color='r')
    plt.xlabel("Number of Clusters (K)")
    plt.ylabel("Silhouette Score")
    plt.title(f"Silhouette Scores for {dataset_name}")
    plt.grid(True)
    plt.savefig(f"{dataset_name.lower().replace(' ', '_')}_silhouette.png", dpi=300)
    plt.close()


def run(dataset, k, repeat=10):
    loss=[]
    silhouette=[]

    for i in range(repeat):
        kmeans=KMeans(n_clusters=k, init="k-means++").fit(dataset)
        loss.append(kmeans.inertia_)
        labels=kmeans.labels_
        silhouette.append(silhouette_score(dataset,labels))

    loss_mean, loss_confidence=calculate_confidence_interval(loss)
    silhouette_mean, silhouette_confidence=calculate_confidence_interval(silhouette)

    return loss_mean, loss_confidence, silhouette_mean, silhouette_confidence


def main(dataset, name):
    K_values=range(2,11)
    loss_results=[]
    silhouette_results=[]

    for i in K_values:
        loss_mean, loss_confidence, silhouette_mean, silhouette_confidence= run(dataset, i)
        loss_results.append((i, loss_mean, loss_confidence))
        silhouette_results.append((i, silhouette_mean, silhouette_confidence ))
        print(f"K={i}, Loss Mean={loss_mean:.3f}, Loss CI=±{loss_confidence:.3f}, Silhouette Mean={silhouette_mean:.3f}, Silhouette CI=±{silhouette_confidence:.3f}")

    plot_results(K_values, loss_results, silhouette_results, name)


main(dataset1, "Dataset 1")
main(dataset2, "Dataset 2")



