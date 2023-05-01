#include <opencv4/opencv2/opencv.hpp> //versione 4.70 di opencv
#include <iostream>
#include <cstdlib>

using namespace cv;
using namespace std;

Mat filter2d_fake (int type, Mat image1,Mat filter);
Mat sobel(Mat image, int type, float c);



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
    Mat type1,type2,type3;

    /*I risultati della funzione Sobel sono stati omessi, ma volendo, si possono confrontare con quelli
     ottenuti con la funzione qui creata, per avere la certezza della bontà di quanto ottenuto.*/

    //Sobel(image,type1,image.type(),1,0);
    //Sobel(image,type2,image.type(),0,1);
    //Sobel(image,type3,image.type(),1,1);

    imshow("Original",image);

    Mat sobelgx,sobelgy,sobelm;
    sobelgx = sobel(image,0,0.4);
    sobelgy = sobel(image,1,0.4);
    sobelm = sobel(image,2,0.4);

    imshow("Sobelgx Sharpened", sobelgx);
    imshow("Sobelgy Sharpened", sobelgy);
    imshow("SobelMagnitudo Sharpened", sobelm);


    //imshow("Original",image);
    //imshow("Sobel 10",type1);
    //imshow("Sobel 01",type2);
    //imshow("Sobel 11",type3);


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

//Funzione che implementa il filtro di sobel 3x3, per ambo le componenti (x e y) e magnitudo.
//In input prende l'immagine, il tipo di filtro (0 sobelgx, 1 sobelgy, 2 magnitudo del gradiente)
//e una costante moltiplicativa che determina la durezza dello sharpening -- deve essere compreso
//tra 0 e 1
Mat sobel(Mat image, int type, float c){

    float sobelgx[] = {-1.0,-2.0,-1.0,0,0,0,1.0,2.0,1.0}; //creo il data per il filtro sobelgx e sobelgy
    Mat sobelGx = Mat(3,3,CV_32F,sobelgx);
    Mat temp = Mat(3,3,CV_32F,sobelgx);
    Mat sobelGy;
    rotate(temp,sobelGy,ROTATE_90_COUNTERCLOCKWISE); //il filtro sobelgy è il sobelgx ma ruotato di 90 gradi in senso antiorario



    if (type == 0 && (c<=1 && c>=0)){

        Mat filtered = filter2d_fake(0,image,sobelGx); //applico il filtro
        imshow("Sobelgx", filtered); //mostro il filtro
        Mat padded, result;
        copyMakeBorder(image,padded,1,1,1,1,BORDER_REPLICATE); //applico il padding all'immagine per effettuare l'operazione successiva
        result = cv::Mat::ones(padded.rows,padded.cols,padded.type()); //creo la matrice per il risultato
        result = padded - (c*filtered); //applico lo sharpening con il fattore c a determinarne la durezza
        return result;



    }
    else if (type == 1 && (c<=1 && c>=0)){

        Mat filtered = filter2d_fake(0,image,sobelGy);//applico il filtro
        imshow("Sobelgy", filtered);//mostro il filtro
        Mat padded, result;
        copyMakeBorder(image,padded,1,1,1,1,BORDER_REPLICATE);//applico il padding all'immagine per effettuare l'operazione successiva
        result = cv::Mat::ones(padded.rows,padded.cols,padded.type());//creo la matrice per il risultato
        result = padded - (c*filtered);//applico lo sharpening con il fattore c a determinarne la durezza
        return result;


    }
    else if (type == 2 && (c<=1 && c>=0)){

        Mat filteredx = filter2d_fake(0,image,sobelGx);//applico il filtro
        Mat filteredy = filter2d_fake(0,image,sobelGy);//applico il filtro
        Mat magnitudo = filteredx + filteredy;//calcolo la magnitudo (è un'immagine che è la somma delle componenti del gradiente)
        imshow("Magnitudo", magnitudo);//mostro la magnitudo
        Mat padded, result;
        copyMakeBorder(image,padded,1,1,1,1,BORDER_REPLICATE);//applico il padding all'immagine per effettuare l'operazione successiva
        result = cv::Mat::ones(padded.rows,padded.cols,padded.type());//creo la matrice per il risultato
        result = padded - (c*magnitudo);//applico lo sharpening con il fattore c a determinarne la durezza
        return result;

    }
    else{

        puts("Wrong call -- Choose 0-1-2 for type // Choose a c between 0 and 1");
        exit(-1);
    }



}
