/**
* @author Olivier Kaluzny
*
* Version : 1.0
* This File represent the vision module. Composed by openCV function, and a class which is use to make biding with python and take a photo
*/
#include <iostream>
#include <alproxy.h>
#include <alvalue.h>
#include <alptr.h>
#include <albroker.h>
#include <almodule.h>
#include <alloggerproxy.h>
#include "objectdetect.h"


using namespace AL;
using namespace std;



 int ObjectDetect::GetColor(IplImage * imagem, int x, int y)
{

  return   (int)(((uchar*)(imagem->imageData + imagem->widthStep*y))[x]);
}


 void ObjectDetect::SetColor(IplImage * imagem, int x, int y, uchar color)
{

  ((uchar*)(imagem->imageData + imagem->widthStep*y))[x] = color;
}


 void ObjectDetect::CheckImg(IplImage * img, uchar c_value, uchar tolerance)
{

  uchar min,max;
  int y_It,x_It;
  
  //Calculate tolerance
  if((int)c_value < (int)tolerance)
    tolerance = c_value;
  if(((int)c_value+(int)tolerance) > 255)
    tolerance = 255 - c_value;
  
  //Calculate the range of value accepted (between min and max)
  min = c_value - tolerance;
  max = c_value + tolerance;
  
  //We look all image and for each pixel, we look if it's the color of pixel we search
  //if it, we color it in white
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

 CvPoint ObjectDetect::GetCenter(IplImage * src)
{

  long int numOfMatchingPoints;
  long int posXsum;
  long int posYsum;
  int x_It, y_It;
  CvPoint Center;
  
  posXsum = 0;
  posYsum = 0;
  numOfMatchingPoints = 0;
  
  //We look all image, and if it's white
  for(y_It=0;y_It<(src->height);y_It++)
    for(x_It=0;x_It<(src->width);x_It++)
      if(GetColor(src,x_It,y_It))
      {
    //if it's white, we add the x and y position, et increment the number of matching point
	posXsum += x_It;
	posYsum += y_It;
	numOfMatchingPoints++;
      }
      
      //We calculate the center in x and y
      if(numOfMatchingPoints > 0)
      {
	Center.x = (int)(posXsum/numOfMatchingPoints);
	Center.y = (int)(posYsum/numOfMatchingPoints);
      }
      else
      {
	Center.x = -1;
	Center.y = -1;
      }

      return Center;
      
}


 long int ObjectDetect::CheckForColor(IplImage * src, IplImage * dst, uchar * c_value, uchar * tolerance, CvPoint * pointCenter)
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

  //Create 3 images with the src image
  for(i=0;i<3;i++)
    m_pChans[i] = cvCreateImage(cvGetSize(src),IPL_DEPTH_8U, 1); 
  
  //For each image we give the layout of img source
  cvSplit(src,m_pChans[0],m_pChans[1],m_pChans[2], NULL);
  
  //We check each channel of image to find the color 
  CheckImg(m_pChans[0],B,B_T);
  CheckImg(m_pChans[1],G,G_T);
  CheckImg(m_pChans[2],R,R_T);
  
  //we make a cvAnd on each result image
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


//______________________________________________
// constructor
//______________________________________________
ObjectDetect::ObjectDetect(AL::ALPtr<AL::ALBroker> broker, const std::string& name ): AL::ALModule(broker, name )
{
  setModuleDescription( "Module to take a photo then analyse them to find the position of an object with the color" );
  
  functionName( "takePhoto", getName() ,  "Function to detect position of an object" );
  addParam("R", "composant Red of the image");
  addParam("G", "composant Green of the image");
  addParam("B", "composant Blue of the image");
  BIND_METHOD( ObjectDetect::takePhoto );
  
  functionName( "registerToVIM", getName(), "Register to the V.I.M." );
  addParam("resolution", "Resolution requested.");
  addParam("colorSpace", "Colorspace requested.");
  addParam("fps", "fps requested.");
  BIND_METHOD( ObjectDetect::registerToVIM );
  
  functionName( "unRegisterFromVIM", getName(), "Unregister from the V.I.M." );
  BIND_METHOD( ObjectDetect::unRegisterFromVIM );
}

//______________________________________________
// destructor
//______________________________________________
ObjectDetect::~ObjectDetect()
{
  
}

void ObjectDetect::init() {
  
  // Create a proxy to the logger module.
  // If this fails, we throw an exception and the module will not be registered.
  try {
    fLogProxy = getParentBroker()->getLoggerProxy();
  } catch (const ALError& e) {
    throw ALError(getName(), "GVMsample()", "Fail to create a proxy to ALLogger "
    "module. Msg " + e.toString() + "\nModule will abort.");
  }
  
  // Create a proxy to the video input module.
  try {
    fCamProxy = getParentBroker()->getProxy( "ALVideoDevice" );
  } catch (const ALError& e) {
    throw ALError(getName(), "GVMsample()", "Could not create a proxy to ALVideoDevice " +
    e.toString());
  }
  
  //Name of module
  fGvmName = "my_GVM";
}

/**
* registerToVIM
*/
void ObjectDetect::registerToVIM(const int &pResolution, const int &pColorSpace, const int &pFps) {
  
  // If we've already registered a module, we need to unregister it first !
  if (fRegisteredToVim) {
    throw ALError(getName(), "registerToVIM()", "A video module has already been "
    "registered. Call unRegisterFromVIM() before trying to register a new module.");
  }
  
  // Call the "subscribe" function with the given parameters.
  fGvmName = fCamProxy->call<std::string>("subscribe", fGvmName, pResolution,   pColorSpace, pFps );
					  
  fLogProxy->info(getName(), " module registered as " + fGvmName);
					  
  // Registration is successful, set fRegisteredToVim to true.
  fRegisteredToVim = true;
}


/**
* unRegisterFromVIM
*/
void ObjectDetect::unRegisterFromVIM() {
  
  if (!fRegisteredToVim) {
    throw ALError(getName(), "unRegisterFromVIM()", "No video module is currently "
    "registered! Call registerToVIM first.");
  }
  
  fLogProxy->info(getName(), "try to unregister " + fGvmName + " module." );
  fCamProxy->callVoid("unsubscribe", fGvmName);
  fLogProxy->info(getName(), "Done.");
  
  // UnRegistration is successful, set fRegisteredToVim to false.
  fRegisteredToVim = false;
}


vector<float> ObjectDetect::takePhoto(const float &R, const float &G, const float &B)
{	 
  //BUG : Use a smaller resolution than VGA. With VGA, sometimes, the photo is corrupted, and that cause a segmentation fault, and exit the modul
  //Get an image from camera
  ALValue results = fCamProxy->call<ALValue>( "getImageRemote", fGvmName );
  
  //Check if the image is OK
  if (results.getType()!= ALValue::TypeArray && results.getSize() != 7) {
    throw ALError(getName(), "takePhoto", "Invalid image returned.");
  }
  
  //Get information about image
  const int width = (int) results[0];
  const int height = (int) results[1];
  const int nbLayers = (int) results[2];
  
  //OPEN CV ALGO

  IplImage * img = NULL;
  CvPoint objectPoint;

  uchar colorToFind[3] = {R,G,B};
  uchar colorToFindTolerance[3] = {15.0,15.0,15.0};
  
  try
  {
    
    //Create new image from the camera
    IplImage* ipImageHeader = cvCreateImageHeader(cvSize(width, height), 8 , nbLayers);
    ipImageHeader->imageData = (char*)(results[6].GetBinary());
    img = ipImageHeader;
    
    //If we have a new image
    if(img){
      IplImage * dst = cvCreateImage(cvGetSize(img),IPL_DEPTH_8U, 1);
      
      //Search the center of object to find
      CheckForColor(img, dst, colorToFind, colorToFindTolerance, &objectPoint);
      

      cvReleaseImage(&dst);
    }
    cvReleaseImage(&img);
    //END OPEN CV ALGO
    
    
  }
  catch(...)
  {
    fLogProxy->error( "ObjectDetect","error opencv" );
  }
  

  vector<float> tmpPair; 
  tmpPair.push_back(0.0f);
  tmpPair.push_back(0.0f);
  
  
  if (objectPoint.x>0)
  {
    try{     
      
      tmpPair[0] = (float)(objectPoint.x) / (float)(width); 
      tmpPair[1] = (float)(objectPoint.y) / (float)(height); 
      
      //Calculate Angle from the position in the image, of object find
      tmpPair = fCamProxy->call< std::vector<float> >("getAngPosFromImgPos", tmpPair); 

    }
    catch(ALError& a)
    {   
      fLogProxy->error( "ObjectDetect",a.toString());
    }
  }
  else
  {
    tmpPair[0] = -4.0f;
    tmpPair[1] = -4.0f;

    fLogProxy->info( "ObjectDetect","No form detected" );
  }
  return tmpPair;
  
}
