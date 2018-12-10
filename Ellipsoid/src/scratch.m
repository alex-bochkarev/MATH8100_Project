%% try to generate
n = 10
m = 7

X = rand(m,n-m)
B = rand(m,m)
bt = rand(m,1)
cB = rand(m,1)
%%
b = B*bt

A = [B B*X]
cN = X' * cB+rand(n-m,1)
c = [cB; cN]

c' - cB'*inv(B)*A

%%
A = [-1.2671  -4.1509   4.6556   4.8490  -0.6408  -1.5868  -1.4751  -4.5380   2.8723   0.5778;
   2.2124   2.9390  -1.7367   1.0600  -2.0158   3.5484   0.4636  -4.2053  -1.8006   3.7595;
   3.0904  -0.7499   3.4418   2.0302  -0.8436  -3.1038   0.0307   1.4260  -3.3161   1.7332;
   0.9060  -4.6563   2.1273   3.9271   4.7708  -1.5231  -0.3412  -3.1519   1.5540   2.6760;
   1.6571   4.2293   1.7603  -0.7819   0.6069   1.4240   4.7163   4.7332   2.5113   2.3030;
   3.8738  -2.6501   0.4057  -3.8490   2.0471  -4.0629   1.9302  -2.8228  -3.3477  -0.3433;
  -0.3001   4.4431   2.7103   3.4253  -2.6144   2.4619   4.4025  -1.7027   0.4541  -0.6806];

b=[0.6274;
   5.1098;
   1.3278;
   3.7374;
   8.6560;
  -1.7243;
   5.7711];

c=[0.0145;
   1.5986;
  -2.3544;
  -3.4849;
   1.1358;
  -0.2618;
   0.7772;
   4.4182;
  -1.1901;
  -1.7232];

%%
d = [-7.3081e+18;
   2.7770e+19;
   3.2090e+19;
  -1.4073e+19;
  -4.3370e+19;
  -2.3865e+19;
   5.4410e+19;
   1.1101e+19;
  -3.5507e+19;
   2.4163e+19];
%% test with cvx

%A = [A; -eye(n)]
%b = [b; zeros(n,1)]
cvx_begin
    variable x(10);
    minimize (c' * x)
    A*x <= b
cvx_end
x
%% my guess
[bt;zeros(n-m,1)]
c'*[bt;zeros(n-m,1)]
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
