# Simple app to convert RMI gridded observation to netcdf format. 

Belgian RMI (Royal Meteorological Institute) provides open data related to weather forecasts, observations, and climatological data [here](https://opendata.meteo.be/download). Gridded observations are provided in monthly csv file.

Nc file are very convenient, for example for processing with cdo software. This simple code convert csv file to nc format

## Usage:

> ./irm2nc --in /home/jo/Documents/Scolyte/Data/climat_Sco/IRM/climateGrid/climategrid_202408.csv --inputGrid /home/jo/app/irmGriddedObs2nc/in/grilleIRMGDL.nc --inputGridDef /home/jo/app/irmGriddedObs2nc/in/gridIRMGDL.txt --out=/home/jo/Documents/Scolyte/Data/climat_Sco/IRM/climateGrid/test.nc

## Linux Installation

>git clone https://github.com/JoLeBelge/rmiGriddedObs2nc

>git submodule init

>git submodule update

### dependencies

>sudo apt install libnetcdf-c++4 libnetcdf-cxx-legacy-dev cdo

compilation is then performed with qmake (from Qt creator e.g.)
