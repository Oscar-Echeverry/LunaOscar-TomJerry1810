
cmake_minimum_required(VERSION 3.16)

project(TomyJerry VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Buscar interfaces de Qt Designer
set(CMAKE_AUTOUIC_SEARCH_PATHS
    "${CMAKE_CURRENT_SOURCE_DIR}/vista"
)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

set(PROJECT_SOURCES

    main.cpp
    widget.cpp
    widget.h
    widget.ui

    Recursos.qrc

    logica/vector2.h
    logica/configuracion.h

    logica/temporizador.h
    logica/temporizador.cpp

    logica/entidad.h
    logica/entidad.cpp

    logica/personajes.h
    logica/personajes.cpp

    logica/fisicajuego.h
    logica/fisicajuego.cpp

    logica/objetos.h
    logica/objetos.cpp

    logica/niveles.h
    logica/niveles.cpp

    ia/ia.h
    ia/ia.cpp

    vista/pantallainicio.h
    vista/pantallainicio.cpp
    vista/inicio.ui

    vista/pantallapausa.h
    vista/pantallapausa.cpp
    vista/pausa.ui

    vista/pantallaresultado.h
    vista/pantallaresultado.cpp
    vista/resultado.ui
)

# Incluir VistaJuego si sus archivos existen
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/vista/vistajuego.h"
   AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/vista/vistajuego.cpp")

    list(APPEND PROJECT_SOURCES
        vista/vistajuego.h
        vista/vistajuego.cpp
    )

endif()

# Crear ejecutable
if(QT_VERSION_MAJOR GREATER_EQUAL 6)

    qt_add_executable(TomyJerry
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        vista/vistajuego.h
        vista/recursosgraficos.h
        vista/recursosgraficos.cpp
        vista/vistajuego.cpp
        juego.h
        juego.cpp
        sonido/sonidos.h
        sonido/sonidos.cpp
        logica/dificultad.h
        logica/eventosonido.h
    )

else()

    if(ANDROID)

        add_library(TomyJerry SHARED
            ${PROJECT_SOURCES}
        )

    else()

        add_executable(TomyJerry
            ${PROJECT_SOURCES}
        )

    endif()

endif()

# Rutas para encontrar archivos .h
target_include_directories(TomyJerry PRIVATE

    "${CMAKE_CURRENT_SOURCE_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/logica"
    "${CMAKE_CURRENT_SOURCE_DIR}/vista"
    "${CMAKE_CURRENT_SOURCE_DIR}/ia"

)

target_link_libraries(TomyJerry PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
)

# Configuracion del ejecutable
if(QT_VERSION VERSION_LESS 6.1.0)

    set(BUNDLE_ID_OPTION
        MACOSX_BUNDLE_GUI_IDENTIFIER
        com.example.TomyJerry
    )

endif()

set_target_properties(TomyJerry PROPERTIES

    ${BUNDLE_ID_OPTION}

    MACOSX_BUNDLE_BUNDLE_VERSION
        ${PROJECT_VERSION}

    MACOSX_BUNDLE_SHORT_VERSION_STRING
        ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}

    MACOSX_BUNDLE TRUE

    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)

install(TARGETS TomyJerry

    BUNDLE DESTINATION .

    LIBRARY DESTINATION
        ${CMAKE_INSTALL_LIBDIR}

    RUNTIME DESTINATION
        ${CMAKE_INSTALL_BINDIR}

)

if(QT_VERSION_MAJOR EQUAL 6)

    qt_finalize_executable(TomyJerry)

endif()
