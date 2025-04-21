import pickle
import numpy as np
from Distance import Distance
from Knn import KNN
from sklearn.model_selection import StratifiedKFold
from sklearn.metrics import precision_score, recall_score, f1_score


# the data is already preprocessed
dataset, labels = pickle.load(open("../datasets/part1_dataset.data", "rb"))



def calculate_confidence_interval(accuracies):
    mean=np.mean(accuracies)
    std=np.std(accuracies)
    confidence_interval = 1.96 * std / np.sqrt(len(accuracies))
    return mean, confidence_interval



#hyperparameters
K_values=[1,3,5,7,10,15,30] 
distance_functions= {
    "Cosine": Distance.calculateCosineDistance,
    "Minkowski": Distance.calculateMinkowskiDistance,
    "Mahalanobis": Distance.calculateMahalanobisDistance,
}
repeats=5
folds=10

results=[]


#shuffle, train 80% and test 20%
shuffled_indices=np.arange(len(dataset))
np.random.shuffle(shuffled_indices)
dataset=dataset[shuffled_indices]
labels=labels[shuffled_indices]

split_index=int(len(dataset) * 0.8)
train_data, test_data=dataset[:split_index], dataset[split_index:]
train_labels, test_labels=labels[:split_index], labels[split_index:]




#skf
for i in range (repeats):
    skf=StratifiedKFold(n_splits=folds, shuffle=True)
    for train_index, test_index in skf.split(train_data, train_labels):
        x_train, x_test=train_data[train_index], train_data[test_index]
        y_train, y_test=train_labels[train_index], train_labels[test_index]

        for j in K_values:
            for name, function in distance_functions.items():
                if name=="Cosine":
                    knn=KNN(x_train, y_train, function, K=j)
                    predictions=[knn.predict(instance) for instance in x_test]
                    accuracy=np.mean(predictions==y_test)
                    f1=f1_score(y_test, predictions, average="weighted")
                    precision=precision_score(y_test, predictions, average="weighted")
                    recall=recall_score(y_test, predictions, average="weighted")
                    results.append({"K": j, "Distance": name, "Accuracy": accuracy ,"F1 Score": f1, "Precision": precision, "Recall": recall})

                elif name=="Minkowski":
                    knn=KNN(x_train, y_train, function, K=j)
                    predictions=[knn.predict(instance) for instance in x_test]
                    accuracy=np.mean(predictions==y_test)
                    f1=f1_score(y_test, predictions, average="weighted")
                    precision=precision_score(y_test, predictions, average="weighted")
                    recall=recall_score(y_test, predictions, average="weighted")
                    results.append({"K": j, "Distance": name, "Accuracy": accuracy ,"F1 Score": f1, "Precision": precision, "Recall": recall})

                elif name=="Mahalanobis":
                    S=np.cov(x_train, rowvar=False)
                    S_minus_1=np.linalg.inv(S)
                    knn=KNN(x_train, y_train, function, similarity_function_parameters=[S_minus_1], K=j)
                    predictions=[knn.predict(instance) for instance in x_test]
                    accuracy=np.mean(predictions==y_test)
                    f1=f1_score(y_test, predictions, average="weighted")
                    precision=precision_score(y_test, predictions, average="weighted")
                    recall=recall_score(y_test, predictions, average="weighted")
                    results.append({"K": j, "Distance": name, "Accuracy": accuracy ,"F1 Score": f1, "Precision": precision, "Recall": recall})


final_results=[]
for k in K_values:
    for name in distance_functions.keys():
        accuracies=[result["Accuracy"] for result in results if result["K"] == k and result["Distance"] == name]
        f1_scores = [result["F1 Score"] for result in results if result["K"] == k and result["Distance"] == name]
        if accuracies:
            mean_accuracy, conf_interval=calculate_confidence_interval(accuracies)
            mean_f1, dummy=calculate_confidence_interval(f1_scores)
            final_results.append({"K": k, "Distance": name, "Mean Accuracy": mean_accuracy, "Confidence Interval": conf_interval, "Mean F1 Score": mean_f1,})
            print(f"K={k}, Distance={name}, Confidence Interval=({mean_accuracy - conf_interval:.3f}, {mean_accuracy + conf_interval:.3f}), Mean F1={mean_f1:.3f}")




best_config = max(final_results, key=lambda x: x["Mean Accuracy"])
best_k = best_config["K"]
best_distance_function_name = best_config["Distance"]
best_distance_function = distance_functions[best_distance_function_name]

#reserved data to test
print(f"Best Configuration: K={best_k}, Distance Function={best_distance_function_name}")
if best_distance_function_name=="Mahalanobis":
    S=np.cov(train_data, rowvar=False)
    S_minus_1=np.linalg.inv(S)
    knn=KNN(train_data,train_labels,best_distance_function,similarity_function_parameters=[S_minus_1],K=best_k,
    )
else:
    knn = KNN(train_data, train_labels, best_distance_function, K=best_k)
    
test_predictions=[knn.predict(instance) for instance in test_data]
test_accuracy=np.mean(test_predictions == test_labels)
test_f1=f1_score(test_labels, test_predictions, average="weighted")
test_precision=precision_score(test_labels, test_predictions, average="weighted")
test_recall=recall_score(test_labels, test_predictions, average="weighted")
print(f"Accuracy on the test set: {test_accuracy:.3f}")
print(f"F1 Score on the test set: {test_f1:.3f}")
print(f"Precision on the test set: {test_precision:.3f}")
print(f"Recall on the test set: {test_recall:.3f}")




