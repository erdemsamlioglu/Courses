import pickle
import numpy as np
import torch
import matplotlib.pyplot as plt
from sklearn.manifold import TSNE
import umap
from pca import PCA
from autoencoder import AutoEncoder



# The datasets are already preprocessed...
dataset1 = pickle.load(open("../datasets/part2_dataset_1.data", "rb"))
dataset2 = pickle.load(open("../datasets/part2_dataset_2.data", "rb"))

def pca(dataset, projection_dim=2):
    pca=PCA(projection_dim)
    pca.fit(dataset)
    projected_data=pca.transform(dataset)
    return projected_data


def autoencoder(dataset,input_dim, projection_dim=2, learning_rate=0.001, iteration_count=100):
    autoencoder=AutoEncoder(input_dim, projection_dim, learning_rate, iteration_count)
    tensor=torch.tensor(dataset, dtype=torch.float32)
    autoencoder.fit(tensor)
    projected_data=autoencoder.transform(tensor).numpy()
    return projected_data



def tsne(dataset, projection_dim=2, perplexity=30):
    tsne=TSNE(n_components=projection_dim, perplexity=perplexity)
    projected_data=tsne.fit_transform(dataset)
    return projected_data

def umap_f(dataset, projection_dim=2, n_neighbors=15, min_dist=0.1):
    embedding=umap.UMAP(n_components=projection_dim, n_neighbors= n_neighbors, min_dist=min_dist)
    projected_data=embedding.fit_transform(dataset)
    return projected_data

def visualization(data, title, filename):
    plt.figure(figsize=(10, 7))
    cmap=plt.colormaps["viridis"]
    colors = np.arange(len(data))
    plt.scatter(
        data[:, 0], 
        data[:, 1], 
        c=colors, 
        cmap=cmap, 
        edgecolor="k", 
        alpha=0.7
    )
    plt.title(title)
    plt.colorbar()
    plt.savefig(filename, dpi=300)
    plt.close()


def experiment(dataset, name):
    for projection_dim in [2, 3]:
        pca_r=pca(dataset, projection_dim=projection_dim)
        visualization(pca_r, f"PCA Visualization ({projection_dim}D) - {name}", f"{name.lower()}_pca_{projection_dim}d.png")

    for projection_dim in [2, 3]:
        autoencoder_r=autoencoder(dataset, input_dim=dataset.shape[1], projection_dim=projection_dim)
        visualization(autoencoder_r, f"Autoencoder Visualization ({projection_dim}D) - {name}", f"{name.lower()}_autoencoder_{projection_dim}d.png")

    for projection_dim in [2, 3]:
        for perplexity in [10, 30, 50]:
            tsne_r=tsne(dataset, projection_dim=projection_dim, perplexity=perplexity)
            visualization(tsne_r, f"t-SNE Visualization ({projection_dim}D, Perplexity={perplexity}) - {name}", f"{name.lower()}_tsne_{projection_dim}d_{perplexity}.png")

    for projection_dim in [2, 3]:
        for n_neighbors in [10, 15, 30, 100]:
            for min_dist in [0.1, 0.3, 0.5]:
                umap_r = umap_f(dataset, projection_dim=projection_dim, n_neighbors=n_neighbors, min_dist=min_dist)
                visualization(umap_r, f"UMAP Visualization ({projection_dim}D, n_neighbors={n_neighbors}, min_dist={min_dist}) - {name}", f"{name.lower()}_umap_{projection_dim}d_{n_neighbors}_{min_dist}.png")


experiment(dataset1, "Dataset1")
experiment(dataset2, "Dataset2")









    
