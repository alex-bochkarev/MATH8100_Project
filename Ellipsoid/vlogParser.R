library(readr)
library(stringr)

mydata = read_file("my.test")


Ctext = str_match_all(mydata, "\ncenter:\n([e0-9,.-[:space:]]+)\n")[[1]][,2]
Htext = str_match_all(mydata, "\nshape:\n([e0-9,.-[:space:]]+)\n")[[1]][,2]
Wtext = str_match_all(mydata, "\nwt:\n([e0-9,.-[:space:]]+)\n")[[1]][,2]

H = lapply(Htext, function(S) as.matrix(read.csv(text = S,header = FALSE)))
C = lapply(Ctext, function(S) as.vector(read.csv(text = S,header = FALSE)))
wt = lapply(Wtext, function(S) as.vector(read.csv(text = S,header = FALSE)))

H
C
wt
