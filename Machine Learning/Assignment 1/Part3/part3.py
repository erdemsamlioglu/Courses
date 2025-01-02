import torch
import numpy as np
import pickle
import torch.nn as nn
import copy
import matplotlib.pyplot as plt


#mlp class
class MLP(nn.Module):
    def __init__(self, activation, input_size, hidden_size, output_size):
        super(MLP, self).__init__()
        self.input_layer = nn.Linear(input_size, hidden_size[0])
        self.hidden_layers = nn.ModuleList()

        for i in range(len(hidden_size) - 1):
            self.hidden_layers.append(nn.Linear(hidden_size[i], hidden_size[i + 1]))

        self.output_layer = nn.Linear(hidden_size[-1], output_size)

        self.activation = activation()

    def forward(self, x):
        x=self.activation(self.input_layer(x))

        for i in self.hidden_layers:
            x=self.activation(i(x))

        x=self.output_layer(x)
        return x



# we load all the datasets of Part 3
# the train data is already shuffled, we don't need to shuffle it...
x_train, y_train = pickle.load(open("../datasets/part3_train_dataset.dat", "rb"))
x_validation, y_validation = pickle.load(open("../datasets/part3_validation_dataset.dat", "rb"))
x_test, y_test = pickle.load(open("../datasets/part3_test_dataset.dat", "rb"))

# We rescale each feature of data instances in the datasets
x_train = x_train/255.0
x_train = x_train.astype(np.float32)

x_test = x_test / 255.0
x_test = x_test.astype(np.float32)

x_validation = x_validation/255.0
x_validation = x_validation.astype(np.float32)

# and converting them into Pytorch tensors in order to be able to work with Pytorch
x_train = torch.from_numpy(x_train)
y_train = torch.from_numpy(y_train).to(torch.long)

x_validation = torch.from_numpy(x_validation)
y_validation = torch.from_numpy(y_validation).to(torch.long)

x_test = torch.from_numpy(x_test)
y_test = torch.from_numpy(y_test).to(torch.long)



#train function
def train(hidden_neurons, learning_rate, num_epochs, activation):
    model= MLP(activation=activation, input_size=784, hidden_size=hidden_neurons, output_size=10)

    #I have chosen Adam optimizer
    optimizer=torch.optim.Adam(model.parameters(), lr=learning_rate, weight_decay=0.01)
    lossf=nn.CrossEntropyLoss()
    best_loss=float('inf')
    best_weights= None
    best_accuracy=0.0

    train_loss=[]
    validation_loss=[]


    for epoch in range(num_epochs):
        model.train()
        optimizer.zero_grad()
        outputs=model(x_train)
        loss=lossf(outputs,y_train)
        loss.backward()
        optimizer.step()
        train_loss.append(loss.item())

        #evaluate on validation set part
        model.eval()
        with torch.no_grad():
            outputs=model(x_validation)
            validationloss= lossf(outputs, y_validation)
            validation_loss.append(validationloss.item())
            dummy1, predicted= torch.max(outputs,1)
            accuracy=(predicted == y_validation).float().mean() * 100

            if validationloss<best_loss:
                best_loss=validationloss
                best_weights=copy.deepcopy(model.state_dict())
                best_accuracy=accuracy.item()


    return train_loss, validation_loss, best_accuracy, best_weights

#confidence interval
def calculate_confidence_interval(values, confidence_level=1.96):
    mean_val=np.mean(values)
    std_dev=np.std(values)
    margin_error=confidence_level*std_dev/np.sqrt(len(values))
    lower_bound=mean_val-margin_error
    upper_bound=mean_val+margin_error
    return mean_val, (lower_bound, upper_bound)






hidden_layer_options = [[8, 8], [8, 16], [16, 16], [32,64], [64,64], [128], [128, 64], [256, 128, 64]]
learning_rate_options = [0.01, 0.001, 0.0001]
epoch_options = [10, 50, 100, 150, 200]
activation_functions = [nn.Sigmoid, nn.Tanh]

#to track without mean
best_all_weights=None
best_all_accuracy=0
best_hidden_neurons = None
best_learning_rate = None
best_num_epochs = None
best_activation = None
#to track with mean
best_mean_accuracy_weights = None
best_mean_accuracy = 0.0
best_mean_hidden_neurons = None
best_mean_learning_rate = None
best_mean_num_epochs = None
best_mean_activation = None

#grid search with each config running 10 times and then taking their mean, and also calculating their confidence interval to find the best config
srm=[]
results=[]
for hidden_neurons in hidden_layer_options:
    for learning_rate in learning_rate_options:
        for num_epochs in epoch_options:
            for activation in activation_functions:
                accuracy_results_10run=[] 
                train_losses_10run=[]
                val_losses_10run=[]
                for i in range(10):
                    train_loss, val_loss, best_accuracy, best_weights = train(hidden_neurons=hidden_neurons,learning_rate=learning_rate,num_epochs=num_epochs,activation=activation)

                    train_losses_10run.append(train_loss)
                    val_losses_10run.append(val_loss)
                    accuracy_results_10run.append(best_accuracy)

                avg_train_loss = np.mean(train_losses_10run, axis=0)
                avg_val_loss = np.mean(val_losses_10run, axis=0)

                srm.append({
                            'Hidden Layers': len(hidden_neurons),
                            'Neurons per Layer': hidden_neurons,
                            'Learning Rate': learning_rate,
                            'Epochs': num_epochs,
                            'Activation': activation.__name__,
                            'Train Losses': avg_train_loss,
                            'Validation Losses': avg_val_loss,
                        })
                
                mean_accuracy, (confidence_lower, confidence_upper) = calculate_confidence_interval(accuracy_results_10run)
                results.append({
                            'Hidden Layers': len(hidden_neurons),
                            'Neurons per Layer': hidden_neurons,
                            'Learning Rate': learning_rate,
                            'Epochs': num_epochs,
                            'Activation': activation.__name__,
                            'Mean Validation Accuracy': mean_accuracy,
                            'Confidence Interval': (confidence_lower, confidence_upper)
                        })
                print(f"Hidden Layers: {len(hidden_neurons)}, Neurons: {hidden_neurons}, "
                        f"Learning Rate: {learning_rate}, Epochs: {num_epochs}, "
                        f"Activation: {activation.__name__}, "
                        f"Mean Validation Accuracy: {mean_accuracy:.2f}%, "
                        f"Confidence Interval: [{confidence_lower:.2f}%, {confidence_upper:.2f}%]")

                if best_accuracy > best_all_accuracy:
                    best_all_accuracy = best_accuracy
                    best_all_weights = best_weights
                    best_hidden_neurons = hidden_neurons
                    best_learning_rate = learning_rate
                    best_num_epochs = num_epochs
                    best_activation = activation

                if mean_accuracy > best_mean_accuracy:
                    best_mean_accuracy = mean_accuracy
                    best_mean_accuracy_weights = best_weights
                    best_mean_hidden_neurons = hidden_neurons
                    best_mean_learning_rate = learning_rate
                    best_mean_num_epochs = num_epochs
                    best_mean_activation = activation    

#saving the best accuracy on 1 run
if best_all_weights is not None:
    best_modelpath= 'best_model.pth'
    torch.save(best_all_weights, best_modelpath)
    print(f"Best model saved with accuracy: {best_all_accuracy:.2f}% at {best_modelpath}")
#saving the best mean accuracy
if best_mean_accuracy_weights is not None:
    best_mean_modelpath = 'best_mean_model.pth'
    torch.save(best_mean_accuracy_weights, best_mean_modelpath)
    print(f"Best model saved based on mean accuracy with accuracy: {best_mean_accuracy:.2f}% at {best_mean_modelpath}")    




#combining the training data and validation data for testing best config
combined_x_train = torch.cat((x_train, x_validation), dim=0)
combined_y_train = torch.cat((y_train, y_validation), dim=0)

#best config I found from the grid search
best_hidden_neurons = [32, 64]
best_learning_rate = 0.01
best_num_epochs = 200
best_activation = nn.Tanh


#for the combined data
def train_and_test_combined(hidden_neurons, learning_rate, num_epochs, activation):
    model = MLP(activation=activation, input_size=784, hidden_size=hidden_neurons, output_size=10)
    optimizer = torch.optim.Adam(model.parameters(), lr=learning_rate, weight_decay=0.01)
    lossf = nn.CrossEntropyLoss()

    model.train()
    for epoch in range(num_epochs):
        optimizer.zero_grad()
        outputs = model(combined_x_train)
        loss = lossf(outputs, combined_y_train)
        loss.backward()
        optimizer.step()

    model.eval()
    with torch.no_grad():
        outputs = model(x_test)
        dummy2, predicted = torch.max(outputs, 1)
        accuracy = (predicted == y_test).float().mean() * 100

    return accuracy.item()


test_accuracies = []
for i in range(10):
    test_accuracy = train_and_test_combined(
        hidden_neurons=best_hidden_neurons,
        learning_rate=best_learning_rate,
        num_epochs=best_num_epochs,
        activation=best_activation
    )
    test_accuracies.append(test_accuracy)

mean_test_accuracy, (ci_lower, ci_upper) = calculate_confidence_interval(test_accuracies)
print(f"Mean Test Accuracy Over 10 Runs: {mean_test_accuracy:.2f}%")
print(f"95% Confidence Interval for Test Accuracy: [{ci_lower:.2f}%, {ci_upper:.2f}%]")




#srm plot
for config_data in srm:
    plt.plot(config_data['Train Losses'], label='Average Train Loss', color='blue', linewidth=1)
    plt.plot(config_data['Validation Losses'], label='Average Validation Loss', color='orange', linestyle='--', linewidth=1.5)
    plt.title(f"SRM Plot - Layers: {config_data['Hidden Layers']}, Neurons: {config_data['Neurons per Layer']}, "
              f"LR: {config_data['Learning Rate']}, Epochs: {config_data['Epochs']}, Activation: {config_data['Activation']}")
    plt.xlabel('Epochs')
    plt.ylabel('Loss')
    plt.legend()
    filename = f"SRM_Layers_{config_data['Hidden Layers']}_Neurons_{config_data['Neurons per Layer']}_LR_{config_data['Learning Rate']}_Epochs_{config_data['Epochs']}_Activation_{config_data['Activation']}.png"
    plt.savefig(filename)
    plt.close()


