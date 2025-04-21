import numpy as np
# In the decision tree, non-leaf nodes are going to be represented via TreeNode
class TreeNode:
    def __init__(self, attribute):
        self.attribute = attribute
        # dictionary, k: subtree, key (k) an attribute value, value is either TreeNode or TreeLeafNode
        self.subtrees = {}

# In the decision tree, leaf nodes are going to be represented via TreeLeafNode
class TreeLeafNode:
    def __init__(self, data, label):
        self.data = data
        self.labels = label

class DecisionTree:
    def __init__(self, dataset: list, labels, features, criterion="information gain"):
        """
        :param dataset: array of data instances, each data instance is represented via an Python array
        :param labels: array of the labels of the data instances
        :param features: the array that stores the name of each feature dimension
        :param criterion: depending on which criterion ("information gain" or "gain ratio") the splits are to be performed
        """
        self.dataset = dataset
        self.labels = labels
        self.features = features
        self.criterion = criterion
        # it keeps the root node of the decision tree
        self.root = None

        # further variables and functions can be added...


    def calculate_entropy__(self, dataset, labels):
        """
        :param dataset: array of the data instances
        :param labels: array of the labels of the data instances
        :return: calculated entropy value for the given dataset
        """
        entropy_value = 0.0

        """
        Entropy calculations
        """

        unique_labels, counts=np.unique(labels, return_counts=True)

        for i in range(len(unique_labels)):
            probability= counts[i]/len(labels)
            if probability>0:
                entropy_value+= -probability*np.log2(probability)


        return entropy_value

    def calculate_average_entropy__(self, dataset, labels, attribute):
        """
        :param dataset: array of the data instances on which an average entropy value is calculated
        :param labels: array of the labels of those data instances
        :param attribute: for which attribute an average entropy value is going to be calculated...
        :return: the calculated average entropy value for the given attribute
        """
        average_entropy = 0.0
        """
            Average entropy calculations
        """

        attribute_values=[data[attribute] for data in dataset]
        unique_values=np.unique(attribute_values)

        for i in unique_values:
            subset_indices=[j for j in range(len(dataset)) if dataset[j][attribute]==i]
            subset=[dataset[k] for k in subset_indices]
            subset_labels=[labels[n] for n in subset_indices]

            subset_entropy=self.calculate_entropy__(subset, subset_labels)
            weight=len(subset)/len(dataset)
            average_entropy+=weight*subset_entropy

        return average_entropy

    def calculate_information_gain__(self, dataset, labels, attribute):
        """
        :param dataset: array of the data instances on which an information gain score is going to be calculated
        :param labels: array of the labels of those data instances
        :param attribute: for which attribute the information gain score is going to be calculated...
        :return: the calculated information gain score
        """
        information_gain = 0.0
        """
            Information gain calculations
        """
        entropy= self.calculate_entropy__(dataset,labels)
        average_entropy= self.calculate_average_entropy__(dataset,labels,attribute)
        information_gain= entropy-average_entropy

        return information_gain

    def calculate_intrinsic_information__(self, dataset, labels, attribute):
        """
        :param dataset: array of data instances on which an intrinsic information score is going to be calculated
        :param labels: array of the labels of those data instances
        :param attribute: for which attribute the intrinsic information score is going to be calculated...
        :return: the calculated intrinsic information score
        """
        intrinsic_info = 0.0
        """
            Intrinsic information calculations for a given attribute
        """
        attribute_values=[data[attribute] for data in dataset]
        unique_values=np.unique(attribute_values)

        for i in unique_values:
            subset_indices=[j for j in range(len(dataset)) if dataset[j][attribute]==i]
            ratio=len(subset_indices)/len(dataset)
            if ratio>0:
                intrinsic_info-=ratio*np.log2(ratio)
            else:
                continue

        return intrinsic_info
    
    def calculate_gain_ratio__(self, dataset, labels, attribute):
        """
        :param dataset: array of data instances with which a gain ratio is going to be calculated
        :param labels: array of labels of those instances
        :param attribute: for which attribute the gain ratio score is going to be calculated...
        :return: the calculated gain ratio score
        """
        """
            Your implementation
        """
        information_gain=self.calculate_information_gain__(dataset, labels, attribute)
        intrinsic_info=self.calculate_intrinsic_information__(dataset, labels,attribute)
        if intrinsic_info==0:
            return 0
        gain_ratio=information_gain/intrinsic_info

        return gain_ratio


    def ID3__(self, dataset, labels, used_attributes):
        """
        Recursive function for ID3 algorithm
        :param dataset: data instances falling under the current  tree node
        :param labels: labels of those instances
        :param used_attributes: while recursively constructing the tree, already used labels should be stored in used_attributes
        :return: it returns a created non-leaf node or a created leaf node
        """
        """
            Your implementation
        """
        unique_labels= np.unique(labels)

        if len(unique_labels)==1:
            return TreeLeafNode(dataset, unique_labels[0])
        
        if len(used_attributes)==len(self.features):
            unique, counts=np.unique(labels, return_counts=True)
            majority=unique[np.argmax(counts)]
            return TreeLeafNode(dataset, majority)
        
        best_attribute=None
        best_gain=0

        for i, attribute in enumerate(self.features):
            if i in used_attributes:
                continue

            if self.criterion=="information gain":
                gain=self.calculate_information_gain__(dataset, labels, i)
            elif self.criterion=="gain ratio":
                gain=self.calculate_gain_ratio__(dataset,labels, i)

            if gain>best_gain:
                best_gain=gain
                best_attribute= i

        if best_attribute is None:
            unique, counts = np.unique(labels, return_counts=True)
            majority = unique[np.argmax(counts)]
            return TreeLeafNode(dataset, majority)
            
        print(f"Chosen attribute: {self.features[best_attribute]} , Gain: {best_gain}")
        node=TreeNode(self.features[best_attribute])
        used_attributes.append(best_attribute)


        unique_values= np.unique([data[best_attribute] for data in dataset])
        for k in unique_values:
            subset_data= [data for data in dataset if data[best_attribute]==k]
            subset_labels= [labels[j] for j, data in enumerate(dataset) if data[best_attribute]==k]

            if len(subset_data)==0:
                unique, counts=np.unique(labels, return_counts=True)
                majority= unique[np.argmax(counts)]
                node.subtrees[k]=TreeLeafNode([], majority)
            else:
                node.subtrees[k]=self.ID3__(subset_data, subset_labels, used_attributes.copy())
                
        return node




    def predict(self, x):
        """
        :param x: a data instance, 1 dimensional Python array 
        :return: predicted label of x
        
        If a leaf node contains multiple labels in it, the majority label should be returned as the predicted label
        """
        """
            Your implementation
        """
        current_node= self.root

        while isinstance(current_node, TreeNode):
            index=self.features.index(current_node.attribute)
            attribute_value= x[index]
            if attribute_value in current_node.subtrees:
                current_node= current_node.subtrees[attribute_value]
            else:
                break

        if isinstance(current_node, TreeLeafNode):
            unique, counts= np.unique(current_node.labels, return_counts=True)
            majority= unique[np.argmax(counts)]
            return majority
        
        return None


    def train(self):
        self.root = self.ID3__(self.dataset, self.labels, [])
        print("Training completed")