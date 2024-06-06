cmake_minimum_required(VERSION 3.10)

project(ava LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 11)

if(CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
    include(GNUInstallDirs)
    set(THREADS_PREFER_PTHREAD_FLAG ON)
    add_compile_options(
        -fvisibility=hidden
        -Wno-deprecated-declarations
        -Wno-deprecated-register
    )
    find_package(Threads REQUIRED)
endif()

if(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    add_definitions(
        -D_WIN32_WINNT=0x0601
        -DNOMINMAX
        -DVC_EXTRALEAN
        -DWIN32_LEAN_AND_MEAN
        -D_CRT_SECURE_NO_WARNINGS
        -D_SCL_SECURE_NO_WARNINGS
        -D_WINSOCK_DEPRECATED_NO_WARNINGS
    )
endif()

set (LIB_SRCS
    driver.cpp
)

set (PUB_HDRS
    iapi.h
    idriver.h
)

add_library(libargo SHARED ${LIB_SRCS})

set_target_properties(libargo PROPERTIES PREFIX "")
set_target_properties(libargo PROPERTIES POSITION_INDEPENDENT_CODE TRUE)
set_target_properties(libargo PROPERTIES PUBLIC_HEADER "${PUB_HDRS}")

target_include_directories(libargo INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/libargo>
)

install(TARGETS libargo EXPORT libargo
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/libargo
    RUNTIME       DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY       DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE       DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

install(EXPORT libargo
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/libargo
    FILE libargo.cmake
)

if(CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
    target_link_libraries(libargo PRIVATE Threads::Threads)
endif()

if(CMAKE_CXX_COMPILER_ID MATCHES MSVC)
    target_link_libraries(libargo wsock32 ws2_32)
    target_compile_definitions(libargo PUBLIC _WINDLL)
endif()
 
set (TEST_SRCS
    testargo.cpp
)

add_executable(testargo ${TEST_SRCS})

target_link_libraries(testargo libargo)

install(TARGETS testargo
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

set (ARGO_SRCS
    argo.cpp
)

add_executable(argo ${ARGO_SRCS})

target_link_libraries(argo libargo)

install (TARGETS argo
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)