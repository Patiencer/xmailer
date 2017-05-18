# libxmailer - Cross-platform C library to send e-mails from any application.
#
# Copyright (c) 2013-2017 Silvio Clecio
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.

cmake_minimum_required(VERSION 3.6)
project(xmailer)

set(CMAKE_C_STANDARD 99)

if (WIN32)
    set(CMAKE_C_FLAGS "-static")
else ()
    set(CMAKE_C_FLAGS "")
endif ()

if (CMAKE_BUILD_TYPE MATCHES Debug)
    set(DEBUG 1)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O1 -g -Wall -Werror")
else ()
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3 -Wall -Werror")
endif ()

find_package(OpenSSL REQUIRED)
if (OPENSSL_FOUND)
    include_directories(${OPENSSL_INCLUDE_DIR})
    set(LIBS ${LIBS} ${OPENSSL_LIBRARIES})
else (SSL_FOUND)
    message(FATAL_ERROR "OpenSSL's development libraries not found")
endif (OPENSSL_FOUND)

find_package(CURL REQUIRED)
if (CURL_FOUND)
    include_directories(${CURL_INCLUDE_DIR})
    set(LIBS ${LIBS} ${CURL_LIBRARIES})
else ()
    message(FATAL_ERROR "Curl development libraries not found")
endif ()

set(LIBS ${LIBS} microutils)

if (MINGW)
    set(LIBS ${LIBS} systre tre iconv)
    find_package(Intl REQUIRED)
    if (INTL_FOUND)
        include_directories(${Intl_INCLUDE_DIRS})
        set(LIBS ${LIBS} ${Intl_LIBRARIES})
    else ()
        message(FATAL_ERROR "Intl development libraries not found")
    endif ()
endif (MINGW)

find_package(ZLIB REQUIRED)
if (ZLIB_FOUND)
    include_directories(${ZLIB_INCLUDE_DIRS})
    set(LIBS ${LIBS} ${ZLIB_LIBRARIES})
else ()
    message(FATAL_ERROR "ZLib development libraries not found")
endif ()

if (UNIX)
    set(LIBS ${LIBS} m)
endif ()

set(LIBS_EX ${LIBS} xmailer)

include_directories(.)
include_directories(include)

set(SRC src)
set(SRC_EX examples)

add_library(xmailer SHARED
        ${SRC}/xm_base.c
        ${SRC}/xm_macros.h
        ${SRC}/xm_smtp.c
        ${SRC}/xm_smtp_defs.c
        ${SRC}/xm_strs.h
        ${SRC}/xm_utils.c)
target_link_libraries(xmailer ${LIBS})

add_executable(sendmail
        ${SRC_EX}/getpass.c
        ${SRC_EX}/sendmail.c)
target_link_libraries(sendmail ${LIBS_EX})