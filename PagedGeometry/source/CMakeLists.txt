
include_directories(${PagedGeometry_SOURCE_DIR}/include)
include_directories(${PagedGeometry_BINARY_DIR}/include)
include_directories(${Ogre_INCLUDE_DIRS})
link_directories   (${Ogre_LIBRARY_DIRS})

FILE(GLOB sources *.cpp ${PagedGeometry_SOURCE_DIR}/include/*.h)

set(LIBNAME "PagedGeometry")

ADD_LIBRARY(${LIBNAME} STATIC ${sources})
