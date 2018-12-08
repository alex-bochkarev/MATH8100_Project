library(readr)
library(stringr)
library(ggplot2)
library(car)
library(dplyr)
library(ggrepel)
library(gridExtra)

showEllipse <- function(H,c,xmin,xmax,ymin,ymax,p0,s,col="red"){
  ell = ellipse(draw = FALSE, center = c, shape = as.matrix(H), radius = 1,segments=s);
  if(is.null(ell) || length(ell)==0) return(p0);

  ell = ell[(ell[,1]<=xmax) & (ell[,1]>=xmin) & (ell[,2]<=ymax) & (ell[,2]>=ymin),];

  p = p0;
  if(!is.null(ell) && length(ell)>2){
    df = data.frame(X = ell[,1], Y = ell[,2]);
    p <- p+geom_point(data=df,aes(x=X,y=Y), color=col,size=0.1)+xlim(xmin,xmax)+ylim(ymin,ymax);
  };

  if(c[1]<=xmax & c[1]>=xmin & c[2]<=ymax & c[1]>=ymin){
    df = data.frame(X = c[1], Y = c[2]);
    p <- p + geom_point(data=df,aes(x=X,y=Y), color=col,size=2);
  }

  return(p);
}

showHyperplane <- function(w,o,p,col="black",...){
  df = data.frame();

  gr = p;

  if(abs(w[2])>0){
    slope = - w[1] / w[2];
    intr = sum(o*w)/w[2];
    gr = gr + geom_abline(slope = slope, intercept = intr,color=col,...)
  }

  if(abs(w[2])==0 & abs(w[1])>0){
    intr = sum(o*w)/w[2];
    gr = gr + geom_abline(xintercept = intr, color=col,...);
  }

  return(gr);
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



## get some ellipses:


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
dfp$txt = paste("(",dfp$X,",",dfp$Y,")",sep="")

g = ggplot(dfp)+
  geom_polygon(aes(x=X,y=Y),fill="blue", alpha=0.2)+
  geom_polygon(aes(x=X,y=Y),color="blue",fill=NA,size=1)+
  geom_point(aes(x=X,y=Y), color="blue", size=5)+
  theme(
    plot.title = element_blank(),
    axis.title.x = element_blank(),
    axis.title.y = element_blank())

## add some isolines

for(i in -3:7)
  g=showHyperplane(c,c(i,0),g,col="grey75",linetype="dashed")


showPic <- function(i,g,xmin=-2,xmax=7.5,ymin=-0.5,ymax=3.5,s=500,title=""){
  p=showEllipse(H[[i-1]],C[[i-1]]$V1,xmin,xmax,ymin,ymax,g,100,col="grey25")
  p=showEllipse(H[[i]],C[[i]]$V1,xmin,xmax,ymin,ymax,p,s)
  showHyperplane(wt[[i-1]]$V1,C[[i-1]]$V1,p)+
    ggtitle(paste(title," -- Step: ",i,sep=""))+
    theme(plot.title = element_text(size = 10, face = "bold",hjust = 0, vjust = 0.5))
}


png("ell-steps.png",width = 7, height = 10, units = "in",res=600)
grid.arrange(
  grobs = list(showPic(51,g,s=1000,title="A"),
               showPic(52,g,s=1000,title="B"),
               showPic(53,g,s=1000,title="C"),
               showPic(54,g,s=1000,title="D"),
               showPic(65,g,s=1000,title="E"),
               showPic(70,g,s=1000,title="F"),
               showPic(75,g,s=1000,title="G"),
               showPic(80,g,s=1000,title="H")),
  widths = c(1, 1),
  layout_matrix = rbind(c(1, 2),
                        c(3, 4),
                        c(5, 6),
                        c(7,8)))
dev.off()
