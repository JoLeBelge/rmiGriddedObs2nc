#ifndef IRMDATA_H
#define IRMDATA_H
#include <netcdfcpp.h>
#include "date.h"
#include <string.h>
#include <vector>
#include <boost/filesystem.hpp>
#include <numeric>
#include <string>
#include <boost/range/algorithm/remove_if.hpp>
#include <boost/algorithm/string.hpp>

using namespace date;

class dataOnePix;
class dataOneDate;
class irmData;

std::vector<std::vector<std::string>> parseCSV2V(std::string aFileIn);

class irmData
{
public:

    irmData(std::string aFileIRM);
    void saveNetCDF(std::string aOut);

protected:
    std::map<year_month_day,dataOneDate> mVAllDates;
    // contient les variables qui sont lues du fichier csv - le nom c++ - et le nom à sauver dans le netcdf
    std::vector<std::pair<std::string,std::pair<std::string,std::string>>> mVVars;
    std::string mInPutFile;
};

class dataOneDate
{
public:
    dataOneDate(year_month_day ymd);

    void addOnePix(std::vector<std::string> & aLigne);
    void addOnePix(std::vector<std::string> & aLigne, int id);
    void addOneDate(dataOneDate * dod);
    year_month_day getDate(){return mDate;}

    double getValForPix(int pixel_id,std::string aVar);
private:
    // key ; pixel id. val : toutes les données.
    std::map<int,dataOnePix> mVData;
    year_month_day mDate;
};

class dataOnePix
{
public:
    dataOnePix(std::vector<std::string> & aLigne);
    void addOneDate(dataOnePix * dop);
    void cat(){std::cout << "Tmean " << Tmean << " Tmax " << Tmax << " Tmin " << Tmin << " P " << P << std::endl;}

    double Tmean,Tmax,Tmin,R,ETP, P, WS, RelHumid,SunDuration;
};

#endif // IRMDATA_H
