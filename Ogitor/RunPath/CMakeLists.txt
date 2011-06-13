#-------------------------------------------------------------------
# This file is part of the CMake build system for OGRE
#     (Object-oriented Graphics Rendering Engine)
# For the latest info, see http://www.ogre3d.org/
#
# The contents of this file are placed in the public domain. Feel
# free to make use of it in any way you like.
#-------------------------------------------------------------------

###############################################################
# Install media
###############################################################

# Set media target directory
if(WIN32)
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/
  DESTINATION ${PREFIX}
  CONFIGURATIONS Release RelWithDebInfo MinSizeRel
  PATTERN ".svn" EXCLUDE
  PATTERN "CMakeLists.txt" EXCLUDE
  PATTERN "Debug" EXCLUDE
  PATTERN "languages" EXCLUDE
  )

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/
  DESTINATION ${PREFIX}
  CONFIGURATIONS Debug
  PATTERN ".svn" EXCLUDE
  PATTERN "CMakeLists.txt" EXCLUDE
  PATTERN "bin" EXCLUDE
  PATTERN "Debug" EXCLUDE
  PATTERN "languages" EXCLUDE
  PATTERN "Plugins/icon_backup" EXCLUDE
  )

install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/bin/plugins_debug.cfg
  ${CMAKE_CURRENT_SOURCE_DIR}/bin/resources.cfg
  DESTINATION bin
  CONFIGURATIONS Debug)
  
endif(WIN32)

if(UNIX)
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/
  DESTINATION ${PREFIX}
  PATTERN ".svn" EXCLUDE
  PATTERN "CMakeLists.txt" EXCLUDE
  PATTERN "Debug" EXCLUDE
  PATTERN "languages" EXCLUDE
  PATTERN "bin" EXCLUDE
  PATTERN "Plugins/icon_backup" EXCLUDE
  )
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/bin/resources.cfg
    DESTINATION bin)
endif(UNIX)


