//
// Created by yagi on 18/07/25.
//

#include "openPosePerson.h"

using namespace std;

//
// Created by yagi on 18/10/17.
//
// ----------------------------- OpenPose C++ API Tutorial - Example 1 - Body from image -----------------------------
// It reads an image, process it, and displays it with the pose keypoints.

// Command-line user intraface
#define OPENPOSE_FLAGS_DISABLE_POSE

#include <sys/stat.h>
#include<iostream>
#include<fstream>

using namespace cv;
using namespace std;


// Custom OpenPose flags
// Producer
//DEFINE_string(image_path, "examples/media/COCO_val2014_000000000192.jpg", "Process an image. Read all standard formats (jpg, png, bmp, etc.).");

// This worker will just read and return all the jpg files in a directory
void display(const std::shared_ptr<std::vector<op::Datum>>& datumsPtr)
{
    // User's displaying/saving/other processing here
    // datum.cvOutputData: rendered frame with pose or heatmaps
    // datum.poseKeypoints: Array<float> with the estimated pose
    if (datumsPtr != nullptr && !datumsPtr->empty())
    {
        // Display image
        cv::imshow("User worker GUI", datumsPtr->at(0).cvOutputData);
        cv::waitKey(1);
    }
    else
        op::log("Nullptr or empty datumsPtr found.", op::Priority::High);
}

void printKeypoints(const std::shared_ptr<std::vector<op::Datum>>& datumsPtr)
{
    // Example: How to use the pose keypoints
    if (datumsPtr != nullptr && !datumsPtr->empty())
    {
        // Alternative 1
//        op::log("Body keypoints: " + datumsPtr->at(0).poseKeypoints.toString());

        // // Alternative 2
//        op::log(datumsPtr->at(0).poseKeypoints);

        // // Alternative 3
        cout << datumsPtr->at(0).poseKeypoints.getVolume() << endl;
        std::cout << datumsPtr->at(0).poseKeypoints << std::endl;

        // // Alternative 4 - Accesing each element of the keypoints
        // op::log("\nKeypoints:");
        // const auto& poseKeypoints = datumsPtr->at(0).poseKeypoints;
        // op::log("Person pose keypoints:");
        // for (auto person = 0 ; person < poseKeypoints.getSize(0) ; person++)
        // {
        //     op::log("Person " + std::to_string(person) + " (x, y, score):");
        //     for (auto bodyPart = 0 ; bodyPart < poseKeypoints.getSize(1) ; bodyPart++)
        //     {
        //         std::string valueToPrint;
        //         for (auto xyscore = 0 ; xyscore < poseKeypoints.getSize(2) ; xyscore++)
        //             valueToPrint += std::to_string(   poseKeypoints[{person, bodyPart, xyscore}]   ) + " ";
        //         op::log(valueToPrint);
        //     }
        // }
        // op::log(" ");
    }
    else
        op::log("Nullptr or empty datumsPtr found.", op::Priority::High);
}

void outputTextFromImage(const std::string video_path, const std::string output_path, vector<cv::Mat> images){

    //ディレクトリ作成
    const char *cstr = output_path.c_str();
    if (mkdir(cstr, 0777) == 0) {
        printf("directory correctly generated\n");
    } else {
        printf("directory already exists\n");
    }
    string output_image_path = output_path + "/images/";
    const char *cstr2 = output_image_path.c_str();
    if (mkdir(cstr2, 0777) == 0) {
        printf("directory correctly generated\n");
    } else {
        printf("directory already exists\n");
    }

    ofstream outputfile(output_path + "/human_pose_info.txt");
    ofstream imageoutputfile(output_path + "/imagelist.txt");

    op::Wrapper opWrapper{op::ThreadManagerMode::Asynchronous};
    opWrapper.start();

    // Process and display image

    for(int i=0; i<images.size();i++){

//        cv::resize(images[i],images[i],Size(), 0.25, 0.25);

        //cv::Mat dummy = cv::Mat::zeros(images[i].cols, images[i].rows * (images[i].), CV_32FC3);

        auto datumProcessed = opWrapper.emplaceAndPop(images[i]);
        if (datumProcessed != nullptr)
        {
//            printKeypoints(datumProcessed);
            if (datumProcessed != nullptr && !datumProcessed->empty())
            {
                // // Alternative 3
//                std::cout << i << std::endl;
//                std::cout << "a " << datumProcessed->at(0).poseKeypoints.getNumberDimensions() << std::endl;
//                std::cout << "b " << datumProcessed->at(0).poseKeypoints.getVolume() << std::endl;
//                std::cout << "c " << datumProcessed->at(0).poseKeypoints.getSize().size()<< std::endl;
//                std::cout << "d " << datumProcessed->at(0).poseKeypoints << std::endl;
//                outputfile << "Frame: " << i << endl;
                int elem_num = datumProcessed->at(0).poseKeypoints.getVolume();
                for (int j = 0; j < elem_num; j++){
                    if(j % 75 == 0){
                        outputfile << "Person " << (j/75) << " (x, y, score):" << endl;
                    }
                    outputfile << datumProcessed->at(0).poseKeypoints[j] << " ";
                    if(j % 3 == 2){
                        outputfile << endl;
                    }
                }

            }
            else
                op::log("Nullptr or empty datumsPtr found.", op::Priority::High);

            display(datumProcessed);
            cv::imwrite(output_image_path + yagi::digitString(i, 4) + ".jpg", datumProcessed->at(0).cvOutputData);
            imageoutputfile << output_image_path + yagi::digitString(i,4) + ".jpg" << endl;
        }
        else
            op::log("Image could not be processed.", op::Priority::High);

    }

    outputfile.close();
    // Return successful message
    op::log("Stopping OpenPose...", op::Priority::High);
}


void outputTextFromVideo(const std::string video_path, const std::string output_path){

    //ディレクトリ作成
    const char *cstr = output_path.c_str();
    if (mkdir(cstr, 0777) == 0) {
        printf("directory correctly generated\n");
    } else {
        printf("directory already exists\n");
    }
    string output_image_path = output_path + "/images/";
    const char *cstr2 = output_image_path.c_str();
    if (mkdir(cstr2, 0777) == 0) {
        printf("directory correctly generated\n");
    } else {
        printf("directory already exists\n");
    }

    ofstream outputfile(output_path + "/human_pose_info.txt");
    ofstream imageoutputfile(output_path + "/imagelist.txt");

    Mat img;
    VideoCapture cap(video_path);
    op::Wrapper opWrapper{op::ThreadManagerMode::Asynchronous};
    opWrapper.start();

    // Process and display image
    int max_frame=cap.get(CV_CAP_PROP_FRAME_COUNT);
    for(int i=0; i<max_frame;i++){
        cap>>img ; //1フレーム分取り出してimgに保持させる
        cv::resize(img,img,Size(), 640.0/img.cols, 320.0/img.rows);

        //cv::Mat dummy = cv::Mat::zeros(img.cols, img.rows * (img.), CV_32FC3);

        auto datumProcessed = opWrapper.emplaceAndPop(img);
        if (datumProcessed != nullptr)
        {
//            printKeypoints(datumProcessed);
            if (datumProcessed != nullptr && !datumProcessed->empty())
            {
                // // Alternative 3
//                std::cout << i << std::endl;
//                std::cout << "a " << datumProcessed->at(0).poseKeypoints.getNumberDimensions() << std::endl;
//                std::cout << "b " << datumProcessed->at(0).poseKeypoints.getVolume() << std::endl;
//                std::cout << "c " << datumProcessed->at(0).poseKeypoints.getSize().size()<< std::endl;
//                std::cout << "d " << datumProcessed->at(0).poseKeypoints << std::endl;
//                outputfile << "Frame: " << i << endl;
                int elem_num = datumProcessed->at(0).poseKeypoints.getVolume();
                for (int j = 0; j < elem_num; j++){
                    if(j % 75 == 0){
                        outputfile << "Person " << (j/75) << " (x, y, score):" << endl;
                    }
                    outputfile << datumProcessed->at(0).poseKeypoints[j] << " ";
                    if(j % 3 == 2){
                        outputfile << endl;
                    }
                }

            }
            else
                op::log("Nullptr or empty datumsPtr found.", op::Priority::High);

            display(datumProcessed);
            cv::imwrite(output_image_path + yagi::digitString(i, 4) + ".jpg", datumProcessed->at(0).cvOutputData);
            imageoutputfile << output_image_path + yagi::digitString(i,4) + ".jpg" << endl;
        }
        else
            op::log("Image could not be processed.", op::Priority::High);

    }

    outputfile.close();
    // Return successful message
    op::log("Stopping OpenPose...", op::Priority::High);
}

void OpenPosePerson::setBodyCoord(vector<string> coord) {
    cv::Point2f coord_f(stof(coord[0]), stof(coord[1]));
    _body_parts_coord.push_back(coord_f);
}

vector<cv::Point2f> OpenPosePerson::getBodyCoord() {
    return this->_body_parts_coord;
}


void OpenPosePerson::clearBodyCoord() {
    _body_parts_coord.clear();
}

cv::Mat OpenPosePerson::getMaskImage(const int radius, cv::Size imSize){
    cv::Mat mask = cv::Mat::zeros(imSize, CV_8U);
    cv::Scalar WHITE(255,255,255);
    for(cv::Point2f pt: this->_body_parts_coord){
        if(pt != cv::Point2f(0,0)) {
            cv::circle(mask, pt, radius, WHITE, -1, CV_AA);
        }
    }
    cv::morphologyEx(mask, mask, cv::MORPH_CLOSE, 2);
    return mask;
}

void DetectTargetPerson(op::Array<float>& poses, vector<OpenPosePerson>& personList, OpenPosePerson& target, bool USING_PROB) {
    int peopleNum = poses.getSize()[0];
    for (int personID = 0; personID < peopleNum; personID++) {
        OpenPosePerson newPerson;
        for (int partID = 0; partID < 25; partID++) {
            cv::Point2f pt(poses[(personID * 75) + (partID * 3)], poses[(personID * 75) + (partID * 3) + 1]);
            newPerson._body_parts_coord.push_back(pt);
            newPerson._probabilityList.push_back(poses[(personID * 75) + (partID * 3)] + 2);
        }
        personList.push_back(newPerson);
    }

    //トラッキング対象人物を決定
    int targetID = 0;
    if (USING_PROB) {
        float maxProb = 0;
        for (int personID = 0; personID < peopleNum; personID++) {
            personList[personID]._probabilityList;
            float sumOfProbability = float(
                    accumulate(personList[personID]._probabilityList.begin(), personList[personID]._probabilityList.end(),
                               0.0));
            if (maxProb < sumOfProbability) {
                targetID = personID;
                maxProb = sumOfProbability;
            }
        }
    }else{
        float minDist = 10000;
        vector<cv::Point2f> clickPoints;
        string trackingFileName = _projects_path + "/trackingTargetPosition.txt";
        ifstream pointFile(trackingFileName);
        yagi::clickPoints(backGroundImage, clickPoints, trackingFileName);

//        if(pointFile.fail()) {
//        }else {
//            string str;
//            vector<string> strList;
//            while (getline(pointFile, str))
//            {
//                cv::Point2f pt;
//                strList = yagi::split(str, ' ');
//                pt.x = stof(strList[0]);
//                pt.y = stof(strList[1]);
//                clickPoints.push_back(pt);
//            }
//        }
        //TODO 複数人のトラッキング
        for (int personID = 0; personID < peopleNum; personID++) {
            float dist = yagi::calc2PointDistance(personList[personID]._body_parts_coord[5], clickPoints[0]);
            if(minDist > dist){
                minDist = dist;
                targetID = personID;
            }
        }
    }
    personList[targetID].humanID = 1;
    target = personList[targetID];
}
