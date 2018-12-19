
#include "openPosePerson.h"

using namespace std;

cv::Mat OpenPosePerson::calcOpenPoseMask(const int OP_MASK_RADIUS, cv::Size imSize){
    cv::Mat mask = cv::Mat::zeros(imSize, CV_8U);
    cv::Scalar WHITE(255,255,255);
    vector<cv::Point2f> pts = this->_body_parts_coord;
    for(cv::Point2f pt: pts){
        if(pt != cv::Point2f(0,0)) {
            cv::circle(mask, pt, OP_MASK_RADIUS, WHITE, -1, CV_AA);
        }
    }
    cv::morphologyEx(mask, mask, cv::MORPH_CLOSE, 2);
    return mask;
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
