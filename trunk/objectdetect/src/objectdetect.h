/**
 * @author Olivier Kaluzny
 * @copyright HE-ARC
 * This file is the ObjectDetect Module
 */
 
#ifndef OBJECTDETECT_OBJECTDETECT_H
#define OBJECTDETECT_OBJECTDETECT_H

#include "alptr.h"

#include "alloggerproxy.h"
#include <string>
#include "opencv/cv.h"
#include "opencv/highgui.h"


namespace AL
{

/**
 * ObjectDetect is use to detect an object of color given, and return angle between camera and object
 */
class ObjectDetect : public AL::ALModule
{

  public:

    /**
     * Default Constructor.
     */
     ObjectDetect(AL::ALPtr<AL::ALBroker> pBroker, const std::string& pName );

    /**
     * Destructor.
     */
    virtual ~ObjectDetect();
 	
 	/**
     * registerToVIM : Register to the V.I.M.
     \param pResolution is the resolution to camera
 	 \param pColorSpace is the space color of image 
 	 \param pFps is the FPS for camera (value possible : 5,15,30)
     */
    void registerToVIM(const int &pResolution, const int &pColorSpace, const int &pFps);

    /**
     * unRegisterFromVIM : Unregister from the V.I.M.
     */
    void unRegisterFromVIM();

    /**
    * Take a photo and analyse it to find the center of form with RGB
     \param R is the red composante
 	 \param G is the green composante
 	 \param B is the blue composanteR,G,B] 
  	 \return vector of two value (alpha,beta) angle between object detected and camera 
    */
    std::vector<float> takePhoto(const float &R, const float &G,const float &B);
    
    virtual void init();
 	
 	//! Function to get color of one pixel.
	/*!
	  \param imagem represent the image pointer.
	  \param x represente the x-axis of the image which want to get color
	  \param y represente the y-axis of the image which want to get coloer
	  \return the color of pixel
	  \sa SetColor()
	*/
	
 	 int GetColor(IplImage * imagem, int x, int y);
 	 //! Function to set color of one pixel.
	/*!
	  \param imagem represent the image pointer.
	  \param x represente the x-axis of the image which want to set color
	  \param y represente the y-axis of the image which want to set color
	  \param color the color
	  \sa GetColor()
	*/
	 void SetColor(IplImage * imagem, int x, int y, uchar color);
	 
	 //! Function to check all image to find a unicolor form.
	/*!
	  \param img represent the image pointer.
	  \param c_value represente the value we want to find
	  \param tolerance represente the tolerance
	*/
	 void CheckImg(IplImage * img, uchar c_value, uchar tolerance);
	 
	 //! function to get center of form detected.
	 /*!
	  \param src represent the image pointer
	  \return the center of form, with a cvpoint
	*/
	 CvPoint GetCenter(IplImage * src);
	 
 	 //! General methode to find the center of form
	 /*!
	  \param src represent a pointer to image 
	  \param dst represent an other pointer of the same image
	  \param c_value represent an array of value we want to find [R,G,B] 
	  \param tolerance represent an array of tolerance for each layer [tolerance R,tolerance G,tolerance B] 
	  \param pointCenter represent a pointer to cvpoint
	*/
 	 long int CheckForColor(IplImage * src, IplImage * dst, uchar * c_value, uchar * tolerance, CvPoint * pointCenter);
  private:

    // Proxy to the logger module.
    ALPtr<AL::ALLoggerProxy> fLogProxy;

    // Proxy to the video input module.
    ALPtr<AL::ALProxy> fCamProxy;
    
    // This is set to true when we have subscribed one module to the VideoDevice.
    bool fRegisteredToVim;
    
    //Name of VIM registred
    std::string fGvmName;

};
}
#endif  // OBJECTDETECT_OBJECTDETECT_H

