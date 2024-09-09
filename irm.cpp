#include "irm.h"

extern std::string inputGrid, inputGridDef;
extern std::string pathOut;
// la colonne dans laquelle est stoqué l'info - varie d'un fichier à un autre.
int colDate(0),colTmean(0),colTmax(0),colTmin(0),colR(0),colETP(0),colP(0), colWS(0),colRH(0), colSD(0), colPix_id(0);

year_month_day baseymd = 1950_y/1/1;

double NoD(-999);

irmData::irmData(std::string aFileIRM):mInPutFile(aFileIRM)
{
    std::cout << " lecture des données irm " << std::endl;
    std::vector<std::vector<std::string>> d=parseCSV2V(aFileIRM);
    std::cout << " fichier texte parsé " << std::endl;

    // détermine les numéro de colonnes pour les variables
    std::vector<std::string> headerline=d.at(0);
    for (int c(0);c<headerline.size();c++){
        std::string header=headerline.at(c);
        std::cout << header << " est la colonne " << c << std::endl;
        if (header.find("TEMPERATURE AVG")!=std::string::npos | header.find("temp_avg")!=std::string::npos){colTmean=c;mVVars.push_back(std::make_pair("Tmean",std::make_pair("TG","TEMPERATURE AVG")));}
        if (header.find("TEMPERATURE MAX")!=std::string::npos | header.find("temp_max")!=std::string::npos){colTmax=c;mVVars.push_back(std::make_pair("Tmax",std::make_pair("TX","TEMPERATURE MAX")));}
        if (header.find("TEMPERATURE MIN")!=std::string::npos | header.find("temp_min")!=std::string::npos){colTmin=c;mVVars.push_back(std::make_pair("Tmin",std::make_pair("TN","TEMPERATURE MIN")));}
        if (header.find("PRECIPITATION (mm)")!=std::string::npos | header.find("precip_quantity")!=std::string::npos){colP=c;mVVars.push_back(std::make_pair("P",std::make_pair("RF","PRECIPITATION (mm)")));}
        if (header.find("RELATIVE HUMIDITY")!=std::string::npos | header.find("humidity_relative")!=std::string::npos){colRH=c;mVVars.push_back(std::make_pair("RH",std::make_pair("RH","RELATIVE HUMIDITY (%)")));}
        if (header.find("sun_duration")!=std::string::npos){colSD=c;mVVars.push_back(std::make_pair("SD",std::make_pair("SD","SUN DURATION (h)")));}
        if (header.find("ET" )!=std::string::npos | header.find("evapotrans_ref")!=std::string::npos){colETP=c;mVVars.push_back(std::make_pair("ETP",std::make_pair("ETP", "Evapotranspiration Potentielle ET0 ")));}
        if (header.find("WIND SPEED")!=std::string::npos){colWS=c;mVVars.push_back(std::make_pair("WS",std::make_pair("WS","WIND SPEED (m/s)")));}
        if (header.find("GLOBAL RADIATION")!=std::string::npos| header.find("SHORT_WAVE_FROM_SKY")!=std::string::npos){colR=c; mVVars.push_back(std::make_pair("R",std::make_pair("QQ","SHORT WAVE FROM_SKY (kWh/m2/day)")));}
        if (header.find("day")!=std::string::npos){colDate=c;}
        if (header.find("pixel_id")!=std::string::npos){colPix_id=c;}
    }
    std::cout << "colPix_id" << colPix_id << " , colDate \n" << colDate << " Radiation colonne " << colR << " T mean colonne " << colTmean << " T max colonne " << colTmax << " T min colonne " << colTmin << "\n" << " Précipitation colonne " << colP << " ET0 colonne " << colETP << " , Wind speed colonne " << colWS << std::endl;

    std::string curDate("");
    for (int c(1); c<d.size();c++){
        std::vector<std::string> line=d.at(c);
        std::string aDate=line.at(colDate);
        aDate.erase(boost::remove_if(aDate, boost::is_any_of("\"")), aDate.end());

        // fonctionnement polyvalent si moyenne trentenaire - en fonction de la taille du champ date il définit le mode de lecture des données
        int d(1),m(1),y(1);
        //std::cout << "aDate " << aDate << std::endl;
        if (aDate.size()==5){
            d=std::stoi(aDate.substr(3,5));
            m=std::stoi(aDate.substr(0,2));
            y=1991;
            //std::cout << " day " << d << " , " << " month " << m << std::endl;
        }else{
            // une nouvelle date
            d=std::stoi(aDate.substr(8,9));
            m=std::stoi(aDate.substr(5,6));
            y=std::stoi(aDate.substr(0,4));
        }

        year_month_day ymd(year{y},month{m},day{d});
        if ( curDate!=aDate){
            //std::cout << " création des données irm pour une date , y " << y << " m " << m << " d " << d << std::endl;
            mVAllDates.emplace(std::make_pair(ymd,dataOneDate(ymd)));
            mVAllDates.at(ymd).addOnePix(line);
            curDate=aDate;
        } else {
            mVAllDates.at(ymd).addOnePix(line);
        }
    }
}

dataOneDate::dataOneDate(year_month_day ymd){
    //std::cout << " création des données irm pour une date , " << ymd << std::endl;
    mDate=ymd;
}

void dataOneDate::addOnePix(std::vector<std::string> & aLigne){
    mVData.emplace(std::make_pair(std::stoi(aLigne.at(colPix_id)),dataOnePix(aLigne)));
}

void dataOneDate::addOnePix(std::vector<std::string> & aLigne, int id){
    mVData.emplace(std::make_pair(id,dataOnePix(aLigne)));
}

dataOnePix::dataOnePix(std::vector<std::string> & aLigne):Tmean(NoD),Tmax(NoD),Tmin(NoD),R(NoD),ETP(NoD),P(NoD),WS(NoD),SunDuration(0),RelHumid(0){
    if(colTmean!=0){try{Tmean=std::stod(aLigne.at(colTmean));}catch (...) {}}
    if(colTmax!=0){try{Tmax=std::stod(aLigne.at(colTmax));}catch (...) {}}
    if(colTmin!=0){try{Tmin=std::stod(aLigne.at(colTmin));}catch (...) {}}
    if(colR!=0){try{R=std::stod(aLigne.at(colR));}catch (...) {}}
    if(colP!=0){try{P=std::stod(aLigne.at(colP));}catch (...) {}}
    if(colETP!=0){try{ETP=std::stod(aLigne.at(colETP));}catch (...) {}}
    if(colWS!=0){try{WS=std::stod(aLigne.at(colWS));}catch (...) {}}
    if(colRH!=0){try{RelHumid=std::stod(aLigne.at(colRH));}catch (...) {}}
    if(colSD!=0){try{SunDuration=std::stod(aLigne.at(colSD));}catch (...) {}}
}

double dataOneDate::getValForPix(int pixel_id,std::string aVar){
    double aRes(-999);
    if (mVData.find(pixel_id)!=mVData.end()){
        if (aVar=="Tmean"){
            aRes=mVData.at(pixel_id).Tmean;
        } else if (aVar=="Tmax"){
            aRes=mVData.at(pixel_id).Tmax;
        } else if (aVar=="Tmin"){
            aRes=mVData.at(pixel_id).Tmin;
        } else if (aVar=="Tmean"){
            aRes=mVData.at(pixel_id).Tmean;
        } else if (aVar=="ETP"){
            aRes=mVData.at(pixel_id).ETP;
        } else if (aVar=="R"){
            aRes=mVData.at(pixel_id).R;
        } else if (aVar=="P"){
            aRes=mVData.at(pixel_id).P;
        } else if (aVar=="WS"){
            aRes=mVData.at(pixel_id).WS;
        } else if (aVar=="SD"){
            aRes=mVData.at(pixel_id).SunDuration;
        }
    }
    return aRes;
}


void irmData::saveNetCDF(std::string aOut){
    int NX(66),NY(55);
    std::string aOutTmp=aOut+"-tmp.nc";

    // GRILLE IRM ---------------------------------
    // ouvrir la grille IRM qui me sert de template (j'ai l'id du pixel dedans)
    NcFile grille(inputGrid.c_str(),NcFile::FileMode::ReadOnly,NULL,0,NcFile::FileFormat::Netcdf4);
    float var_in[NY][NX];

    // CREATION NOUVEAU NETCDF  ---------------------------------
    int NTIME(mVAllDates.size());
    std::cout << "total de " << NTIME << " TIMES " << std::endl;

    NcFile ncOut(aOutTmp.c_str(),NcFile::FileMode::Replace);
    ncOut.add_att("institution","IRM");
    std::string source="IRM -gridded observations from file "+mInPutFile;
    ncOut.add_att("data_source",source.c_str());
    ncOut.add_att("gridXY","5km IRM Grid enlarged to include Grand-Duché de Luxembourg");
    ncOut.add_att("srcGridXY","+proj=lcc +lat_2=50.569898649999999 +lat_1=50.569898649999999 +lon_0=4.553615160000000 +units=m +no_defs +a=6371229.0 +es=0.0");
    //ncOut.add_att("contact","Lisein Jonathan - liseinjon@hotmail.com");
    NcDim * dimTime = ncOut.add_dim("TIME");
    ncOut.add_dim("bnds",2);
    //grille de l'IRM
    NcDim * dimX = ncOut.add_dim("X",NX);
    NcDim * dimY = ncOut.add_dim("Y",NY);
    // variable TIME (pas dimension mais variable associée)
    NcVar* varTime  = ncOut.add_var("TIME",ncFloat,dimTime);
    varTime->add_att("units","days since 1950-01-01 00:00:00");
    varTime->add_att("standard_name","time");
    varTime->add_att("time_origin","01-JAN-1950 00:00:00");
    varTime->add_att("bounds","TIME_bnds"); // https://daac.ornl.gov/submit/netcdfrequirements/
    ncOut.add_var("TIME_bnds",ncFloat,dimTime);

    NcVar *idVar=grille.get_var("LON");
    if (!idVar->get(&var_in[0][0], 1, NY, NX)){
        std::cout << "getvarin bad\n" << std::endl;
    }
    NcVar* varLON  = ncOut.add_var("LON",ncDouble,dimY,dimX);
    varLON->put(&var_in[0][0],NY,NX);

    idVar=grille.get_var("LAT");
    if (!idVar->get(&var_in[0][0], 1, NY, NX)){
        std::cout << "getvarin bad\n" << std::endl;
    }
    NcVar* varLAT  = ncOut.add_var("LAT",ncDouble,dimY,dimX);
    varLAT->put(&var_in[0][0],NY,NX);

    idVar=grille.get_var("ID");

    if (!idVar->get(&var_in[0][0], 1, NY, NX)){
        std::cout << "getvarin bad\n" << std::endl;
    }
    NcVar* varPixID  = ncOut.add_var("PIXEL_ID",ncInt,dimY,dimX);
    varPixID->put(&var_in[0][0],NY,NX);
    varPixID->add_att("name","identifiant du pixel de 5kmx5km de la grille de l'IRM");
    varPixID->add_att("_FillValue",0);

    float timebuf[NTIME];
    int t(0);
    for (auto & kv : mVAllDates){
        year_month_day ymd = kv.first;
        days d=(sys_days{ymd}-sys_days{baseymd});
        timebuf[t]=d.count();
        t++;
    }
    varTime->put(&timebuf[0],NTIME);
    // create and populate the variables
    for (auto & pair : mVVars){
        std::string aVar =pair.first;
        //std::cout << "ajout variables " << aVar << std::endl;
        NcVar* myvar  = ncOut.add_var(pair.second.first.c_str(),ncDouble,dimTime,dimY,dimX);
        myvar->add_att("nom",pair.second.second.c_str());
        myvar->add_att("_FillValue",-999.0);
        // sur longue série temporelle, je ne parvient pas à créer un tableau si grand. il faut chunker je suppose. par temps, c'est le plus logique
        t=0;
        for (auto & kv : mVAllDates){
            //std::cout << "time " << t << std::endl;
            myvar->set_rec(dimTime,t);
            float varbuf[NY][NX];
            //float varbuf[1];
            for (int y = 0; y < NY; y++){
                for (int x = 0; x < NX; x++)
                {
                    varbuf[y][x]=kv.second.getValForPix(var_in[y][x],aVar);
                }
            }
            myvar->put_rec(dimTime,&varbuf[0][0]);
            t++;
        }
        // ça c'est pour écrire toute les données d'un seul coup - fonctionne pas si beaucoup de time..
        //varbuf[t][y][x]=kv.second.getValForPix(var_in[y][x],aVar);
        //myvar->put(&varbuf[0][0][0],NTIME, NY,NX);
    }
    ncOut.close();
    // le Y est à l'envers quand j'ouvre le netcdf dans Qgis; c'est du au fait que la grille n'est pas définie comme dans grilleIRMGDL.nc ou on donne l'extend et la résolution en Y qui est négative.
    // Ajout aussi des variales lat lon
    std::string aCommand="cdo -setgrid,"+inputGridDef+" " + aOutTmp + " " + aOut;
    std::cout << aCommand << std::endl;
    system(aCommand.c_str());
}


std::vector<std::vector<std::string>> parseCSV2V(std::string aFileIn){
    std::vector<std::vector<std::string>> aOut;
    std::ifstream aFichier(aFileIn.c_str());
    if(aFichier)
    {
        std::string aLine;
        while(!aFichier.eof())
        {
            getline(aFichier,aLine,'\n');
            if(aLine.size() != 0)
            {
                std::vector<std::string> result;
                boost::split(result, aLine, boost::is_any_of(";"),boost::token_compress_on);
                aOut.push_back(result);
            }
        }
        aFichier.close();
    } else {
        std::cout << "file " << aFileIn << " not found " <<std::endl;
    }
    return aOut;
}
