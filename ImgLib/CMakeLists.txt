cmake_minimum_required(VERSION 3.11)

project(ImgLib CXX)
set(CMAKE_CXX_STANDARD 17)

# добавляем новую переменную - путь к LibJPEG
set(LIBJPEG_DIR "D:/Study/yandex_tasks/libjpeg" CACHE STRING "LibJPEG static library directory")
if(NOT LIBJPEG_DIR)
    message(FATAL_ERROR "Please, specify LibJPEG directory via -DLIBJPEG_DIR=<dir>")
endif()
message(STATUS "LibJPEG dir is ${LIBJPEG_DIR}, change via -DLIBJPEG_DIR=<dir>")

set(IMGLIB_MAIN_FILES img_lib.h img_lib.cpp)

# к файлам форматов добавим JPEG
set(IMGLIB_FORMAT_FILES 
    ppm_image.h ppm_image.cpp 
    jpeg_image.h jpeg_image.cpp
    bmp_image.h bmp_image.cpp)

add_library(ImgLib STATIC ${IMGLIB_MAIN_FILES} 
            ${IMGLIB_FORMAT_FILES})

# Include-директории теперь включают LibJPEG
target_include_directories(ImgLib PUBLIC "${LIBJPEG_DIR}/include")



# Явно указываем путь к библиотеке в зависимости от конфигурации
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(JPEG_LIB_PATH "${LIBJPEG_DIR}/lib/Debug/libjpeg.a")
    message(STATUS "Using Debug JPEG library")
else()
    set(JPEG_LIB_PATH "${LIBJPEG_DIR}/lib/Release/libjpeg.a")
    message(STATUS "Using Release JPEG library")
endif()

message(STATUS "JPEG library path: ${JPEG_LIB_PATH}")

# Проверяем существование библиотеки
if(NOT EXISTS "${JPEG_LIB_PATH}")
    message(FATAL_ERROR "JPEG library not found at: ${JPEG_LIB_PATH}")
endif()

# Явно указываем путь к библиотеке
target_link_libraries(ImgLib PUBLIC "${JPEG_LIB_PATH}")



# # Флаг INTERFACE обозначает видимость параметра - только для зависимых целей.
# # Для самой ImgLib не нужно указывать зависимости компоновки, поскольку
# # статическая библиотека не компонуется, а просто архивируется
# target_link_directories(ImgLib PRIVATE
#     "${LIBJPEG_DIR}/lib/$<IF:$<CONFIG:Debug>,Debug,Release>"
#     )

# # В качестве зависимости указано jpeg. Компоновщик будет искать
# # файл libjpeg.a
# target_link_libraries(ImgLib PRIVATE jpeg)