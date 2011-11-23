# Copyright 2006-2008 The FLWOR Foundation.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

#
# LibTidy
#
IF (ZORBA_SUPPRESS_LIBTIDY)
  MESSAGE (STATUS "ZORBA_SUPPRESS_LIBTIDY is true - not searching for LibTidy.")
ELSE (ZORBA_SUPPRESS_LIBTIDY)

  MESSAGE (STATUS "Looking for LibTidy")
  FIND_PACKAGE (LibTidy)
  
  IF (LIBTIDY_FOUND)
    MESSAGE (STATUS "Found LibTidy library -- " ${LIBTIDY_LIBRARIES})
    SET (HTML_LINK_LIBRARIES ${LIBTIDY_LIBRARIES})

    INCLUDE_DIRECTORIES (${LIBTIDY_INCLUDE_DIR})
    INCLUDE_DIRECTORIES ("html.xq.src")
    DECLARE_ZORBA_SCHEMA (FILE "html-options.xsd"
      URI "http://www.zorba-xquery.com/modules/converters/html-options")
    DECLARE_ZORBA_MODULE (URI "http://www.zorba-xquery.com/modules/converters/html" VERSION 1.0 FILE "html.xq" LINK_LIBRARIES "${LIBTIDY_LIBRARIES}")
    ADD_TEST_DIRECTORY ("${PROJECT_SOURCE_DIR}/test_html")

    ADD_TEST(link_crawler_test_for_compilation "${ZORBA_EXE}" -f -q "${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../test_html/Queries/converters/html/link_crawler2.xq2" --compile-only)
  ELSE (LIBTIDY_FOUND)
    MESSAGE (STATUS "LibTidy library not found -- if you want to use HTML Tidy functionality please set LIBTIDY_INCLUDE_DIR and LIBTIDY_LIBRARIES cmake parameters.")
  ENDIF (LIBTIDY_FOUND)
ENDIF (ZORBA_SUPPRESS_LIBTIDY)
MESSAGE (STATUS "")


#
# Jansson
#
IF (ZORBA_SUPPRESS_JANSSON)
  MESSAGE (STATUS "ZORBA_SUPPRESS_JANSSON is true - not searching for Jansson.")
ELSE (ZORBA_SUPPRESS_JANSSON)

  MESSAGE (STATUS "Looking for Jansson")
  FIND_PACKAGE (Jansson)
     
  IF (Jansson_FOUND)
    MESSAGE (STATUS "Found Jansson library -- " ${Jansson_LIBRARIES})

    SET (JSON_LINK_LIBRARIES ${Jansson_LIBRARIES})
    INCLUDE_DIRECTORIES (${Jansson_INCLUDE_DIRS})
    INCLUDE_DIRECTORIES ("json.xq.src")
    DECLARE_ZORBA_SCHEMA (FILE "json-options.xsd"
      URI "http://www.zorba-xquery.com/modules/converters/json-options")
    DECLARE_ZORBA_MODULE (URI "http://www.zorba-xquery.com/modules/converters/json" VERSION 1.0 FILE "json.xq" LINK_LIBRARIES "${Jansson_LIBRARIES}")
    ADD_TEST_DIRECTORY ("${PROJECT_SOURCE_DIR}/test_json")

  ELSE (Jansson_FOUND)
    MESSAGE (STATUS "Jansson library not found -- if you want to use Json functionality please set Jansson_INCLUDE_DIRS and Jansson_LIBRARIES cmake parameters.")
  ENDIF (Jansson_FOUND)
ENDIF (ZORBA_SUPPRESS_JANSSON)
MESSAGE (STATUS "")


# csv
DECLARE_ZORBA_SCHEMA (FILE csv-options.xsd
  URI "http://www.zorba-xquery.com/modules/converters/csv-options")
DECLARE_ZORBA_MODULE (URI "http://www.zorba-xquery.com/modules/converters/csv" VERSION 1.0 FILE "csv.xq")

# add unit test for streambuf implementation in csv, related to streamable strings
CREATE_TEST_SOURCELIST (
  StreambufUnitTests
  StreambufUnitTests.cpp
  streambuftest.cpp
)
ADD_EXECUTABLE (StreambufUnitTests ${StreambufUnitTests} csv.xq.src/stringiterator_streambuf.cpp)
SET_TARGET_PROPERTIES (StreambufUnitTests PROPERTIES
  FOLDER "Tests"
)

ADD_TEST ("streamable_string_streambuf" StreambufUnitTests "streambuftest")
ADD_TEST_DIRECTORY("${PROJECT_SOURCE_DIR}/test")



##### EXI

FIND_PACKAGE(JNI)
#FIND_PACKAGE(JAVA)
#FIND_PACKAGE(EXIficient)

IF(JNI_FOUND)
  DECLARE_ZORBA_SCHEMA (FILE "exi-options.xsd" URI "http://www.zorba-xquery.com/modules/converters/exi-options")
  DECLARE_ZORBA_SCHEMA (FILE "exi-header.xsd" URI "http://www.zorba-xquery.com/modules/converters/exi-header") #"http://www.w3.org/2009/exi")
  DECLARE_ZORBA_MODULE (URI "http://www.zorba-xquery.com/modules/converters/exi" VERSION 1.0 FILE "exi.xq") # LINK_LIBRARIES "${JAVA_JVM_LIBRARY}")
  INCLUDE_DIRECTORIES ("exi.xq.src")
  INCLUDE_DIRECTORIES (${JNI_INCLUDE_DIRS})

  #FILE(COPY exi.xq.src/java/exificient.jar DESTINATION exi.xq.src/)
  #FILE(COPY exi.xq.src/java/xercesImpl.jar DESTINATION exi.xq.src/)
  #FILE(COPY exi.xq.src/java/xml-apis.jar DESTINATION exi.xq.src/)
  #FILE(COPY exi.xq.src/java/exificient_stub.jar DESTINATION exi.xq.src/)
  ADD_COPY_RULE(LIB "${CMAKE_CURRENT_SOURCE_DIR}/exi.xq.src/java/exificient_stub.jar" "com/zorba-xquery/www/modules/converters/exificient_stub.jar" "" "" "runtime")
  ADD_COPY_RULE(LIB "${CMAKE_CURRENT_SOURCE_DIR}/exi.xq.src/java/exificient.jar" "com/zorba-xquery/www/modules/converters/exificient.jar" "" "" "runtime")
  ADD_COPY_RULE(LIB "${CMAKE_CURRENT_SOURCE_DIR}/exi.xq.src/java/xercesImpl.jar" "com/zorba-xquery/www/modules/converters/xercesImpl.jar" "" "" "runtime")
  ADD_COPY_RULE(LIB "${CMAKE_CURRENT_SOURCE_DIR}/exi.xq.src/java/xml-apis.jar" "com/zorba-xquery/www/modules/converters/xml-apis.jar" "" "" "runtime")
  INSTALL (FILES "${CMAKE_CURRENT_SOURCE_DIR}/exi.xq.src/java/exificient_stub.jar"
                 "${CMAKE_CURRENT_SOURCE_DIR}/exi.xq.src/java/exificient.jar"
                 "${CMAKE_CURRENT_SOURCE_DIR}/exi.xq.src/java/xercesImpl.jar"
                 "${CMAKE_CURRENT_SOURCE_DIR}/exi.xq.src/java/xml-apis.jar"
    DESTINATION "${ZORBA_CORE_URI_DIR}/com/zorba-xquery/www/modules/converters/"
    )

  #IF(Java_JAR_EXECUTABLE)
  #  MESSAGE(STATUS "Found Jar")
  #  FILE(TO_NATIVE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/exi.xq.src/java/exificient_stub.jar" SOURCE_EXIFICIENT_STUB_JAR)
  #  ADD_CUSTOM_COMMAND(TARGET ${module_lib_target}
  #              POST_BUILD
  #              COMMAND ${Java_JAR_EXECUTABLE} cf "\"${SOURCE_EXIFICIENT_STUB_JAR}\"" -C "\"${CMAKE_CURRENT_SOURCE_DIR}/exi.xq.src/java/\"" "com/zorbaxquery/exi/exificient_stub.class"
  #              COMMAND copy /Y "\"${SOURCE_EXIFICIENT_STUB_JAR}\"" "\"${CMAKE_BINARY_DIR}/LIB_PATH/com/zorba-xquery/www/modules/converters/exificient_stub.jar\""
  #              )
  #  #ADD_DEPENDENCIES(${module_lib_target} "${CMAKE_CURRENT_SOURCE_DIR}/exi.xq.src/java/exificient_stub.java")
  #ELSE(Java_JAR_EXECUTABLE)
  #  MESSAGE(STATUS "Java archiver (jar) is not found, so exificient_stub.jar will not be regenerated. Jar can be found in JDK.")
  #ENDIF(Java_JAR_EXECUTABLE)
  ADD_TEST_DIRECTORY("${PROJECT_SOURCE_DIR}/test_exi")
ELSE(JNI_FOUND)
  MESSAGE(STATUS "JNI is not found. EXI module will not be generated. JNI is found in JRE or better in JDK.")
ENDIF(JNI_FOUND)

