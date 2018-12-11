library(rvest)
library(dplyr)
library(stringr)
library(ggmap)
library(maps)
library(ggrepel)
library(geosphere)
######################################################################
## Download and clean the data

parksURL = "https://en.wikipedia.org/w/index.php?title=List_of_national_parks_of_the_United_States"

temp <- parksURL %>%
  read_html %>%
  html_nodes("table")

df = html_table(temp[1])[[1]] # loaded the parks table
df$Image <- NULL ## ignore empty images col

str(df)
length(df$Name)

## let's extract proper coordinates

## Clean names
## df$Name <- str_trim(str_match(df$Name,"([A-Za-z ].+).+")[,2]) # will adjust later

coords = str_match(df$Location,"/ ([^/]+) [(]")[,2]
coords = str_match(coords,"^(.+); (.+)[^0-9]+$")

df$Lat = as.numeric(coords[,2]);
df$Lon = as.numeric(coords[,3]);

df$Loc = str_match(df$Location,"^([A-Za-z ]+).+")[,2]

## drop some parks for simplicity

drop_loc = c("Alaska", "Hawaii", "American Samoa")
drop_parks = c("Virgin Islands", "Dry Tortugas", "Biscayne", "Isle Royale") # Mostly based on islands

df = filter(df, !((Loc %in% drop_loc) | (Name %in% drop_parks)))
length(df$Name)

df = select(df, Name, Loc, Lon, Lat, Description);

df = rbind(data.frame(Name="Clemson University", Loc = "South Carolina", Lon = -82.836410, Lat = 34.676709, Description="Start/end point of the trip (Cooper's library)"),df);

write.csv(df,file="data/parks.csv")

######################################################################
## Save a distance map

D = distm(x=cbind(df$Lon, df$Lat), y=cbind(df$Lon, df$Lat)) / 1000 ## calculate distances in km (straight line)
rownames(D) <- df$Name
colnames(D) <- df$Name
write.csv(D,file="data/dist_line.csv")

######################################################################
## draw a preliminary map
states <- map_data("state")

ggplot(data = states) +
  geom_polygon(aes(x = long, y = lat, fill = region, group = group), color = "white") + 
  coord_fixed(1.3) +
  guides(fill=FALSE)+  # do this to leave off the color legend
  geom_point(data=df, aes(x=Lon,y=Lat))+
  geom_text_repel(data=df,aes(x=Lon,y=Lat, label=Name))+
  xlab("Longitude")+
  ylab("Latitude")+
  labs(caption="MATH 8100 IE Team")+
  ggtitle("U.S. National Parks under consideration")

ggsave("data/parks.png",width = 16, height = 10)

######################################################################
## draw the resulting map

milestones = c("Clemson University", "Congaree", "Yellowstone", "Yosemite *")

steps = c("Clemson University", "Congaree", "Gateway Arch", "Wind Cave", "Yellowstone", "Yosemite *");

path = data.frame(From = steps[1:(length(steps)-1)], To = steps[2:length(steps)])
path = merge(x=path, y=select(df, Name, Lon, Lat), by.x = "From", by.y = "Name")
path = merge(x=path, y=select(df, Name, Lon, Lat), by.x = "To", by.y = "Name")

ggplot(data = states) +
  geom_polygon(aes(x = long, y = lat, fill = region, group = group), color = "white") + 
  coord_fixed(1.3) +
  guides(fill=FALSE)+  # do this to leave off the color legend
  geom_segment(data=path, aes(x=Lon.x, y=Lat.x, xend=Lon.y, yend=Lat.y), arrow = arrow(length=unit(0.30,"cm")), color="blue", size=2)+
  geom_point(data=df, aes(x=Lon,y=Lat))+
  geom_label_repel(data=filter(df, !(Name %in% milestones)),aes(x=Lon,y=Lat, label=Name), alpha=0.7)+
  geom_point(data=filter(df, Name %in% milestones), aes(x=Lon,y=Lat),color="blue", size=4)+
  geom_label_repel(data=filter(df, Name %in% milestones),aes(x=Lon,y=Lat, label=Name), alpha=0.7, color="blue",size=8)+
  xlab("Longitude")+
  ylab("Latitude")+
  labs(caption="MATH 8100 IE Team")+
  ggtitle("National Parks Tour -- the solution: Clemson -> Congaree -> Gateway Arch -> Wind Cave -> Yellowstone -> Yosemite")

ggsave("data/parks-sol.png",width = 16, height = 10)
