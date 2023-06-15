#include <opencv4/opencv2/opencv.hpp> //versione 4.70 di opencv
#include <iostream>
#include <cstdlib>

using namespace cv;
using namespace std;

Mat harris (Mat image);



int main( int argc, char** argv )
{
    string imageName;


    if( argc < 2){
        cout<<"usage: "<<argv[0]<<" image_name"<<endl;
        exit(0);
    }
    imageName = argv[1];


    Mat image = imread(imageName, IMREAD_GRAYSCALE);


    if( image.empty() ){
        cout <<  "Could not open or find the image" << std::endl ;
        return -1;
    }
    int th = 200;


    imshow("Original",image);

    Mat blurred;


    /*VERSIONE DEL PROF*/

    Mat outputPROF = image.clone();
    int blockSize = 2;
    int apertureSize = 3;
    double k = 0.04;
    Mat dst = Mat::zeros( image.size(), CV_32FC1 );
    cornerHarris( image, dst, blockSize, apertureSize, k );
    Mat dst_norm, dst_norm_scaled;
    normalize( dst, dst_norm, 0, 255, NORM_MINMAX, CV_32FC1, Mat() );
    //convertScaleAbs( dst_norm, dst_norm_scaled );
    for( int i = 0; i < dst_norm.rows ; i++ )
    {
        for( int j = 0; j < dst_norm.cols; j++ )
        {
            if( dst_norm.at<float>(i,j) > th )
            {
                circle( outputPROF, Point(j,i), 5,  Scalar(0), 2, 8, 0 );
            }
        }
    }

    imshow("harris prof", outputPROF);






    /*VERSIONE MIA*/

    Mat modified;
    modified = harris(image);
    Mat outputMIO = image.clone();

    Mat mod_norm;
    normalize(modified,mod_norm,0,255,NORM_MINMAX,CV_32F);



    for( int i = 0; i < modified.rows ; i++ )
    {
        for( int j = 0; j < modified.cols; j++ )
        {
            if( mod_norm.at<float>(i,j) > th )
            {
                circle(outputMIO , Point(j,i), 5,  Scalar(0,0,255), 2, 8, 0 );
            }
        }
    }
    imshow("harris mio",outputMIO);




    waitKey(0);




    return 0;
}




Mat harris (Mat image){


    Mat Gx,Gy,Gxy;

    Sobel(image,Gx,CV_32F,1,0);
    Sobel(image,Gy,CV_32F,0,1);

    multiply(Gx,Gy,Gxy);

    pow(Gx,2,Gx);
    pow(Gy,2,Gy);
    pow(Gxy,2,Gxy);

    GaussianBlur(Gx,Gx,Size(5,5),0,0);
    GaussianBlur(Gy,Gy,Size(5,5),0,0);
    GaussianBlur(Gxy,Gxy,Size(5,5),0,0);

    Mat det, track, r, Gx2y2;
    multiply(Gx,Gy,Gx2y2);

    det = Gx2y2 - Gxy;
    track = Gx + Gy;
    pow(track,2,track);
    r = det - (0.04*(track));


    return r;



}

