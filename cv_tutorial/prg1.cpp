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
   cv::Mat image=cv::imread(argv[1]);
   if( image.rows==0) {cerr<<"Error reading image"<<endl;return 0;}
    cv::cvtColor(image,image,cv::COLOR_BGR2GRAY);
    cv::imwrite("gandalf.jpg",image);
    }
}catch(std::exception &ex)
{
  cout<<ex.what()<<endl;
}

}
