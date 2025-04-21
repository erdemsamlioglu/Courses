import pickle
import numpy as np
import torch
import matplotlib.pyplot as plt
from sklearn.manifold import TSNE
import umap
from pca import PCA
from part3 import HAC
from part3 import dbscan


# The dataset is already preprocessed...
dataset = pickle.load(open("../datasets/part3_dataset.data", "rb"))

best_hac_clustering=HAC(dataset, linkage_criteria="single", distance_similarity="euclidean", n_clusters=2)
best_hac_labels=best_hac_clustering.labels_
best_dbscan_labels=dbscan(dataset, eps=18, min_samples=7, metric="euclidean")





def pca(dataset, projection_dim=2):
    pca=PCA(projection_dim)
    pca.fit(dataset)
    projected_data=pca.transform(dataset)
    return projected_data

def tsne(dataset, projection_dim=2, perplexity=30):
    tsne=TSNE(n_components=projection_dim, perplexity=perplexity)
    projected_data=tsne.fit_transform(dataset)
    return projected_data

def umap_f(dataset, projection_dim=2, n_neighbors=15, min_dist=0.1):
    embedding=umap.UMAP(n_components=projection_dim, n_neighbors= n_neighbors, min_dist=min_dist)
    projected_data=embedding.fit_transform(dataset)
    return projected_data

def visualization(data, labels, title, filename):
    plt.figure(figsize=(10, 7))
    cmap=plt.colormaps["viridis"]
    colors = np.arange(len(data))
    plt.scatter(
        data[:, 0], 
        data[:, 1], 
        c=labels, 
        cmap=cmap, 
        edgecolor="k", 
        alpha=0.7
    )
    plt.title(title)
    plt.colorbar()
    plt.savefig(filename, dpi=300)
    plt.close()


def experiment(dataset, name, labels=None):
    for projection_dim in [2]:
        pca_r = pca(dataset, projection_dim=projection_dim)
        visualization(pca_r, labels, f"PCA Visualization ({projection_dim}D) - {name}", f"{name.lower()}_pca_{projection_dim}d.png")
    for projection_dim in [2]:
        for perplexity in [10, 30, 50]:
            tsne_r = tsne(dataset, projection_dim=projection_dim, perplexity=perplexity)
            visualization(tsne_r, labels, f"t-SNE Visualization ({projection_dim}D, Perplexity={perplexity}) - {name}", f"{name.lower()}_tsne_{projection_dim}d_{perplexity}.png")
    for projection_dim in [2]:
        for n_neighbors in [15,30]:
            for min_dist in [0.1, 0.3]:
                umap_r = umap_f(dataset, projection_dim=projection_dim, n_neighbors=n_neighbors, min_dist=min_dist)
                visualization(umap_r, labels, f"UMAP Visualization ({projection_dim}D, n_neighbors={n_neighbors}, min_dist={min_dist}) - {name}", f"{name.lower()}_umap_{projection_dim}d_{n_neighbors}_{min_dist}.png")





def visualization_without_labels(data, title, filename):
    plt.figure(figsize=(10, 7))
    cmap = plt.colormaps["viridis"]
    plt.scatter(
        data[:, 0],
        data[:, 1],
        c=np.arange(len(data)) / len(data),
        cmap=cmap,
        edgecolor="k",
        alpha=0.7,
    )
    plt.title(title)
    plt.colorbar()
    plt.savefig(filename, dpi=300)
    plt.close()



def experiment2(dataset, name):
    for projection_dim in [2]:
        pca_r = pca(dataset, projection_dim=projection_dim)
        visualization_without_labels(pca_r, f"PCA Visualization ({projection_dim}D) - {name}", f"{name.lower()}_pca_{projection_dim}d_no_labels.png")
    for projection_dim in [2]:
        for perplexity in [10, 30, 50]:
            tsne_r = tsne(dataset, projection_dim=projection_dim, perplexity=perplexity)
            visualization_without_labels(tsne_r, f"t-SNE Visualization ({projection_dim}D, Perplexity={perplexity}) - {name}", f"{name.lower()}_tsne_{projection_dim}d_{perplexity}_no_labels.png")
    for projection_dim in [2]:
        for n_neighbors in [15,30]:
            for min_dist in [0.1, 0.3,]:
                umap_r = umap_f(dataset, projection_dim=projection_dim, n_neighbors=n_neighbors, min_dist=min_dist)
                visualization_without_labels(umap_r, f"UMAP Visualization ({projection_dim}D, n_neighbors={n_neighbors}, min_dist={min_dist}) - {name}", f"{name.lower()}_umap_{projection_dim}d_{n_neighbors}_{min_dist}_no_labels.png")



experiment(dataset, "Dataset (HAC)", best_hac_labels)
experiment(dataset, "Dataset (DBSCAN)", best_dbscan_labels)
experiment2(dataset, "")






