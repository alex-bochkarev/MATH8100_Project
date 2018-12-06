%% init A for 2d case
A = [-1,1;
1,-1;
-1,-1];
 
b = [1 1 1]';
 
c = [1 1]';


cvx_begin
    variable x(2);
    minimize (c' * x)
    A*x <= b
cvx_end

x

%%

%% generate some random matrices

A=round(rand(20,20)-1/2,3)
A=[A*10;-eye(20)]
b=round(rand(20,1)*100,3)
b=[b; zeros(20,1)]
c=round((rand(20,1)-1/2)*100,3)

%% check if it is feasible

cvx_begin
    variable x(20);
    minimize (c' * x)
    A*x <= b
cvx_end

x

%% save the matrix and params
csvwrite("A.data",A)
csvwrite("b.data",b)
csvwrite("c.data",c)
csvwrite("x.data",x)

%% loading back
