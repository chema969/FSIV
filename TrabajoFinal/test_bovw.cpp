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
        TCLAP::ValueArg<std::string> n_runsArg("", "dict", "Dictionary of the boVw.", false, "../dictionary.yml", "pathname");
        cmd.add(n_runsArg);
	TCLAP::ValueArg<int> ntest("", "classifier", "Classifier YML file.", false, "../classifier.yml", "pathname");
	cmd.add(ntest);

	cmd.parse(argc, argv);
         



}
