#include <opencv4/opencv2/opencv.hpp> //versione 4.70 di opencv
#include <iostream>
#include <cstdlib>

using namespace cv;
using namespace std;

Mat hough (Mat image,int thresh);
void polarToCartesian(double rho, int theta, Point& p1, Point& p2);
Mat houghOriginal(Mat image, int thresh);



int main( int argc, char** argv )
{
    string imageName;


    if( argc < 2){
        cout<<"usage: "<<argv[0]<<" image_name"<<endl;
        exit(0);
    }
    imageName = argv[1];


    Mat image = imread(imageName, IMREAD_COLOR);


    if( image.empty() ){
        cout <<  "Could not open or find the image" << std::endl ;
        return -1;
    }

    int threshold = 80;

    Mat houghL = hough(image,threshold);
    Mat houghOriginalL = houghOriginal(image,threshold);

    imshow("Immagine originale",image);

    imshow("Output Hough mio",houghL);

    imshow("Output Hough originale",houghL);

    waitKey(0);



    return 0;

}

Mat houghOriginal(Mat image, int thresh){

    Mat blurred,canny;

    GaussianBlur(image,blurred,Size(9,9),0,0);
    Canny(blurred, canny, 80, 160, 3);

    // Definisci i parametri dell'accumulatore di Hough
    float rhoResolution = 1.0;  // Risoluzione di rho
    float thetaResolution = CV_PI / 180.0;  // Risoluzione di theta in radianti
     // Soglia per l'accumulatore di Hough

    // Esegue la trasformata di Hough per le linee
    vector<cv::Vec2f> lines;
    HoughLines(canny, lines, rhoResolution, thetaResolution, thresh);

    // Disegna le linee rilevate sull'immagine di input
    Mat output = image.clone();
    for (const auto& line : lines) {
        float rho = line[0];
        float theta = line[1];

        double cosTheta = std::cos(theta);
        double sinTheta = std::sin(theta);

        double x0 = rho * cosTheta;
        double y0 = rho * sinTheta;
        double x1 = x0 + 1000 * (-sinTheta);
        double y1 = y0 + 1000 * (cosTheta);
        double x2 = x0 - 1000 * (-sinTheta);
        double y2 = y0 - 1000 * (cosTheta);

        cv::Point pt1(x1, y1);
        cv::Point pt2(x2, y2);

        cv::line(output, pt1, pt2, cv::Scalar(0, 0, 255), 1, cv::LINE_AA);
    }

    return output;
}


Mat hough (Mat image,int thresh){

    Mat blurred, canny,output;

    output = image.clone();

    GaussianBlur(image,blurred,Size(9,9),0,0);
    Canny(blurred, canny, 80, 160, 3);


    int maxRho = hypot(canny.rows, canny.cols);
    int maxTheta = 180;


    Mat M = Mat::zeros(2*maxRho,maxTheta,CV_8U);

    double rho;

    for (int i=0; i<canny.rows; i++){

        for (int j=0; j<canny.cols; j++){


            if (canny.at<uchar>(i,j) == 255){

                for (int theta=0; theta<=maxTheta; theta++){

                    rho = j*cos(theta) + i*sin(theta) + maxRho;

                    M.at<uchar>((saturate_cast<int>(rho)),theta)++;


                }


            }
        }

    }



    for (int i=0; i<M.rows; i++){

        for (int j=0; j<M.cols; j++){

            if (M.at<uchar>(i,j) >= thresh){

                Point p1,p2;
                polarToCartesian(i-maxRho,j,p1,p2);
                line(output, p1, p2, Scalar(0, 0, 255), 1, LINE_AA);

            }
        }
    }



    return output;

}


void polarToCartesian(double rho, int theta, Point& p1, Point& p2) {

	int x0 = cvRound(rho * cos(theta));
	int y0 = cvRound(rho * sin(theta));

	p1.x = cvRound(x0 + 1000 * (-sin(theta)));
	p1.y = cvRound(y0 + 1000 * (cos(theta)));

	p2.x = cvRound(x0 - 1000 * (-sin(theta)));
	p2.y = cvRound(y0 - 1000 * (cos(theta)));
}

