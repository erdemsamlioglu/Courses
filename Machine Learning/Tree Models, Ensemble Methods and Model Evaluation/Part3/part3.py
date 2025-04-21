import pickle
import numpy as np
from sklearn.neighbors import KNeighborsClassifier
from sklearn.svm import SVC
from sklearn.tree import DecisionTreeClassifier
from sklearn.ensemble import RandomForestClassifier, GradientBoostingClassifier
from sklearn.neural_network import MLPClassifier
from sklearn.model_selection import RepeatedStratifiedKFold, GridSearchCV
from sklearn.metrics import f1_score, make_scorer
from sklearn.preprocessing import MinMaxScaler
from sklearn.pipeline import Pipeline
from collections import Counter

dataset, labels = pickle.load(open("../datasets/part3_dataset.data", "rb"))
X = np.array(dataset)
y = np.array(labels)


algorithms = {
    "KNN": (
        Pipeline([
            ("scaler", MinMaxScaler(feature_range=(-1, 1))),
            ("model", KNeighborsClassifier())
        ]),
        {
            "model__n_neighbors": [3, 5],
            "model__metric": ["euclidean", "manhattan"]
        }
    ),
    "SVM": (
        Pipeline([
            ("scaler", MinMaxScaler(feature_range=(-1, 1))),
            ("model", SVC())
        ]),
        {
            "model__C": [0.1, 1.0],
            "model__kernel": ["linear", "rbf"]
        }
    ),
    "DecisionTree": (
        Pipeline([
            ("scaler", MinMaxScaler(feature_range=(-1, 1))),
            ("model", DecisionTreeClassifier(random_state=0))
        ]),
        {
            "model__max_depth": [5, 10],
            "model__criterion": ["gini", "entropy"]
        }
    ),
    "RandomForest": (
        Pipeline([
            ("scaler", MinMaxScaler(feature_range=(-1, 1))),
            ("model", RandomForestClassifier(random_state=0))
        ]),
        {
            "model__n_estimators": [50, 100],
            "model__max_depth": [5, 10]
        }
    ),
    "MLP": (
        Pipeline([
            ("scaler", MinMaxScaler(feature_range=(-1, 1))),
            ("model", MLPClassifier(max_iter=1000, random_state=0))
        ]),
        {
            "model__hidden_layer_sizes": [(50,), (100,)],
            "model__alpha": [1e-3, 1e-4]
        }
    ),
    "GradientBoosting": (
        Pipeline([
            ("scaler", MinMaxScaler(feature_range=(-1, 1))),
            ("model", GradientBoostingClassifier(random_state=0))
        ]),
        {
            "model__n_estimators": [50, 100],
            "model__learning_rate": [0.01, 0.1]
        }
    )
}


outer_cv=RepeatedStratifiedKFold(
    n_splits=3,
    n_repeats=5,
    random_state=0
)

f1_scorer=make_scorer(f1_score, average="macro")


results_dict= {}


for alg_name, (pipeline_model, param_grid) in algorithms.items():
    print(f"\n=== {alg_name} ===")

    outer_scores=[]
    best_params_per_fold=[]

    for fold_idx, (train_idx, test_idx) in enumerate(outer_cv.split(X, y)):
        X_train, X_test = X[train_idx], X[test_idx]
        y_train, y_test = y[train_idx], y[test_idx]

        inner_cv= RepeatedStratifiedKFold(
            n_splits=5,
            n_repeats=5,
            random_state=1
        )

        grid_search= GridSearchCV(
            estimator=pipeline_model,
            param_grid=param_grid,
            scoring=f1_scorer,
            cv=inner_cv,
            n_jobs=-1
        )
        grid_search.fit(X_train, y_train)

        fold_best_params= grid_search.best_params_
        best_params_per_fold.append(fold_best_params)

        best_estimator= grid_search.best_estimator_
        y_pred=best_estimator.predict(X_test)
        fold_f1= f1_score(y_test, y_pred, average="macro")
        outer_scores.append(fold_f1)


    outer_scores=np.array(outer_scores)
    mean_f1=np.mean(outer_scores)
    std_f1=np.std(outer_scores, ddof=1)
    n_outer=len(outer_scores)
    ci95=1.96 * (std_f1 / np.sqrt(n_outer))

    print(f"Outer F1 scores: {outer_scores}")
    print(f"Mean F1 = {mean_f1:.4f} ± {ci95:.4f}")

    param_counter=Counter()
    for bp in best_params_per_fold:
        param_counter[frozenset(bp.items())] += 1

    print("Best Params Frequency (out of {} folds):".format(n_outer))
    for paramset, count in param_counter.items():
        param_dict=dict(paramset)
        print(f"  {param_dict} chosen {count} times")

    results_dict[alg_name] = (mean_f1, ci95, param_counter)



print("\n=== FINAL COMPARISON OF MODELS (MEAN F1 ± 95% CI) ===")
for model_name, (mean_f1, ci95, param_counter) in results_dict.items():
    print(f"{model_name}: {mean_f1:.4f} ± {ci95:.4f}")