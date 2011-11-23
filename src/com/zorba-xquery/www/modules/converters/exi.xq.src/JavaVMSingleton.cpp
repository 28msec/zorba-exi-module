/*
 * Copyright 2006-2008 The FLWOR Foundation.
 * 
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 * http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifdef WIN32
#include <Windows.h>
#include <tchar.h>
#else
#include <unistd.h>
#endif
#include <iostream>
#include <string>
#include <cstdlib>
#include <cstring>
#include <sstream>

#include <zorba/file.h>
#include <zorba/item.h>
#include <zorba/zorba.h>
#include <zorba/user_exception.h>

#include "JavaVMSingleton.h"

namespace zorba { namespace exi {
JavaVMSingleton* JavaVMSingleton::instance = NULL;

typedef jint (JNICALL  *JNI_CreateJavaVM_func)(JavaVM **pvm, void **penv, void *args);
#define CHECK_EXCEPTION(env)  if ((lException = env->ExceptionOccurred())) throw JavaException()

JavaVMSingleton::JavaVMSingleton(const char* classPath)
{
  JavaVMInitArgs args;
  JavaVMOption options[1];
  jint r;
  jint nOptions = 1;

  std::string classpathOption = "-Djava.class.path=";
  classpathOption += classPath;
  options[0].optionString = (char*)classpathOption.c_str();
  options[0].extraInfo = NULL;
  memset(&args, 0, sizeof(args));
  args.version  = JNI_VERSION_1_6;
  args.nOptions = nOptions;
  args.options  = options;
  args.ignoreUnrecognized = JNI_TRUE;

#ifdef WIN32
  HMODULE hVM = LoadLibrary(_T("jvm.dll"));
  if (hVM == NULL) {
    throw VMOpenException();
  }
  JNI_CreateJavaVM_func create_jvm = (JNI_CreateJavaVM_func)GetProcAddress(hVM, "JNI_CreateJavaVM");
#else
  void *libVM = dlopen("jvm.so", RTLD_LAZY);
  if (libVM == NULL) {
    throw VMOpenException();
  }
  JNI_CreateJavaVM_func create_jvm = dlsym(libVM, "JNI_CreateJavaVM");
#endif

  r = create_jvm(&m_vm, (void **)&m_env, &args);
  if (r != JNI_OK) {
    throw VMOpenException();
  }

  jthrowable lException = 0;
  exificient_stub_class = m_env->FindClass("com/zorbaxquery/exi/exificient_stub");
  CHECK_EXCEPTION(m_env);
  decode_method_id= m_env->GetStaticMethodID(exificient_stub_class, 
                                                    "decodeSchemaInformed", 
                                                    "([BLcom/zorbaxquery/exi/exificient_options;)Ljava/lang/String;");
  CHECK_EXCEPTION(m_env);
  encode_method_id= m_env->GetStaticMethodID(exificient_stub_class, 
                                                    "encodeSchemaInformed", 
                                                    "(Ljava/lang/String;Lcom/zorbaxquery/exi/exificient_options;)[B");
  CHECK_EXCEPTION(m_env);
}

JavaVMSingleton::~JavaVMSingleton()
{
  if (instance) {
    instance = NULL;
  }
  m_vm->DestroyJavaVM();
}

JavaVMSingleton* JavaVMSingleton::getInstance()
{
  if (instance == NULL) {
    instance = new JavaVMSingleton(findExificient().c_str());
  }
  return instance;
}

void JavaVMSingleton::destroyInstance() {
  delete instance;
}

JavaVM* JavaVMSingleton::getVM()
{
  return m_vm;
}

JNIEnv* JavaVMSingleton::getEnv()
{
  return m_env;
}

void JavaVMSingleton::throwError(std::string aName) 
{
  Item lQName = Zorba::getInstance(0)->getItemFactory()->createQName("http://www.zorba-xquery.com/modules/converters/exi",
      "JAR-NOT-FOUND");
  throw USER_EXCEPTION(lQName, aName);
}

std::string JavaVMSingleton::findExificient()
{
  std::string lDirectorySeparator(File::getDirectorySeparator());
/*
  std::string lExificientHome;
  {
    char* lExificientHomeEnv = getenv("EXIFICIENT_HOME");
    if (lExificientHomeEnv != 0) {
      lExificientHome = lExificientHomeEnv;
    }
#ifdef APPLE
    else {
      std::string lExificientPath("/opt/local/share/java/exificient/");
      File_t lRootDir = File::createFile(lExificientPath);
      if (lRootDir->exists() && lRootDir->isDirectory()) {
        lExificientHome = lExificientPath;
      }
    }
#endif
  }
  std::string lExificientLibDir;
  if(lExificientHome.empty())
  {
    char* lEnv = getenv("EXIFICIENT_LIB_DIR");
    if (lEnv != 0) {
      lExificientLibDir = lEnv;
    }
#ifdef LINUX
    else {
      lExificientLibDir = "/usr/share/java";
    }
#endif
  }
  // If neither a path to the fop install dir, nor a path
  // to the jar files was found so far, we throw an exception.
  if (lExificientHome == "" && lExificientLibDir == "") {
    throwError("None of the environment variables EXIFICIENT_HOME and EXIFICIENT_LIB_DIR have been set.");
  }
  std::string lExificientJarFile;
  {
    // Here we look for the exificient.jar file.
    File_t lJarFile;
    std::string lExificientJarFile1;
    if(!lExificientHome.empty())
    {
      lExificientJarFile = lExificientHome + lDirectorySeparator + "lib" + lDirectorySeparator + "exificient.jar";
      lExificientJarFile1 = lExificientJarFile;
      lJarFile = File::createFile(lExificientJarFile);
    }
    if (lExificientHome.empty() || !lJarFile->exists()) {
      lExificientJarFile = lExificientLibDir + lDirectorySeparator + "exificient.jar";
      lJarFile = File::createFile(lExificientJarFile);
      if (!lJarFile->exists()) {
        std::string errmsg = "Could not find exificient.jar. If you are using Ubuntu or Mac OS X, please make sure, ";
        errmsg += "that you have installed it, else make sure, that you have set the environment variable ";
        errmsg += "EXIFICIENT_HOME or EXIFICIENT_LIB_DIR correctly. Tried '";
        errmsg +=  lExificientJarFile1;
        errmsg += "' and '";
        errmsg += lExificientJarFile;
        errmsg += "'.";
        throwError(errmsg);
      }
    }
  }
  std::string lClassPaths;
  lClassPaths = lExificientJarFile;
  {
    std::string lJarDir = lExificientJarFile;
    std::string::size_type  bslashpos;
    while((bslashpos=lExificientJarFile.find('\\')) != std::string::npos)
      lExificientJarFile.replace(bslashpos, 1, 1, '/');
    lJarDir = lExificientJarFile.substr(0, lExificientJarFile.rfind('/'));
    // This is a list of all jar files, EXIficient depends on.
    lClassPaths += File::getPathSeparator();
    lClassPaths += lJarDir + "xercesImpl";
    lClassPaths += File::getPathSeparator();
    lClassPaths += lJarDir + "xml-apis";
    lClassPaths += File::getPathSeparator();
  }
  return lClassPaths;
*/
  File_t lJarFile;
  std::string jar_path;
#ifdef WIN32
  TCHAR   module_path[1024];
  HMODULE dll_handle;
  GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT, 
                    (LPCTSTR)&JavaVMSingleton::instance,
                    &dll_handle);
  GetModuleFileName(dll_handle, module_path, sizeof(module_path)/sizeof(TCHAR));
#ifndef UNICODE
  jar_path = module_path;
#else
  char  ascii_path[1024];
  ascii_path[0] = 0;
  WideCharToMultiByte(CP_UTF8, 0, module_path, -1, ascii_path, sizeof(ascii_path), NULL, NULL);
  jar_path = ascii_path;
#endif
  std::string::size_type last_bslash = jar_path.rfind('\\');
  jar_path = jar_path.substr(0, last_bslash+1);
  lJarFile = File::createFile(jar_path + "exificient_stub.jar");
  if (!lJarFile->exists()) {
    jar_path += "..\\";
    lJarFile = File::createFile(jar_path + "exificient_stub.jar");
    if (!lJarFile->exists()) {
        throwError("Could not find exificient_stub.jar exificient.jar xercesImpl.jar xml-apis.jar");
    }
  }
#endif

  return jar_path + "exificient_stub.jar;" +
         jar_path + "exificient.jar;" +
         jar_path + "xercesImpl.jar;" +
         jar_path + "xml-apis.jar";
  //return std::string("./") + lDirectorySeparator + std::string("../") + lDirectorySeparator;// + "Debug" + lDirectorySeparator + "Release" + lDirectorySeparator;
  //return "E:\\xquery_development\\zorba_repo\\z_m2\\conv\\src\\com\\zorba-xquery\\www\\modules\\converters\\exi.xq.src\\java\\exificient_stub.jar";
    //"E:\\xquery_development\\xercesImpl.jar;"
    //"E:\\xquery_development\\xml-apis.jar";
}


}} // namespace zorba, xslfo
