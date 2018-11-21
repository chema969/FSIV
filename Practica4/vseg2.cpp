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

  vector<vector <model> > gaussMod(inFrame.rows,vector <model>  (inFrame.cols));
  vector<vector <model> > gaussSecMod(inFrame.rows,vector <model>  (inFrame.cols));

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
  int r = 1;
  Mat dev=Mat::zeros(inFrame.size(), CV_32FC3);
  Mat med=Mat::zeros(inFrame.size(), CV_32FC3);
  Mat mask=Mat::zeros(inFrame.size(), CV_32FC3);
  cv::namedWindow("Output");
          int fakeTyp=typical*10;
  while(wasOk && key!=27 )
  {
      Mat truemask;
      inFrame.convertTo(inFrame,CV_32FC3);   
      if(webcam) setWebcamParams(inputFrame2,input);
      frameNumber++;
      if(frameNumber<bVal){
         accumulateWeighted(inFrame,med,alpha);  
        // med=med/2;
         Mat puwu, subs;
         subtract(inFrame,med,subs);
         pow(subs,2,puwu);
         accumulateWeighted(puwu,dev,alpha);

     
      }
      else{
        cv::namedWindow("Typical",0);

        cv::createTrackbar("TypicalVal","Typical",&fakeTyp,70);
        typical=fakeTyp/10;
        Mat abdi,mult,s[3];
        absdiff(med,inFrame,abdi);
        multiply(dev,typical,mult);
        compare(abdi,mult,mask,CMP_GT);
        split(mask,s);//split source  
        bitwise_and(s[0],s[1], truemask);
        bitwise_and(truemask,s[2], truemask);
        Mat opening;
        Mat close;

    
    
        imshow ("Mask", truemask);
        if(cVal%frameNumber==0){ 
             accumulateWeighted(inFrame,med,alpha,truemask);  
             //med=med/2;
             Mat puwu, subs;
             subtract(inFrame,med,subs);
             pow(subs,2,puwu);
             accumulateWeighted(puwu,dev,alpha,truemask);
         }
             assert(inFrame.cols==truemask.cols);
             assert(inFrame.rows==truemask.rows);
            
             inFrame.copyTo(outFrame,truemask);
       }
       

     inFrame=inFrame/255;
     cv::imshow ("Input", inFrame);    
     
     outFrame=outFrame/255;
     imshow ("Output", outFrame);
     wasOk=input.read(inFrame);
    Mat aux;

    output<<outFrame;
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

