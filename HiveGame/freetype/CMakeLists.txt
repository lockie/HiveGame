
cmake_minimum_required(VERSION 2.8)

project(freetype)

set(SOURCE_FILES
	src/autofit/autofit.c
	src/bdf/bdf.c
	src/cff/cff.c
	src/base/ftbase.c
	src/base/ftbitmap.c
	src/cache/ftcache.c
	src/base/ftfstype.c
	src/base/ftgasp.c
	src/base/ftglyph.c
	src/gzip/ftgzip.c
	src/base/ftinit.c
	src/lzw/ftlzw.c
	src/base/ftstroke.c
	src/base/ftsystem.c
	src/smooth/smooth.c
	src/base/ftbbox.c
	src/base/ftmm.c
	src/base/ftpfr.c
	src/base/ftsynth.c
	src/base/fttype1.c
	src/base/ftwinfnt.c
	src/base/ftxf86.c
	src/base/ftlcdfil.c
	src/base/ftgxval.c
	src/base/ftotval.c
	src/base/ftpatent.c
	src/pcf/pcf.c
	src/pfr/pfr.c
	src/psaux/psaux.c
	src/pshinter/pshinter.c
	src/psnames/psmodule.c
	src/raster/raster.c
	src/sfnt/sfnt.c
	src/truetype/truetype.c
	src/type1/type1.c
	src/cid/type1cid.c
	src/type42/type42.c
	src/winfonts/winfnt.c
	builds/win32/ftdebug.c
)

include_directories("include")
add_library(freetype STATIC ${HEADER_FILES} ${SOURCE_FILES})

if(MSVC)
	set_target_properties(freetype PROPERTIES
		COMPILE_DEFINITIONS "WIN32;_LIB;_CRT_SECURE_NO_WARNINGS;FT2_BUILD_LIBRARY"
	)
endif(MSVC)
