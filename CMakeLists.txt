cmake_minimum_required(VERSION 3.16)

project(CifradoDatos VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Encontrar Qt
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets LinguistTools)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets LinguistTools)

# Archivo de traducción
set(TS_FILES CifradoDatos_es_CR.ts)

# Fuentes del proyecto
set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.h
        mainwindow.ui
        users.json
        ${TS_FILES}
)

# Incluir Crypto++ (puede requerir ajustes si no está en rutas estándar)
find_library(CRYPTOPP_LIBRARY cryptopp REQUIRED)
include_directories(/usr/include/cryptopp)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(CifradoDatos
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        users.json
    )

    qt_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
else()
    if(ANDROID)
        add_library(CifradoDatos SHARED
            ${PROJECT_SOURCES}
        )
    else()
        add_executable(CifradoDatos
            ${PROJECT_SOURCES}
        )
    endif()

    qt5_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
endif()

# Enlazar bibliotecas
target_link_libraries(CifradoDatos PRIVATE Qt${QT_VERSION_MAJOR}::Widgets ${CRYPTOPP_LIBRARY})

# Configuración para macOS e iOS
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.CifradoDatos)
endif()

set_target_properties(CifradoDatos PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

# Instalación
include(GNUInstallDirs)
install(TARGETS CifradoDatos
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(CifradoDatos)
endif()
