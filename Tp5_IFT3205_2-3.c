//------------------------------------------------------
// Prog    : Tp5_IFT3205                          
// Auteur  : Guillaume Riou, Nicolas Hurtubise
// Date    :                                  
// version : Reptillions
// langage : C                                          
// labo    : DIRO                                       
//------------------------------------------------------


//------------------------------------------------
// FICHIERS INCLUS -------------------------------
//------------------------------------------------
#include "FonctionDemo5.h"

//------------------------------------------------
// DEFINITIONS -----------------------------------
//------------------------------------------------
#define NAME_VISUALISER "display "
#define NAME_IMG_IN  "lena512"
#define NAME_IMG_OUT "image-TpIFT3205-2-3"
#define NAME_IMG_DEG "lena512_DegradedAAAAAAAAAAAAAAAAAAAAA"

//------------------------------------------------
// PROTOTYPE DE FONCTIONS  -----------------------
//------------------------------------------------
//----------------------------------------------------------
// copy matrix
//----------------------------------------------------------
void copy_matrix(float** mat1,float** mat2,int lgth,int wdth)
{
    int i,j;

    for(i=0;i<lgth;i++) for(j=0;j<wdth;j++) mat1[i][j]=mat2[i][j];
}



//------------------------------------------------
// CONSTANTES ------------------------------------
//------------------------------------------------
#define SIGMA_NOISE  30 

//------------------------------------------------
//------------------------------------------------
// FONCTIONS  ------------------------------------
//------------------------------------------------
//------------------------------------------------

void
treshold_cancel(float** tcd_imagette, int treshold) {
    int n = 8;
    int i, j;

    for(i=0; i<8; i++)
        for(j=0; j<8; j++) {

            if(fabs(tcd_imagette[i][j]) < treshold)
                tcd_imagette[i][j] = 0;
        }
}

void
hard_denoise(float** image, int length, int width, int treshold) {

    float** imagette = fmatrix_allocate_2d(8, 8);

    int bloc_i, bloc_j;
    int i, j;
    
    for(bloc_i=0; bloc_i<length/8; bloc_i++)
        for(bloc_j=0; bloc_j<width/8; bloc_j++) {

            // Copie le bloc 8x8
            for(i=0; i<8; i++)
                for(j=0; j<8; j++) {
                    imagette[i][j] = image[bloc_i * 8 + i][bloc_j * 8 + j];
                }
            
            // TCD le bloc 8x8
            ddct8x8s(-1, imagette);
            
            // Hard-treshold cancel le shit
            treshold_cancel(imagette, treshold);
            
            // TCD^-1 le shit
            ddct8x8s(1, imagette);
            
            // Copy back
            
            for(i=0; i<8; i++)
                for(j=0; j<8; j++) {
                    image[bloc_i * 8 + i][bloc_j * 8 + j] = imagette[i][j];
                }
        }

    free_fmatrix_2d(imagette);
}

//---------------------------------------------------------
//---------------------------------------------------------
// PROGRAMME PRINCIPAL   ----------------------------------                     
//---------------------------------------------------------
//---------------------------------------------------------
int main(int argc,char** argv)
{
    int length,width;
    char BufSystVisuImg[NBCHAR];

    //>Lecture Image 
    float** Img=LoadImagePgm(NAME_IMG_IN,&length,&width);
 
    //>Allocation memory
    float** ImgDegraded=fmatrix_allocate_2d(length,width);
    float** ImgDenoised=fmatrix_allocate_2d(length,width);  

    //>Degradation 
    copy_matrix(ImgDegraded,Img,length,width);
    add_gaussian_noise(ImgDegraded,length,width,SIGMA_NOISE*SIGMA_NOISE);
 
    printf("\n\n  Info Noise");
    printf("\n  ------------");
    printf("\n    > MSE = [%.2f]\n",computeMMSE(ImgDegraded,Img,length)); 
 

    //=========================================================
    //== PROG =================================================
    //=========================================================

    int n;
    float** img_denoise = fmatrix_allocate_2d(length, width);

    float best_mmse = INFINITY, mmse;
    int best_n = 0;
    
    for(n=0; n<256; n++) {

        copy_matrix(img_denoise, ImgDegraded, length, width);

        hard_denoise(img_denoise, length, width, n);

        mmse = computeMMSE(img_denoise, Img, length);
        
        printf("n=%d => MSE = [%.2f]\n", n, mmse);

        if(mmse < best_mmse) {
            best_mmse = mmse;
            best_n = n;
        }
    }

    printf("Meilleur MSE : %d => MSE = [%.2f]\n", best_n, best_mmse);

    copy_matrix(img_denoise, ImgDegraded, length, width);
    
    hard_denoise(img_denoise, length, width, best_n);

    int i, j;
    for(i=0; i<length; i++)
        for(j=0; j<width; j++) {

            if(img_denoise[i][j] < 0)
                img_denoise[i][j] = 0;
            else if(img_denoise[i][j] > 255)
                img_denoise[i][j] = 255;
            
        }

    //---------------------------------------------
    // SAUVEGARDE
    //----------------------------------------------
    SaveImagePgm(NAME_IMG_OUT, img_denoise, length, width);

    //Visu ImgFiltered
    strcpy(BufSystVisuImg,NAME_VISUALISER);
    strcat(BufSystVisuImg,NAME_IMG_OUT);
    strcat(BufSystVisuImg,".pgm&");
    printf("\n > %s",BufSystVisuImg);
    system(BufSystVisuImg);

    
//--------------- End -------------------------------------     
//----------------------------------------------------------

    //Liberation memoire pour les matrices
    if (Img)          free_fmatrix_2d(Img);
    if (ImgDegraded)  free_fmatrix_2d(ImgDegraded);
    if (ImgDenoised)  free_fmatrix_2d(ImgDenoised);
  
    //Return
    printf("\n C'est fini... \n");; 
    return 0;
}
 


//----------------------------------------------------------
// Allocation matrix 3d float
// --------------------------
//
// float*** fmatrix_allocate_3d(int dsize,int vsize,int hsize)
// > Alloue dynamiquement un tableau 3D [dsize][vsize][hsize]
//
//-----------------------------------------------------------

//----------------------------------------------------------
//  ddct8x8s(int isgn, float** tab)
// ---------------------------------
//
// Fait la TCD (sgn=-1) sur un tableau 2D tab[8][8]
// Fait la TCD inverse (sgn=1) sur un tableau 2D tab[8][8]
//
//-----------------------------------------------------------
