function J = computeCost(X, y, theta)
%COMPUTECOST Compute cost for linear regression
%   J = COMPUTECOST(X, y, theta) computes the cost of using theta as the
%   parameter for linear regression to fit the data points in X and y

% Initialize some useful values
m = length(y); % number of training examples

% You need to return the following variables correctly 
J = 0;
% ====================== YOUR CODE HERE ======================
% Instructions: Compute the cost of a particular choice of theta
%               You should set J to the cost.

n = length(X(1,:));
for i = 1:m
    S = 0.0;
    for j = 1:n
        S += X(i, j) * theta(j);
    end
    J += (S - y(i)) * (S - y(i));
end
J /= 2.0 * m;
% =========================================================================

end
