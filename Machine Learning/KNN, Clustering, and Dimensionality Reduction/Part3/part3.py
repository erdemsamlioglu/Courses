import pickle
import numpy as np
import matplotlib.pyplot as plt
from sklearn.cluster import AgglomerativeClustering, DBSCAN
from sklearn.metrics import silhouette_score
from scipy.cluster.hierarchy import dendrogram
from sklearn.neighbors import NearestNeighbors

# The dataset is already preprocessed...
dataset = pickle.load(open("../datasets/part3_dataset.data", "rb"))

def silhouette_calculate(data,labels):
    if len(np.unique(labels))>1:
        return silhouette_score(data,labels)
    return -1


def plot_dendrogram(model, **kwargs): ##took it directly from the recit code sources which is same as https://scikit-learn.org/stable/auto_examples/cluster/plot_agglomerative_dendrogram.html
    # Create linkage matrix and then plot the dendrogram

    # create the counts of samples under each node
    counts = np.zeros(model.children_.shape[0])
    n_samples = len(model.labels_)
    for i, merge in enumerate(model.children_):
        current_count = 0
        for child_idx in merge:
            if child_idx < n_samples:
                current_count += 1  # leaf node
            else:
                current_count += counts[child_idx - n_samples]
        counts[i] = current_count

    linkage_matrix = np.column_stack(
        [model.children_, model.distances_, counts]
    ).astype(float)

    # Plot the corresponding dendrogram
    dendrogram(linkage_matrix, **kwargs)


def plot_dbscan_results(eps_values, min_sample_values, silhouette_scores, metric):
    plt.figure(figsize=(10, 6))
    for i, scores in enumerate(silhouette_scores):
        plt.plot(eps_values, scores, label=f'min_samples={min_sample_values[i]}')
    plt.xlabel("Epsilon (eps)")
    plt.ylabel("Silhouette Score")
    plt.title(f"DBSCAN Silhouette Scores (Metric={metric})")
    plt.legend()
    plt.savefig(f"dbscan_silhouette_{metric}.png", dpi=300)
    plt.close()    


def HAC(data, linkage_criteria, distance_similarity, n_clusters=None,distance_threshold=None):
    clustering=AgglomerativeClustering(linkage=linkage_criteria, metric= distance_similarity, n_clusters=n_clusters, distance_threshold=distance_threshold)
    clustering.fit(data)
    return clustering

def dbscan(data, eps, min_samples, metric):
    clustering=DBSCAN(eps=eps, min_samples=min_samples, metric=metric).fit(data)
    labels= clustering.labels_
    return labels




def experiment_hac(data):
    distance_functions=["cosine", "euclidean"]
    linkage_functions=["single", "complete"]
    k_values=[2,3,4,5]

    for i in distance_functions:
        for j in linkage_functions:
            print(f"Running HAC for Distance Metric: {i}, "f"Linkage Criteria: {j}")
            silhouette_scores=[]
            k_plot=[]
            for k in k_values:
                print(f"Running HAC with {k} clusters")
                clustering = HAC(data, j, i, n_clusters=k, distance_threshold=None)
                labels = clustering.labels_
                silhouette_avg = silhouette_calculate(data, labels)
                silhouette_scores.append(silhouette_avg)
                k_plot.append(k)
                print(f"Silhouette Score (k={k}): {silhouette_avg:.3f}")

            plt.plot(k_plot, silhouette_scores, marker="o")
            plt.title(f"K vs Silhouette Score (Distance={i}, Linkage={j})")
            plt.xlabel("Number of Clusters (k)")
            plt.ylabel("Silhouette Score")
            plt.savefig(f"k_vs_silhouette_{i}_{j}.png", dpi=300)
            plt.close()    

            clustering=HAC(data, j, i, n_clusters=None, distance_threshold=0)
            plt.title(f"Hierarchical Clustering Dendrogram (Linkage={j}, Distance={i})")
            plot_dendrogram(clustering, truncate_mode="level", p=3)
            plt.savefig(f"dendrogram_{j}_{i}.png", dpi=300)
            plt.close()

def experiment_dbscan(data):
    eps_values=list(range(1, 101)) 
    min_sample_values=[3, 5, 7]
    distance_metrics=["euclidean"]

    for i in distance_metrics:
        silhouette_scores=[]
        for j in min_sample_values:
            scores_per_eps = []
            for k in eps_values:
                print(f"Running DBSCAN for eps={k}, min_samples={j}, metric={i}")
                labels=dbscan(data, k, j, i)
                silhouette_avg=silhouette_calculate(data, labels)
                scores_per_eps.append(silhouette_avg)
                print(f"Silhouette Score: {silhouette_avg:.3f}")
            silhouette_scores.append(scores_per_eps)
        plot_dbscan_results(eps_values, min_sample_values, silhouette_scores, i)


if __name__ == "__main__":
    experiment_hac(dataset)
    experiment_dbscan(dataset)



















