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


read_caselog <- function(filename){
  mydata = read_file(filename)

  ## extract the basic problem info
  Atext  = str_match_all(mydata, "\nA matrix is:\n([e0-9,.[-][+][:space:]]+)\n\n")[[1]][,2]
  A = read.table(text = Atext);

  btext = str_match_all(mydata, "rhs is:\n([e0-9,.[-][+][:space:]]+)\n\n")[[1]][,2]
  b = read.table(text=btext)$V1

  ctext = str_match_all(mydata, "Costs vector is:\n([e0-9,.[-][+][:space:]]+)\n\n")[[1]][,2]
  c = read.table(text=ctext)$V1

  ## extract the algorithm steps
  Ctext = str_match_all(mydata, "\ncenter:\n([e0-9,.-[+][:space:]]+)\n")[[1]][,2]
  Htext = str_match_all(mydata, "\nshape:\n([e0-9,.-[+][:space:]]+)\n")[[1]][,2]
  Wtext = str_match_all(mydata, "\nwt:\n([e0-9,.-[+][:space:]]+)\n")[[1]][,2]


  H = lapply(Htext, function(S) as.matrix(read.csv(text = S,header = FALSE)))
  C = lapply(Ctext, function(S) as.vector(read.csv(text = S,header = FALSE)))
  wt = lapply(Wtext, function(S) as.vector(read.csv(text = S,header = FALSE)))


  convdata = str_match_all(mydata, "CONV. SUMMARY: Step=([0-9]+); criterion=([infe[.]0-9[+][-]]+); E.vol=([e[.]0-9[+][-]]+); df=([inf[.]0-9[+][-]]+)")[[1]];

  convd = data.frame(step = as.numeric(convdata[,2]), crit = as.numeric(convdata[,3]), vol = as.numeric(convdata[,4]), df = -as.numeric(convdata[,5]))

  return(list(A=A,b=b,c=c,H=H,C=C,wt=wt,convdata=convd));
}

## try to draw
p = ggplot()

Mx = 7
My = 3
## prepare steps for the constraints graph
x1 = seq(-Mx,Mx,length.out = 300);
x2 = seq(-My,My,length.out = 300);


  cased = read_caselog("logs/2d-for-pic.log");
  A = cased$A;
  b = cased$b;
  c = cased$c;
  H = cased$H;
  C = cased$C;
  wt = cased$wt;

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


showPic <- function(i,g,xmin=-2,xmax=7.5,ymin=-0.5,ymax=3.5,s=500,title="",showPrev=TRUE){
  if(showPrev) {
    p=showEllipse(H[[i-1]],C[[i-1]]$V1,xmin,xmax,ymin,ymax,g,100,col="grey25");
  }else p = g;

  p=showEllipse(H[[i]],C[[i]]$V1,xmin,xmax,ymin,ymax,p,s)+
    ggtitle(paste(title," -- Step: ",i,sep=""))+
    theme(plot.title = element_text(size = 10, face = "bold",hjust = 0, vjust = 0.5))

  if(showPrev) {
    showHyperplane(wt[[i-1]]$V1,C[[i-1]]$V1,p)
  }else p
}


png("images/ell-steps.png",width = 7, height = 10, units = "in",res=600)
grid.arrange(
  grobs = list(showPic(51,g,s=1000,title="Panel A",showPrev = FALSE),
               showPic(52,g,s=1000,title="Panel B"),
               showPic(53,g,s=1000,title="Panel C"),
               showPic(54,g,s=1000,title="Panel D"),
               showPic(65,g,s=1000,title="Panel E"),
               showPic(70,g,s=1000,title="Panel F"),
               showPic(75,g,s=1000,title="Panel G"),
               showPic(80,g,s=1000,title="Panel H")),
  widths = c(1, 1),
  layout_matrix = rbind(c(1, 2),
                        c(3, 4),
                        c(5, 6),
                        c(7,8)))

dev.off()

######################################################################
## A non-full-dimensional case figure
dfp2 = data.frame(X = c(1),Y = c(1));
dfpA = cbind(dfp2, data.frame(Xe=3,Ye=3));

gg= ggplot(dfp2)+
  geom_point(aes(x=X,y=Y), color="blue", size=5)+
  geom_segment(data = dfpA,
               aes(x=X, xend=Xe, y=Y, yend=Ye), size = 2,color="blue",
               arrow = arrow(length = unit(0.5, "cm")))+
  xlim(0.5,3)+xlab("x1")+
  ylim(0.5,3)+ylab("x2")

for(i in 0:8)
  gg=showHyperplane(c(1,1),c(i,0),gg,col="grey75",linetype="dashed")

gg
ggsave("images/ell-non-full-dim.png", width = 5, height = 4, units = "in")

######################################################################
## Convergence tests graphs

## Extract data
## randcaseNN10x20_16

badc = read_caselog("logs/randcaseF10x15_30-verb.log")
badc2 = read_caselog("logs/randcaseF10x15_47.log")
badc3 = read_caselog("logs/randcaseNN10x20_16.log")

## Make plots
ggplot(badc3$convdata)+
  geom_line(aes(x=step, y=crit), color="red")+
  geom_line(aes(x=step,y=df),color="blue")+
  xlim(1000,2500)+
  ylim(-5,5)+
  ylab("criterion")+xlab("step")



ggplot(filter(badc3$convdata,step>2000))+
  geom_line(aes(x=step, y=crit,color="criterion"))+
  geom_line(aes(x=step,y=-df,color="delta f"))+
  ggtitle("Bad case: NN10x20_16")

ggplot(badc$convdata)+
  geom_line(aes(x=step, y=log(vol)), color="orange")

goodc = read_caselog("logs/randcaseF10x15_17-verb.log")
goodc2 = read_caselog("logs/randcaseF10x15_46.log")
goodc3 = read_caselog("logs/randcaseNN10x15_14.log")

ggplot(goodc$convdata)+
  geom_line(aes(x=step, y=crit), color="red")+
  geom_line(aes(x=step,y=df),color="blue")+
  xlim(0,max(goodc$convdata$step))+
  ylim(-20,20)+
  ylab("criterion")+xlab("step")


p1b = ggplot(badc$convdata)+
  geom_line(aes(x=step, y=log(crit),color="criterion"))+
  geom_line(aes(x=step,y=log(-df),color="delta f"))+
  ggtitle("Bad case: F10x15_30")

p2b = ggplot(badc2$convdata)+
  geom_line(aes(x=step, y=log(crit),color="criterion"))+
  geom_line(aes(x=step,y=log(-df),color="delta f"))+
  ggtitle("Bad case: F10x15_47")

p3b = ggplot(badc3$convdata)+
  geom_line(aes(x=step, y=log(crit),color="criterion"))+
  geom_line(aes(x=step,y=log(-df),color="delta f"))+
  ggtitle("Bad case: NN10x20_16")

p1g = ggplot(goodc$convdata)+
  geom_line(aes(x=step, y=log(crit),color="criterion"))+
  geom_line(aes(x=step,y=log(-df),color="delta f"))+
  ggtitle("Good case: F10x15_17")

p2g = ggplot(goodc2$convdata)+
  geom_line(aes(x=step, y=log(crit),color="criterion"))+
  geom_line(aes(x=step,y=log(-df),color="delta f"))+
  ggtitle("Good case: F10x15_46")

p3g = ggplot(goodc3$convdata)+
  geom_line(aes(x=step, y=log(crit),color="criterion"))+
  geom_line(aes(x=step,y=log(-df),color="delta f"))+
  ggtitle("Good case: NN10x15_14")

grid.arrange(p1g,p2g,p3g,p1b,p2b,p3b,ncol=3)

pcase = read_caselog("logs/spec-F10x15_17_plain.log")
kcase = read_caselog("logs/spec-F10x15_17_kha.log")

p1 = ggplot(pcase$convdata)+
  geom_line(aes(x=step, y=log(crit),color="criterion"))+
  geom_line(aes(x=step,y=log(-df),color="delta f"))+
  ggtitle("Good case: F10x15_17, Plain update")

p2 = ggplot(kcase$convdata)+
  geom_line(aes(x=step, y=log(crit),color="criterion"))+
  geom_line(aes(x=step,y=log(-df),color="delta f"))+
  ggtitle("Good case: F10x15_17, Khachiyan update")


grid.arrange(p1,p2,ncol=2)


p1 = ggplot()+
  geom_line(data=pcase$convdata, aes(x=step, y=log(crit),color="plain"))+
  geom_line(data=kcase$convdata, aes(x=step, y=log(crit),color="Khachiyan"))+
  ggtitle("Good case: F10x15_17, log(criterion)")

p2 = ggplot()+
  geom_line(data=pcase$convdata, aes(x=step,y=log(-df),color="plain"))+
  geom_line(data=kcase$convdata, aes(x=step,y=log(-df),color="Khachiyan"))+
  ggtitle("Good case: F10x15_17, delta f")


grid.arrange(p1,p2,ncol=2)

pcase2 = read_caselog("logs/testcase-20d.log")
kcase2 = read_caselog("logs/spec-20dcase.log")

p1 = ggplot()+
  geom_line(data=pcase2$convdata, aes(x=step, y=log(crit),color="plain"))+
  geom_line(data=kcase2$convdata, aes(x=step, y=log(crit),color="Khachiyan"))+
  ggtitle("Good case: F10x15_17, log(criterion)")

p2 = ggplot()+
  geom_line(data=pcase2$convdata, aes(x=step,y=log(-df),color="plain"))+
  geom_line(data=kcase2$convdata, aes(x=step,y=log(-df),color="Khachiyan"))+
  ggtitle("Good case: F10x15_17, delta f")

grid.arrange(p1,p2,ncol=2)

######################################################################
## Klee-Minty cubes example

st = 1:10

st = lapply(3:12, function(i) {
  dt = read_caselog(paste("logs/KMC_",i,".log",sep=""));
  st[i-2] = max(dt$convdata$step);
})

kmc = data.frame(D = 3:12, steps = unlist(st));
kmc$stepsL = c(NA, kmc$steps[1:(length(kmc$steps)-1)]); 
kmc$ratio = kmc$steps / kmc$stepsL

ggplot(kmc)+
  geom_point(aes(x=D,y=ratio),color="blue")+
  geom_line(aes(x=D,y=ratio),color="blue")

ggplot(kmc)+
  geom_point(aes(x=log(D),y=log(steps)),color="blue")+
  geom_line(aes(x=log(D),y=log(steps)),color="blue")
