#include <iostream>
#include "irm.h"
#include "boost/program_options.hpp"
#include "date/include/date/date.h"

using namespace date;
using namespace std;
namespace po = boost::program_options;

std::string input;
std::string inputGrid;
std::string inputGridDef;
std::string pathOut;

std::string irmFile(std::string dir,int y );

// origin and resol of IRM grid
int oX=-145000;
int oY=6.44051e+06;
double res=5000;// 5km

int main(int argc, char *argv[])
{
    int mode=1;
    po::options_description desc("Allowed options");
    desc.add_options()
            ("help", "produce help message")
            ("in", po::value< std::string>()->required(), "file in")
            ("inputGrid", po::value< std::string>()->required(), "directory of netcdf template for IRM containing the grid with associated pixel_id")
            ("inputGridDef", po::value< std::string>()->required(), "txt with definition of grid")
            ("out", po::value< std::string>()->required(), "file out")
            ("outil", po::value<int>(), "outil ; def outil number 1")
            ("X", po::value< double>()->required(), "position X (custom irm coordinate system)")
            ("Y", po::value< double>()->required(), "position Y (custom irm coordinate system)")
            ;
    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);
    if (vm.count("help")) {
        std::cout << desc << "\n";
        return 1;
    }
    if (vm.count("in")) {input=vm["in"].as<std::string>();
        std::cout << " je vais utiliser le fichier de donnée IRM " << input << std::endl;
    }
    if (vm.count("inputGrid")) {inputGrid=vm["inputGrid"].as<std::string>();
        std::cout << " je vais utiliser le nc template " << inputGrid << std::endl;}
    if (vm.count("inputGridDef")) {inputGridDef=vm["inputGridDef"].as<std::string>();}
    if (vm.count("out")) {pathOut=vm["out"].as<std::string>();
        std::cout << " j'écrirai les résutats dans le fichier " << pathOut << std::endl;}

     if (vm.count("outil")) {mode=vm["outil"].as<int>();}

     if (mode==1){

    std::cout << " create netcdf for IRM gridded observation" << std::endl;
    if (boost::filesystem::exists(input)){
        irmData d(input);
        d.saveNetCDF(pathOut);
    } else {
        std::cout << " je ne peux rien faire car " << input << " n'existe pas..." <<std::endl;
    }

     } else {
       std::cout << " création d'un ficher de climat pour modèle Walsi à partir d'une série temporelle de fichier netcdf (situé dans 'in') mensuels et d'une position en région Wallonne" << std::endl;
       double X=0;
       double Y=0;
       if (vm.count("X")) {X=vm["X"].as<double>();}
       if (vm.count("Y")) {Y=vm["Y"].as<double>();}

       // calcul de la position du pixel
       int U(33),V(33);

       // pas round, floor!
       U=floor((X-oX)/res);
       V=floor((-Y+oY)/res);
       std::cout << "position of your forest site (X " << X << ", Y "<< Y << ") in pixel is " << U << ", " << V << std::endl;
       // vérification  (indexbox commence à 1, pas comme c++ qui commence à 0):
       // cdo -selindexbox,U+1,U+1,V+1,V+1 -selvar,TG IRM-monthly-1960.nc tmp.nc
       // ncdump tmp.nc

        std::ofstream ofs (pathOut, std::ofstream::out);
        ofs << "#Year\tMonth\tMean_T\tSum_P\tSum_pet\n";
       for (int y(1950);y<2025;y++){
           std::cout << "année " << std::to_string(y) << std::endl;
           std::string ncFile=irmFile(input,y);
           const char * p=ncFile.c_str();
           NcFile in(p,NcFile::FileMode::ReadOnly,NULL,0,NcFile::FileFormat::Netcdf4);
           if (!in.is_valid())
           {
               std::cout << "Couldn't open file : " << ncFile << "!\n";
           } else{

               //std::cout << "input File has " << in.num_dims() << " dimensions and " <<in.num_vars() << " vars and "  << in.num_atts() << " atts" << std::endl ;
               NcVar *varT, *varRF, *varETP;
               //timeVar=in.get_var("TIME");
               //NcDim * time=in.get_dim("TIME");
               NcVar * lon=in.get_var("X");
               int NLON = lon->num_vals();
               NcVar * lat=in.get_var("Y");
               int NLAT = lat->num_vals();
               float var_in[NLAT][NLON];

               //std::cout << "lon is " << NLON << ", lat "<< NLAT << std::endl;

               //std::cout << "get var TG, RF and ETP" << std::endl;
               varT=in.get_var("TG");
               varRF=in.get_var("RF");
               varETP=in.get_var("ETP");

               for (int i(0); i <12;i++){
                   int rec= i;
                   varT->set_cur(rec, 0, 0);
                   varT->get(&var_in[0][0], 1, NLAT, NLON);
                    float T =  var_in[V][U];
                   varRF->set_cur(rec, 0, 0);
                   varRF->get(&var_in[0][0], 1, NLAT, NLON);
                   float RF =  var_in[V][U];
                   varETP->set_cur(rec, 0, 0);
                   varETP->get(&var_in[0][0], 1, NLAT, NLON);
                   float ETP =  var_in[V][U];
                   ofs <<y<<"\t"<< i+1 << "\t" << T << "\t" << RF << "\t" << ETP << "\n";

               }// end month
           }// end test file exist

     }// end year
     }// end mode ==2
    std::cout << "done" << std::endl;
    return 0;
}

std::string irmFile(std::string dir,int y ){return dir+"/IRM-monthly-"+std::to_string(y)+".nc";}

