cmake_minimum_required(VERSION 2.8.12)
project(OpenHMDPlugin)

find_package(osvr REQUIRED)

# OpenHMD
set(OPENHMD_DIR "${CMAKE_CURRENT_LIST_DIR}/vendor/OpenHMD")
include_directories(${OPENHMD_DIR}/include)
set(OPENHMD_SOURCES
	${OPENHMD_DIR}/src/openhmd.c
	${OPENHMD_DIR}/src/platform-win32.c
	${OPENHMD_DIR}/src/drv_dummy/dummy.c
	${OPENHMD_DIR}/src/omath.c
	${OPENHMD_DIR}/src/platform-posix.c
	${OPENHMD_DIR}/src/fusion.c
)
OPTION(OPENHMD_DRIVER_OCULUS_RIFT "Oculus Rift DK1 and DK2" ON)
OPTION(OPENHMD_DRIVER_EXTERNAL "External sensor driver" OFF)
OPTION(OPENHMD_DRIVER_ANDROID "General Android driver" OFF)

if(OPENHMD_DRIVER_OCULUS_RIFT)
	set(OPENHMD_SOURCES ${OPENHMD_SOURCES} 
	${OPENHMD_DIR}/src/drv_oculus_rift/rift.c
	${OPENHMD_DIR}/src/drv_oculus_rift/packet.c
	)
	add_definitions(-DDRIVER_OCULUS_RIFT)
endif(OPENHMD_DRIVER_OCULUS_RIFT)

if (OPENHMD_DRIVER_EXTERNAL)
	set(OPENHMD_SOURCES ${OPENHMD_SOURCES} 
	${OPENHMD_DIR}/src/drv_external/external.c
	)
	add_definitions(-DDRIVER_EXTERNAL)
endif(OPENHMD_DRIVER_EXTERNAL)

if (OPENHMD_DRIVER_ANDROID)
	set(OPENHMD_SOURCES ${OPENHMD_SOURCES} 
	${OPENHMD_DIR}/src/drv_android/android.c
	)
	add_definitions(-DDRIVER_ANDROID)
endif(OPENHMD_DRIVER_ANDROID)

add_definitions(-DOHMD_STATIC)

if (WIN32)
   execute_process(COMMAND powershell -Command "(gc ${OPENHMD_DIR}/src/platform-win32.c) -replace '__stdcall DWORD', 'DWORD __stdcall' | Out-File ${OPENHMD_DIR}/src/platform-win32.c")
endif(WIN32)

# HIDAPI
set(HIDAPI_DIR "${CMAKE_CURRENT_LIST_DIR}/vendor/hidapi")
include_directories(${HIDAPI_DIR}/hidapi)

IF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
   set (HIDAPI_SOURCES ${HIDAPI_DIR}/mac/hid.c)
   set (LIBS ${LIBS} "-framework CoreFoundation -framework IOKit")
ENDIF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")

if (WIN32)
   set (HIDAPI_SOURCES ${HIDAPI_DIR}/windows/hid.c)
   set (LIBS ${LIBS} setupapi)
endif(WIN32)

IF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
	set (HIDAPI_SOURCES ${HIDAPI_DIR}/libusb/hid.c)
	set (LIBS ${LIBS} pkg-config libusb-1.0)
ENDIF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")

# OSVR Plugin
osvr_convert_json(je_nourish_openhmd_json
    je_nourish_openhmd.json
    "${CMAKE_CURRENT_BINARY_DIR}/je_nourish_openhmd_json.h")

include_directories("${CMAKE_CURRENT_BINARY_DIR}")

osvr_add_plugin(NAME je_nourish_openhmd
    CPP # indicates we'd like to use the C++ wrapper
    SOURCES
    je_nourish_openhmd.cpp
    "${CMAKE_CURRENT_BINARY_DIR}/je_nourish_openhmd_json.h"
	${HIDAPI_SOURCES}
	${OPENHMD_SOURCES})

target_link_libraries(je_nourish_openhmd ${LIBS})
