# CreateSoilFiles
This repository contains  source code for programs to create the grid, node, and soil files for MAIZSIM. The grid file defines the 2D finite element grid and the node file which contains the initial values for the nodes when the crop model begins. 

The system consists of three programs:
1. CreateSoilFiles.exe  - compiled from several C# source files - this program reads the input parameter file an calls the dll to create the grid file template and  then fills the template. It also fills the initial values for the nodes and writes to the node file. It also calls the C++ program (an exe) called Rosetta to calculate soil hydraulic properties for the van Genuchten equation. 
https://www.ars.usda.gov/pacific-west-area/riverside-ca/agricultural-water-efficiency-and-salinity-research-unit/docs/model/rosetta-model/

the input file consists of lines of data that define the soil and grid properties. 
