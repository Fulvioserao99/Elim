#include <opencv4/opencv2/opencv.hpp> //versione 4.70 di opencv
#include <iostream>
#include <cstdlib>

using namespace cv;
using namespace std;

Mat filter2d_fake (int type, Mat image1,Mat filter);


int main( int argc, char** argv )
{
    string imageName;
    float mean9 = 1.0/9.0;
    float data[] = {mean9,mean9,mean9,mean9,mean9,mean9,mean9,mean9,mean9}; //creo i valori per il filtro media
    Mat filter = Mat(3,3,CV_32F,data); //li inserisco nel filtro media



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

    Mat result = filter2d_fake(0,image,filter); //chiamo il filtro media sull'immagine con correlazione
    imshow("Original", image);
    imshow("Filtered", result);
    waitKey(0);




    return 0;
}

//funzione che "emula" filter2d: in input prende il tipo (0 correlaz/1 convoluz), l'immagine e il filtro
Mat filter2d_fake (int type, Mat image1, Mat filter){

    int padding = filter.rows/2;
    Mat padded;
    copyMakeBorder(image1,padded,padding,padding,padding,padding,BORDER_REPLICATE);
    Mat result(padded);
    imshow("newmatrix", result);
    waitKey(0);





    if(type == 0){
        for(int i=padding; i<padded.rows-1; i++)
            for (int j=padding; j<padded.cols-1; j++){
                float sum = 0;
                for (int k=-padding; k<=padding; k++)
                    for (int m=-padding; m<=padding; m++){
                        sum += padded.at<uchar>(i+k,j+m) * filter.at<float>(k+padding,m+padding); //applico la formula delle slides

                    }
                result.at<uchar>(i,j) = (float) sum;

            }
    }


    else if (type == 1){
        Mat r_filter;
        rotate(filter,r_filter,ROTATE_180);
        for(int i=padding; i<padded.rows-1; i++)
            for (int j=padding; j<padded.cols-1; j++){
                float sum = 0;
                for (int k=-padding; k<=padding; k++)
                    for (int m=-padding; m<=padding; m++){
                        sum += padded.at<uchar>(i+k,j+m) * r_filter.at<float>(k+padding,m+padding);//applico la formula delle slides
                    }
                result.at<uchar>(i,j) = (float) sum;
            }
    }

    else{
        puts("Wrong first argument -- 1 for correl or 2 for convol");
        exit(-1);
    }



    return result;


}
