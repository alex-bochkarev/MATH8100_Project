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
df$Name <- trimws(str_match(df$Name,"([A-Za-z ].+).+")[,2])

coords = str_match(df$Location,"/ ([^/]+) [(]")[,2]
coords = str_match(coords,"^(.+); (.+)[^0-9]+$")

df$Lat = as.numeric(coords[,2]);
df$Lon = as.numeric(coords[,3]);

df$Loc = str_match(df$Location,"^([A-Za-z ]+).+")[,2]

## drop some parks for simplicity

drop_loc = c("Alaska", "Hawaii", "American Samoa")
drop_parks = c("Virgin Islands", "Dry Tortugas", "Biscayne", "Isle Royale") # Mostly based on islands

df = filter(df, !(Loc %in% drop_parks) & !(Name %in% drop_parks))
length(df$Name)

write.csv(select(df,Name,Loc,Lon,Lat, Description),file="data/parks.csv")

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
