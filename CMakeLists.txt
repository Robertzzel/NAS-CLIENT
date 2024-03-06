# Generated from NAS.pro.

cmake_minimum_required(VERSION 3.16)
project(NAS VERSION 1.0 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

if(NOT DEFINED INSTALL_EXAMPLESDIR)
    set(INSTALL_EXAMPLESDIR "examples")
endif()

set(INSTALL_EXAMPLEDIR "${INSTALL_EXAMPLESDIR}")

find_package(QT NAMES Qt5 Qt6 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Gui Network)
find_package(Qt${QT_VERSION_MAJOR} OPTIONAL_COMPONENTS Widgets)

qt_add_executable(NAS WIN32 MACOSX_BUNDLE
    commands.h
    file.h
    filewidget.cpp filewidget.h
    main.cpp
    mainwindow.cpp mainwindow.h mainwindow.ui
    messagehandler.cpp messagehandler.h
)
target_link_libraries(NAS PUBLIC
    Qt::Core
    Qt::Gui
    Qt::Network
)


# Resources:
set(resources_resource_files
    "images/directory.png"
    "images/file.png"
    "images/html.png"
    "images/image.png"
    "images/pdf.png"
    "images/xml.png"
    "images/zip.png"
)

qt6_add_resources(NAS "resources"
    PREFIX
        "/"
    FILES
        ${resources_resource_files}
)

if((QT_VERSION_MAJOR GREATER 4))
    target_link_libraries(NAS PUBLIC
        Qt::Widgets
    )
endif()

install(TARGETS NAS
    RUNTIME DESTINATION "${INSTALL_EXAMPLEDIR}"
    BUNDLE DESTINATION "${INSTALL_EXAMPLEDIR}"
    LIBRARY DESTINATION "${INSTALL_EXAMPLEDIR}"
)
