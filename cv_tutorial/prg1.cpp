/***********************************
 * Autor: Rafael Muñoz Salinas
 * Universidad de Córdoba (Spain)
 * 
 * Reads an image from file and shows it
 * 
 * ******************************************/
#include <opencv2/core/core.hpp> //core routines
#include <opencv2/highgui/highgui.hpp>//imread,imshow,namedWindow,waitKey
#include <iostream>
using namespace std;
int main(int argc,char **argv){
try{
  if(argc!=2) {cerr<<"Usage:image"<<endl;return 0;} 
  //loads the image from file
   cv::Mat image=cv::imread(argv[1]);
   if( image.rows==0) {cerr<<"Error reading image"<<endl;return 0;}
   //creates a window
   cv::namedWindow("myimage");
   //displays the image in the window
   cv::imshow("myimage",image);
   //wait for a key to be pressed
   cv::waitKey(0);
}catch(std::exception &ex)
{
  cout<<ex.what()<<endl;
}

}
