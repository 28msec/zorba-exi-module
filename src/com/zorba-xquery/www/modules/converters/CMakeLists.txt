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


##### EXI

FIND_PACKAGE (Java)

IF(JNI_FOUND)
  MESSAGE(STATUS "Found JNI, add EXI module")
  DECLARE_ZORBA_SCHEMA (FILE "exi-options.xsd" URI "http://www.zorba-xquery.com/modules/converters/exi-options")
  DECLARE_ZORBA_SCHEMA (FILE "exi-header.xsd" URI "http://www.zorba-xquery.com/modules/converters/exi-header") #"http://www.w3.org/2009/exi")

  INCLUDE_DIRECTORIES (${zorba_util-jvm_module_INCLUDE_DIRS})
  DECLARE_ZORBA_MODULE (
    URI "http://www.zorba-xquery.com/modules/converters/exi"
    VERSION 1.0 FILE "exi.xq"
    LINK_LIBRARIES "${JAVA_JVM_LIBRARY}" "${zorba_util-jvm_module_LIBRARIES}")
  INCLUDE_DIRECTORIES ("exi.xq.src")
  INCLUDE_DIRECTORIES (${JNI_INCLUDE_DIRS})

  # Compile/jar .java files
  SET (JAVA_SOURCE_FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/exi.xq.src/java/com/zorbaxquery/exi/exificient_options.java"
    "${CMAKE_CURRENT_SOURCE_DIR}/exi.xq.src/java/com/zorbaxquery/exi/exificient_stub.java"
    "${CMAKE_CURRENT_SOURCE_DIR}/exi.xq.src/java/com/zorbaxquery/exi/MyFragmentContentHandler.java")
  SET (JAVA_CLASS_PATH
    "${CMAKE_CURRENT_SOURCE_DIR}/exi.xq.src/java/exificient.jar"
    "${CMAKE_CURRENT_SOURCE_DIR}/exi.xq.src/java/xercesImpl.jar"
    "${CMAKE_CURRENT_SOURCE_DIR}/exi.xq.src/java/xml-apis.jar")
  IF (NOT WIN32)
    STRING (REPLACE ";" ":" JAVA_CLASS_PATH "${JAVA_CLASS_PATH}")
  ENDIF (NOT WIN32)
  SET (JAR_NAME "${PROJECT_NAME}.jar")

  ADD_CUSTOM_TARGET (${PROJECT_NAME}.jar.classes
    COMMAND "${JAVA_COMPILE}" -d "${CMAKE_CURRENT_BINARY_DIR}"
            -cp "${JAVA_CLASS_PATH}"
	    ${JAVA_SOURCE_FILES}
    SOURCES ${JAVA_SOURCE_FILES})
  ADD_DEPENDENCIES (${PROJECT_NAME}.jar.classes ${JAVA_SOURCE_FILES})

  ADD_CUSTOM_COMMAND (
    OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/${JAR_NAME}"
    COMMAND "${JAVA_ARCHIVE}" cvf "${JAR_NAME}" com
    DEPENDS "${PROJECT_NAME}.jar.classes"
    WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")
  ADD_CUSTOM_TARGET (${JAR_NAME} ALL
    DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/${JAR_NAME}")

  DECLARE_ZORBA_JAR(FILE "${CMAKE_CURRENT_SOURCE_DIR}/exi.xq.src/java/exificient.jar")
  DECLARE_ZORBA_JAR(FILE "${CMAKE_CURRENT_SOURCE_DIR}/exi.xq.src/java/xercesImpl.jar")
  DECLARE_ZORBA_JAR(FILE "${CMAKE_CURRENT_SOURCE_DIR}/exi.xq.src/java/xml-apis.jar")
  DECLARE_ZORBA_JAR(FILE "${CMAKE_CURRENT_BINARY_DIR}/${JAR_NAME}"
    TARGET ${JAR_NAME})
  ADD_TEST_DIRECTORY("${PROJECT_SOURCE_DIR}/test_exi")
ELSE(JNI_FOUND)
  MESSAGE(STATUS "JNI is not found. EXI module will not be generated. JNI is found in JDK.")
ENDIF(JNI_FOUND)

