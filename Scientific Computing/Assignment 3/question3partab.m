%My variables
c_true= [1.5, -0.75, 0.5, -0.25]; 

n_values= [5, 10, 100];
observations= cell(length(n_values), 2);
An_matrices= cell(length(n_values), 1);
bn_vectors= cell(length(n_values), 1);


for i= 1:length(n_values)
    n= n_values(i);
    [t_vals, obs_vals] = generate_observations(n, c_true);
    
    A_n=[ones(n, 1), t_vals', t_vals'.^2, t_vals'.^3];

    b_n= obs_vals';
   
    An_matrices{i}= A_n;
    bn_vectors{i}= b_n;
end


c_estimated_values = cell(length(n_values), 1);

for i= 1:length(n_values)
    A_n= An_matrices{i};
    b_n= bn_vectors{i};

    c_estimated= A_n \ b_n;

    c_estimated_values{i}= c_estimated;

    fprintf('Matrix A_%d:\n', n_values(i));
    disp(A_n);
    fprintf('Vector b_%d:\n', n_values(i));
    disp(b_n);
    fprintf('Coefficients for n=%d:\n', n_values(i));
    disp(c_estimated);
end


function N_t= N(t, c)
    N_t= 0.3+2*t-1.2*t^2+0.5*t^3+sum(c.*(t.^(0:(length(c)-1))));
end

function [t_vals, obs_vals] = generate_observations(n, c)
    t_vals=(1:n)/n;
    obs_vals=arrayfun(@(t) N(t, c)+ 0.01*randn(), t_vals);
end





