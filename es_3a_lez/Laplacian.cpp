#include <opencv4/opencv2/opencv.hpp> //versione 4.70 di opencv
#include <iostream>
#include <cstdlib>

using namespace cv;
using namespace std;

Mat filter2d_fake (int type, Mat image1,Mat filter);
Mat laplaciano(Mat image, int type, float c);


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
    Mat type1,type2,laplacian;
    type1=laplaciano(image,0,0.5);
    type2=laplaciano(image,1,0.5);
    Laplacian(image,laplacian,image.type());

    //in realtà è opzionale, opencv lo fa già di suo, a quanto sembra
    //normalize(laplacian, laplacian, 0, 255, NORM_MINMAX, image.type());

    imshow("Original",image);
    imshow("Function Laplacian", laplacian);
    imshow("Result laplacian 90", type1);
    imshow("Result laplacian 45", type2);
    waitKey(0);




    return 0;
}

//funzione che "emula" filter2d: in input prende il tipo (0 correlaz/1 convoluz), l'immagine e il filtro
Mat filter2d_fake (int type, Mat image1, Mat filter){

    int padding = filter.rows/2;
    Mat padded;
    copyMakeBorder(image1,padded,padding,padding,padding,padding,BORDER_REPLICATE); //applico il padding all'immagine
    Mat result = cv::Mat::ones(padded.rows, padded.cols, CV_8U);
    waitKey(0);





    if(type == 0){
        for(int i=padding; i<padded.rows-1; i++)
            for (int j=padding; j<padded.cols-1; j++){
                float sum = 0;
                for (int k=-padding; k<=padding; k++)
                    for (int m=-padding; m<=padding; m++){
                        sum += padded.at<uchar>(i+k,j+m) * filter.at<float>(k+padding,m+padding); //applico la formula delle slides

                    }
                result.at<uchar>(i,j) = saturate_cast<uchar>(sum); //la somma va trattata in modo speciale // se > di 255 deve essere 255, se < 0 deve essere 0

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
                result.at<uchar>(i,j) = saturate_cast<uchar>(sum); //la somma va trattata in modo speciale // se > di 255 deve essere 255, se < 0 deve essere 0
            }
    }

    else{
        puts("Wrong first argument -- 1 for correl or 2 for convol");
        exit(-1);
    }




    return result;


}

//La function che implementa il laplaciano // prende in input l'immagine, il tipo di filtro(0 -> laplaciano 90 / 1 -> laplaciano 45)
//e la costante moltiplicativa per la quale moltiplicare il filtro (deve essere compresa tra 0 e 1)
Mat laplaciano(Mat image, int type, float c){



    if (type == 0 && (c<=1 && c>=0)){

        float lapl_90[] = {0,1.0,0,1.0,-4.0,1.0,0,1.0,0}; //creo i valori per il filtro laplaciano 90
        Mat flapl_90 = Mat(3,3,CV_32F,lapl_90); //li inserisco nel filtro laplaciano_90
        Mat laplacian_90 = filter2d_fake(1,image,flapl_90); //applico il filtro all'immagine
        imshow("Current laplacian 90", laplacian_90);
        waitKey(0);
        Mat padded;
        copyMakeBorder(image,padded,1,1,1,1,BORDER_REPLICATE); //applico un padding all'immagine originale per effettuare le operazioni di sharpening
        Mat result = cv::Mat::ones(padded.rows, padded.cols, CV_8U); //creo la matrice risultato
        result = padded - (c*laplacian_90); //applico lo sharpening -- sottraggo alla matrice originale, il laplaciano moltiplicato per la costante
        return result;


    }
    else if (type == 1 && (c<=1 && c>=0)){

        float lapl_45[] = {1.0,1.0,1.0,1.0,-8.0,1.0,1.0,1.0,1.0}; //creo i valori per il filtro laplaciano 45
        Mat flapl_45 = Mat(3,3,CV_32F,lapl_45); //li inserisco nel filtro laplaciano_45
        Mat laplacian_45 = filter2d_fake(1,image,flapl_45); //applico il filtro all'immagine
        imshow("Current laplacian 45", laplacian_45);
        waitKey(0);
        Mat padded;
        copyMakeBorder(image,padded,1,1,1,1,BORDER_REPLICATE); //applico un padding all'immagine originale per effettuare le operazioni di sharpening
        Mat result = cv::Mat::ones(padded.rows, padded.cols, CV_8U); //creo la matrice risultato
        result = padded - (c*laplacian_45); //applico lo sharpening -- sottraggo alla matrice originale, il laplaciano moltiplicato per la costante
        return result;

    }
    else{

        puts("Wrong call -- Choose 0 or 1 for type // Choose a c between 0 and 1");
        exit(-1);
    }



}
