import pickle
import numpy as np
from sklearn.svm import SVC
from sklearn.preprocessing import StandardScaler
from sklearn.model_selection import GridSearchCV, RepeatedStratifiedKFold
from sklearn.pipeline import Pipeline

dataset, labels = pickle.load(open("../datasets/part2_dataset_2.data", "rb"))

param_grid = {
    "svm__C": [1, 10],
    "svm__kernel": ["linear", "rbf"]
}

pipeline = Pipeline([
    ("scaler", StandardScaler()),
    ("svm", SVC())

])

repeated_cv=RepeatedStratifiedKFold(n_splits=10,n_repeats=5, random_state=0)

grid_search=GridSearchCV(estimator=pipeline,param_grid=param_grid, scoring="accuracy", cv=repeated_cv, return_train_score=False, refit=True)
grid_search.fit(dataset, labels)

results=grid_search.cv_results_
mean_scores= results["mean_test_score"]
std_devs= results["std_test_score"]

confidence_level=0.95
z_value=1.96

n_splits=repeated_cv.get_n_splits(X=dataset, y=labels)

for i, params in enumerate(results["params"]):
    mean_acc=mean_scores[i]
    std_dev=std_devs[i]
    margin_of_error= z_value * (std_dev / np.sqrt(n_splits))

    lower_bound=mean_acc - margin_of_error
    upper_bound=mean_acc + margin_of_error

    print(f"Params: {params} -> "f"Mean Accuracy: {mean_acc:.3f}, "f"95% CI: [{lower_bound:.3f}, {upper_bound:.3f}]")