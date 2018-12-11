#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <tclap/CmdLine.h>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/ml/ml.hpp>
#include <opencv2/videoio.hpp>
#include "common_code.hpp"
int main(int argc,char** argv){
   TCLAP::CmdLine cmd("Testing the results with a given image", ' ', "0.0");

	TCLAP::ValueArg<std::string> imgArg("", "img", "input image.", false, "", "pathname");
	cmd.add(imgArg);
	TCLAP::ValueArg<std::string> configFile("", "config_file", "configuration file for the dataset.", false, "101_ObjectCategories_conf.txt", "pathname");
	cmd.add(configFile);
        TCLAP::ValueArg<std::string> dictionary("", "dict", "Dictionary of the boVw.", false, "../dictionary.yml", "pathname");
        cmd.add(dictionary);
	TCLAP::ValueArg<std::string> classifierYML("", "classifierYML", "Classifier YML file.", false, "../classifier.yml", "pathname");
	cmd.add(classifierYML);
       TCLAP::ValueArg<std::string> descriptor("", "descriptor", "Descriptor used for test. Default SIFT.", false, "SIFT", "string");
	cmd.add(descriptor);
        TCLAP::ValueArg<int> ndesc("", "ndesc", "[SIFT] Number of descriptors per image. Value 0 means extract all. Default 0.", false, 0, "int");
	cmd.add(ndesc);
	TCLAP::ValueArg<int> surf_threshold("", "threshold", "Threshold for surf. Default 400.", false, 400, "int");
	cmd.add(surf_threshold);
	TCLAP::ValueArg<int> nneigh("", "neighbours", "Number of neighbours used for the Knn. Default 1.", false, 1, "int");
	cmd.add(nneigh);
	TCLAP::ValueArg<int> steps("", "steps", "In dense SIFT, the distance from you take the keypoints. Default 10.", false, 10, "int");
	cmd.add(steps);
        TCLAP::ValueArg<int> scales("", "scales", "In dense SIFT, the number of scales you use. Default 3.", false, 3, "int");
	cmd.add(scales);
        TCLAP::ValueArg<int> PHOW_level("", "levels", "In PHOW, the levels in which you divide the image. Default 3.", false, 3, "int");
	cmd.add(PHOW_level);
        TCLAP::ValueArg<std::string> classifierType("", "classifier", "Classifier used. By default is used the KNN.", false, "KNN", "string");
	cmd.add(classifierType);
        TCLAP::ValueArg<std::string> kernel("", "kernel", "Kernel for the SVM. By default is used the linear.", false, "linear", "string");
	cmd.add(kernel);
        TCLAP::ValueArg<int> number_boosting("", "boosting_number", "The number of weaks classifiers used. By default is 100.", false, 100, "int");
	cmd.add(number_boosting);
        TCLAP::ValueArg<std::string> videoArg("", "video", "input video.", false, "", "pathname");
	cmd.add(videoArg);

	cmd.parse(argc, argv);
       

        std::string config=configFile.getValue();
        cv::FileStorage dictFile;
        dictFile.open(dictionary.getValue(), cv::FileStorage::READ);
        cv::FileStorage classFile;
        classFile.open(classifierYML.getValue(), cv::FileStorage::READ);
    int keywords,default_k;
    dictFile["keywords"]>>keywords;
    cv::Ptr<cv::ml::KNearest> dict = cv::Algorithm::read<cv::ml::KNearest>(dictFile.root());
    dictFile.release();
    cv::Ptr<cv::ml::StatModel> classifier;

     if(classifierType.getValue()=="SVM"){
         cv::Ptr<cv::ml::SVM> classifSVM;
          classifSVM = cv::Algorithm::load<cv::ml::SVM>(classifierYML.getValue());
          if(kernel.getValue()=="linear")
            classifSVM->setKernel(cv::ml::SVM::LINEAR);
          if(kernel.getValue()=="polynomial")
            classifSVM->setKernel(cv::ml::SVM::POLY);
          if(kernel.getValue()=="chi2")
            classifSVM->setKernel(cv::ml::SVM::CHI2);
          if(kernel.getValue()=="RBF")
            classifSVM->setKernel(cv::ml::SVM::RBF);
          if(kernel.getValue()=="sigmoid")
            classifSVM->setKernel(cv::ml::SVM::SIGMOID);
          if(kernel.getValue()=="inter")
            classifSVM->setKernel(cv::ml::SVM::INTER);
          classifier=classifSVM;
          }
     else{ 
         if(classifierType.getValue()=="KNN"){
            cv::Ptr<cv::ml::KNearest> classif; 
            classif = cv::Algorithm::load<cv::ml::KNearest>(classifierYML.getValue());
            classif->setDefaultK(nneigh.getValue());
            classifier=classif;
          }
         else{
           if(classifierType.getValue()=="BOOSTING"){
               cv::Ptr<cv::ml::Boost> classif = cv::Algorithm::load<cv::ml::Boost>(classifierYML.getValue()); 
               classif->setWeakCount(number_boosting.getValue());
               classifier=classif;
           }
           else{
               std::cout<<"Classifier not found"<<std::endl; return 0;
              }
         }
     }
   classFile.release();
   std::vector<std::string> categories;
   std::vector<int> samples_per_cat;
   int retCode;
   if ((retCode = load_dataset_information(configFile.getValue(), 
   categories, samples_per_cat)) != 0)
	{
		std::cerr << "Error: could not load dataset information from '"
			<< configFile.getValue()
			<< "' (" << retCode << ")." << std::endl;
		exit(-1);
	}
   if(imgArg.getValue()!=""){
   cv::Mat img=cv::imread(imgArg.getValue(), cv::IMREAD_GRAYSCALE);


   resize(img, img, cv::Size(IMG_WIDTH, round(IMG_WIDTH*img.rows / img.cols)));                    

   cv::Mat descs;
           if(descriptor.getValue()=="SIFT")
      		descs = extractSIFTDescriptors(img,  ndesc.getValue());

  	   else{
    	          if(descriptor.getValue()=="SURF")
       	 		descs = extractSURFdescriptors(img, surf_threshold.getValue());
      
      		  else{
        	   if(descriptor.getValue()=="DSIFT")descs = extractDSIFTdescriptors(img,ndesc.getValue(),steps.getValue(),scales.getValue());
                   else{
       	             if(descriptor.getValue()=="PHOW")descs = extractPHOWdescriptors(img,ndesc.getValue(),steps.getValue(),scales.getValue(),PHOW_level.getValue()); 
                     else{std::cout<<"NO DESCRIPTOR"<<std::endl;return -1;}
                  }
    	      }
          }

   cv::Mat prediction;
   cv::Mat bovw=compute_bovw(dict,keywords,descs);
   classifier->predict(bovw,prediction);

   std::cout<<"Image belongs to category "<<prediction.at<float>(0,0)<<" "<<categories[prediction.at<float>(0,0)]<<std::endl;
   }


  if(videoArg.getValue()!=""){
      cv::Mat frame;
      cv::VideoCapture video(videoArg.getValue());
       while(video.read(frame)){
         cv::Mat img;
         cv::cvtColor(frame,img,cv::COLOR_RGB2GRAY);

        resize(img, img, cv::Size(IMG_WIDTH, round(IMG_WIDTH*img.rows / img.cols)));                    

        cv::Mat descs;
           if(descriptor.getValue()=="SIFT")
      		descs = extractSIFTDescriptors(img,  ndesc.getValue());

  	   else{
    	          if(descriptor.getValue()=="SURF")
       	 		descs = extractSURFdescriptors(img, surf_threshold.getValue());
      
      		  else{
        	   if(descriptor.getValue()=="DSIFT")descs = extractDSIFTdescriptors(img,ndesc.getValue(),steps.getValue(),scales.getValue());
                   else{
       	             if(descriptor.getValue()=="PHOW")descs = extractPHOWdescriptors(img,ndesc.getValue(),steps.getValue(),scales.getValue(),PHOW_level.getValue()); 
                     else{std::cout<<"NO DESCRIPTOR"<<std::endl;return -1;}
                  }
    	      }
          }

        cv::Mat prediction;
        cv::Mat bovw=compute_bovw(dict,keywords,descs);
        classifier->predict(bovw,prediction);
        int a=frame.cols-50;
        putText(frame,categories[prediction.at<float>(0,0)],cv::Point(10,a), cv::FONT_HERSHEY_SIMPLEX, 4,(255,255,255),2);
        cv::imshow("Live", frame);
        if (cv::waitKey(5) >= 0)
           break;
      }
  }
}
