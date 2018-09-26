#include "auxiliarFunctions.hpp"
#include <iostream>
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


std::vector<int> getHistogram(std::vector< std::vector<int> > i){
   std::vector<int> aux(256,0);
    for(unsigned int y=0;y<i.size();y++){
      for(unsigned int x=0;x<i[y].size();x++){
        //std::cout<<"matriz"<<y<<","<<x<<"="<<i[y][x]<<std::endl;
        aux[i[y][x]]+=1;
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
        aux[i]=(int)(((float)aux[i]/aux[255])*255);
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










void masked(cv::Mat &image,cv::Mat &mask,std::vector<int> cumulativeHistogram){
    for(int y=0;y<image.rows;y++){
      uchar *ptr2=mask.ptr<uchar>(y);
      uchar *ptr=image.ptr<uchar>(y);
      for(int x=0;x<image.cols;x++){
        if(ptr2[x]>0)
          ptr[x]=cumulativeHistogram[ptr[x]];    
       }
     }
}











cv::Mat equalizedRadiusImage(cv::Mat &image,int rValue){
    int radius=2*rValue+1;
    cv::Mat image2(image.rows,image.cols,image.type());
    for(int y=0;y<image2.rows;y++){
      uchar *ptr=image2.ptr<uchar>(y);
      uchar *ptr2=image.ptr<uchar>(y);
      for(int x=0;x<image2.cols;x++){
        ptr[x]=pixelVal(image,radius,x,y,ptr2[x]);    
       }
     }
  return image2;
}



int pixelVal(cv::Mat &image,int radius,int x,int y,int pixel){
   int xMinValue=x-radius;
   if(xMinValue<0) xMinValue=0;
   
   int xMaxValue=x+radius;
   if(xMaxValue>image.cols) xMaxValue=image.cols;
 
   int yMinValue=y-radius;
   if(yMinValue<0) yMinValue=0;

   int yMaxValue=y+radius;
   if(yMaxValue>image.rows) yMaxValue=image.rows;

   std::vector< std::vector <int> > v;
   v.resize(xMaxValue-xMinValue);
   
   for(int r=0;r<xMaxValue-xMinValue;r++)
      v[r].resize(yMaxValue-yMinValue);
   int r=0;

   for(int aux1=xMinValue;aux1<xMaxValue;aux1++){
     int s=0;
     uchar *ptr=image.ptr<uchar>(aux1);
       for(int aux2=yMinValue;aux2<yMaxValue;aux2++){
         v[r][s]=ptr[aux2];
         s++;
        }

    r++;
    }

   std::vector<int> histogram=getCumulativeNormalizedHistogram(getHistogram(v));
   return histogram[pixel];

}

