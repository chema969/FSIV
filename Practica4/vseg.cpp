/* 
   (c) Fundamentos de Sistemas Inteligenties en Vision
   University of Cordoba, Spain  
*/

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
  int thresh;
 Mat outFrame;
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
  const char * filein = 0;
  const char * fileout = 0;
  char opt;
  bool webcam=false;
  int optind = 1;

  TCLAP::CmdLine cmd("Video segmentation", ' ', "0.1");

  TCLAP::ValueArg<std::string> filename("v", "videoname", "Video path", true, "video.avi", "string");
  cmd.add(filename);
  TCLAP::ValueArg<std::string> outname("o", "outfilename", "Output video path", true, "out.avi", "string");
  cmd.add(outname);
  
  TCLAP::ValueArg<int> thres("t", "threshold", "Threshold value", false, 13, "int");
  cmd.add(thres);
  
  TCLAP::ValueArg<int> radius("s", "radius", "Radius value", false, 0, "int");
  cmd.add(radius);
  // Parse input arguments
  cmd.parse(argc, argv);

  filein = filename.getValue().c_str();
  fileout = outname.getValue().c_str();

  thresh = thres.getValue();
  if(thresh<0){
      cout<<"Threshold is under 0, abbort"<<endl;
      return 0;
      }
 
  int r = radius.getValue();

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

  cv::Mat fram2;
  Mat inFrame;
  bool wasOk = input.read(inFrame);
  if (!wasOk)
  {
    cerr << "Error: could not read any image from stream.\n";
    abort();
  }
  
  double fps=25.0;
  if (!cameraInput)
    fps=input.get(CV_CAP_PROP_FPS);
  
  bool wasOk2 = inputFrame2.read(fram2);


  outFrame = Mat::zeros(inFrame.size(), CV_8UC1);
  
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
    
    if(webcam) setWebcamParams(inputFrame2,input);

     wasOk2 = inputFrame2.read(fram2);
     if(wasOk2){
       frameNumber++;
        
       cv::imshow ("Input", inFrame);    
     
	 // Do your processing
       Mat auxImage,auxImage2;

       cvtColor(inFrame, auxImage, CV_RGB2GRAY);
       cvtColor(fram2, auxImage2, CV_RGB2GRAY);

       absdiff(auxImage,auxImage2,outFrame);

     

 
    
     cv::namedWindow("Values",0);
     cv::createTrackbar("r value","Values",&thresh,255);
     threshold(outFrame,outFrame,thresh,255,THRESH_BINARY);

     Mat opening;
     Mat close;

     if(r>0){
     	Mat kern=getStructuringElement(MORPH_RECT, Size(r*2+1,r*2+1),Point( r, r ));
     
        	//Doing the opening process
     	erode(outFrame,opening,kern); 
     	dilate(outFrame,opening,kern);

        	//Doing the closing process
     	dilate(outFrame,close,kern); 
     	erode(close,close,kern); 
    
     	outFrame=opening+close;
           }
    

     Mat rgbOut;
     outFrame.convertTo(outFrame,inFrame.type());

       cvtColor(outFrame, outFrame, CV_GRAY2RGB);
 output<<outFrame;
     bitwise_and(inFrame, outFrame, rgbOut);
     imshow ("Output", outFrame);
     imshow ("Substracted",rgbOut);
     wasOk=input.read(inFrame);

     key = cv::waitKey(20);
     if(key==32){std::cout<<"SAVING IMAGE"<<std::endl;
         string aux=filein;
         string outImgFile=aux.substr(0,aux.size()-4)+to_string(frameNumber)+".jpg";
         cv::imwrite(outImgFile,outFrame);}
      } 
   
   else wasOk=false;
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

