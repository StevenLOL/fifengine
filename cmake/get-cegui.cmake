#------------------------------------------------------------------------------
# External Project:     CeGUI
# Downloads:            http://cegui.org.uk/download                                          
#------------------------------------------------------------------------------

include(ExternalProject)

set(CEGUI_VERSION "0.8.7")

# For Windows it's faster to download the pre-compiled development binaries.
if (WIN32)

  #if(MINGW) # download pre-compiled dev package for MinGW 32/64-bit
    
    #ExternalProject_Add(
      #fifechan
      #PREFIX              ${DEPENDENCY_EXTRACT_DIR}
      #DOWNLOAD_DIR        ${DEPENDENCY_DOWNLOAD_DIR}
      #URL                 https://github.com/fifengine/fifechan/releases/tag/${FIFECHAN_VERSION}/fifechan-v${FIFECHAN_VERSION}-MinGW.tar.gz
      #URL                 https://ci.appveyor.com/api/projects/jakoch/fifechan/artifacts/fifechan-shared-MinGW.tar.gz
      #URL_MD5             1
      #CONFIGURE_COMMAND   ""
      #BUILD_COMMAND       ""
      #INSTALL_COMMAND 
        #COMMAND ${CMAKE_COMMAND} -E copy_directory ${DEPENDENCY_EXTRACT_DIR}/src/fifechan/lib     ${DEPENDENCY_INSTALL_DIR}/lib
        #COMMAND ${CMAKE_COMMAND} -E copy_directory ${DEPENDENCY_EXTRACT_DIR}/src/fifechan/include ${DEPENDENCY_INSTALL_DIR}/include/fifechan
    #)

  #endif(MINGW)

  #if(MSVC) # download pre-compiled dev package for Visual C++ 32/64-bit

    #ExternalProject_Add(
      #cegui
      #PREFIX              ${DEPENDENCY_EXTRACT_DIR}
      #DOWNLOAD_DIR        ${DEPENDENCY_DOWNLOAD_DIR}
      #URL                 https://github.com/fifengine/fifechan/releases/tag/${FIFECHAN_VERSION}/fifechan-v${FIFECHAN_VERSION}-VC14.zip
      #URL_MD5             1
      #CONFIGURE_COMMAND   ""
      #BUILD_COMMAND       ""
      #INSTALL_COMMAND 
        #COMMAND ${CMAKE_COMMAND} -E copy_directory ${DEPENDENCY_EXTRACT_DIR}/src/fifechan/bin     ${DEPENDENCY_INSTALL_DIR}/bin
        #COMMAND ${CMAKE_COMMAND} -E copy_directory ${DEPENDENCY_EXTRACT_DIR}/src/fifechan/lib     ${DEPENDENCY_INSTALL_DIR}/lib
        #COMMAND ${CMAKE_COMMAND} -E copy_directory ${DEPENDENCY_EXTRACT_DIR}/src/fifechan/include ${DEPENDENCY_INSTALL_DIR}/include/fifechan
    #)

  endif(MSVC)

else() # build from source

  set(CEGUI_CMAKE_ARGS
      -DCMAKE_CXX_FLAGS=${CMAKE_CXX_FLAGS}
      -DCMAKE_INSTALL_PREFIX:PATH=<INSTALL_DIR> 
      -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
      -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}
      -DCMAKE_OSX_ARCHITECTURES=${CMAKE_OSX_ARCHITECTURES}
      -DCEGUI_BUILD_APPLICATION_TEMPLATES:BOOL=OFF
      -DCEGUI_BUILD_APPLICATION_TEMPLATE_GLFW3:BOOL=OFF
      -DCEGUI_BUILD_APPLICATION_TEMPLATE_SDL2:BOOL=OFF
      -DCEGUI_BUILD_DATAFILES_TEST:BOOL=OFF
      -DCEGUI_BUILD_IMAGECODEC_CORONA:BOOL=OFF
      -DCEGUI_BUILD_IMAGECODEC_DEVIL:BOOL=OFF
      -DCEGUI_BUILD_IMAGECODEC_FREEIMAGE:BOOL=OFF
      -DCEGUI_BUILD_IMAGECODEC_PVR:BOOL=OFF
      -DCEGUI_BUILD_IMAGECODEC_SDL2:BOOL=OFF
      -DCEGUI_BUILD_IMAGECODEC_SILLY:BOOL=ON
      -DCEGUI_BUILD_IMAGECODEC_STB:BOOL=OFF
      -DCEGUI_BUILD_IMAGECODEC_TGA:BOOL=OFF
      -DCEGUI_BUILD_LUA_GENERATOR:BOOL=OFF
      -DCEGUI_BUILD_LUA_MODULE:BOOL=OFF
      -DCEGUI_BUILD_PYTHON_MODULES:BOOL=ON
      -DCEGUI_BUILD_RENDERER_DIRECT3D10:BOOL=OFF
      -DCEGUI_BUILD_RENDERER_DIRECT3D11:BOOL=OFF
      -DCEGUI_BUILD_RENDERER_DIRECT3D9:BOOL=OFF
      -DCEGUI_BUILD_RENDERER_DIRECTFB:BOOL=OFF
      -DCEGUI_BUILD_RENDERER_IRRLICHT:BOOL=OFF
      -DCEGUI_BUILD_RENDERER_NULL:BOOL=OFF
      -DCEGUI_BUILD_RENDERER_OGRE:BOOL=OFF
      -DCEGUI_BUILD_RENDERER_OPENGL:BOOL=ON
      -DCEGUI_BUILD_RENDERER_OPENGL3:BOOL=ON
      -DCEGUI_BUILD_RENDERER_OPENGLES:BOOL=OFF
      -DCEGUI_BUILD_XMLPARSER_EXPAT:BOOL=ON
      -DCEGUI_BUILD_XMLPARSER_LIBXML2:BOOL=OFF
      -DCEGUI_BUILD_XMLPARSER_RAPIDXML:BOOL=OFF
      -DCEGUI_BUILD_XMLPARSER_TINYXML:BOOL=OFF
      -DCEGUI_BUILD_XMLPARSER_XERCES:BOOL=OFF
  )
   
  ExternalProject_Add(
    cegui
    PREFIX              ${DEPENDENCY_EXTRACT_DIR}
    DOWNLOAD_DIR        ${DEPENDENCY_DOWNLOAD_DIR}
    URL                 https://downloads.sourceforge.net/project/crayzedsgui/CEGUI%20Mk-2/0.8/cegui-0.8.7.zip?r=http%3A%2F%2Fcegui.org.uk%2Fdownload&ts=1486380450&use_mirror=freefr
    URL_MD5             1
    #CONFIGURE_COMMAND <SOURCE_DIR>/configure --prefix=${DEPENDENCY_INSTALL_DIR}
    INSTALL_DIR         ${DEPENDENCY_INSTALL_DIR}
    CMAKE_ARGS          ${CEGUI_CMAKE_ARGS}
  )

endif()
