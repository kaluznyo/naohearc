/**
 * \mainpage
 * \section Author
 * @author Olivier Kaluzny
 * \section Copyright
 * @copyright HE-ARC
 * Version : 1
 *
 * \section Description
 *
 * This file is the main of ObjectDetect
 */

#include <signal.h>
#include <alptr.h>
#include <albroker.h>
#include <almodule.h>
#include <albrokermanager.h>
#include <altoolsmain.h>

#include "objectdetect.h"



#ifdef OBJECTDETECT_IS_REMOTE_OFF

#ifdef _WIN32
    #define ALCALL __declspec(dllexport)
#else
    #define ALCALL
#endif
#endif

#ifdef __cplusplus
extern "C"
{
#endif


ALCALL int _createModule( AL::ALPtr<AL::ALBroker> pBroker )
{      
  // init broker with the main broker inctance 
  // from the parent executable
  AL::ALBrokerManager::setInstance(pBroker->fBrokerManager.lock());
  AL::ALBrokerManager::getInstance()->addBroker(pBroker);

  std::string s = "ObjectDetect";  
  AL::ALModule::createModule<AL::ObjectDetect>( pBroker, s );


  return 0;
}

ALCALL int _closeModule(  )
{
  return 0;
}

# ifdef __cplusplus
}
# endif

#ifdef OBJECTDETECT_IS_REMOTE_ON

int main( int argc, char *argv[] )
{
  // pointer to createModule
  TMainType sig;
  sig = &_createModule;

  // call main
  ALTools::mainFunction("ObjectDetect",argc, argv,sig);
}
#endif

