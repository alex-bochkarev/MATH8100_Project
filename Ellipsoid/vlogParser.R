library(readr)
library(stringr)
library(ggplot2)
library(car)

mydata = read_file("logs/2d-for-pic.log")

# extract the basic problem info
Atext  = str_match_all(mydata, "\nA matrix is:\n([e0-9,.[-][+][:space:]]+)\n\n")[[1]][,2]
A = read.table(text = Atext);

btext = str_match_all(mydata, "rhs is:\n([e0-9,.[-][+][:space:]]+)\n\n")[[1]][,2]
b = read.table(text=btext)


# extract the algorithm steps
Ctext = str_match_all(mydata, "\ncenter:\n([e0-9,.-[+][:space:]]+)\n")[[1]][,2]
Htext = str_match_all(mydata, "\nshape:\n([e0-9,.-[+][:space:]]+)\n")[[1]][,2]
Wtext = str_match_all(mydata, "\nwt:\n([e0-9,.-[+][:space:]]+)\n")[[1]][,2]


H = lapply(Htext, function(S) as.matrix(read.csv(text = S,header = FALSE)))
C = lapply(Ctext, function(S) as.vector(read.csv(text = S,header = FALSE)))
wt = lapply(Wtext, function(S) as.vector(read.csv(text = S,header = FALSE)))

H
C
wt

## try to draw
p = ggplot()

Mx = 10
My = 5
## prepare steps for the constraints graph
x1 = seq(-Mx,Mx,length.out = 300);
x2 = seq(-My,My,length.out = 300);

for(i in 1:nrow(A)){
  if(A[i,1]!=0){
    df = data.frame(X=b$V1[i]/A[i,1] - x2*A[i,2]/A[i,1], Y = x2);
    p = p + geom_point(data=df,aes(x=X, y=Y),color="blue",size = 0.1);
  }else{
    df = data.frame(X=x1, Y = b$V1[i]/A[i,2] - x1*A[i,1] / A[i,2]);
    p = p + geom_point(data=df,aes(x=X, y=Y),color="blue",size = 0.1);
  }
}


showEllipse <- function(H,c,Mx,My,p0,s){
  ell = ellipse(draw = FALSE, center = c, shape = as.matrix(H), radius = 1,segments=s);
  ell = ell[(abs(ell[,1])<= Mx) & (abs(ell[,2])<=My),];

  p = p0;
  if(nrow(ell)>0){
    df = data.frame(X = ell[,1], Y = ell[,2]);
    p <- p+geom_point(data=df,aes(x=X,y=Y), color="red",size=0.1);
  };

  if(abs(c[1])<=Mx & abs(c[2])<=My){
    df = data.frame(X = c[1], Y = c[2]);
    p <- p + geom_point(data=df,aes(x=X,y=Y), color="red",size=2);
  }

  return(p);
}

## get some ellipses
i <- 50;

f = showEllipse(as.matrix(H[[i]]),C[[i]]$V1,Mx,My,p,500)
f
w = wt[[i]]$V1;

if(w[1]>0){
  df = data.frame(X = x1, Y = t(w2)*x2)
}
c0 = C[[10]]$V1;
H0 = H[[10]];


abs(ell[,1])<=10 & abs(ell[,2]) <=10
