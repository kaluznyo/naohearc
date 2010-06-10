// Auto-generated C Code - S2i Harpia
/*
*	In order to compile this source code run, in a terminal window, the following command:
*	gcc sourceCodeName.c `pkg-config --libs --cflags opencv` -o outputProgramName
*	
*	the `pkg-config ... opencv` parameter is a inline command that returns the path to both 
*	the libraries and the headers necessary when using opencv. The command also returns other necessary compiler options.
*/
// header:
#include <stdio.h>
#include <stdlib.h>
#include <cv.h>
#include <highgui.h>




int GetColor(IplImage * imagem, int x, int y)
{
	return   (int)(((uchar*)(imagem->imageData + imagem->widthStep*y))[x]);
}

void SetColor(IplImage * imagem, int x, int y, uchar color)
{
	((uchar*)(imagem->imageData + imagem->widthStep*y))[x] = color;
}


void CheckImg(IplImage * img, uchar c_value, uchar tolerance)
{
	uchar min,max;
	int y_It,x_It;
	if((int)c_value < (int)tolerance)
		tolerance = c_value;

	if(((int)c_value+(int)tolerance) > 255)
		tolerance = 255 - c_value;

	min = c_value - tolerance;
	max = c_value + tolerance;

	for(y_It=0;y_It<(img->height);y_It++)
		for(x_It=0;x_It<(img->width);x_It++)
		{
				uchar val;
				val = GetColor(img,x_It,y_It);
				if(val >= min && val <= max)
					SetColor(img,x_It,y_It,255);
				else
					SetColor(img,x_It,y_It,0);
		}
}

CvPoint GetCenter(IplImage * src)
{
	long int numOfMatchingPoints;
	long int posXsum;
	long int posYsum;
	int x_It, y_It;
	CvPoint Center;
	
	posXsum = 0;
	posYsum = 0;
	numOfMatchingPoints = 0;

	for(y_It=0;y_It<(src->height);y_It++)
		for(x_It=0;x_It<(src->width);x_It++)
			if(GetColor(src,x_It,y_It))
			{
				posXsum += x_It;
				posYsum += y_It;
				numOfMatchingPoints++;
			}
	printf("%ld",numOfMatchingPoints);
	if(numOfMatchingPoints > 0)
	{
		Center.x = (int)(posXsum/numOfMatchingPoints);
		Center.y = (int)(posYsum/numOfMatchingPoints);
	}
	else
	{
			Center.x = -1;
		Center.y = -1;
				numOfMatchingPoints = -1;
	}


	return Center;
}



long int CheckForColor(IplImage * src, IplImage * dst, uchar * c_value, uchar * tolerance, CvPoint * pointCenter)
{
	uchar B,B_T,G,G_T,R,R_T;
	int i;

	CvPoint centro;
	IplImage * m_pChans[3] = {NULL,NULL,NULL};


	B = c_value[0];
	G = c_value[1];
	R = c_value[2];

	B_T = tolerance[0];
	G_T = tolerance[1];
	R_T = tolerance[2];

	for(i=0;i<3;i++)
		m_pChans[i] = cvCreateImage(cvGetSize(src),IPL_DEPTH_8U, 1);

	cvSplit(src,m_pChans[0],m_pChans[1],m_pChans[2], NULL);

	CheckImg(m_pChans[0],B,B_T);
	CheckImg(m_pChans[1],G,G_T);
	CheckImg(m_pChans[2],R,R_T);

	cvAnd(m_pChans[0], m_pChans[1], dst, NULL );
	cvAnd(m_pChans[2], dst, dst, NULL );

	centro = GetCenter(dst);


	pointCenter->x = centro.x;
	pointCenter->y = centro.y;

	cvReleaseImage( &m_pChans[0] );
	cvReleaseImage( &m_pChans[1] );
	cvReleaseImage( &m_pChans[2] );

	return 1;
}
			
int main(int argc, char ** argv)
{
	//declaration block
char block2_arg_Filename[] = "/home/nao/Bureau/lena.png";
IplImage * block2_img_o1 = NULL;
CvPoint block1_point_o1;
uchar block1c_value[3] = {57.0,136.0,57.0};
uchar block1tolerance[3] = {20.0,20.0,20.0};


block2_img_o1 = cvLoadImage(block2_arg_Filename,-1);


if(block2_img_o1){
	IplImage * block1_img_t1 = cvCreateImage(cvGetSize(block2_img_o1),IPL_DEPTH_8U, 1);
	
	CheckForColor(block2_img_o1, block1_img_t1, block1c_value, block1tolerance, &block1_point_o1);
	printf("X = %d",block1_point_o1.x);
	printf("Y = %d",block1_point_o1.y);

	cvReleaseImage(&block1_img_t1);
}

	//deallocation block
cvReleaseImage(&block2_img_o1);



return 0;
 } //closing main()
