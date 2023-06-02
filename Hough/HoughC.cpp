#include <opencv4/opencv2/opencv.hpp> //versione 4.70 di opencv
#include <iostream>
#include <cstdlib>

using namespace cv;
using namespace std;

Mat hough (Mat image,int rmin, int rmax, int th);

Mat houghOriginal(Mat image);



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

    int minR = 40, maxR = 200, th = 150;

    Mat houghC = hough(image,minR,maxR,th);
    Mat houghCO = houghOriginal(image);


    imshow("Immagine originale",image);

    imshow("Output Hough mio",houghC);

    imshow("Output Hough originale",houghCO);



    waitKey(0);



    return 0;

}

Mat houghOriginal(Mat image){

    Mat blurred,gray;
    Mat output = image.clone();

    GaussianBlur(image,blurred,Size(9,9),0,0);

    cvtColor(blurred, gray, COLOR_BGR2GRAY);

    // Store the detected circles in a 3d-vector
    vector<Vec3f> circles;

    // Apply hough transform
    HoughCircles(gray, circles, HOUGH_GRADIENT, 1,5,160,100,20,100); //ci sarebbe da giocare coi valori

    // Draw the detected circles
    for( size_t i = 0; i < circles.size(); i++ )
    {
         Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));
         int radius = cvRound(circles[i][2]);
         // Draw the center point of the circle
         circle( output, center, 3, Scalar(0,255,0), -1, 8, 0 );
         // Draw the circle shape
         circle( output, center, radius, Scalar(0,255,0), 3, 8, 0 );
    }


    return output;
}


Mat hough (Mat image,int rmin, int rmax, int th){

    int maxrows = 2*image.rows;
    int maxcols = 2*image.cols;
    int dim[3]={maxrows,maxcols,rmax-rmin+1};
    Mat H(3,dim,CV_8U,Scalar(0));

    Mat output = image.clone();
    Mat blurred,canny;

    double a,b;

    GaussianBlur(image,blurred,Size(9,9),0,0);
    Canny(blurred, canny, 80, 160, 3);

    for (int i=0; i<canny.rows; i++){

        for (int j=0; j<canny.cols; j++){


            if (canny.at<uchar>(i,j) == 255){


                for (int theta=0; theta<=360; theta++){

                    for (int R=rmin; R<=rmax; R++){

                        a = ((j - R*cos(theta*(CV_PI/180)))) + image.cols;
                        b = ((i - R*sin(theta*(CV_PI/180)))) + image.rows;

                        if((a+R < maxcols) && (b+R < maxrows)){
                            H.at<uchar>(round(b),round(a),R)++;
                        }

                    }
                }
            }
        }
    }


    for (int i=0; i<maxrows; i++){

        for (int j=0; j<maxcols; j++){

            for (int R=rmin; R<=rmax; R++){

                if(H.at<uchar>(i,j,R) > th){

                    int a,b;

                    a = i-image.rows;
                    b = j-image.cols;
                    cout<<"c";
                    circle(output , Point(b,a), R,  Scalar(0,0,255), 2, 8, 0 );
                }

            }

        }
    }



    return output;

}
