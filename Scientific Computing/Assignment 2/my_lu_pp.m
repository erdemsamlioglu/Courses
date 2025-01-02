function [L, U, P] = my_lu_pp(A)
    
    n=size(A, 1);

    
    L=eye(n);
    U=A;
    P=eye(n);

    %partial pivoting
    for k = 1:n-1
        [~, m]= max(abs(U(k:n, k)));
        m= m + k - 1;

        
        U([k, m], :)= U([m, k], :);

        
        P([k, m], :)= P([m, k], :);

        
        if k > 1
            L([k, m], 1:k-1)= L([m, k], 1:k-1);
        end

        
        for i = k+1:n
            L(i, k)= U(i, k) / U(k, k);
            U(i, k:n)= U(i, k:n) - L(i, k) * U(k, k:n);
        end
    end
end
