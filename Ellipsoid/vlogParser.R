library(readr)
library(stringr)
library(ggplot2)
library(car)
library(dplyr)

showEllipse <- function(H,c,Mx,My,p0,s){
  ell = ellipse(draw = FALSE, center = c, shape = as.matrix(H), radius = 1,segments=s);
  if(is.null(ell) || length(ell)==0) return(p0);

  ell = ell[(abs(ell[,1])<= Mx) & (abs(ell[,2])<=My),];

  p = p0;
  if(!is.null(ell) && length(ell)>2){
    df = data.frame(X = ell[,1], Y = ell[,2]);
    p <- p+geom_point(data=df,aes(x=X,y=Y), color="red",size=0.1);
  };

  if(abs(c[1])<=Mx & abs(c[2])<=My){
    df = data.frame(X = c[1], Y = c[2]);
    p <- p + geom_point(data=df,aes(x=X,y=Y), color="red",size=2);
  }

  return(p);
}


mydata = read_file("logs/2d-for-pic.log")

# extract the basic problem info
Atext  = str_match_all(mydata, "\nA matrix is:\n([e0-9,.[-][+][:space:]]+)\n\n")[[1]][,2]
A = read.table(text = Atext);

btext = str_match_all(mydata, "rhs is:\n([e0-9,.[-][+][:space:]]+)\n\n")[[1]][,2]
b = read.table(text=btext)$V1

ctext = str_match_all(mydata, "Costs vector is:\n([e0-9,.[-][+][:space:]]+)\n\n")[[1]][,2]
c = read.table(text=ctext)$V1

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

Mx = 7
My = 3
## prepare steps for the constraints graph
x1 = seq(-Mx,Mx,length.out = 300);
x2 = seq(-My,My,length.out = 300);

for(i in 1:nrow(A)){
  if(A[i,1]!=0){
    df = data.frame(X=b[i]/A[i,1] - x2*A[i,2]/A[i,1], Y = x2);
    p = p + geom_point(data=df,aes(x=X, y=Y),color="blue",size = 0.1);
  }else{
    df = data.frame(X=x1, Y = b[i]/A[i,2] - x1*A[i,1] / A[i,2]);
    p = p + geom_point(data=df,aes(x=X, y=Y),color="blue",size = 0.1);
  }
}



## get some ellipses

for (i in 1:100){
  f = showEllipse(as.matrix(H[[i]]),C[[i]]$V1,Mx,My,p,500)
  w = wt[[i]]$V1;
  df = data.frame();

  if(abs(w[2])>0) df = data.frame(X = x1, Y = sum(w*C[[i]]$V1)/w[2] - w[1]*x1/w[2]);
  if(abs(w[1])>0) df = data.frame(X = sum(w*C[[i]]$V1)/w[1] - w[2]*x2/w[1], Y = x2);

  if(nrow(df)>0) df = filter(df, abs(X)<=Mx & abs(Y)<=My)

  if(nrow(df)>0) f=f+geom_line(data=df,aes(x=X, y=Y),size=0.5,color="orange");

  print(f+labs(title=paste("Step: ",i)));
  if(readline()=="q") break;
}

abs(ell[,1])<=10 & abs(ell[,2]) <=10


## Figure preparation for the Ellipsoid algorithm introduction

dfp = data.frame(X = c(1,2,4,5),Y = c(1,2,2,1));

ggplot(dfp)+
  geom_polygon(aes(x=X,y=Y),fill="blue", )
