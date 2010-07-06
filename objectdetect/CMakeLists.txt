cmake_minimum_required(VERSION 2.6.4 FATAL_ERROR)

include("${CMAKE_CURRENT_SOURCE_DIR}/bootstrap.cmake")
use(NAOQI-PLUGINS-TOOLS)

create_module( objectdetect )

SET ( OBJECTDETECT_SRCS
  src/objectdetectmain.cpp
  src/objectdetect.h
  src/objectdetect.cpp

)

configure_src_module( objectdetect ${OBJECTDETECT_SRCS} )

use_lib( objectdetect ALCOMMON OPENCV ALMATH LIBCORE TOOLS ALVALUE)
