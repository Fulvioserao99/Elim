#include <opencv4/opencv2/opencv.hpp> //versione 4.70 di opencv
#include <iostream>
#include <cstdlib>

using namespace cv;
using namespace std;

Mat canny (Mat image,int th1,int th2);



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

    Mat blurred;


    //filter2D(image,blurred,CV_32F,getGaussianKernel(9,1));

    Mat canny_orig;
    Canny(image, canny_orig, 100, 240, 3);
    Mat modified;
    modified = canny(image,100,240);
    imshow("Canny mio",modified);
    imshow("Canny vero",canny_orig);



    waitKey(0);




    return 0;
}




Mat canny (Mat image,int th1,int th2){

    Mat blurred;
    //GaussianBlur(image,blurred,Size(3,3),0,0);
    filter2D(image,blurred,CV_32F,getGaussianKernel(9,1));
    Mat Gx,Gy,Mag,Dir,NMS;

    Sobel(blurred,Gx,CV_32F,1,0);
    Sobel(blurred,Gy,CV_32F,0,1);
    pow(Gx,2,Gx);
    normalize(Gx,Gx,0,255,NORM_MINMAX,CV_32F);
    pow(Gy,2,Gy);
    normalize(Gy,Gy,0,255,NORM_MINMAX,CV_32F);
    Mag = Gx+Gy;
    sqrt(Mag,Mag); //Magnitudo

    normalize(Mag,Mag,0,255,NORM_MINMAX,CV_8U);
    Mag.copyTo(NMS);


    phase(Gy,Gx,Dir,true);
    normalize(Dir,Dir,-180,180,NORM_MINMAX,CV_32F);


    for(int i=0; i<NMS.rows; i++){

        for (int j=0; j<NMS.cols; j++){

            if((-22.5 < Dir.at<float>(i,j) &&  Dir.at<float>(i,j)<= 22.5) || (-157.5 < Dir.at<float>(i,j) && Dir.at<float>(i,j) <= 157.5)){

                if(Mag.at<uchar>(i,j) < Mag.at<uchar>(i,j+1) || Mag.at<uchar>(i,j) < Mag.at<uchar>(i,j-1))

                    NMS.at<uchar>(i,j) = 0;

            }

            else if ((-112.5 < Dir.at<float>(i,j) && Dir.at<float>(i,j) <= -67.5) || (67.5 < Dir.at<float>(i,j) && Dir.at<float>(i,j) <= 112.5)){

                if(Mag.at<uchar>(i,j) < Mag.at<uchar>(i+1,j) || Mag.at<uchar>(i,j) < Mag.at<uchar>(i-1,j))

                    NMS.at<uchar>(i,j) = 0;


            }

            else if ((-67.5 < Dir.at<float>(i,j) && Dir.at<float>(i,j) <= -22.5) || (112.5 < Dir.at<float>(i,j) && Dir.at<float>(i,j) <= 157.5)){

                if(Mag.at<uchar>(i,j) < Mag.at<uchar>(i-1,j-1) || Mag.at<uchar>(i,j) < Mag.at<uchar>(i+1,j+1))

                    NMS.at<uchar>(i,j) = 0;


            }

            else{

                if(Mag.at<uchar>(i,j) < Mag.at<uchar>(i+1,j-1) || Mag.at<uchar>(i,j) < Mag.at<uchar>(i-1,j+1))

                    NMS.at<uchar>(i,j) = 0;


            }

        }

    }


    Mat NMS_th2;

    threshold(NMS,NMS_th2,th2,255,THRESH_TRUNC);

    for(int i=0; i<NMS_th2.rows; i++){

        for (int j=0; j<NMS_th2.cols; j++){


            if(NMS_th2.at<uchar>(i,j) > th1){

                for(int k=-1; k<=1; k++){

                    for(int m=-1; m<=1; m++){

                        if(NMS_th2.at<uchar>(i+k,j+m) == 255){

                            NMS_th2.at<uchar>(i,j) = 255;


                        }


                    }
                }
            }
        }


    }

    //normalize(NMS,NMS,0,255,NORM_MINMAX,CV_8U);







    return NMS_th2;

}

