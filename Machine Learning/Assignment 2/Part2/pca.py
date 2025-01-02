import numpy as np

#I have used Chapter 6 of the Alpaydin book for equations.
class PCA:
    def __init__(self, projection_dim: int):
        """
        Initializes the PCA method
        :param projection_dim: the projection space dimensionality
        """
        self.projection_dim = projection_dim
        # keeps the projection matrix information
        self.projection_matrix = None
        self.mean_vector=None

    def fit(self, x: np.ndarray) -> None:
        """
        Applies the PCA method and obtains the projection matrix
        :param x: the data matrix on which the PCA is applied
        :return: None

        this function should assign the resulting projection matrix to self.projection_matrix
        """
        self.mean_vector=np.mean(x,axis=0)
        x_centered=x-self.mean_vector

        covariance_matrix = np.cov(x_centered, rowvar=False)

        eigenvalues, eigenvectors=np.linalg.eig(covariance_matrix)

        sorted_indices=np.argsort(eigenvalues)[::-1]
        high_indices=sorted_indices[:self.projection_dim]
        sorted_eigenvectors=eigenvectors[:, high_indices]
        self.projection_matrix=sorted_eigenvectors


    def transform(self, x: np.ndarray) -> np.ndarray:
        """
        After learning the projection matrix on a given dataset,
        this function uses the learned projection matrix to project new data instances
        :param x: data matrix which the projection is applied on
        :return: transformed (projected) data instances (projected data matrix)
        this function should utilize self.projection_matrix for the operations
        """
        x_centered=x-self.mean_vector
        projected=np.dot(x_centered, self.projection_matrix)
        return np.real(projected)


