#ifndef AUXILIARFUNCTIONS_HPP
#define AUXILIARFUNCTIONS_HPP
#include <string>
#include <vector>
#include <opencv2/core/core.hpp> 



class CmdLineParser{
   int argc;
   char **argv;
public:
  CmdLineParser(int _argc,char **_argv):argc(_argc),argv(_argv){}

  bool operator[] ( std::string param ) {
      int idx=-1; 
      for ( int i=0; i<argc && idx==-1; i++ ) 
         if ( std::string ( argv[i] ) ==param ) 
            idx=i; 
      return ( idx!=-1 ) ; 
   }


  std::string operator()(std::string param,std::string defvalue="-1"){
      int idx=-1;
      for ( int i=0; i<argc && idx==-1; i++ ) 
           if ( std::string ( argv[i] ) ==param ) 
             idx=i; 
      if ( idx==-1 ) 
      return defvalue; 
      else return ( argv[ idx+1] ); 
    }
};

std::vector<int> getHistogram(cv::Mat &image);

std::vector<int> getCumulativeNormalizedHistogram(std::vector <int> histogram);

void equalizate(cv::Mat &image, std::vector<int> cumulativeHistogram);

cv::Mat equalizedRadiusImage(cv::Mat &image,int rValue);

int pixelVal(cv::Mat &image,int radius,int y,int x,int pixel);

std::vector<int> getHistogram(std::vector< std::vector<int> > i);





#endif
