I= imread('catgrayscale.jpg');

if size(I, 3)== 3
    I= rgb2gray(I);
end

I= double(I);

[U, S, V]= svd(I);

r= min(size(I));

k_rdivided2= round(r / 2);
k_rminus10= r- 10;

Irdivided2=U(:, 1:k_rdivided2) * S(1:k_rdivided2, 1:k_rdivided2) * V(:, 1:k_rdivided2)';
Irmminus10=U(:, 1:k_rminus10) * S(1:k_rminus10, 1:k_rminus10) * V(:, 1:k_rminus10)';

figure;
imshow(normalize(I));
title('Original Image');

figure;
imshow(normalize(Irdivided2));
title(['Low Rank Approximation(r/2) with k= ', num2str(k_rdivided2)]);

figure;
imshow(normalize(Irmminus10));
title(['Low Rank Approximation(r-10) with k= ', num2str(k_rminus10)]);

sigma = diag(S);
S_k = sqrt(cumsum(sigma(end:-1:1).^2));

errors = zeros(1, r);
for k = 1:r
    I_k = U(:, 1:k) * S(1:k, 1:k) * V(:, 1:k)';
    errors(k) = norm(I - I_k, 'fro');
end

%Reverse
S_k = S_k(end:-1:1);

figure;
loglog(1:r, sigma, 'b', 'LineWidth', 2);
hold on;

loglog(1:r, S_k, 'r', 'LineWidth', 2);

loglog(1:r, errors, 'g', 'LineWidth', 2);

legend('Singular Values', 'S(k)', 'Frobenius Norm Error');
xlabel('k');
ylabel('Value');
title('Singular Values, S(k), and Error in Frobenius Norm');
grid on;

function myimgnormalize = normalize(img)
    img_min = min(img(:));
    img_max = max(img(:));
    myimgnormalize = (img - img_min)/(img_max - img_min) * 255;
    myimgnormalize = uint8(myimgnormalize);
end