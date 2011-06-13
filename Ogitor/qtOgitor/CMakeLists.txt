PROJECT(QtOgitor)
SET(QT_MIN_VERSION "4.5.0")
FIND_PACKAGE(Qt4 COMPONENTS QtSvg REQUIRED)

IF(NOT QT4_FOUND)
    MESSAGE(SEND_ERROR "Failed to find Qt 4.5 or greater.")
ELSE(NOT QT4_FOUND)

    INCLUDE(${QT_USE_FILE})
    SET(CMAKE_PACKAGE_QTGUI TRUE)

    file(GLOB UI_FILES *.ui)

    SET(SRCS
        ./include/aboutdialog.hxx
        ./src/aboutdialog.cpp
        ./include/actiontoolbar.hxx
        ./src/actiontoolbar.cpp
        ./include/addscriptactiondialog.hxx
        ./src/addscriptactiondialog.cpp
        ./include/addtemplatedialog.hxx
        ./src/addtemplatedialog.cpp
        ./include/addterraindialog.hxx
        ./src/addterraindialog.cpp
        ./include/calculateblendmapdialog.hxx
        ./src/calculateblendmapdialog.cpp
        ./include/colourpicker.hxx
        ./src/colourpicker.cpp
        ./include/complexproperties.hxx
        ./src/complexproperties.cpp
        ./include/entityview.hxx
        ./src/entityview.cpp
        ./include/importheightmapdialog.hxx
        ./src/importheightmapdialog.cpp
        ./include/layerview.hxx
        ./src/layerview.cpp
        ./include/lineeditwithhistory.hxx
        ./src/lineeditwithhistory.cpp
        ./src/main.cpp
        ./include/mainwindow.hxx
        ./src/mainwindow.cpp
        ./src/mainwindowactions.cpp
        ./include/magickwidget.hxx
        ./src/magickwidget.cpp
        ./include/objectsview.hxx
        ./src/objectsview.cpp
        ./include/ogitorpreferenceswidget.hxx
        ./src/ogitorpreferenceswidget.cpp
        ./include/ogrewidget.hxx
        ./src/ogrewidget.cpp
        ./include/paths.h
        ./src/paths.cpp
        ./include/preferencesmanager.hxx
        ./src/preferencesmanager.cpp
        ./include/propertiesviewcustom.hxx
        ./src/propertiesviewcustom.cpp
        ./include/propertiesviewgeneral.hxx
        ./src/propertiesviewgeneral.cpp
        ./include/qtogitorsystem.h
        ./src/qtogitorsystem.cpp
        ./include/sceneview.hxx
        ./src/sceneview.cpp
        ./include/settingsdialog.hxx
        ./src/settingsdialog.cpp
        ./include/shortcuts.hxx
        ./src/shortcuts.cpp
        ./include/shortcutsettings.hxx
        ./src/shortcutsettings.cpp
        ./include/templateview.hxx
        ./src/templateview.cpp
        ./include/terraintoolswidget.hxx
        ./src/terraintoolswidget.cpp
        ./include/userdefined.hxx
        ./src/userdefined.cpp
        ./include/welcome.hxx
        ./src/welcome.cpp
    )

    SET(MOC_SRCS2 
        ./include/aboutdialog.hxx
        ./include/actiontoolbar.hxx
        ./include/addscriptactiondialog.hxx
        ./include/addtemplatedialog.hxx
        ./include/addterraindialog.hxx
        ./include/calculateblendmapdialog.hxx
        ./include/colourpicker.hxx
        ./include/complexproperties.hxx
        ./include/entityview.hxx
        ./include/importheightmapdialog.hxx
        ./include/layerview.hxx
        ./include/lineeditwithhistory.hxx
        ./include/mainwindow.hxx
        ./include/magickwidget.hxx
        ./include/objectsview.hxx
        ./include/ogitorpreferenceswidget.hxx
        ./include/ogrewidget.hxx
        ./include/preferencesmanager.hxx
        ./include/propertiesviewcustom.hxx
        ./include/propertiesviewgeneral.hxx
        ./include/sceneview.hxx
        ./include/settingsdialog.hxx
        ./include/shortcuts.hxx
        ./include/shortcutsettings.hxx
        ./include/templateview.hxx
        ./include/terraintoolswidget.hxx
        ./include/userdefined.hxx
        ./include/welcome.hxx
    )

    if(OGITOR_EXPERIMENTAL_INTERFACE)
        set(OGITOR_QT_PREPROCESSOR "${OGITOR_QT_PREPROCESSOR} -DOGITOR_EXPERIMENTAL_INTERFACE")
    endif(OGITOR_EXPERIMENTAL_INTERFACE)

    MESSAGE(STATUS "INFO QTOGITOR PREPROCESSOR: " ${OGITOR_QT_PREPROCESSOR})

    QT4_WRAP_CPP(MOC_SRCS ${MOC_SRCS2} OPTIONS ${OGITOR_QT_PREPROCESSOR})
    QT4_WRAP_UI(UI_SRCS ${UI_FILES})

    SOURCE_GROUP("Moc Files" FILES ${MOC_SRCS})
    SOURCE_GROUP("Ui Files" FILES ${UI_FILES})

    # if we don't include this CMake will not include ui headers properly:
    include_directories(${CMAKE_CURRENT_BINARY_DIR})
    include_directories(./include)

    include_directories(${DEPENDENCIES_INCLUDES})

    QT4_ADD_RESOURCES(RESOURCE_SRCS ./res/qtOgitor.qrc)
    SOURCE_GROUP("Qrc Files" FILES ${RESOURCE_SRCS})

    SET(SRCS ${SRCS} ${MOC_SRCS} ${UI_FILES})

    include_directories(../Ogitor/include
      ./qtpropertybrowser-2.5/src
    )

    if(WIN32)
        file(GLOB qtOgitor_res ./res/*.rc)
    endif(WIN32)
  
    if(NOT APPLE)
        ADD_EXECUTABLE(qtOgitor WIN32 ${SRCS} ${RESOURCE_SRCS} ${qtOgitor_res})
    else()
        # The first time we build Ogitor.framework will not exist so create a dummy file if necessary
        FIND_FILE(OGITOR_FRAMEWORK_FILE Ogitor.framework "${CMAKE_BINARY_DIR}/Ogitor/lib/Debug/" NO_SYSTEM_ENVIRONMENT_PATH)
        IF(NOT OGITOR_FRAMEWORK_FILE)
            FILE(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/Ogitor/lib/Debug/Ogitor.framework")
        endif()

        SET(COPY_FRAMEWORKS ${OGRE_LIBRARIES} ${Cg_LIBRARY_FWK} "${CMAKE_BINARY_DIR}/Ogitor/lib/Debug/Ogitor.framework")
        SET(COPY_RESOURCES "${MAC_PATH}/resources.cfg" "${MAC_PATH}/plugins.cfg" "${RUNPATH}/Media")
        SET(COPY_PLUGINS 
            ${OGRE_RenderSystem_GL_LIBRARY_DBG} 
            ${OGRE_Plugin_ParticleFX_LIBRARY_DBG} 
            ${OGRE_Plugin_OctreeSceneManager_LIBRARY_DBG} 
            ${OGRE_Plugin_CgProgramManager_LIBRARY_DBG}
            "${RUNPATH}/Plugins/Icons")

        SET(COPY_COMPONENTS
        ${OGRE_Terrain_LIBRARY} 
            ${OGRE_Paging_LIBRARY}
        )

        # Note: For OSX we have to add resources here as well
        ADD_EXECUTABLE(qtOgitor MACOSX_BUNDLE 
            ${SRCS} 
            ${RESOURCE_SRCS} 
            ${qtOgitor_res} 
	    )
    add_dependencies(qtOgitor OgCaelum)
    add_dependencies(qtOgitor OgHydrax)
    add_dependencies(qtOgitor OgSkyx)
    add_dependencies(qtOgitor OgModularZone)
    add_dependencies(qtOgitor DotSceneSerializer)
    include(BundleUtilities)
    get_target_property(qtOgitorLocation qtOgitor LOCATION)
    get_dotapp_dir(${qtOgitorLocation} qtOgitorLocationAppdir)
    set(MakeBundle cp ${QtSolutions_PropertyBrowser_BINARY_DIR}/${CMAKE_CFG_INTDIR}/libQtSolutions_PropertyBrowser*.dylib ${OGRE_Terrain_LIBRARY_DBG} ${OGRE_Paging_LIBRARY_DBG} ${qtOgitorLocationAppdir}/Contents/Components && cp ${OGRE_RenderSystem_GL_LIBRARY_DBG} ${OGRE_Plugin_OctreeSceneManager_LIBRARY_DBG} ${OGRE_Plugin_ParticleFX_LIBRARY_DBG} ${OGRE_Plugin_CgProgramManager_LIBRARY_DBG} ${qtOgitorLocationAppdir}/Contents/Plugins && cp -r /Library/Frameworks/OGRE.framework ${qtOgitorLocationAppdir}/Contents/Frameworks && cp -r ../lib/${CMAKE_CFG_INTDIR}/Ogitor.framework ${qtOgitorLocationAppdir}/Contents/Frameworks && cp ../Plugins/lib/${CMAKE_CFG_INTDIR}/libOgCaelum*.dylib ../Plugins/lib/${CMAKE_CFG_INTDIR}/libOgHydrax*.dylib ../Plugins/lib/${CMAKE_CFG_INTDIR}/libOgModularZone*.dylib ../Plugins/lib/${CMAKE_CFG_INTDIR}/libOgSkyx*.dylib ../Plugins/lib/${CMAKE_CFG_INTDIR}/libDotSceneSerializer*.dylib ${qtOgitorLocationAppdir}/Contents/Plugins/Ogitor && cp -r ../RunPath/Media ${qtOgitorLocationAppdir}/Contents/Resources && cp -r ../RunPath/Plugins/Icons ${qtOgitorLocationAppdir}/Contents/Plugins && cp ../Mac/plugins.cfg ../Mac/resources.cfg ${qtOgitorLocationAppdir}/Contents/Resources)
    ADD_CUSTOM_COMMAND(TARGET qtOgitor POST_BUILD COMMAND mkdir -p ${qtOgitorLocationAppdir}/Contents/Resources && mkdir -p ${qtOgitorLocationAppdir}/Contents/Components && mkdir -p ${qtOgitorLocationAppdir}/Contents/Plugins/Ogitor && mkdir -p ${qtOgitorLocationAppdir}/Contents/Frameworks)
    ADD_CUSTOM_COMMAND(TARGET qtOgitor POST_BUILD COMMAND ${MakeBundle})
    endif(NOT APPLE)
  
    SET_TARGET_PROPERTIES( qtOgitor PROPERTIES DEBUG_POSTFIX _d )

    if(OGITOR_EXPERIMENTAL_INTERFACE)
        TARGET_LINK_LIBRARIES(qtOgitor QtSolutions_PropertyBrowser ${QT_QTMAIN_LIBRARY} ${QT_LIBRARIES} ${OGRE_LIBRARIES} ${OGRE_Paging_LIBRARY} ${OGRE_Terrain_LIBRARY} Ogitor meshmagick_shared_lib ${Boost_REGEX_LIBRARY})
    else(OGITOR_EXPERIMENTAL_INTERFACE)
        TARGET_LINK_LIBRARIES(qtOgitor QtSolutions_PropertyBrowser ${QT_QTMAIN_LIBRARY} ${QT_LIBRARIES} ${OGRE_LIBRARIES} ${OGRE_Paging_LIBRARY} ${OGRE_Terrain_LIBRARY} Ogitor ${Boost_REGEX_LIBRARY})
    endif(OGITOR_EXPERIMENTAL_INTERFACE)
    if(UNIX AND NOT APPLE)
        TARGET_LINK_LIBRARIES(qtOgitor optimized QtSolutions_PropertyBrowser ${Boost_LIBRARIES})
    elseif(APPLE)
        TARGET_LINK_LIBRARIES(qtOgitor QtSolutions_PropertyBrowser)
    elseif(WIN32)
        TARGET_LINK_LIBRARIES(qtOgitor optimized QtSolutions_PropertyBrowser)
    endif()

    if(APPLE)
        install(TARGETS qtOgitor BUNDLE DESTINATION ${PREFIX}bin CONFIGURATIONS Release RelWithDebInfo MinSizeRel)
        install(TARGETS qtOgitor BUNDLE DESTINATION ${PREFIX}Debug CONFIGURATIONS Debug)
    endif()

    IF(WIN32)
        add_custom_command( TARGET qtOgitor PRE_BUILD
            COMMAND if not exist ..\\RunPath\\bin mkdir ..\\RunPath\\bin )
        add_custom_command( TARGET qtOgitor POST_BUILD
            COMMAND copy \"$(TargetPath)\" ..\\RunPath\\bin )
    ENDIF(WIN32)

    IF(UNIX AND NOT APPLE)
        GET_TARGET_PROPERTY(OGITORFILE qtOgitor LOCATION)
        add_custom_command( TARGET qtOgitor POST_BUILD
            COMMAND cp -r ${OGITORFILE} ../RunPath/bin/${TARGET} )
    ENDIF(UNIX AND NOT APPLE)

    install(DIRECTORY ${CMAKE_SOURCE_DIR}/RunPath/languages
        DESTINATION ${PREFIX}
        PATTERN ".svn" EXCLUDE
        PATTERN "translate.pro" EXCLUDE
        PATTERN "*.ts" EXCLUDE
    )


    IF(WIN32)
        set(QT_DLL_DIR ${QT_BINARY_DIR})

    if(OGITOR_EXPERIMENTAL_INTERFACE)
    set_target_properties(qtOgitor PROPERTIES 
        COMPILE_DEFINITIONS "QT_QTPROPERTYBROWSER_IMPORT;OGITOR_EXPERIMENTAL_INTERFACE")
    else(OGITOR_EXPERIMENTAL_INTERFACE)
    set_target_properties(qtOgitor PROPERTIES 
        COMPILE_DEFINITIONS QT_QTPROPERTYBROWSER_IMPORT)
    endif(OGITOR_EXPERIMENTAL_INTERFACE)

        install(FILES 
            ${QT_DLL_DIR}/QtCore4.dll
            ${QT_DLL_DIR}/QtGui4.dll
            ${QT_DLL_DIR}/QtSvg4.dll
            ${QT_BINARY_DIR}/assistant.exe
            ${QT_DLL_DIR}/QtHelp4.dll
            ${QT_DLL_DIR}/QtNetwork4.dll
            ${QT_DLL_DIR}/QtSql4.dll
            ${QT_DLL_DIR}/QtCLucene4.dll
            ${QT_DLL_DIR}/QtXml4.dll
            DESTINATION bin
            CONFIGURATIONS Release)
        install(FILES 
            ${QT_DLL_DIR}/QtCored4.dll
            ${QT_DLL_DIR}/QtGuid4.dll
            ${QT_DLL_DIR}/QtSvgd4.dll
            ${QT_BINARY_DIR}/assistant.exe
            ${QT_DLL_DIR}/QtHelpd4.dll
            ${QT_DLL_DIR}/QtNetworkd4.dll
            ${QT_DLL_DIR}/QtSqld4.dll
            ${QT_DLL_DIR}/QtCLucened4.dll
            ${QT_DLL_DIR}/QtXmld4.dll
            DESTINATION bin
            CONFIGURATIONS Debug)
        install(FILES ${QT_PLUGINS_DIR}/iconengines/qsvgicon4.dll
            DESTINATION bin/iconengines
            CONFIGURATIONS Release)
        install(FILES ${QT_PLUGINS_DIR}/iconengines/qsvgicond4.dll
            DESTINATION bin/iconengines
            CONFIGURATIONS Debug)
        install(FILES ${QT_PLUGINS_DIR}/sqldrivers/qsqlite4.dll
            DESTINATION bin/sqldrivers
            CONFIGURATIONS Release)
        install(FILES ${QT_PLUGINS_DIR}/sqldrivers/qsqlited4.dll
            DESTINATION bin/sqldrivers
            CONFIGURATIONS Debug)
        install(TARGETS qtOgitor
            RUNTIME DESTINATION bin
            CONFIGURATIONS Release)
    ENDIF(WIN32)

ENDIF(NOT QT4_FOUND)

