//
// Created by yagi on 18/01/10.
//

#include "basicFunction.h"
#include <sys/stat.h>
#include <sstream>

using namespace std;
//グローバル変数
struct mouseParam {
    int x;
    int y;
};
bool clicked = false;
cv::Point2f clicked_point;


//コールバック関数
void runnerCallBackFunc(int eventType, int x, int y, int flags, void *userdata) {
    switch (eventType) {
        case cv::EVENT_LBUTTONUP:
            std::cout << x << " , " << y << std::endl;
            clicked_point.x = x;
            clicked_point.y = y;
            clicked = true;
    }
}

float yagi::sumOfDistOfPoints(vector<cv::Point2f> ptList1, vector<cv::Point2f> ptList2){
//    CV_ASSERT(ptList1.size() == ptList2.size());
    float sumDist = 0;
    for(int i = 0; i < ptList1.size(); i++){
        sumDist += yagi::calc2PointDistance(ptList1[i], ptList2[i]);
    }
    sumDist /= ptList1.size();
    return sumDist;
}

void yagi::clickPoints(cv::Mat image, vector<cv::Point2f> & clickedPoints, const string file_name) {
    vector<cv::Scalar> colors;
    setColor(&colors);
    ofstream outputTxt(file_name);

    //最初のフレームでスタートラインクリック
    mouseParam mouseEvent;
    string windowName = "click points (Q: finish clicking)";
    cv::namedWindow(windowName, CV_WINDOW_AUTOSIZE);
    cv::setMouseCallback(windowName, runnerCallBackFunc, &mouseEvent);

    while (1) {
        cv::imshow(windowName, image);
        int key = cv::waitKey(1);

        if (clicked) {
            clicked = false;
            cv::circle(image, clicked_point, 2, colors[0], 2);
            cv::Point2f pt(clicked_point.x, clicked_point.y);
            clickedPoints.push_back(pt);
            outputTxt << pt.x << " " << pt.y << endl;
        }
        if (key == 'q')
            break;
    }

    cv::destroyAllWindows();
}


string yagi::digitString(int num, int digit) {
    char c[32];
    sprintf(c, "%d", num);

    string s(c);
    while (s.length() < digit) {
        s = "0" + s;
    }
    return s.c_str();
}

//string用のsplit関数
vector<string> yagi::split(const string &s, char delim) {
    vector<string> elems;
    stringstream ss(s);
    string item;
    while (getline(ss, item, delim)) {
        if (!item.empty()) {
            elems.push_back(item);
        }
    }
    return elems;
}

void yagi::setColor(vector<cv::Scalar> *colors) {
    cv::Scalar color1(255, 255, 255);
    cv::Scalar color2(255, 0, 0);
    cv::Scalar color3(0, 255, 0);
    cv::Scalar color4(0, 0, 255);
    cv::Scalar color5(200, 0, 0);
    cv::Scalar color6(0, 200, 0);
    cv::Scalar color7(255, 0, 200);
    cv::Scalar color8(255, 255, 0);
    cv::Scalar color9(0, 255, 255);
    cv::Scalar color10(100, 255, 0);
    cv::Scalar color11(0, 100, 255);

    colors->push_back(color1);
    colors->push_back(color2);
    colors->push_back(color3);
    colors->push_back(color4);
    colors->push_back(color5);
    colors->push_back(color6);
    colors->push_back(color7);
    colors->push_back(color8);
    colors->push_back(color9);
    colors->push_back(color10);
    colors->push_back(color11);

}

void yagi::loadImage(string imagePath, vector<cv::Mat>* imageList){
    std::ifstream ifs(imagePath);
    std::string str;
    if (ifs.fail())
    {
        std::cerr << "ファイル読み込み失敗" << std::endl;
    }
    while (getline(ifs, str))
    {
        imageList->push_back(cv::imread(str));
//        cv::imshow("im", cv::imread(str));
//        cv::waitKey(1);
    }
}

void yagi::loadImageFromVideo(string videoPath, vector<cv::Mat>& imageList){
    cv::VideoCapture cap(videoPath);
    int IMNUM = int(cap.get(CV_CAP_PROP_FRAME_COUNT));
    cv::Mat image;
    for(int i = 0; i < IMNUM; i++)
    {
        cap >> image;
        imageList.push_back(image);
    }
}

template<typename type>
void yagi::vectorSum(std::vector<type> vec){
    type sum = 0.0;
    for(type elem: vec){
        sum += elem;
    }
    return sum/vec.size();
}

void yagi::generatePointClouds(std::vector<cv::Point3f>& objectCorners, int H, int W, float SCALE, int Wstart, int Hstart){
    int Hrange = H - Hstart;
    int Wrange = W - Wstart;

    for (int i = 0; i < Hrange; i++) {
        for (int j = 0; j < Wrange; j++) {
            objectCorners.push_back(cv::Point3f((j + Wstart) * SCALE, (i + Hstart) * SCALE, 0.0f));
        }
    }
};


void yagi::generatePointCloudsIn2Dscale(std::vector<cv::Point2f>& objectCorners, int H, int W, float SCALE, const int AREA_W, const int AREA_H){
    for (int i = 0; i < H; i++) {
        for (int j = 0; j < W; j++) {
            objectCorners.push_back(cv::Point2f((j * SCALE) + AREA_W, (i * SCALE) + AREA_H));
        }
    }
};

void yagi::generatePointCloudsAsBlocks(std::vector<cv::Point3f>& objectCorners, int H, int W, float SCALE, int Wstart, int Hstart, int blockCols, int blockRows) {
    int Hrange = H - Hstart;
    int Wrange = W - Wstart;

    int blockWidth = Wrange / blockCols;
    int blockHeight = Hrange / blockRows;

    vector<cv::Point3f> blocks[blockCols][blockRows];
    for (int blockCol = 0; blockCol < blockCols; blockCol++) {
        for (int blockRow = 0; blockRow < blockRows; blockRow++) {
            vector<cv::Point3f> block;
            for (int i = 0; i < blockWidth; i++) {
                for (int j = 0; j < blockWidth; j++) {
                    float x = SCALE * (i - W + blockWidth * blockRow);
                    float y = SCALE * (j - H + blockHeight * blockCol);
                    block.push_back(cv::Point3f(x, y, 0.0f));
                    objectCorners.push_back(cv::Point3f(x, y, 0.0f));
                }
            }
            blocks[blockCol][blockRow] = block;
        }
    }
}

void yagi::print_info(const cv::Mat& mat)
{
    using namespace std;

    // 要素の型とチャンネル数の組み合わせ。
    // 紙面の都合により、サンプルで使用する値のみ記述
    cout << "type: " << (
            mat.type() == CV_8UC3 ? "CV_8UC3" :
            mat.type() == CV_16SC1 ? "CV_16SC1" :
            mat.type() == CV_64FC2 ? "CV_64FC2" :
            to_string(mat.type())
    ) << endl;

    // 要素の型
    cout << "depth: " << (
            mat.depth() == CV_8U ? "CV_8U" :
            mat.depth() == CV_16S ? "CV_16S" :
            mat.depth() == CV_32F ? "CV_32F" :
            mat.depth() == CV_64F ? "CV_64F" :
            to_string(mat.depth())
    ) << endl;

    // チャンネル数
    cout << "channels: " << mat.channels() << endl;

    // バイト列が連続しているか
    cout << "continuous: " <<
         (mat.isContinuous() ? "true" : "false")<< endl;
    cout << "rows: " << mat.rows << endl;
    cout << "cols: " << mat.cols << endl;
    cout << "size: " << mat.size() << endl;
    cout << "dims: " << mat.dims << endl;
    cout << endl;
}

void yagi::some_filter(const cv::Mat& in, cv::Mat& out)
{
    // フィルタが unsigned char 3チャンネルのみ受け付ける場合のチェック
    // 大抵の場合、チェックが失敗する時は呼び出し側の実装がミスってるので
    // assert()で落ちてしまって良いと思う
    assert(in.type() == CV_8UC3 && "input image must be CV_8UC3");
}

cv::Mat yagi::maskAofB(cv::Mat A, cv::Mat B) {
    cv::Mat dst;
    A.copyTo(dst, B);
    return dst;
}


//2点からその直線の方程式
void yagi::getGradSegment(cv::Point2f pt1, cv::Point2f pt2, float *grad, float *segment) {
//    cout << "get grad segment " << pt1 << pt2 << endl;
    *grad = (pt1.y - pt2.y) / (pt1.x - pt2.x);
    *segment = pt1.y - (pt1.x * (*grad));
}

//2点を結ぶ直線描画
void yagi::drawLine(cv::Mat edgeMask, cv::Point2f pt1, cv::Point2f pt2, int thickness, cv::Scalar color) {
    float grad = (pt1.y - pt2.y) / (pt1.x - pt2.x);
    pt1.y = pt1.y - (grad * pt1.x);
    pt1.x = 0;

    pt2.x = edgeMask.cols;
    pt2.y = grad * pt2.x + pt1.y;

    cv::line(edgeMask, pt1, pt2, color, thickness);
}

bool yagi::checkFileExistence(const std::string& str)
{
    std::ifstream ifs(str);
    return ifs.is_open();
}

cv::Point2f yagi::getCrossingPoint(float a1, float b1, float a2, float b2){
    float x1 = (b2 - b1)/(a1 - a2);
    float y1 = a1 * x1 + b1;
    return cv::Point2f(x1, y1);
}


void yagi::push4PointsToVector(vector<cv::Point2f> &points, cv::Point2f Hb_pt1, cv::Point2f Hb_pt2, cv::Point2f Hb_pt3, cv::Point2f Hb_pt4){
    points.push_back(Hb_pt1);
    points.push_back(Hb_pt2);
    points.push_back(Hb_pt3);
    points.push_back(Hb_pt4);
}

void yagi::push4_3DPointsToVector(vector<cv::Point3f> &points, cv::Point3f Hb_pt1, cv::Point3f Hb_pt2, cv::Point3f Hb_pt3, cv::Point3f Hb_pt4){
    points.push_back(Hb_pt1);
    points.push_back(Hb_pt2);
    points.push_back(Hb_pt3);
    points.push_back(Hb_pt4);
}

cv::Rect yagi::obtainRectFrom4Points(std::vector<cv::Point2f> &points){
    float minX, maxX, minY, maxY;
    minX = 10000;
    minY = 10000;
    maxX = 0;
    maxY = 0;
    for(cv::Point2f pt: points){
        if(minX > pt.x)
            minX = pt.x;
        if(minY > pt.y)
            minY = pt.y;
        if(maxX > pt.x)
            maxX = pt.x;
        if(maxY > pt.y)
            maxY = pt.y;
    }
    return cv::Rect(minX, minY, maxX - minX, maxY - minY);
}


void yagi::VecToMat(const vector<vector<unsigned char>> src, cv::Mat &dst) {
    // Assume it already has edge information
//    cout << "aa" << endl;
    int height = src.size(), width = src[0].size();
    cv::Mat matrix(height, width, CV_8U);
    matrix = cv::Scalar::all(0);
    for (int j = 0; j < height; j++) {
        for (int i = 0; i < width; i++) {
            matrix.at<unsigned char>(j, i) = src[j][i];
        }
    }
    dst = matrix;
}





float yagi::distPoint2Line(cv::Point2f pt, float a, float b){
    float grad = -(1/a);
    float segment = pt.y - (grad * pt.x);
    cv::Point2f pt_line;
    pt_line.x = (segment - b)/(a - grad);
    pt_line.y = a * pt_line.x + b;

    return calc2PointDistance(pt, pt_line);

}

template<typename Point>
float yagi::calc2PointDistance(Point p1, Point p2) {
    float x_dist = pow((p1.x - p2.x), 2);
    float y_dist = pow((p1.y - p2.y), 2);
    float distance = sqrt((x_dist + y_dist));
    return distance;
}

void  yagi::mycalcfloatWarpedPoint(vector<cv::Point2f> next, vector<cv::Point2f> *warped, cv::Mat H) {

    for (int i = 0; i < next.size(); i++) {
        cv::Point2f next_pt = next[i];
        cv::Point2f warp_pt;

        warp_pt.x =
                H.at<float>(0, 0) * next_pt.x +
                H.at<float>(0, 1) * next_pt.y +
                H.at<float>(0, 2) * 1;

        warp_pt.y =
                H.at<float>(1, 0) * next_pt.x +
                H.at<float>(1, 1) * next_pt.y +
                H.at<float>(1, 2) * 1;

        float z = H.at<float>(2, 0) * next_pt.x +
                  H.at<float>(2, 1) * next_pt.y +
                  H.at<float>(2, 2) * 1;


        cout << warp_pt << endl;
        warp_pt.x = warp_pt.x / z;
        warp_pt.y = warp_pt.y / z;
        cout << warp_pt << endl;

        warped->push_back(warp_pt);

    }
}

cv::Point2f yagi::warpPoint(cv::Point2f srcPt, cv::Mat H) {

    cv::Point2f warp_pt;

    warp_pt.x =
            H.at<double>(0, 0) * srcPt.x +
            H.at<double>(0, 1) * srcPt.y +
            H.at<double>(0, 2) * 1;

    warp_pt.y =
            H.at<double>(1, 0) * srcPt.x +
            H.at<double>(1, 1) * srcPt.y +
            H.at<double>(1, 2) * 1;

    float z = H.at<double>(2, 0) * srcPt.x +
              H.at<double>(2, 1) * srcPt.y +
              H.at<double>(2, 2) * 1;

    warp_pt.x = warp_pt.x / z;
    warp_pt.y = warp_pt.y / z;

    return warp_pt;
}


void yagi::mycalcWarpedPoint(vector<cv::Point2f> next, vector<cv::Point2f> *warped, cv::Mat H) {

    for (int i = 0; i < next.size(); i++) {
        cv::Point2f next_pt = next[i];
        cv::Point2f warp_pt;

//        warp_pt.x =
//                H.at<double>(0, 0) * next_pt.x +
//                H.at<double>(1, 0) * next_pt.x +
//                H.at<double>(2, 0) * next_pt.x;
//
//        warp_pt.y =
//                H.at<double>(0, 1) * next_pt.y +
//                H.at<double>(1, 1) * next_pt.y +
//                H.at<double>(2, 1) * next_pt.y;
//
//        double z = H.at<double>(0, 2) +
//                   H.at<double>(1, 2) +
//                   H.at<double>(2, 2);

//        warp_pt.x =
//                H.at<double>(0, 0) * next_pt.x +
//                H.at<double>(1, 0) * next_pt.y +
//                H.at<double>(2, 0) * 1;
//
//        warp_pt.y =
//                H.at<double>(0, 1) * next_pt.x +
//                H.at<double>(1, 1) * next_pt.y +
//                H.at<double>(2, 1) * 1;
//
//        double z = H.at<double>(0, 2) * next_pt.x +
//                  H.at<double>(1, 2) * next_pt.y +
//                  H.at<double>(2, 2) * 1;

        warp_pt.x =
                H.at<double>(0, 0) * next_pt.x +
                H.at<double>(0, 1) * next_pt.y +
                H.at<double>(0, 2) * 1;

        warp_pt.y =
                H.at<double>(1, 0) * next_pt.x +
                H.at<double>(1, 1) * next_pt.y +
                H.at<double>(1, 2) * 1;

        double z = H.at<double>(2, 0) * next_pt.x +
                   H.at<double>(2, 1) * next_pt.y +
                   H.at<double>(2, 2) * 1;

        warp_pt.x = warp_pt.x / z;
        warp_pt.y = warp_pt.y / z;
////        cout << "homo" << H << endl;
////        for (int ia = 0; ia < 3; ia++) {
//            for (int j = 0; j < 3; j++) {
////                cout << H.at<double>(ia, j) << endl;
//            }
//        }
//        cout << "next" << next_pt << endl;
//        cout << "warp" << warp_pt << endl;

        warped->push_back(warp_pt);

    }
}

void yagi::myMkdir(std::string dir) {
    //ディレクトリ作成
    const char *cstr = dir.c_str();
    cout << dir << endl;
    if (mkdir(cstr, 0777) == 0) {
        printf("directory correctly generated\n");
    } else {
        printf("directory already exists\n");
    }
}


void yagi::videoToImage(std::string videoPath, std::string imageDir,
                        bool imListTxt, std::string imFormat) {
    //入力ファイル名確認
    string video_path = videoPath;
    cout << "input video name: " << video_path << endl;

    //動画読み込み
    cv::VideoCapture capture(videoPath);
    int frame_counter = 0;
    int max_frame=capture.get(CV_CAP_PROP_FRAME_COUNT);

    //画像保存先の確保
    string image_folder = imageDir;
    myMkdir(image_folder);

    //動画からフレームへの分割
    //テキストファイルに出力する場合
    if (imListTxt) {

        //テキストファイルオープン
        string imagelist_path = imageDir + "/imList.txt";
        ofstream outputfile(imagelist_path);
        cout << "saving imagelist.txt: " << imagelist_path << endl;

        //フレーム分だけキャプチャ
        while (capture.grab()) {
            cv::Mat frame;
            capture >> frame;
            string number = digitString(frame_counter++, 4);

            //画像保存
            string image_file_name;
            image_file_name = image_folder + "/image" + number + imFormat;
            cv::imwrite(image_file_name, frame);
            outputfile << image_file_name << endl;
        }
    } else {
        //テキストファイル出力しない場合
        while (frame_counter < max_frame - 1) {
        //フレーム分だけキャプチャ
            cv::Mat frame;
            capture >> frame;

            //画像保存
            string number = digitString(frame_counter++, 4);
            cv::imwrite(image_folder + "/image" + number + "." + imFormat, frame);
        }
    }
}
