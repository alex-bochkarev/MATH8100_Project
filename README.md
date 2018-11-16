# MATH 8100 Project: Fall 2018, the IE team
- **Course deadline:** Dec. 7th, 2018
- this repo will contain main code and auxiliary materials for the project.

## Contents and TODO's

The project will consist of five problems in total.

### National Parks problem (no. 7)
Problem folder is [NationalParks](./NationalParks/).
- park coordinates / distance matrix -- AB: I have scraped the wiki page and downloaded parks coordinates; what is there:
	+ all the data are saved in the [data](./NationalParks/data) folder.
	+ I have dropped several parks: that were situated in Alaska, Hawaii and American Samoa (according to the problem directions by Dr. Yang) PLUS, additonally, parks that were on islands, viz., Virgin Islands, Dry Tortugas, Biscayne, Isle Royale;
	+ `parks.png` is a map with the parks that I suggest to work with;
	+ `parks.csv` is basic parks data (coordinates, location and description);
	+ `dist_line.csv` is a distance matrix, from where to where, straight line distances, in km. **MURWAN, THIS IS THE FILE YOU NEED** 
	+ all the aux code (in [R](https://www.r-project.org/)) is in the `getParks.R` file (I'll amend it as/if we move to road distances instead) 
- Questions tbd:
	+ if we'll need nice pics/routes on the map -- pls let me know, seems like it is not that difficult to do in R.

- TODO: problem coding -- MS

### Ellipsoid method
- TODO: Method presentation -- AB

### SVM
- TODO: Method presentation -- YC

--------

### Admin questions
- Two other questions -- ??? something simple?
