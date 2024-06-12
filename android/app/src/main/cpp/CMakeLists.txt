cmake_minimum_required(VERSION 3.22.1)
project("argo")
set(LIB_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../../../../..)
add_library(argo SHARED
        native-lib.cpp
        ${LIB_PATH}/channel.cpp
        ${LIB_PATH}/driver.cpp
        ${LIB_PATH}/json.cpp
        ${LIB_PATH}/logger.cpp
        ${LIB_PATH}/path.cpp
        ${LIB_PATH}/socket.cpp
        ${LIB_PATH}/udpchannel.cpp
)
target_include_directories(argo PRIVATE
        ${LIB_PATH}
)
target_link_libraries(argo
        android
        log
)