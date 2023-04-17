#include <opencv4/opencv2/opencv.hpp> //versione 4.70 di opencv
#include <iostream>

using namespace cv;
using namespace std;

Mat padding(Mat image, int n);
Mat blur(Mat image);
uchar sum_neigh(Mat image, int i, int j, int rgb);
Mat padding_plus(Mat image, int n);
Mat blur_plus(Mat image);


int main( int argc, char** argv )
{
    string imageName;

    if( argc > 1)
    imageName = argv[1];

    else{

        puts("Usage -- ./p_name img_name \n");
        exit(-1);
    }


    Mat image;

    image = imread( samples::findFile( imageName ), IMREAD_COLOR);

    Mat padded = padding(image,1);

    Mat blurred = blur(image);

    Mat padded_plus = padding_plus(image,1);

    Mat blurred_plus = blur_plus(image);




    if( image.empty() ){
        cout <<  "Could not open or find the image" << std::endl ;
        return -1;
    }



    imshow( "Immagine originale", image );
    waitKey(0);

    imshow( "Padding black", padded );
    waitKey(0);

    imshow( "Padding omogeneo", padded_plus );
    waitKey(0);

    imshow( "Sfocata padding black", blurred );
    waitKey(0);

    imshow( "Sfocata padding plus", blurred_plus );
    waitKey(0);

    return 0;
}

Mat padding(Mat image, int n){ //padding di una immagine aggiungendo 2*n righe e 2*n colonne

    Mat padded(image.rows+2*n,image.cols+2*n,image.type(),Scalar(0));
    image.copyTo(padded(Rect(n,n,image.cols,image.rows))); //copiami la Mat image nel rettangolo di padded che parte dall'elemento (n;n) e arriva all'elemento (cols-n;rows-n)

    return padded;

}

Mat padding_plus(Mat image, int n){ //padding di una immagine specchiando le righe // da rivedere

    Mat padded;
    padded.create(image.rows+2*n,image.cols+2*n,image.type());
    padded.setTo(Scalar(0,0,0)); //settami i pixel della Mat a 0
    image.copyTo(padded(Rect(n,n,image.cols,image.rows))); //copiami la Mat image nel rettangolo di padded che parte dall'elemento (n;n) e arriva all'elemento (cols-n;rows-n)

    image.row(0).copyTo(padded(Range(0,1),Range(1,padded.cols-1)));
    image.row(image.rows-1).copyTo(padded(Range(padded.rows-1,padded.rows),Range(1,padded.cols-1)));
    image.col(0).copyTo(padded(Range(1,padded.rows-1),Range(0,1)));
    image.col(image.cols-1).copyTo(padded(Range(1,padded.rows-1),Range(padded.cols-1,padded.cols)));

    if(padded.type() == CV_8UC1){

        padded.at<uchar>(0,0) = padded.at<uchar>(1,1);
        padded.at<uchar>(padded.rows-1,0) = padded.at<uchar>(padded.rows-2,1);
        padded.at<uchar>(0,padded.cols-1) = padded.at<uchar>(1,padded.cols-2);
        padded.at<uchar>(padded.rows-1,padded.cols-1) = padded.at<uchar>(padded.rows-2,padded.cols-2);

    }

    else if(padded.type() == CV_8UC3){

        padded.at<Vec3b>(0,0)[0] = padded.at<Vec3b>(1,1)[0];
        padded.at<Vec3b>(0,0)[1] = padded.at<Vec3b>(1,1)[1];
        padded.at<Vec3b>(0,0)[2] = padded.at<Vec3b>(1,1)[2];

        padded.at<Vec3b>(padded.rows-1,0)[0] = padded.at<Vec3b>(padded.rows-2,1)[0];
        padded.at<Vec3b>(padded.rows-1,0)[1] = padded.at<Vec3b>(padded.rows-2,1)[1];
        padded.at<Vec3b>(padded.rows-1,0)[2] = padded.at<Vec3b>(padded.rows-2,1)[2];

        padded.at<Vec3b>(0,padded.cols-1)[0] = padded.at<Vec3b>(1,padded.cols-2)[0];
        padded.at<Vec3b>(0,padded.cols-1)[1] = padded.at<Vec3b>(1,padded.cols-2)[1];
        padded.at<Vec3b>(0,padded.cols-1)[2] = padded.at<Vec3b>(1,padded.cols-2)[2];

        padded.at<Vec3b>(padded.rows-1,padded.cols-1)[0] = padded.at<Vec3b>(padded.rows-2,padded.cols-2)[0];
        padded.at<Vec3b>(padded.rows-1,padded.cols-1)[1] = padded.at<Vec3b>(padded.rows-2,padded.cols-2)[1];
        padded.at<Vec3b>(padded.rows-1,padded.cols-1)[2] = padded.at<Vec3b>(padded.rows-2,padded.cols-2)[2];
    }







    return padded;

}


Mat blur(Mat image){ //effettua un padding standard e sfoca l'immagine

    Mat blurred = padding(image,1);
    Mat copy(blurred);

    if(image.type() == CV_8UC1)

        for (int i=0; i<image.rows; i++)
            for(int j=0; j<image.cols; j++)//somma i 9 pixel di una zona 3x3 e metti il valore medio all'interno della nuova Mat // per ogni pixel

                blurred.at<Vec3b>(i, j)[0] = sum_neigh(copy,i,j,0);




    else if(image.type() == CV_8UC3){

        for (int i=1; i<image.rows; i++)
            for(int j=1; j<image.cols; j++){//somma i 9 pixel di una zona 3x3 e metti il valore medio all'interno della nuova Mat // per ogni pixel

                blurred.at<Vec3b>(i, j)[0] = sum_neigh(copy,i,j,0);

                blurred.at<Vec3b>(i, j)[1] = sum_neigh(copy,i,j,1);

                blurred.at<Vec3b>(i, j)[2] = sum_neigh(copy,i,j,2);
            }
    }


    return blurred;

}

Mat blur_plus(Mat image){ //effettua un padding standard e sfoca l'immagine

    Mat blurred = padding_plus(image,1);
    Mat copy(blurred);

    if(blurred.type() == CV_8UC1)

        for (int i=1; i<image.rows; i++)
            for(int j=1; j<image.cols; j++)//somma i 9 pixel di una zona 3x3 e metti il valore medio all'interno della nuova Mat // per ogni pixel

                blurred.at<Vec3b>(i, j)[0] = sum_neigh(copy,i,j,0);



    else if(blurred.type() == CV_8UC3){

        for (int i=1; i<image.rows; i++)
            for(int j=1; j<image.cols; j++){//somma i 9 pixel di una zona 3x3 e metti il valore medio all'interno della nuova Mat // per ogni pixel

                blurred.at<Vec3b>(i, j)[0] = sum_neigh(copy,i,j,0);

                blurred.at<Vec3b>(i, j)[1] = sum_neigh(copy,i,j,1);

                blurred.at<Vec3b>(i, j)[2] = sum_neigh(copy,i,j,2);

            }
    }


    return blurred;

}


uchar sum_neigh(Mat image, int i, int j, int rgb){

    return (image.at<Vec3b>(i, j)[rgb] + image.at<Vec3b>(i-1, j-1)[rgb] + image.at<Vec3b>(i-1, j)[rgb] + image.at<Vec3b>(i-1, j+1)[rgb] + image.at<Vec3b>(i, j-1)[rgb] + image.at<Vec3b>(i, j+1)[rgb] + image.at<Vec3b>(i+1, j-1)[rgb] + image.at<Vec3b>(i+1, j)[rgb] + image.at<Vec3b>(i+1, j+1)[rgb])/9;


}


