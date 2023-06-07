#include <iostream>
#include <stack>
#include <opencv4/opencv2/opencv.hpp>


const int Threshold = 200;
const uchar max_region_num = 100;
const double min_region_area_factor = 0.01;


using namespace cv;
using namespace std;

void grow(Mat &image, Mat &mask, Mat &dest, Point seed, int th);

int main(int argc, char **argv){

    Mat image = imread(argv[1]);

    if (image.cols > 500 || image.rows > 500) {
        resize(image, image, Size(0, 0), 0.5, 0.5);
    }

    imshow("Source image", image);
    waitKey(0);

    int min_region_area = int(min_region_area_factor * image.cols * image.rows);


    uchar padding = 1;
    Mat dest = Mat::zeros(image.size(), CV_8UC1);


    Mat mask = Mat::zeros(image.size(), CV_8UC1);

    int mask_area;


    for (int i=0; i<image.cols; ++i) {
        for (int j=0; j<image.rows; ++j) {

            if (dest.at<uchar>(Point(i,j)) == 0){

                grow(image,mask,dest,Point(i,j),Threshold);

                mask_area = (int)cv::sum(mask).val[0];
                if(mask_area > min_region_area){

                    dest += mask*padding;
                    imshow("mask", mask*255);
                    waitKey(0);

                    if(++padding > max_region_num) {

                        printf("Numero di regioni esaurito");
                        return -1;

                    }

                }
                else
                    dest += mask*255;

                mask.setTo(0);
            }
        }
    }

   return 0;

}




void grow(Mat &image, Mat &mask, Mat &dest, Point seed, int th){


    int delta;
    stack<Point> Stack;
    Stack.push(seed);

    while(!Stack.empty()){

        Point actual = Stack.top();
        mask.at<uchar>(actual) = 1;
        Stack.pop();

        for (int k=-1; k<=1; ++k){

            for (int l=-1; l<=1; ++l){

                Point possible = actual + Point(l,k);

                if (possible == actual)

                    continue;

                if ((possible.x >= 0) && (possible.x < image.cols) && (possible.y >= 0) && (possible.y < image.rows)){

                    delta = int(pow(image.at<Vec3b>(actual)[0] - image.at<Vec3b>(possible)[0], 2)
                                + pow(image.at<Vec3b>(actual)[1] - image.at<Vec3b>(possible)[1], 2)
                                + pow(image.at<Vec3b>(actual)[2] - image.at<Vec3b>(possible)[2], 2));


                    if (dest.at<uchar>(possible) == 0 && mask.at<uchar>(possible) == 0 && delta < th){

                        mask.at<uchar>(possible) = 1;
                        Stack.push(possible);

                    }
                }
            }
        }
    }



}
