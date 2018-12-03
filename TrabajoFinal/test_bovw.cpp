#include <iomanip>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include "common_code.hpp"
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/features2d.hpp>
#include <opencv2/xfeatures2d.hpp>
#include <tclap/CmdLine.h>

int main(int argc,char* argv){
   TCLAP::CmdLine cmd("Testing the results with a given image", ' ', "0.0");

	TCLAP::ValueArg<std::string> imgArg("", "img", "input image.", true, "", "pathname");
	cmd.add(imgArg);
	TCLAP::ValueArg<std::string> configFile("", "config_file", "configuration file for the dataset.", false, "101_ObjectCategories_conf.txt", "pathname");
	cmd.add(configFile);
        TCLAP::ValueArg<std::string> dictionary("", "dict", "Dictionary of the boVw.", false, "../dictionary.yml", "pathname");
        cmd.add(n_runsArg);
	TCLAP::ValueArg<std::string> classifier("", "classifier", "Classifier YML file.", false, "../classifier.yml", "pathname");
	cmd.add(ntest);
       TCLAP::ValueArg<std::string> descriptor("", "descriptor", "Descriptor used for test. Default SIFT.", false, "SIFT", "string");
	cmd.add(nneigh);
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
    cv::Ptr<cv::ml::KNearest> classif = cv::Algorithm::read<cv::ml::KNearest>(classFile.root());
    classFile.release();

   classif->setDefaultK(default_k);
   cv::Mat img=cv::imread(imgArg.getValue(), cv::IMREAD_GRAYSCALE);


   resize(img, img, cv::Size(IMG_WIDTH, round(IMG_WIDTH*img.rows / img.cols)));                    
                    
   cv::Mat descs;
   if(descriptor.getValue()=="SIFT")
      descs = extractSIFTDescriptors(img, keywords);

   else{
      if(descriptor.getValue()=="SURF")
       descs = extractSURFdescriptors(img, surf_threshold.getValue());
      
      else{
        if(descriptor.getValue()=="DSIFT") std::cout<<" "<<std::endl;
        else{std::cout<<"NO EXISTE DESCRIPTOR"<<std::endl;return -1;}
       }
    }
   cv::Mat prediction;
   cv::Mat bovw=compute_bovw(dict,keywords,descs);
   classif->predict(bovw,prediction);
   
   std::vector<std::string> categories;
   std::vector<int> samples_per_cat;
   
   if ((retCode = load_dataset_information(dataset_desc_file, categories, samples_per_cat)) != 0)
	{
		std::cerr << "Error: could not load dataset information from '"
			<< dataset_desc_file
			<< "' (" << retCode << ")." << std::endl;
		exit(-1);
	}
   std::cout<<"Image belongs to category "<<prediction.at<float>(0,0)<<" "<<categories[prediction.at<float>(0,0)]<<std::endl
}
