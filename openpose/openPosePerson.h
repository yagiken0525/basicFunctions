//
// Created by yagi on 18/07/25.
//

#ifndef SFMDR_FOOTPRINT_OPENPOSEPERSON_H
#define SFMDR_FOOTPRINT_OPENPOSEPERSON_H

#include <iostream>
#include <vector>
#include <opencv2/opencv.hpp>

class OpenPosePerson {
public:
    OpenPosePerson() {};
    ~OpenPosePerson() {};

    cv::Mat calcOpenPoseMask(std::vector<cv::Point2f>& body_parts_coord, const int radius, cv::Size imSize);
    void setBodyCoord(std::vector<std::string> coord);
    std::vector<cv::Point2f> getBodyCoord();
    void clearBodyCoord();

    int humanID = 0;
    cv::Mat getMaskImage(const int radius, cv::Size imSize);
    std::vector<cv::Point2f> _body_parts_coord;
    std::vector<float> _probabilityList;
    cv::Rect humanRect;
};





#endif //SFMDR_FOOTPRINT_OPENPOSEPERSON_H
