/* 
   (c) Fundamentos de Sistemas Inteligenties en Vision
   University of Cordoba, Spain  
*/
#include <cassert>
#include <iostream>
#include <vector>
#include <string>
#include <cstdlib>
//#include <unistd.h>
#include <ctype.h>
#include <cmath>

#include <tclap/CmdLine.h>

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/video/video.hpp>

using namespace std;
using namespace cv;
  float typical;
 Mat outFrame;

struct model{
  double mean;
  double stdDev;
};

double standardDev(double a, double b, double c);
double calcMean(double a, double b, double c);
bool isInteger(const std::string & s);
void setWebcamParams(VideoCapture &inputFrame2,VideoCapture &input);
/*
  Use CMake to compile
*/

int main (int argc, char * const argv[])
{
  /* Default values */
  bool cameraInput=false;
  bool useWhitePatchCorrecction=false;
  bool useChromaticCooridnates=false;
  int bVal,cVal;
  const char * filein = 0;
  const char * fileout = 0;
  char opt;
  bool webcam=false;
  float alpha;
  int optind = 1;

  TCLAP::CmdLine cmd("Video segmentation", ' ', "0.1");

  TCLAP::ValueArg<std::string> filename("v", "videoname", "Video path", true, "video.avi", "string");
  cmd.add(filename);
  TCLAP::ValueArg<std::string> outname("o", "outfilename", "Output video path", true, "out.avi", "string");
  cmd.add(outname);
  
  TCLAP::ValueArg<float> typ("t", "threshold", "Typical value", false, 2.5, "float");
  cmd.add(typ);
  
  TCLAP::ValueArg<int> firstXFram("b", "radius", "First x frame values", false, 100, "int");
  cmd.add(firstXFram);

  TCLAP::ValueArg<int> ChangeFreq("c", "changefreq", "Change frequency", false, 10, "int");
  cmd.add(ChangeFreq);

  TCLAP::ValueArg<float> alp("a", "alpha", "Alpha value", false, 0.1, "float");
  cmd.add(alp);
  
  // Parse input arguments
  cmd.parse(argc, argv);

  filein = filename.getValue().c_str();
  fileout = outname.getValue().c_str();

  typical = typ.getValue();

  if(typical<0){
      cout<<"T value is under 0, abbort"<<endl;
      return 0;
      }
 
  bVal = firstXFram.getValue();
  if(bVal<0){
      cout<<"B value is under 0, abbort"<<endl;
      return 0;
      }
  cVal = ChangeFreq.getValue();
  alpha= alp.getValue();

  std::cout << "Input stream:" << filein << endl;
  std::cout << "Output:" << fileout << endl;

  VideoCapture input;
  VideoCapture inputFrame2; 

  if (cameraInput)
    input.open(atoi(filein));
  else  
    input.open(filein);
  
  if (cameraInput)
    inputFrame2.open(atoi(filein));
  else  
    inputFrame2.open(filein);

  if (!input.isOpened())
  {
    cerr << "Error: the input stream is not opened.\n";
    abort();
  }

 
  Mat inFrame;
  bool wasOk = input.read(inFrame);
  if (!wasOk)
  {
    cerr << "Error: could not read any image from stream.\n";
    abort();
  }
      inFrame.convertTo(inFrame,CV_32FC3);
  vector<vector <model> > gaussMod(inFrame.rows,vector <model>  (inFrame.cols));
  vector<vector <model> > gaussSecMod(inFrame.rows,vector <model>  (inFrame.cols));
    for(int y=0;y<inFrame.rows;y++){
 	 float *ptr=inFrame.ptr<float>(y); //pointer to the y-th image row
  	for(int x=0;x<inFrame.cols;x++,ptr+=3){
   	 gaussMod[y][x].mean=calcMean(ptr[0],ptr[1],ptr[2]);
         gaussMod[y][x].stdDev=standardDev(ptr[0],ptr[1],ptr[2]);
  	 }
 	}
  
  double fps=25.0;
  if (!cameraInput)
    fps=input.get(CV_CAP_PROP_FPS);
  
 

  outFrame = Mat::zeros(inFrame.size(), CV_32FC3);
  
  VideoWriter output;
  cv::Size S = cv::Size(inFrame.cols,inFrame.rows);    //Acquire input size
             
  output.open(fileout, CV_FOURCC('M','J','P','G'), fps, S);
  if (!output.isOpened())
  {
    cerr << "Error: the ouput stream is not opened.\n";
  }  

  if(isInteger(filein)) webcam=true;

  int frameNumber=0;
  int key = 0;

  cv::namedWindow("Output");
  
  while(wasOk && key!=27 )
  {
     outFrame =outFrame*0;
      if(webcam) setWebcamParams(inputFrame2,input);
      inFrame.convertTo(inFrame,CV_32FC3);
      frameNumber++;
      if(frameNumber<bVal){

       for(int y=0;y<inFrame.rows;y++){

 	 float *ptr=inFrame.ptr<float>(y); //pointer to the y-th image row

  	for(int x=0;x<inFrame.cols;x++,ptr+=3){
   	 gaussSecMod[y][x].mean=calcMean(ptr[0],ptr[1],ptr[2]);
         gaussSecMod[y][x].stdDev=standardDev(ptr[0],ptr[1],ptr[2]);
      
         gaussMod[y][x].mean=(alpha*gaussMod[y][x].mean)+((1-alpha)*gaussSecMod[y][x].mean);
         gaussMod[y][x].stdDev=(alpha*gaussMod[y][x].stdDev)+((1-alpha)*gaussSecMod[y][x].stdDev);
  	 }
 	}
       outFrame=inFrame.clone();
      }
      else{
        /*cv::namedWindow("WebcamValues",0);
        cv::createTrackbar("Time","WebcamValues",&time,255);*/
      if(frameNumber%cVal==0){

       for(int y=0;y<inFrame.rows;y++){
 	 float *ptr=inFrame.ptr<float>(y); //pointer to the y-th image row
  	 for(int x=0;x<inFrame.cols&&x<outFrame.cols;x++,ptr+=3){
   	  if((abs(gaussMod[y][x].mean-ptr[0])>(typical*gaussMod[y][x].stdDev))&&(abs(gaussMod[y][x].mean-ptr[1])>(typical*gaussMod[y][x].stdDev))&&(abs(gaussMod[y][x].mean-ptr[2])>(typical*gaussMod[y][x].stdDev))){
         gaussSecMod[y][x].mean=calcMean(ptr[0],ptr[1],ptr[2]);
         gaussSecMod[y][x].stdDev=standardDev(ptr[0],ptr[1],ptr[2]);
      
         gaussMod[y][x].mean=(alpha*gaussMod[y][x].mean)+((1-alpha)*gaussSecMod[y][x].mean);
         gaussMod[y][x].stdDev=(alpha*gaussMod[y][x].stdDev)+((1-alpha)*gaussSecMod[y][x].stdDev);
          }
         
  	 }
 	}

       }
     
       for(int y=0;y<inFrame.rows;y++){
 	 float *ptr=inFrame.ptr<float>(y); //pointer to the y-th image row
         float *ptr2=outFrame.ptr<float>(y); 
  	 for(int x=0;x<inFrame.cols&&x<outFrame.cols;x++,ptr+=3,ptr2+=3){
   	  if((abs(gaussMod[y][x].mean-ptr[0])>(typical*gaussMod[y][x].stdDev))&&(abs(gaussMod[y][x].mean-ptr[1])>(typical*gaussMod[y][x].stdDev))&&(abs(gaussMod[y][x].mean-ptr[2])>(typical*gaussMod[y][x].stdDev)))
           ptr2[0]=ptr2[1]=ptr2[2]=255; 
         
          else ptr2[0]=ptr2[1]=ptr2[2]=0;
  	 }
 	}

       }
      inFrame=inFrame/255;

     cv::imshow ("Input", inFrame);    
          bitwise_and(inFrame, outFrame, outFrame);
     outFrame=outFrame/255;

     imshow ("Output", outFrame);
     wasOk=input.read(inFrame);

     key = cv::waitKey(20);
     if(key==32){std::cout<<"SAVING IMAGE"<<std::endl;
         string aux=filein;
         string outImgFile=aux.substr(0,aux.size()-4)+to_string(frameNumber)+".jpg";
         cv::imwrite(outImgFile,outFrame);}
  
   

  }           
  return 0;
}

bool isInteger(const std::string & s)
{
   if(s.empty() || ((!isdigit(s[0])) && (s[0] != '-') && (s[0] != '+'))) return false ;

   char * p ;
   strtol(s.c_str(), &p, 10) ;

   return (*p == 0) ;
}

double standardDev(double a, double b, double c){
    double mean = calcMean(a,b,c);
 
        double temp = 0;


             temp += pow((a - mean),2) ;
             temp += pow((b - mean),2) ;
             temp += pow((c - mean),2) ;

        return sqrt(temp / 3);



}

double calcMean(double a, double b, double c){
   double sum = 0; 
   sum=a+b+c;
   return sum/3;
}

void setWebcamParams(VideoCapture &inputFrame2,VideoCapture &input){
   int time,gain,brightness;
   time=inputFrame2.get(CAP_PROP_FPS);
   gain=inputFrame2.get(CAP_PROP_GAIN);
   brightness=inputFrame2.get(CAP_PROP_BRIGHTNESS);
    cv::namedWindow("WebcamValues",0);
     cv::createTrackbar("Time","WebcamValues",&time,255);
     cv::createTrackbar("Gain","WebcamValues",&gain,255);
     cv::createTrackbar("Brighness","WebcamValues",&brightness,255);
     inputFrame2.set(CAP_PROP_FPS,time);
     inputFrame2.set(CAP_PROP_GAIN,gain);
     inputFrame2.set(CAP_PROP_BRIGHTNESS,brightness);
     input.set(CAP_PROP_FPS,time);
     input.set(CAP_PROP_GAIN,gain);
     input.set(CAP_PROP_BRIGHTNESS,brightness); 


}

