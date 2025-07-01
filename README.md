# Simple app to convert RMI gridded observation to netcdf format. 

Belgian RMI (Royal Meteorological Institute) provides open data related to weather forecasts, observations, and climatological data [here](https://opendata.meteo.be/download). Gridded observations are provided in monthly csv file.

Nc file are very convenient, for example for processing with cdo software. This simple code convert csv file to nc format

## Usage:

> ./irm2nc --in /home/jo/Documents/Scolyte/Data/climat_Sco/IRM/climateGrid/climategrid_202408.csv --inputGrid /home/jo/app/irmGriddedObs2nc/in/grilleIRMGDL.nc --inputGridDef /home/jo/app/irmGriddedObs2nc/in/gridIRMGDL.txt --out=/home/jo/Documents/Scolyte/Data/climat_Sco/IRM/climateGrid/test.nc

script:

#!/bin/bash

for i in {1..9}
do
    ./irm2nc --in /home/jo/Documents/Scolyte/Data/climat_Sco/IRM/climateGrid/climategrid_20240$i.csv --inputGrid /home/jo/app/irmGriddedObs2nc/in/grilleIRMGDL.nc --inputGridDef /home/jo/app/irmGriddedObs2nc/in/gridIRMGDL.txt --out=/home/jo/Documents/Scolyte/Data/climat_Sco/IRM/climateGrid/20240$i.nc
done

for i in {10..12}
do
    ./irm2nc --in /home/jo/Documents/Scolyte/Data/climat_Sco/IRM/climateGrid/climategrid_2024$i.csv --inputGrid /home/jo/app/irmGriddedObs2nc/in/grilleIRMGDL.nc --inputGridDef /home/jo/app/irmGriddedObs2nc/in/gridIRMGDL.txt --out=/home/jo/Documents/Scolyte/Data/climat_Sco/IRM/climateGrid/2024$i.nc
done

cdo copy 2024*.nc IRM-2024.nc


## Other specifi usage: export txt file of monthly climate for one XY position for usage in Walsi module (CAPSIS)
this tool (outil number 2 called by --outil 2) use as input a directory containing year-based file of monthly gridded-observation. Daily to montly observations may be computed with cdo with the following call (one call for each year): 

cdo -b F64 merge -monsum -selname,RF,ETP /home/jo/Documents/Scolyte/Data/climat_Sco/IRM/nc/IRM-griddedObservation/IRM-1960.nc -monmean -selname,TG /home/jo/Documents/Scolyte/Data/climat_Sco/IRM/nc/IRM-griddedObservation/IRM-1960.nc /home/jo/Documents/Scolyte/Data/climat_Sco/IRM/nc/IRM-griddedObservation/MAR-monthly/IRM-monthly-1960.nc

then time serie for one pixel is extracted and written in a txt file with this command. Toto and tata argument value aren't used, there are only there because the arguments "inputGrid" and "inputGridDef" are mandatory (for tool number 1)

./irm2nc --in /home/jo/Documents/Scolyte/Data/climat_Sco/IRM/nc/IRM-griddedObservation/MAR-monthly --inputGrid toto --inputGridDef tata --out=/home/jo/Documents/suiviTempMAB/data/capsis/irm2walsi.txt --outil 2 --X 60000 --Y 6300000

Be catious, the position of the forest site you provide with X and Y is in the coordinate system of the IRM grid (+proj=lcc +lat_2=50.569898649999999 +lat_1=50.569898649999999 +lon_0=4.553615160000000 +units=m +no_defs +a=6371229.0 +es=0.0) that can be added in QGis as a "custom CRS"

## Linux Installation

>git clone https://github.com/JoLeBelge/rmiGriddedObs2nc

>git submodule init

>git submodule update

### dependencies

>sudo apt install libnetcdf-c++4 libnetcdf-cxx-legacy-dev cdo

compilation is then performed with qmake (from Qt creator e.g.)
