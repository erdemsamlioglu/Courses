function [L, U, P, Q] = my_lu_cp(A)
    
    n=size(A, 1);

    
    L=eye(n);
    U=A;
    P=eye(n);
    Q=eye(n);

    %complete pivoting
    for k = 1:n-1
        
        [max_col, max_row]= find(abs(U(k:n, k:n)) == max(max(abs(U(k:n, k:n)))));
        max_col= max_col(1) + k - 1;
        max_row= max_row(1) + k - 1;

        
        U([k, max_col], :)= U([max_col, k], :);
        U(:, [k, max_row])= U(:, [max_row, k]);
        P([k, max_col], :)= P([max_col, k], :);
        Q(:, [k, max_row])= Q(:, [max_row, k]);

       
        if k > 1
            L([k, max_col], 1:k-1)= L([max_col, k], 1:k-1);
        end

        
        for i = k+1:n
            L(i, k)= U(i, k) / U(k, k);
            U(i, k:n)= U(i, k:n) - L(i, k) * U(k, k:n);
        end
    end
end
