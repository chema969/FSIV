#include "auxiliarFunctions.hpp"
#include <iostream>
#include <iomanip>
#include <cassert>
#include <opencv2/core/core.hpp> 
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/utility.hpp>
#include <opencv2/highgui/highgui.hpp>

cv::Mat createBoxFilter(int r){
   assert(r>0);

   cv::Mat aux(r,r,CV_32FC1);

   for(int y=0;y<aux.rows;y++){
      float *ptr=aux.ptr<float>(y);
      for(int x=0;x<aux.cols;x++){
          ptr[x]=(float)1/(r*r);    
       }
     }
   return aux;
    
}

void filt(cv::Mat& in,cv::Mat& filter, cv::Mat& out){

  for(int y=0;y<in.rows;y++){
       float *ptr=out.ptr<float>(y);
       for(int x=0;x<in.cols;x++){
           
           if((y>(filter.rows/2))&&(x>(filter.cols/2))&&((in.rows-y)>filter.rows/2)&&((in.cols-x)>filter.cols/2)){
               float result=0;
               int ry=0; int rx=0;
               for(int j=y-filter.rows/2;j<=y+filter.rows/2;j++){
                     float *ptrfilter=filter.ptr<float>(ry);
                     float *ptrconv=in.ptr<float>(j);           
                     rx=0;
                     for(int k=x-(filter.rows/2);k<=x+filter.rows/2;k++){
                          result+=(ptrfilter[rx]*ptrconv[k]);
                          rx++;
                          }
                     ry++;
              }

           ptr[x]=result;
           }
           else ptr[x]=0;
      }
   }

}

void multiplyMat(cv::Mat& in, int g){
    for(int y=0;y<in.rows;y++){
       float *ptr=in.ptr<float>(y);
       for(int x=0;x<in.cols;x++){
           ptr[x]=ptr[x]*g;
          }
     }
}

void convolve(const cv::Mat& in, const cv::Mat& filter, cv::Mat& out){
    for(int y=0;y<in.rows;y++){
       const float *ptrIn=in.ptr<const float>(y);
       const float *ptrFilter=filter.ptr<const float>(y); 
       float *ptrOutput=out.ptr<float>(y);           
       for(int x=0;x<in.cols;x++){
          ptrOutput[x]=ptrIn[x]-ptrFilter[x];
       }
    }
}
