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

/*
  Use CMake to compile
*/
void threshfunction(int,char*);

int main (int argc, char * const argv[])
{
  /* Default values */
  bool cameraInput=false;
  bool useWhitePatchCorrecction=false;
  bool useChromaticCooridnates=false;
  int thresh;
  const char * filein = 0;
  const char * fileout = 0;
  char opt;
  
  int optind = 1;

  TCLAP::CmdLine cmd("Video segmentation", ' ', "0.1");

  TCLAP::ValueArg<std::string> filename("v", "videoname", "Video path", true, "video.avi", "string");
  cmd.add(filename);
  TCLAP::ValueArg<std::string> outname("o", "outfilename", "Output video path", true, "out.avi", "string");
  cmd.add(outname);
  
  TCLAP::ValueArg<int> thres("t", "threshold", "Threshold value", false, 13, "int");
  cmd.add(thres);
  
  TCLAP::ValueArg<int> radius("r", "radius", "Radius value", false, 0, "int");
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


  Mat outFrame = Mat::zeros(inFrame.size(), CV_8UC1);
  
  VideoWriter output;
  cv::Size S = cv::Size(inFrame.cols,inFrame.rows);    //Acquire input size
             
  output.open(fileout, CV_FOURCC('M','J','P','G'), fps, S, 0);
  if (!output.isOpened())
  {
    cerr << "Error: the ouput stream is not opened.\n";
  }  

  int frameNumber=0;
  int key = 0;

  cv::namedWindow("Output");

  while(wasOk && key!=27 )
  {
     wasOk2 = inputFrame2.read(fram2);
     if(wasOk2){
       frameNumber++;
        
       cv::imshow ("Input", inFrame);    
     
	 // Do your processing

       absdiff(inFrame,fram2,outFrame);
       threshold(outFrame,outFrame,thresh,255,THRESH_TOZERO);

     cv::imshow ("Output", outFrame);
 

     /*cv::namedWindow("Values",0);
     cv::createTrackbar("r value","Values",&thresh,50,threshfunction);
    */
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
     cv::imshow ("Output", outFrame);
     wasOk=input.read(inFrame);

     key = cv::waitKey(20);
     }
  }           
  return 0;
}


void threshfunction(int,char*){

  //threshold(frame,output,thresh,255,THRESH_TOZERO);

}
