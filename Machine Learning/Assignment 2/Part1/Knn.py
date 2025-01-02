class KNN:
    def __init__(self, dataset, data_label, similarity_function, similarity_function_parameters=None, K=1):
        """
        :param dataset: dataset on which KNN is executed, 2D numpy array
        :param data_label: class labels for each data sample, 1D numpy array
        :param similarity_function: similarity/distance function, Python function
        :param similarity_function_parameters: auxiliary parameter or parameter array for distance metrics
        :param K: how many neighbors to consider, integer
        """
        self.K = K
        self.dataset = dataset
        self.dataset_label = data_label
        self.similarity_function = similarity_function
        self.similarity_function_parameters = similarity_function_parameters

    def predict(self, instance):

        distances=[]

        for index, data_point in enumerate(self.dataset):
            if self.similarity_function_parameters is not None:
                distance=self.similarity_function(instance, data_point, *self.similarity_function_parameters)
            else:
                distance=self.similarity_function(instance, data_point)
            distances.append((distance, self.dataset_label[index]))


        sorted_distances=sorted(distances, key=lambda x: x[0])
        k_nearest_neighbors=sorted_distances[:self.K]
        k_labels=[label for i, label in k_nearest_neighbors]
        predicted=max(set(k_labels), key=k_labels.count)
        
        return predicted

    



    

