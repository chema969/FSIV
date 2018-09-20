#include "auxiliarFunctions.hpp"
#include <vector>
#include <opencv2/core/core.hpp> 
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

std::vector<int> getHistogram(cv::Mat &image){
   std::vector<int> aux(256,0);
    for(int y=0;y<image.rows;y++){
      uchar *ptr=image.ptr<uchar>(y);
      for(int x=0;x<image.cols;x++){
        aux[ptr[x]]+=1;
       }
     }
  return aux;
}

std::vector<int> getCumulativeNormalizedHistogram(std::vector <int> histogram){
     std::vector<int> aux(256);
     aux[0]=histogram[0];
     for(int i=1;i<256;i++){
        aux[i]=aux[i-1]+histogram[i];
       }
     for(int i=0;i<256;i++){
        aux[i]=(int)(((float)aux[i]/aux[255])*256);
     }
    return aux; 
   }


void equalizate(cv::Mat &image, std::vector<int> cumulativeHistogram){
    for(int y=0;y<image.rows;y++){
      uchar *ptr=image.ptr<uchar>(y);
      for(int x=0;x<image.cols;x++){
        ptr[x]=cumulativeHistogram[ptr[x]];    
       }
     }
}
