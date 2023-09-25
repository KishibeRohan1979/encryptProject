cmake_minimum_required(VERSION 3.22)
project(encryptProject)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(CMAKE_PREFIX_PATH "D:/C++/qt/qt-windows/Qt5.14.2/5.14.2/mingw73_64")

find_package(Qt5 COMPONENTS
        Core
        Gui
        Widgets
        REQUIRED)

include_directories(
        ${PROJECT_SOURCE_DIR}/include
        ${PROJECT_SOURCE_DIR}/Sources/Headers
        ${PROJECT_SOURCE_DIR}/Sources/Forms
)

#查找当前目录下、及aesin目录下所有cpp文件，保存在变量AES_SRC中
file(GLOB AES_SRC
        # AES库：处理AES加密解密
        "${PROJECT_SOURCE_DIR}/include/QT-AES/*.cpp"
)

set(SOURCES_LIST
        ${AES_SRC}
)

set(SOURCE_FILES
        Sources/main.cpp
        static/ico/ico.rc
        Sources/choosewindow.cpp
        Sources/Headers/choosewindow.h
        Sources/Forms/choosewindow.ui
        Sources/filewindow.cpp
        Sources/Headers/filewindow.h
        Sources/Forms/filewindow.ui
        Sources/stringwindow.cpp
        Sources/Headers/stringwindow.h
        Sources/Forms/stringwindow.ui
        )

add_executable(${PROJECT_NAME} ${SOURCE_FILES} ${SOURCES_LIST})

target_link_libraries(${PROJECT_NAME}
        Qt5::Core
        Qt5::Gui
        Qt5::Widgets
        )

if (WIN32)
    set(DEBUG_SUFFIX)
    #    if (MSVC AND CMAKE_BUILD_TYPE MATCHES "Debug")
    #        set(DEBUG_SUFFIX "d")
    #    endif ()
    set(QT_INSTALL_PATH "${CMAKE_PREFIX_PATH}")
    if (NOT EXISTS "${QT_INSTALL_PATH}/bin")
        set(QT_INSTALL_PATH "${QT_INSTALL_PATH}/..")
        if (NOT EXISTS "${QT_INSTALL_PATH}/bin")
            set(QT_INSTALL_PATH "${QT_INSTALL_PATH}/..")
        endif ()
    endif ()
    if (EXISTS "${QT_INSTALL_PATH}/plugins/platforms/qwindows${DEBUG_SUFFIX}.dll")
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E make_directory
                "$<TARGET_FILE_DIR:${PROJECT_NAME}>/plugins/platforms/")
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy
                "${QT_INSTALL_PATH}/plugins/platforms/qwindows${DEBUG_SUFFIX}.dll"
                "$<TARGET_FILE_DIR:${PROJECT_NAME}>/plugins/platforms/")
    endif ()
    foreach (QT_LIB Core Gui Widgets)
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy
                "${QT_INSTALL_PATH}/bin/Qt5${QT_LIB}${DEBUG_SUFFIX}.dll"
                "$<TARGET_FILE_DIR:${PROJECT_NAME}>")
    endforeach (QT_LIB)
endif ()