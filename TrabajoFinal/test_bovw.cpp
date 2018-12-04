#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <tclap/CmdLine.h>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/ml/ml.hpp>
#include "common_code.hpp"
int main(int argc,char** argv){
   TCLAP::CmdLine cmd("Testing the results with a given image", ' ', "0.0");

	TCLAP::ValueArg<std::string> imgArg("", "img", "input image.", true, "", "pathname");
	cmd.add(imgArg);
	TCLAP::ValueArg<std::string> configFile("", "config_file", "configuration file for the dataset.", false, "101_ObjectCategories_conf.txt", "pathname");
	cmd.add(configFile);
        TCLAP::ValueArg<std::string> dictionary("", "dict", "Dictionary of the boVw.", false, "../dictionary.yml", "pathname");
        cmd.add(dictionary);
	TCLAP::ValueArg<std::string> classifier("", "classifier", "Classifier YML file.", false, "../classifier.yml", "pathname");
	cmd.add(classifier);
       TCLAP::ValueArg<std::string> descriptor("", "descriptor", "Descriptor used for test. Default SIFT.", false, "SIFT", "string");
	cmd.add(descriptor);
        TCLAP::ValueArg<int> ndesc("", "ndesc", "[SIFT] Number of descriptors per image. Value 0 means extract all. Default 0.", false, 0, "int");
	cmd.add(ndesc);
	TCLAP::ValueArg<int> surf_threshold("", "threshold", "Threshold for surf. Default 400.", false, 400, "int");
	cmd.add(surf_threshold);
	TCLAP::ValueArg<int> nneigh("", "neighbours", "Number of neighbours used for the Knn. Default 1.", false, 1, "int");
	cmd.add(nneigh);
        TCLAP::ValueArg<int> step("", "step", "Number of neighbours used for the Knn. Default 10.", false, 10, "int");
	cmd.add(step);
	cmd.parse(argc, argv);
       

        std::string config=configFile.getValue();
        cv::FileStorage dictFile;
        dictFile.open(dictionary.getValue(), cv::FileStorage::READ);
        cv::FileStorage classFile;
        classFile.open(classifier.getValue(), cv::FileStorage::READ);
    int keywords,default_k;
    dictFile["keywords"]>>keywords;
    classFile["default_k"]>>default_k;
    cv::Ptr<cv::ml::KNearest> dict = cv::Algorithm::read<cv::ml::KNearest>(dictFile.root());
    dictFile.release();
    cv::Ptr<cv::ml::KNearest> classif = cv::Algorithm::load<cv::ml::KNearest>(classifier.getValue());
    classFile.release();

   classif->setDefaultK(nneigh.getValue());
   cv::Mat img=cv::imread(imgArg.getValue(), cv::IMREAD_GRAYSCALE);


   resize(img, img, cv::Size(IMG_WIDTH, round(IMG_WIDTH*img.rows / img.cols)));                    
                    
   cv::Mat descs;
   if(descriptor.getValue()=="SIFT")
      descs = extractSIFTDescriptors(img,ndesc.getValue());

   else{
      if(descriptor.getValue()=="SURF")
       descs = extractSURFdescriptors(img, surf_threshold.getValue());
      
      else{
        if(descriptor.getValue()=="DSIFT") descs=extractDSIFTdescriptors(img, ndesc.getValue(), step.getValue());
        else{std::cout<<"NO EXISTE DESCRIPTOR"<<std::endl;return -1;}
       }
    }
   cv::Mat prediction;
   cv::Mat bovw=compute_bovw(dict,keywords,descs);
   classif->predict(bovw,prediction);
   
   std::vector<std::string> categories;
   std::vector<int> samples_per_cat;
   int retCode;
   std::cout<<configFile.getValue()<<std::endl;
   if ((retCode = load_dataset_information(configFile.getValue(), 
categories, samples_per_cat)) != 0)
	{
		std::cerr << "Error: could not load dataset information from '"
			<< configFile.getValue()
			<< "' (" << retCode << ")." << std::endl;
		exit(-1);
	}
   std::cout<<"Image belongs to category "<<prediction.at<float>(0,0)<<" "<<categories[prediction.at<float>(0,0)]<<std::endl;
}
