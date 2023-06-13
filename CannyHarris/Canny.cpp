#include <opencv4/opencv2/opencv.hpp> //versione 4.70 di opencv
#include <iostream>
#include <cstdlib>

using namespace cv;
using namespace std;

Mat canny (Mat image,float th1,float th2);
void cannyT(int,void*);

Mat blurred;

int th1=1;
int th2=1;

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



    imshow("Original",image);

    //Mat blurred;



    //filter2D(image,blurred,CV_32F,getGaussianKernel(9,1));

    GaussianBlur(image,blurred,Size(5,5),0,0);
    Mat canny_orig;
    Canny(blurred, canny_orig, 60, 180, 3);
    Mat modified;

    modified = canny(image,th1,th2);

    imshow("Canny mio",modified);
    //createTrackbar("Min_t","Canny mio",&th1,200,cannyT);
    //createTrackbar("Max_t","Canny mio",&th2,200,cannyT);
    //cannyT(0,0);
    imshow("Canny vero",canny_orig);



    waitKey(0);




    return 0;
}

void cannyT(int,void*){

    Mat mod;
    mod = canny(blurred,th1,th2);

    imshow("Canny mio",mod);

}



Mat canny (Mat image,float th1,float th2){

    Mat blurred;
    //GaussianBlur(image,blurred,Size(5,5),0);
    filter2D(image,blurred,CV_32FC1,getGaussianKernel(25,4));
    Mat Gx,Gy,Mag,Dir,NMS;
    Mat Gx2,Gy2;
    Sobel(image,Gx,CV_32FC1,1,0);
    Sobel(image,Gy,CV_32FC1,0,1);
    pow(Gx,2,Gx2);
    //normalize(Gx2,Gx2,0,255,NORM_MINMAX,CV_32F);
    pow(Gy,2,Gy2);
    //normalize(Gy2,Gy2,0,255,NORM_MINMAX,CV_32F);
    Mag = Gx2+Gy2;
    sqrt(Mag,Mag); //Magnitudo

    Mat Mag2;
    normalize(Mag,Mag2,0,255,NORM_MINMAX,CV_32FC1);
    Mag2.copyTo(NMS);


    phase(Gy,Gx,Dir,true);
    normalize(Dir,Dir,-180,180,NORM_MINMAX,CV_32FC1);


    for(int i=0; i<NMS.rows; i++){

        for (int j=0; j<NMS.cols; j++){

            if((-22.5 <= Dir.at<float>(i,j) &&  Dir.at<float>(i,j)< 22.5) || (-157.5 < Dir.at<float>(i,j) && Dir.at<float>(i,j) <= 157.5)){

                if(Mag2.at<float>(i,j) < Mag2.at<float>(i,j+1) || Mag2.at<float>(i,j) < Mag2.at<float>(i,j-1))

                    NMS.at<float>(i,j) = 0;

            }

            else if ((-112.5 <= Dir.at<float>(i,j) && Dir.at<float>(i,j) < -67.5) || (67.5 < Dir.at<float>(i,j) && Dir.at<float>(i,j) <= 112.5)){

                if(Mag2.at<float>(i,j) < Mag2.at<float>(i+1,j) || Mag2.at<float>(i,j) < Mag2.at<float>(i-1,j))

                    NMS.at<float>(i,j) = 0;


            }

            else if ((-67.5 <= Dir.at<float>(i,j) && Dir.at<float>(i,j) < -22.5) || (112.5 < Dir.at<float>(i,j) && Dir.at<float>(i,j) <= 157.5)){

                if(Mag2.at<float>(i,j) < Mag2.at<float>(i-1,j-1) || Mag2.at<float>(i,j) < Mag2.at<float>(i+1,j+1))

                    NMS.at<float>(i,j) = 0;


            }

            else{

                if(Mag2.at<float>(i,j) < Mag2.at<float>(i+1,j-1) || Mag2.at<float>(i,j) < Mag2.at<float>(i-1,j+1))

                    NMS.at<float>(i,j) = 0;


            }

        }

    }


    /*Mat NMS_th2;

    threshold(NMS,NMS_th2,th2,255,THRESH_TRUNC);

    for(int i=0; i<NMS_th2.rows; i++){

        for (int j=0; j<NMS_th2.cols; j++){


            if(NMS_th2.at<uchar>(i,j) > th1){

                for(int k=-1; k<=1; k++){

                    for(int m=-1; m<=1; m++){

                        if(NMS_th2.at<uchar>(i+k,j+m) == 255){

                            NMS_th2.at<uchar>(i,j) = 255;
                            continue;


                        }


                    }
                }
            }

        }


    }*/

    for(int i=0; i<NMS.rows; i++){

        for (int j=0; j<NMS.cols; j++){

            if(NMS.at<float>(i,j) > th2)

                NMS.at<float>(i,j) = 255;


            else if (NMS.at<float>(i,j) < th1)

                NMS.at<float>(i,j) = 0;

            else if(NMS.at<float>(i,j) > th1 && NMS.at<float>(i,j) < th2){

                bool strong = false;


                for(int k=-1; k<=1; k++){

                    for(int m=-1; m<=1; m++){

                        if (NMS.at<float>(i+k,j+m) == 255)

                            strong = true;
                    }

                }

                if (strong)

                    NMS.at<float>(i,j) = 255;

                else

                    NMS.at<float>(i,j) = 0;


            }

        }

    }

    //threshold(NMS_th2,NMS_th2,th1,255,THRESH_TOZERO);

    //normalize(NMS,NMS,0,255,NORM_MINMAX,CV_8U);







    return NMS;

}

