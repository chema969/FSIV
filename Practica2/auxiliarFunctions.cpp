#include "auxiliarFunctions.hpp"
#include <iostream>
#include <iomanip>
#include <cassert>
#include <cmath>
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



cv::Mat createGaussianFilter(int r){
    assert(r>0);

    cv::Mat aux(r,r,CV_32FC1);

   int center=(r/2);
   for(int y=0;y<aux.rows;y++){
      float *ptr=aux.ptr<float>(y);
      for(int x=0;x<aux.cols;x++){
          ptr[x]=gauss(y-center,x-center);
       }
     }
   return aux;




}



double gauss(int x,int y){
    double e=2.71828182845904523;
    
    double exponent=-1*((pow(x,2)+pow(y,2))/2);
    
    double base=pow(e,exponent);
    
    double base2=1/(2*3.1415926535897932384);
   
    return base*base2;

}




void filt(cv::Mat& in,cv::Mat& filter, cv::Mat& out,bool circular){
    
  for(int y=0;y<in.rows;y++){
       float *ptr=out.ptr<float>(y);
       for(int x=0;x<in.cols;x++){
               bool exit=false;
               float result=0;
               int ry=0; int rx=0,l,m;
               for(int j=y-filter.rows/2;(j<=y+filter.rows/2)&&(!exit);j++){

                    if(((j<0)||(j>=in.rows))&&!circular){
                    result=0;
                    exit=true;}
                  
                    else{
               
                      if(((j<0)||(j>=in.rows))&&circular) m=((j%in.rows)+in.rows)%in.rows;
                      else m=j;
                      float *ptrfilter=filter.ptr<float>(ry);
                      //std::cout<<"M="<<m<<std::endl;
                      float *ptrconv=in.ptr<float>(m);           
                      rx=0;

                      for(int k=x-(filter.rows/2);(k<=x+filter.rows/2)&&(!exit);k++){

                         if(((k<0)||(k>=in.cols))&&!circular){

                            result=0;
                            exit=true;
                            }
                         else{
                            if(((k<0)||(k>=in.cols))&&circular) l=((k%in.cols)+in.cols)%in.cols;
                            else l=k;

                             result+=(ptrfilter[rx]*ptrconv[l]);
                          rx++;
                          }
                          }
                     ry++;
                     
                     }
                     }
 
           ptr[x]=result;
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



void resize(cv::Mat& in){
    for(int y=0;y<in.rows;y++){
       float *ptr=in.ptr<float>(y);
       for(int x=0;x<in.cols;x++){
           ptr[x]=ptr[x]/255;
          }
     }
}


void unresize(cv::Mat& in){
    for(int y=0;y<in.rows;y++){
       float *ptr=in.ptr<float>(y);
       for(int x=0;x<in.cols;x++){
           ptr[x]=ptr[x]*255;
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
          if(ptrOutput[x]>255) ptrOutput[x]=255;
       }
    }
}



