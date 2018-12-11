# MATH 8100 Project: Fall 2018, the IE team
- **Course deadline:** Dec. 7th, 2018
- This repository contains main code and auxiliary materials (including generated test-instances) for the Ellipsoid algorithm part of the project as well as some technical scripts for producing figures in the National Parks problem.

### National Parks problem (no. 7)
Problem folder is [NationalParks](./NationalParks/).
- park coordinates / distance matrix 
	+ all the data are saved in the [data](./NationalParks/data) folder.
	+ all the aux code (in [R](https://www.r-project.org/)) is in the `getParks.R` file (I'll amend it as/if we move to road distances instead) 

### Ellipsoid method
- written in C++, compiled under Linux with `armadillo` and `blas` libraries;
- all the sources are in the [src](./Ellipsoid/src) folder;
- test cases data are in the [data](./Ellipsoid/data) folder, the format is (hopefully) self-explanatory.
- test runs are summarized in a [table](./Ellipsoid/logs/tests.log), logs for specific runs can be found the the [logs](./Ellipsoid/logs/) folder

If you have any questions -- please feel free to reach us by mail or open an "issue" here on Github.
