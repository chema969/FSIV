/*! \file train_bovw.cpp
    \brief Train a Bag of Visual Words model
    \authors Fundamentos de Sistemas Inteligentes en Vision
*/

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


int main(int argc, char * argv[])
{
	TCLAP::CmdLine cmd("Train and test a BoVW model", ' ', "0.0");

	TCLAP::ValueArg<std::string> basenameArg("", "basename", "basename for the dataset.", false, "./data", "pathname");
	cmd.add(basenameArg);
	TCLAP::ValueArg<std::string> configFile("", "config_file", "configuration file for the dataset.", false, "101_ObjectCategories_conf.txt", "pathname");
	cmd.add(configFile);
    TCLAP::ValueArg<int> n_runsArg("", "n_runs", "Number of trials train/set to compute the recognition rate. Default 10.", false, 10, "int");
    cmd.add(n_runsArg);
	TCLAP::ValueArg<int> dict_runs("", "dict_runs", "[SIFT] Number of trials to select the best dictionary. Default 5.", false, 5, "int");
	cmd.add(dict_runs);
    TCLAP::ValueArg<int> ndesc("", "ndesc", "[SIFT] Number of descriptors per image. Value 0 means extract all. Default 0.", false, 0, "int");
	cmd.add(ndesc);
    TCLAP::ValueArg<int> keywords("", "keywords", "[SIFT] Number of keywords generated. Default 100.", false, 100, "int");
	cmd.add(keywords);
    TCLAP::ValueArg<int> ntrain("", "ntrain", "Number of samples per class used to train. Default 15.", false, 15, "int");
	cmd.add(ntrain);
	TCLAP::ValueArg<int> ntest("", "ntest", "Number of samples per class used to test. Default 50.", false, 50, "int");
	cmd.add(ntest);
	TCLAP::ValueArg<int> nneigh("", "neighbours", "Number of neighbours used for the Knn. Default 1.", false, 1, "int");
	cmd.add(nneigh);
        TCLAP::ValueArg<std::string> descriptor("", "descriptor", "Descriptor used for train. Default SIFT.", false, "SIFT", "string");
	cmd.add(descriptor);
	TCLAP::ValueArg<int> surf_threshold("", "threshold", "Threshold for surf. Default 400.", false, 400, "int");
	cmd.add(surf_threshold);
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
        TCLAP::ValueArg<int> c_value("", "c_value", "The margin of the SVM. By default is 100.", false, 100, "int");
	cmd.add(c_value);
        TCLAP::SwitchArg PHOWAllowed("", "PHOW", "Allow using PHOW. By default is false.");
	cmd.add(PHOWAllowed);
        TCLAP::ValueArg<int> number_boosting("", "boosting_number", "The number of weaks classifiers used. By default is 100.", false, 100, "int");
	cmd.add(number_boosting);
	cmd.parse(argc, argv);

	std::vector<std::string> categories;
	std::vector<int> samples_per_cat;
	
	std::string dataset_desc_file = basenameArg.getValue() + "/" + configFile.getValue();
	
        /*Reading the config file*/
	int retCode;
	if ((retCode = load_dataset_information(dataset_desc_file, categories, samples_per_cat)) != 0)
	{
		std::cerr << "Error: could not load dataset information from '"
			<< dataset_desc_file
			<< "' (" << retCode << ")." << std::endl;
		exit(-1);
	}

    std::cout << "Found " << categories.size() << " categories: ";
    if (categories.size()<2)
    {
        std::cerr << "Error: at least two categories are needed." << std::endl;
        return -1;
    }

    for (size_t i=0;i<categories.size();++i)
        std::cout << categories[i] << ' ';
    std::cout << std::endl;
    std::vector<float> rRates(n_runsArg.getValue(), 0.0);

    int sift_type = 0;

    std::vector<int> siftScales{ 9, 13 }; // 5 , 9

    cv::Ptr<cv::ml::KNearest> best_dictionary;
    cv::Ptr<cv::ml::StatModel> best_classifier;
    double best_rRate = 0.0;

    for (int trail=0; trail<n_runsArg.getValue(); trail++)
    {
        std::clog << "######### TRIAL " << trail+1 << " ##########" << std::endl;


        std::vector<std::vector<int>> train_samples;
        std::vector<std::vector<int>> test_samples;
        create_train_test_datasets(samples_per_cat, ntrain.getValue(), ntest.getValue(), train_samples, test_samples);

        std::clog << "Training ..." << std::endl;
        std::clog << "\tCreating dictionary ... " << std::endl;
        std::clog << "\t\tComputing descriptors..." << std::endl;
        cv::Mat train_descs;
        std::vector<int> ndescs_per_sample;
        ndescs_per_sample.resize(0);

        for (size_t c = 0; c < train_samples.size(); ++c)
        {
            
            std::clog << "  " << std::setfill(' ') << std::setw(3) << (c * 100) / train_samples.size() << " %   \015";
            for (size_t s = 0; s < train_samples[c].size(); ++s)
            {
                
                std::string filename = compute_sample_filename(basenameArg.getValue(), categories[c], train_samples[c][s]);
                cv::Mat img = cv::imread(filename, cv::IMREAD_GRAYSCALE);
                if (img.empty())
                {
                    std::cerr << "Error: could not read image '" << filename << "'." << std::endl;
                    exit(-1);
                }
                else
                {
                    // Fix size
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
       	             if(descriptor.getValue()=="DAISY")descs = extractDAISYDescriptors(img,steps.getValue(),scales.getValue()); 
                     else{std::cout<<"NO DESCRIPTOR"<<std::endl;return -1;}
                  }
    	      }
          }

                    if (train_descs.empty())
                        train_descs = descs;
                    else
                    {
                        cv::Mat dst;
                        cv::vconcat(train_descs, descs, dst);
                        train_descs = dst;
                    }
                    ndescs_per_sample.push_back(descs.rows); //we could really have less of wished descriptors.
                }
            }
            
        }
        std::clog << std::endl;
        CV_Assert(ndescs_per_sample.size() == (categories.size()*ntrain.getValue()));
        std::clog << "\t\tDescriptors size = " << train_descs.rows*train_descs.cols * sizeof(float) / (1024.0 *1024.0) << " MiB." << std::endl;
        std::clog << "\tGenerating " << keywords.getValue() << " keywords ..." << std::endl;
        cv::Mat keyws;
        cv::Mat labels;
        double compactness = cv::kmeans(train_descs, keywords.getValue(), labels,
                                        cv::TermCriteria(cv::TermCriteria::EPS + cv::TermCriteria::MAX_ITER, 10, 1.0),
                                        dict_runs.getValue(),
                                        cv::KmeansFlags::KMEANS_PP_CENTERS, //cv::KMEANS_RANDOM_CENTERS,
                                        keyws);
        CV_Assert(keywords.getValue() == keyws.rows);
        //free not needed memory
        labels.release();

        std::clog << "\tGenerating the dictionary ... " << std::endl;
        cv::Ptr<cv::ml::KNearest> dict = cv::ml::KNearest::create();
        dict->setAlgorithmType(cv::ml::KNearest::BRUTE_FORCE);
        dict->setIsClassifier(true);
        cv::Mat indexes(keyws.rows, 1, CV_32S);
        for (int i = 0; i < keyws.rows; ++i)
            indexes.at<int>(i) = i;
        dict->train(keyws, cv::ml::ROW_SAMPLE, indexes);
        std::clog << "\tDictionary compactness " << compactness << std::endl;

        std::clog << "\tTrain classifier ... " << std::endl;

        //For each train image, compute the corresponding bovw.
        std::clog << "\t\tGenerating the a bovw descriptor per train image." << std::endl;
        int row_start = 0;
        cv::Mat train_bovw;
        std::vector<int> train_labels_v;
        train_labels_v.resize(0);///AQUIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIII

   if(!PHOWAllowed.getValue()){
        for (size_t c = 0, i = 0; c < train_samples.size(); ++c)
            for (size_t s = 0; s < train_samples[c].size(); ++s, ++i)
            {
                cv::Mat descriptors = train_descs.rowRange(row_start, row_start + ndescs_per_sample[i]);
                row_start += ndescs_per_sample[i];
                cv::Mat bovw = compute_bovw(dict, keyws.rows, descriptors);
                train_labels_v.push_back(c);
                if (train_bovw.empty())
                    train_bovw = bovw;
                else
                {
                    cv::Mat dst;
                    cv::vconcat(train_bovw, bovw, dst);
                    train_bovw = dst;
                }
            }
     }
    else{
          cv::Mat perIMG;
         int rr=0;
         for (size_t c = 0; c < train_samples.size(); ++c)
        {
            
            std::clog << "  " << std::setfill(' ') << std::setw(3) << (c * 100) / train_samples.size() << " %   \015";
            for (size_t s = 0; s < train_samples[c].size(); ++s)
            {
                
                std::string filename = compute_sample_filename(basenameArg.getValue(), categories[c], train_samples[c][s]);
                cv::Mat img = cv::imread(filename, cv::IMREAD_GRAYSCALE);
                if (img.empty())
                {
                    std::cerr << "Error: could not read image '" << filename << "'." << std::endl;
                    exit(-1);
                }
                else
                {
                    // Fix size
                    resize(img, img, cv::Size(IMG_WIDTH, round(IMG_WIDTH*img.rows / img.cols)));  
                    std::vector<cv::Mat> images_vector;
                                         train_labels_v.push_back(c);
   			 for(int k=0;k<PHOW_level.getValue();k++){
      
        			int step2=img.rows/(pow(2,k));
       				int step3=img.cols/(pow(2,k));
      				  for(int i=0;i<=img.rows-step2;i+=step2){
                                    for(int j=0;j<=img.cols-step3;j+=step3){
                                       cv::Mat subimg=img(cv::Range(i,i+step2),cv::Range(j,j+step3));
                                       resize(subimg, subimg, cv::Size(IMG_WIDTH, round(IMG_WIDTH*img.rows / img.cols))); 
                                       images_vector.push_back(subimg);
                                    }
                                  }
      
                          }     
                 for(int i=0;i<images_vector.size();i++){
                    cv::Mat descs;
                    if(descriptor.getValue()=="SIFT")
      		      descs = extractSIFTDescriptors(images_vector[i],  ndesc.getValue());

  	            else{
    	              if(descriptor.getValue()=="SURF")
       	 		descs = extractSURFdescriptors(images_vector[i], surf_threshold.getValue());
      
      		        else{
        	          if(descriptor.getValue()=="DSIFT")descs = extractDSIFTdescriptors(images_vector[i],ndesc.getValue(),steps.getValue(),scales.getValue());
                          else{
       	                     if(descriptor.getValue()=="DAISY")descs = extractDAISYDescriptors(images_vector[i],steps.getValue(),scales.getValue()); 
                             else{std::cout<<"NO DESCRIPTOR"<<std::endl;return -1;}
                          }
    	                }
                     }

                     cv::Mat bovw = compute_bovw(dict, keyws.rows, descs);
                      if (perIMG.empty())
                         perIMG = bovw;
                     else
                     {
                       cv::Mat dst;
                       cv::hconcat(perIMG, bovw, dst);
                       perIMG = dst;
                    }
                   }
                }
              if (train_bovw.empty())
                         train_bovw = perIMG;
              else
                     {
                       cv::Mat dst;
                       cv::vconcat(train_bovw, perIMG, dst);
                       train_bovw = dst;
                    }
             cv::Mat aux;
              perIMG=aux;
            }
            
        }
     }
        //free not needed memory
        train_descs.release();
        cv::Ptr<cv::ml::StatModel> classifier;

        //Create the classifier.
            //Train a KNN classifier using the training bovws like patterns.
         std::clog<<"\tPutting the classifier parameters\n";
            
            if(classifierType.getValue()=="KNN"){
              cv::Ptr<cv::ml::KNearest> knnClassifier = cv::ml::KNearest::create();
              knnClassifier->setAlgorithmType(cv::ml::KNearest::BRUTE_FORCE);            
              knnClassifier->setDefaultK(nneigh.getValue());
              knnClassifier->setIsClassifier(true);
              classifier = knnClassifier;
             }
            else{
              if(classifierType.getValue()=="SVM"){
                 cv::Ptr<cv::ml::SVM> classifSVM = cv::ml::SVM::create();
                 if(kernel.getValue()=="linear")
                   classifSVM->setKernel(cv::ml::SVM::LINEAR);
                 if(kernel.getValue()=="polynomial"){
                   classifSVM->setKernel(cv::ml::SVM::POLY);
                   classifSVM->setDegree(1);}
                 if(kernel.getValue()=="chi2")
                   classifSVM->setKernel(cv::ml::SVM::CHI2);
                 if(kernel.getValue()=="RBF")
                   classifSVM->setKernel(cv::ml::SVM::RBF);
                 if(kernel.getValue()=="sigmoid")
                   classifSVM->setKernel(cv::ml::SVM::SIGMOID);
                 if(kernel.getValue()=="inter")
                   classifSVM->setKernel(cv::ml::SVM::INTER);
                 classifSVM->setTermCriteria(cv::TermCriteria(cv::TermCriteria::MAX_ITER, 100, 1e-6));
                 classifSVM->setType(cv::ml::SVM::C_SVC);
                 classifSVM->setC(c_value.getValue());
                 classifier=classifSVM;
              }
            else{
              if(classifierType.getValue()=="BOOSTING"){
                 cv::Ptr<cv::ml::Boost> classif = cv::ml::Boost::create(); 
		 classif->setBoostType(cv::ml::Boost::DISCRETE);
                 classif->setWeakCount(number_boosting.getValue());
                 classifier=classif;
                }
             else{
                 std::cout<<"Classifier not found"<<std::endl; return 0;
                }
             }
        }
        cv::Mat train_labels(train_labels_v);
        classifier->train(train_bovw, cv::ml::ROW_SAMPLE, train_labels);
        //free not needed memory.
        train_bovw.release();
        train_labels_v.resize(0);

        std::clog << "Testing .... " << std::endl;

        //load test images, generate SIFT descriptors and quantize getting a bovw for each image.
        //classify and compute errors.

        //For each test image, compute the corresponding bovw.
        std::clog << "\tCompute image descriptors for test images..." << std::endl;
        cv::Mat test_bovw;
        std::vector<float> true_labels;
        true_labels.resize(0);
                      cv::Mat perIMG;
        for (size_t c = 0; c < test_samples.size(); ++c)
        {
            std::clog << "  " << std::setfill(' ') << std::setw(3) << (c * 100) / train_samples.size() << " %   \015";
            for (size_t s = 0; s < test_samples[c].size(); ++s)
            {
                std::string filename = compute_sample_filename(basenameArg.getValue(), categories[c], test_samples[c][s]);
                cv::Mat img = cv::imread(filename, cv::IMREAD_GRAYSCALE);
                if (img.empty())
                    std::cerr << "Error: could not read image '" << filename << "'." << std::endl;
                else
                {
                    // Fix size
                    resize(img, img, cv::Size(IMG_WIDTH, round(IMG_WIDTH*img.rows / img.cols)));
                if(!PHOWAllowed.getValue()){
                    //cv::Mat descs = extractSIFTDescriptors(img, ndesc.getValue());
                    cv::Mat descs;

                        if(descriptor.getValue()=="SIFT")
      		descs = extractSIFTDescriptors(img,  ndesc.getValue());

  	   else{
                 
    	          if(descriptor.getValue()=="SURF")
       	 		descs = extractSURFdescriptors(img, surf_threshold.getValue());
      
      		   else{
        	    if(descriptor.getValue()=="DSIFT")descs = extractDSIFTdescriptors(img,ndesc.getValue(),steps.getValue(),scales.getValue());
                      else{
       	                if(descriptor.getValue()=="DAISY")descs = extractDAISYDescriptors(img,steps.getValue(),scales.getValue()); 
                        else{std::cout<<"NO DESCRIPTOR"<<std::endl;return -1;}
                       }
    	               }
                   }


                    cv::Mat bovw = compute_bovw(dict, keyws.rows, descs);
                    if (test_bovw.empty())
                        test_bovw = bovw;
                    else
                    {
                        cv::Mat dst;
                        cv::vconcat(test_bovw, bovw, dst);
                        test_bovw = dst;
                    }
                    true_labels.push_back(c);
                }
               else{
                         true_labels.push_back(c);
                         std::vector<cv::Mat> images_vector;
   			 for(int k=0;k<PHOW_level.getValue();k++){
      
        			int step2=img.rows/(pow(2,k));
       				int step3=img.cols/(pow(2,k));
      				  for(int i=0;i<=img.rows-step2;i+=step2){
                                    for(int j=0;j<=img.cols-step3;j+=step3){
                                       cv::Mat subimg=img(cv::Range(i,i+step2),cv::Range(j,j+step3));
                                       resize(subimg, subimg, cv::Size(IMG_WIDTH, round(IMG_WIDTH*img.rows / img.cols))); 
                                       images_vector.push_back(subimg);
                                    }
                                  }
      
                          }     
                 for(int i=0;i<images_vector.size();i++){
                    cv::Mat descs;
                    if(descriptor.getValue()=="SIFT")
      		      descs = extractSIFTDescriptors(images_vector[i],  ndesc.getValue());

  	            else{
    	              if(descriptor.getValue()=="SURF")
       	 		descs = extractSURFdescriptors(images_vector[i], surf_threshold.getValue());
      
      		        else{
        	          if(descriptor.getValue()=="DSIFT")descs = extractDSIFTdescriptors(images_vector[i],ndesc.getValue(),steps.getValue(),scales.getValue());
                          else{
       	                     if(descriptor.getValue()=="DAISY")descs = extractDAISYDescriptors(images_vector[i],steps.getValue(),scales.getValue()); 
                             else{std::cout<<"NO DESCRIPTOR"<<std::endl;return -1;}
                          }
    	                }
                     }

                     cv::Mat bovw = compute_bovw(dict, keyws.rows, descs);
                      if (perIMG.empty())
                         perIMG = bovw;
                     else
                     {
                       cv::Mat dst;
                       cv::hconcat(perIMG, bovw, dst);
                       perIMG = dst;
                    }
                   }
                }   
              }
            if(PHOWAllowed.getValue()){
               if (test_bovw.empty())
                         test_bovw = perIMG;
              else
                     {
                       cv::Mat dst;
                       cv::vconcat(test_bovw, perIMG, dst);
                       test_bovw = dst;
                    }
             cv::Mat aux;
              perIMG=aux;
            }
          }
        }
        std::clog << std::endl;
        std::clog << "\tThere are " << test_bovw.rows << " test images." << std::endl;

        //classify the test samples.
        std::clog << "\tClassifying test images." << std::endl;
        cv::Mat predicted_labels;

        classifier->predict(test_bovw, predicted_labels);
        std::clog<<predicted_labels.rows<<" == "<<true_labels.size()<<std::endl;
        CV_Assert(predicted_labels.depth() == CV_32F);
        CV_Assert(predicted_labels.rows == test_bovw.rows);
        CV_Assert(predicted_labels.rows == true_labels.size());

        //compute the classifier's confusion matrix.
        std::clog << "\tComputing confusion matrix." << std::endl;
        cv::Mat confusion_mat = compute_confusion_matrix(categories.size(), cv::Mat(true_labels), predicted_labels);
        CV_Assert(int(cv::sum(confusion_mat)[0]) == test_bovw.rows);
        double rRate_mean, rRate_dev;
        compute_recognition_rate(confusion_mat, rRate_mean, rRate_dev);
        std::cerr << "Recognition rate mean = " << rRate_mean * 100 << "% dev " << rRate_dev * 100 << std::endl;
        rRates[trail]=rRate_mean;

        if (trail==0 || rRate_mean > best_rRate )
        {
            best_dictionary = dict;
            best_classifier = classifier;
            best_rRate = rRate_mean;
        }
    }

    //Saving the best models.
    cv::FileStorage dictFile;
    dictFile.open("dictionary.yml", cv::FileStorage::WRITE);
    dictFile << "keywords" << keywords.getValue();
   // dictFile << "configFile" <<dataset_desc_file;
    best_dictionary->write(dictFile);
    dictFile.release();
    best_classifier->save("classifier.yml");

    std::clog << "###################### FINAL STATISTICS  ################################" << std::endl;

    double rRate_mean = 0.0;
    double rRate_dev = 0.0;

    for (size_t i=0; i<rRates.size();++i)
    {
        const float v=rRates[i];
        rRate_mean += v;
        rRate_dev += v*v;
    }
    rRate_mean /= double(rRates.size());
    rRate_dev = rRate_dev/double(rRates.size()) - rRate_mean*rRate_mean;
    rRate_dev = sqrt(rRate_dev);
    std::clog << "Recognition Rate mean " << rRate_mean*100.0 << "% dev " << rRate_dev*100.0 << std::endl;
    return 0;
}
