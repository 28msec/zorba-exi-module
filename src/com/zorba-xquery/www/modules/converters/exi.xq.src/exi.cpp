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
#include <sstream>
#include <iostream>
#include <cstdlib>
#include <list>
#include <jni.h>

#include <zorba/base64.h>
#include <zorba/empty_sequence.h>
#include <zorba/diagnostic_list.h>
#include <zorba/function.h>
#include <zorba/external_module.h>
#include <zorba/user_exception.h>
#include <zorba/file.h>
#include <zorba/item_factory.h>
#include <zorba/serializer.h>
#include <zorba/singleton_item_sequence.h>
#include <zorba/vector_item_sequence.h>
#include <zorba/zorba.h>
#include <zorba/serializer.h>
#include <zorba/store_consts.h>

#include "JavaVMSingleton.h"

#define EXI_MODULE_NAMESPACE "http://www.zorba-xquery.com/modules/converters/exi"


#define CHECK_EXCEPTION(env)  if ((lException = env->ExceptionOccurred())) throw JavaException()

namespace zorba { namespace exi {
 
static unsigned char get_base64_byte(char b64)
{
  unsigned char offset = 0;
  if((b64 >= 'A') && (b64 <= 'Z'))
    return b64-'A';
  offset += 'Z'-'A'+1;
  if((b64 >= 'a') && (b64 <= 'z'))
    return b64-'a'+offset;
  offset += 'z'-'a'+1;
  if((b64 >= '0') && (b64 <= '9'))
    return b64-'0'+offset;
  offset += '9'-'0'+1;
  if(b64 == '+')
    return offset;
  if(b64 == '/')
    return offset+1;
  return 255;
}

void Base64_decode(const char* aSource, size_t in_len, unsigned char** aResult, size_t *out_len)
{
  //size_t in_len = aSource.size();
  int i = 0;
  int j = 0;
  int in_ = 0;
  char char_array_4[4], char_array_3[3];

  *aResult = (unsigned char*)malloc(in_len * 6 / 8 + 2);
  *out_len = 0;

  while (in_len-- && ( aSource[in_] != '=') )//&& is_base64(aSource[in_])) 
  {
    char_array_4[i++] = aSource[in_]; in_++;
    if (i ==4) {
      for (i = 0; i < 4; i++)
        char_array_4[i] = get_base64_byte(char_array_4[i]);

      char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
      char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
      char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

      for (i = 0; (i < 3); i++)
        (*aResult)[(*out_len)++] = char_array_3[i];
      i = 0;
    }
  }

  if (i) {
    for (j = i; j < 4; j++)
      char_array_4[j] = 0;

    for (j = 0; j <4; j++)
      char_array_4[j] = get_base64_byte(char_array_4[j]);

    char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
    char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
    char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

    for (j = 0; (j < i - 1); j++) 
      (*aResult)[(*out_len)++] = char_array_3[j];
  }
}

class EXISerializeFunction : public NonContextualExternalFunction {
  private:
    const ExternalModule* theModule;
    ItemFactory* theFactory;
  public:
    EXISerializeFunction(const ExternalModule* aModule) :
      theModule(aModule), theFactory(Zorba::getInstance(0)->getItemFactory()) {}
    ~EXISerializeFunction() {
      JavaVMSingleton::destroyInstance();
    }

  public:
    virtual String getURI() const { return theModule->getURI(); }

    virtual String getLocalName() const { return "serialize-internal"; }

    virtual ItemSequence_t 
    evaluate(const ExternalFunction::Arguments_t& args) const;
};

class EXIParseFunction : public NonContextualExternalFunction {
  private:
    const ExternalModule* theModule;
    ItemFactory* theFactory;
  private:
    void throwError(std::string aName) const;
  public:
    EXIParseFunction(const ExternalModule* aModule) :
      theModule(aModule), theFactory(Zorba::getInstance(0)->getItemFactory()) {}

    virtual String getURI() const { return theModule->getURI(); }

    virtual String getLocalName() const { return "parse-internal"; }

    virtual ItemSequence_t 
    evaluate(const ExternalFunction::Arguments_t& args) const;
};

class EXIModule : public ExternalModule {
  private:
    EXISerializeFunction* func_serialize;
    EXIParseFunction*     func_parse;
  public:
    EXIModule() :
      func_serialize(new EXISerializeFunction(this)),
      func_parse(new EXIParseFunction(this))
  {}
    ~EXIModule() {
      delete func_serialize;
      delete func_parse;
    }

    virtual String getURI() const { return EXI_MODULE_NAMESPACE; }

    virtual ExternalFunction* getExternalFunction(const String& localName);

    virtual void destroy() {
      delete this;
    }
};

ExternalFunction* EXIModule::getExternalFunction(const String& localName) {
  if (localName == func_serialize->getLocalName()) {
    return func_serialize;
  } else if (localName == func_parse->getLocalName()) {
    return func_parse;
  }
  return 0;
}

static bool getChildElement(Iterator_t iter, std::string localname, Item *child)
{
  Item c1;
  while(iter->next(c1))
  {
    if(c1.getNodeKind() != store::StoreConsts::elementNode)
      continue;
    Item c1_name;
    c1.getNodeName(c1_name);
    String  c1_localname;
    c1_localname = c1_name.getLocalName();
    if(c1_localname.compare(localname) == 0)
    {
      if(child)
        *child = c1;
      return true;
    }
  }
  return false;
}

static bool getChildElement(Item parent, std::string localname, Item *child)
{
  Iterator_t  iter;
  iter = parent.getChildren();
  iter->open();
  bool retval = getChildElement(iter, localname, child);
  iter->close();
  return retval;
}

static jobject build_java_qname_from_elem(JNIEnv* env, Item elem)
{
  jthrowable lException = 0;
  Item elem_name;
  elem.getNodeName(elem_name);
  jstring  elem_ns = env->NewStringUTF(elem_name.getNamespace().c_str());
  CHECK_EXCEPTION(env);
  jstring  elem_prefix = env->NewStringUTF(elem_name.getPrefix().c_str());
  CHECK_EXCEPTION(env);
  jstring  elem_local = env->NewStringUTF(elem_name.getLocalName().c_str());
  CHECK_EXCEPTION(env);

  jclass  jqname_class = env->FindClass("javax/xml/namespace/QName");
  CHECK_EXCEPTION(env);
  jmethodID jqname_constructor = env->GetMethodID(jqname_class, "<init>", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V");
  CHECK_EXCEPTION(env);
  jobject jqname = env->NewObject(jqname_class, jqname_constructor, elem_ns, elem_local, elem_prefix);
  CHECK_EXCEPTION(env);
  env->DeleteLocalRef(elem_ns);
  CHECK_EXCEPTION(env);
  env->DeleteLocalRef(elem_local);
  CHECK_EXCEPTION(env);
  env->DeleteLocalRef(elem_prefix);
  CHECK_EXCEPTION(env);
  env->DeleteLocalRef(jqname_class);
  CHECK_EXCEPTION(env);

  return jqname;
}

static void add_self_contained(JNIEnv* env, 
                               jclass exificient_options_class, 
                               jobject exificient_options, 
                               std::vector<Item> root_elems)
{
  if(!root_elems.size())
    return;
  jthrowable lException = 0;

  jclass  jqname_class = env->FindClass("Ljavax/xml/namespace/QName");
  CHECK_EXCEPTION(env);

  jfieldID fid;
  fid = env->GetFieldID(exificient_options_class, "selfContained", "[Ljavax/xml/namespace/QName;");
  CHECK_EXCEPTION(env);

  jobjectArray jqname_array2 = env->NewObjectArray(root_elems.size(), jqname_class, NULL);
  CHECK_EXCEPTION(env);
  for(jsize i=0;i<(jsize)root_elems.size();i++)
  {
    jobject jqname = build_java_qname_from_elem(env, root_elems.at(i));
    env->SetObjectArrayElement(jqname_array2, i, jqname);
    CHECK_EXCEPTION(env);
    env->DeleteLocalRef(jqname);
    CHECK_EXCEPTION(env);
  }
  env->SetObjectField(exificient_options, fid, jqname_array2);
  CHECK_EXCEPTION(env);
  env->DeleteLocalRef(jqname_array2);
  CHECK_EXCEPTION(env);
  env->DeleteLocalRef(jqname_class);
  CHECK_EXCEPTION(env);

}

static void add_datatyperepresentationmap(JNIEnv* env, 
                                          jclass exificient_options_class, 
                                          jobject exificient_options, 
                                          std::vector<Item> datatypes, 
                                          std::vector<Item> representations)
{
  if(!datatypes.size())
    return;
  jthrowable lException = 0;
  jclass  jqname_class = env->FindClass("javax/xml/namespace/QName");
  CHECK_EXCEPTION(env);

  jfieldID datatypes_fid;
  datatypes_fid = env->GetFieldID(exificient_options_class, "dtrMapTypes", "[Ljavax/xml/namespace/QName;");
  CHECK_EXCEPTION(env);
  jfieldID representations_fid;
  representations_fid = env->GetFieldID(exificient_options_class, "dtrMapRepresentations", "[Ljavax/xml/namespace/QName;");
  CHECK_EXCEPTION(env);

  jobjectArray jdatatypes_array = env->NewObjectArray(datatypes.size(), jqname_class, NULL);
  jobjectArray jrepresentations_array = env->NewObjectArray(datatypes.size(), jqname_class, NULL);
  CHECK_EXCEPTION(env);
  for(jsize i=0;i<(jsize)datatypes.size();i++)
  {
    jobject jqname_datatype = build_java_qname_from_elem(env, datatypes.at(i));
    jobject jqname_representation = build_java_qname_from_elem(env, representations.at(i));
    env->SetObjectArrayElement(jdatatypes_array, i, jqname_datatype);
    CHECK_EXCEPTION(env);
    env->DeleteLocalRef(jqname_datatype);
    CHECK_EXCEPTION(env);
    env->SetObjectArrayElement(jrepresentations_array, i, jqname_representation);
    CHECK_EXCEPTION(env);
    env->DeleteLocalRef(jqname_representation);
    CHECK_EXCEPTION(env);
  }
  env->SetObjectField(exificient_options, datatypes_fid, jdatatypes_array);
  CHECK_EXCEPTION(env);
  env->SetObjectField(exificient_options, datatypes_fid, jrepresentations_array);
  CHECK_EXCEPTION(env);
  env->DeleteLocalRef(jdatatypes_array);
  CHECK_EXCEPTION(env);
  env->DeleteLocalRef(jrepresentations_array);
  CHECK_EXCEPTION(env);
  env->DeleteLocalRef(jqname_class);
  CHECK_EXCEPTION(env);
}

static jobject parse_options(bool is_serialize, const ExternalFunction::Arguments_t& args)
{
  bool has_selfcontained = false;
  int  nr_items = 0;
  Iterator_t lIter;
  jthrowable lException = 0;

  lIter = args[1]->getIterator();
  lIter->open();
  Item exi_options;
  if(!lIter->next(exi_options))
  {
    lIter->close();
    return NULL;
  }

  JNIEnv* env = JavaVMSingleton::getInstance()->getEnv();
  jclass exificient_options_class = env->FindClass("com/zorbaxquery/exi/exificient_options");
  CHECK_EXCEPTION(env);
  jobject exificient_options = env->NewObject(exificient_options_class,
                                    env->GetMethodID(exificient_options_class, "<init>", "()V"));
  CHECK_EXCEPTION(env);

  Item header_child;
  if(getChildElement(exi_options, "header", &header_child))
  {
    Item lesscommon_child;
    if(getChildElement(header_child, "lesscommon", &lesscommon_child))
    {
      Item uncommon_child;
      if(getChildElement(lesscommon_child, "uncommon", &uncommon_child))
      {
        Item alignment_child;
        if(getChildElement(uncommon_child, "alignment", &alignment_child))
        {
          jfieldID fid;
          fid = env->GetFieldID(exificient_options_class, "coding", "I");
          CHECK_EXCEPTION(env);
          if(getChildElement(alignment_child, "byte", NULL))
          {
            env->SetIntField(exificient_options, fid, 1);
            CHECK_EXCEPTION(env);
          }
          else if(getChildElement(alignment_child, "pre-compress", NULL))
          {
            env->SetIntField(exificient_options, fid, 3);
            CHECK_EXCEPTION(env);
          }
        }
        if(is_serialize && getChildElement(uncommon_child, "selfContained", NULL))
        {
          has_selfcontained = true;
          Iterator_t lIterInput;

          std::vector<Item> root_elems;
          lIterInput = args[0]->getIterator();
          Item input;
          lIterInput->open();
          while(lIterInput->next(input))
          {
            int node_kind = input.getNodeKind();
            switch(node_kind)
            {
            case store::StoreConsts::documentNode:
              {
                Item root_elem;
                Iterator_t iter = input.getChildren();
                iter->open();
                while(iter->next(root_elem) && (root_elem.getNodeKind() != store::StoreConsts::elementNode));
                iter->close();
                root_elems.push_back(root_elem);
                nr_items++;
              }break;
            case store::StoreConsts::elementNode:
              {
                root_elems.push_back(input);
                nr_items++;
              }break;
            }
          }
          lIterInput->close();
          add_self_contained(env, exificient_options_class, exificient_options, root_elems);
        }
        Item valueMaxLength_child;
        if(getChildElement(uncommon_child, "valueMaxLength", &valueMaxLength_child))
        {
          jfieldID fid;
          fid = env->GetFieldID(exificient_options_class, "valueMaxLength", "I");
          CHECK_EXCEPTION(env);
          unsigned int max_len = valueMaxLength_child.getUnsignedIntValue();
          env->SetIntField(exificient_options, fid, (jint)max_len);
          CHECK_EXCEPTION(env);
        }
        Item valuePartitionCapacity_child;
        if(getChildElement(uncommon_child, "valuePartitionCapacity", &valuePartitionCapacity_child))
        {
          jfieldID fid;
          fid = env->GetFieldID(exificient_options_class, "valuePartitionCapacity", "I");
          CHECK_EXCEPTION(env);
          unsigned int cap = valuePartitionCapacity_child.getUnsignedIntValue();
          env->SetIntField(exificient_options, fid, (jint)cap);
          CHECK_EXCEPTION(env);
        }

        Iterator_t uncommon_iter = uncommon_child.getChildren();
        Item dttrm_item;
        uncommon_iter->open();
        std::vector<Item>   datatypes;
        std::vector<Item>   representations;
        while(getChildElement(uncommon_iter, "datatypeRepresentationMap", &dttrm_item))
        {
          Item datatype;
          Item representation;
          Iterator_t   dttrm_iter = dttrm_item.getChildren();
          dttrm_iter->open();
          dttrm_iter->next(datatype);
          datatypes.push_back(datatype);
          dttrm_iter->next(representation);
          representations.push_back(representation);
          dttrm_iter->close();
        }
        uncommon_iter->close();
        add_datatyperepresentationmap(env, exificient_options_class, exificient_options, datatypes, representations);
      }
      Item preserve_child;
      if(getChildElement(lesscommon_child, "preserve", &preserve_child))
      {
        if(getChildElement(preserve_child, "dtd", NULL))
        {
          jfieldID fid;
          fid = env->GetFieldID(exificient_options_class, "preserve_dtds", "Z");
          CHECK_EXCEPTION(env);
          env->SetBooleanField(exificient_options, fid, JNI_TRUE);
          CHECK_EXCEPTION(env);
        }
        if(getChildElement(preserve_child, "prefixes", NULL))
        {
          jfieldID fid;
          fid = env->GetFieldID(exificient_options_class, "preserve_prefixes", "Z");
          CHECK_EXCEPTION(env);
          env->SetBooleanField(exificient_options, fid, JNI_TRUE);
          CHECK_EXCEPTION(env);
        }
        if(getChildElement(preserve_child, "lexicalValues", NULL))
        {
          jfieldID fid;
          fid = env->GetFieldID(exificient_options_class, "preserve_lexical_values", "Z");
          CHECK_EXCEPTION(env);
          env->SetBooleanField(exificient_options, fid, JNI_TRUE);
          CHECK_EXCEPTION(env);
        }
        if(getChildElement(preserve_child, "comments", NULL))
        {
          jfieldID fid;
          fid = env->GetFieldID(exificient_options_class, "preserve_comments", "Z");
          CHECK_EXCEPTION(env);
          env->SetBooleanField(exificient_options, fid, JNI_TRUE);
          CHECK_EXCEPTION(env);
        }
        if(getChildElement(preserve_child, "pis", NULL))
        {
          jfieldID fid;
          fid = env->GetFieldID(exificient_options_class, "preserve_pis", "Z");
          CHECK_EXCEPTION(env);
          env->SetBooleanField(exificient_options, fid, JNI_TRUE);
          CHECK_EXCEPTION(env);
        }
      }
      Item blockSize_child;
      if(getChildElement(lesscommon_child, "blockSize", &blockSize_child))
      {
        jfieldID fid;
        fid = env->GetFieldID(exificient_options_class, "blockSize", "I");
        CHECK_EXCEPTION(env);
        int block_size = blockSize_child.getUnsignedIntValue();
        env->SetIntField(exificient_options, fid, (jint)block_size);
        CHECK_EXCEPTION(env);
      }
    }
    Item common_child;
    if(getChildElement(header_child, "common", &common_child))
    {
      if(getChildElement(common_child, "compression", NULL))
      {
        jfieldID fid;
        fid = env->GetFieldID(exificient_options_class, "coding", "I");
        CHECK_EXCEPTION(env);
        env->SetIntField(exificient_options, fid, 2);
        CHECK_EXCEPTION(env);
      }
      if(getChildElement(common_child, "fragment", NULL))
      {
        jfieldID fid;
        fid = env->GetFieldID(exificient_options_class, "is_fragment", "Z");
        CHECK_EXCEPTION(env);
        env->SetBooleanField(exificient_options, fid, JNI_TRUE);
        CHECK_EXCEPTION(env);
      }
      Item schemaId_child;
      if(getChildElement(common_child, "schemaId", &schemaId_child))
      {
        jfieldID fid;
        fid = env->GetFieldID(exificient_options_class, "schema_location", "Ljava/lang/String;");
        CHECK_EXCEPTION(env);
        String  schema_loc = schemaId_child.getStringValue();
        jstring jschema_loc;
        jschema_loc = env->NewStringUTF(schema_loc.c_str());
        env->SetObjectField(exificient_options, fid, jschema_loc);
        CHECK_EXCEPTION(env);
        env->DeleteLocalRef(jschema_loc);
        CHECK_EXCEPTION(env);
      }
    }
    if(getChildElement(header_child, "strict", NULL))
    {
      jfieldID fid;
      fid = env->GetFieldID(exificient_options_class, "strict_schema", "Z");
      CHECK_EXCEPTION(env);
      env->SetBooleanField(exificient_options, fid, JNI_TRUE);
      CHECK_EXCEPTION(env);
    }
  }
  if(getChildElement(exi_options, "cookie", NULL))
  {
    jfieldID fid;
    fid = env->GetFieldID(exificient_options_class, "include_cookie", "Z");
    CHECK_EXCEPTION(env);
    env->SetBooleanField(exificient_options, fid, JNI_TRUE);
    CHECK_EXCEPTION(env);
  }
  if(getChildElement(exi_options, "add-options", NULL))
  {
    jfieldID fid;
    fid = env->GetFieldID(exificient_options_class, "include_options", "Z");
    CHECK_EXCEPTION(env);
    env->SetBooleanField(exificient_options, fid, JNI_TRUE);
    CHECK_EXCEPTION(env);
  }
  if(getChildElement(exi_options, "add-insignificant-xsi-nil", NULL))
  {
    jfieldID fid;
    fid = env->GetFieldID(exificient_options_class, "include_insignificant_xsi_nil", "Z");
    CHECK_EXCEPTION(env);
    env->SetBooleanField(exificient_options, fid, JNI_TRUE);
    CHECK_EXCEPTION(env);
  }
  if(getChildElement(exi_options, "add-insignificant-xsi-type", NULL))
  {
    jfieldID fid;
    fid = env->GetFieldID(exificient_options_class, "include_insignificant_xsi_type", "Z");
    CHECK_EXCEPTION(env);
    env->SetBooleanField(exificient_options, fid, JNI_TRUE);
    CHECK_EXCEPTION(env);
  }
  if(getChildElement(exi_options, "add-xsi-schemalocation", NULL))
  {
    jfieldID fid;
    fid = env->GetFieldID(exificient_options_class, "include_xsi_schemalocation", "Z");
    CHECK_EXCEPTION(env);
    env->SetBooleanField(exificient_options, fid, JNI_TRUE);
    CHECK_EXCEPTION(env);
  }
  if(getChildElement(exi_options, "preserve-entity-references", NULL))
  {
    jfieldID fid;
    fid = env->GetFieldID(exificient_options_class, "preserve_entity_references", "Z");
    CHECK_EXCEPTION(env);
    env->SetBooleanField(exificient_options, fid, JNI_TRUE);
    CHECK_EXCEPTION(env);
  }

  env->DeleteLocalRef(exificient_options_class);
  CHECK_EXCEPTION(env);

  return exificient_options;
}

ItemSequence_t EXIParseFunction::evaluate(const ExternalFunction::Arguments_t& args) const
{
  Iterator_t lIter;

  lIter = args[0]->getIterator();
  lIter->open();
  Item exi_input;
  lIter->next(exi_input);
  lIter->close();
  jthrowable lException = 0;
  JNIEnv* env;
  try {
    env = JavaVMSingleton::getInstance()->getEnv();

    jclass  exificient_stub_class;
    jmethodID decode_method_id;
    exificient_stub_class = env->FindClass("com/zorbaxquery/exi/exificient_stub");
    CHECK_EXCEPTION(env);
    decode_method_id= env->GetStaticMethodID(exificient_stub_class, 
                                                "decodeSchemaInformed", 
                                                "([BLcom/zorbaxquery/exi/exificient_options;)Ljava/lang/String;");
    CHECK_EXCEPTION(env);
    zorba::String exibase64 = exi_input.getStringValue();
    //zorba::String exibinstr = zorba::encoding::Base64::decode(exibase64);
    //const jbyte *exibin = (const jbyte*)exibinstr.data();
    //String::size_type  exilen = exibinstr.length();
    unsigned char *exibin;
    size_t        exilen;
    Base64_decode(exibase64.c_str(), exibase64.size(), &exibin, &exilen);
    jbyteArray  exiarray = env->NewByteArray(exilen);
    env->SetByteArrayRegion(exiarray, 0, exilen, (jbyte*)exibin);
    CHECK_EXCEPTION(env);
    free(exibin);

    jobject options = parse_options(false, args);
    jstring decoded_exi = (jstring)env->CallStaticObjectMethod(exificient_stub_class, decode_method_id, exiarray, options);
    CHECK_EXCEPTION(env);
    env->DeleteLocalRef(exiarray);
    CHECK_EXCEPTION(env);
    env->DeleteLocalRef(options);
    CHECK_EXCEPTION(env);

    const char *existr = env->GetStringUTFChars(decoded_exi, NULL);
    CHECK_EXCEPTION(env);
    String  result_string(existr);
    env->ReleaseStringUTFChars(decoded_exi, existr);
    CHECK_EXCEPTION(env);
    env->DeleteLocalRef(decoded_exi);
    CHECK_EXCEPTION(env);

    env->DeleteLocalRef(exificient_stub_class);
    CHECK_EXCEPTION(env);

    StaticContext_t   sctx = Zorba::getInstance(0)->createStaticContext();
    Item    parse_xml_name;
    parse_xml_name = theFactory->createQName("http://www.w3.org/2005/xpath-functions", "fn", "parse-xml");
    std::vector<ItemSequence_t> parse_xml_args;
    parse_xml_args.push_back(ItemSequence_t(new SingletonItemSequence(theFactory->createString(result_string))));
    ItemSequence_t  xmldoc_seq = sctx->invoke(parse_xml_name, parse_xml_args);
    return xmldoc_seq;

  /*
    // Create the result
    dataSize = env->GetArrayLength(res);
    dataElements = env->GetByteArrayElements(res, &isCopy);

    std::string lBinaryString((const char*) dataElements, dataSize);
    std::stringstream lStream(lBinaryString);
    String base64S = encoding::Base64::encode(lStream);
    Item lRes = theFactory->createBase64Binary(base64S.c_str(), base64S.length());
    return ItemSequence_t(new SingletonItemSequence(lRes));
  */
  } catch (VMOpenException&) {
    Item lQName = theFactory->createQName(EXI_MODULE_NAMESPACE,
        "VM001");
    throw USER_EXCEPTION(lQName, "Could not start the Java VM (is the classpath set?)");
  } catch (JavaException&) {
    jclass stringWriterClass = env->FindClass("java/io/StringWriter");
    jclass printWriterClass = env->FindClass("java/io/PrintWriter");
    jclass throwableClass = env->FindClass("java/lang/Throwable");
    jobject stringWriter = env->NewObject(
        stringWriterClass,
        env->GetMethodID(stringWriterClass, "<init>", "()V"));
    jobject printWriter = env->NewObject(
        printWriterClass, env->GetMethodID(printWriterClass, "<init>", "(Ljava/io/Writer;)V"), stringWriter);
    env->CallObjectMethod(lException, env->GetMethodID(throwableClass, "printStackTrace", "(Ljava/io/PrintWriter;)V"), printWriter);
    //env->CallObjectMethod(printWriter, env->GetMethodID(printWriterClass, "flush", "()V"));
    jmethodID toStringMethod = env->GetMethodID(stringWriterClass, "toString", "()Ljava/lang/String;");
    jobject errorMessageObj = env->CallObjectMethod(
        stringWriter, toStringMethod);
    jstring errorMessage = (jstring) errorMessageObj;
    const char *errMsg = env->GetStringUTFChars(errorMessage, 0);
    std::stringstream s;
    s << "A Java Exception was thrown:" << std::endl << errMsg;
    env->ReleaseStringUTFChars(errorMessage, errMsg);
    std::string err("");
    err += s.str();
    env->ExceptionClear();
    Item lQName = theFactory->createQName(EXI_MODULE_NAMESPACE,
        "JAVA-EXCEPTION");
    env->DeleteLocalRef(errorMessage);
    CHECK_EXCEPTION(env);
    env->DeleteLocalRef(printWriter);
    CHECK_EXCEPTION(env);
    env->DeleteLocalRef(stringWriter);
    CHECK_EXCEPTION(env);
    env->DeleteLocalRef(throwableClass);
    CHECK_EXCEPTION(env);
    env->DeleteLocalRef(printWriterClass);
    CHECK_EXCEPTION(env);
    env->DeleteLocalRef(stringWriterClass);
    CHECK_EXCEPTION(env);
    throw USER_EXCEPTION(lQName, err);
  }
  return ItemSequence_t(new EmptySequence());
}

ItemSequence_t EXISerializeFunction::evaluate(const ExternalFunction::Arguments_t& args) const
{
  //Iterator_t lIter;

  //lIter = args[0]->getIterator();
  //lIter->open();
  //Item element_input;
  //lIter->next(element_input);
  //lIter->close();
  jthrowable lException = 0;
  JNIEnv* env;
  try {
    env = JavaVMSingleton::getInstance()->getEnv();
    jclass  exificient_stub_class;
    jmethodID encode_method_id;
    exificient_stub_class = env->FindClass("com/zorbaxquery/exi/exificient_stub");
    CHECK_EXCEPTION(env);
    encode_method_id= env->GetStaticMethodID(exificient_stub_class, 
                                            "encodeSchemaInformed", 
                                            "(Ljava/lang/String;Lcom/zorbaxquery/exi/exificient_options;)[B");
    CHECK_EXCEPTION(env);

    Zorba_SerializerOptions lOptions;
    lOptions.ser_method = ZORBA_SERIALIZATION_METHOD_XML;
    Serializer_t lSerializer = Serializer::createSerializer(lOptions);
    std::ostringstream  lOutStream;
    lSerializer->serialize(args[0], lOutStream);
    jstring xml_string;
    xml_string = env->NewStringUTF(lOutStream.str().c_str());
    CHECK_EXCEPTION(env);

    jobject options = parse_options(true, args);
    jbyteArray encoded_exi = (jbyteArray)env->CallStaticObjectMethod(exificient_stub_class, encode_method_id, xml_string, options);
    CHECK_EXCEPTION(env);
    env->DeleteLocalRef(xml_string);
    CHECK_EXCEPTION(env);
    env->DeleteLocalRef(options);
    CHECK_EXCEPTION(env);
    jbyte*  exi_bytes = env->GetByteArrayElements(encoded_exi, NULL);
    CHECK_EXCEPTION(env);
    zorba::Item result = theFactory->createBase64Binary((const unsigned char*)exi_bytes, env->GetArrayLength(encoded_exi));
    env->ReleaseByteArrayElements(encoded_exi, exi_bytes, 0);
    CHECK_EXCEPTION(env);
    env->DeleteLocalRef(encoded_exi);
    CHECK_EXCEPTION(env);
    env->DeleteLocalRef(exificient_stub_class);
    CHECK_EXCEPTION(env);
    return ItemSequence_t(new SingletonItemSequence(result));

  } catch (VMOpenException&) {
    Item lQName = theFactory->createQName(EXI_MODULE_NAMESPACE,
        "VM001");
    throw USER_EXCEPTION(lQName, "Could not start the Java VM (is the classpath set?)");
  } catch (JavaException&) {
    jclass stringWriterClass = env->FindClass("java/io/StringWriter");
    jclass printWriterClass = env->FindClass("java/io/PrintWriter");
    jclass throwableClass = env->FindClass("java/lang/Throwable");
    jobject stringWriter = env->NewObject(
        stringWriterClass,
        env->GetMethodID(stringWriterClass, "<init>", "()V"));
    jobject printWriter = env->NewObject(
        printWriterClass, env->GetMethodID(printWriterClass, "<init>", "(Ljava/io/Writer;)V"), stringWriter);
    env->CallObjectMethod(lException, env->GetMethodID(throwableClass, "printStackTrace", "(Ljava/io/PrintWriter;)V"), printWriter);
    //env->CallObjectMethod(printWriter, env->GetMethodID(printWriterClass, "flush", "()V"));
    jmethodID toStringMethod = env->GetMethodID(stringWriterClass, "toString", "()Ljava/lang/String;");
    jobject errorMessageObj = env->CallObjectMethod(
        stringWriter, toStringMethod);
    jstring errorMessage = (jstring) errorMessageObj;
    const char *errMsg = env->GetStringUTFChars(errorMessage, 0);
    std::stringstream s;
    s << "A Java Exception was thrown:" << std::endl << errMsg;
    env->ReleaseStringUTFChars(errorMessage, errMsg);
    std::string err("");
    err += s.str();
    env->ExceptionClear();
    Item lQName = theFactory->createQName(EXI_MODULE_NAMESPACE,
        "JAVA-EXCEPTION");
    env->DeleteLocalRef(errorMessage);
    CHECK_EXCEPTION(env);
    env->DeleteLocalRef(printWriter);
    CHECK_EXCEPTION(env);
    env->DeleteLocalRef(stringWriter);
    CHECK_EXCEPTION(env);
    env->DeleteLocalRef(throwableClass);
    CHECK_EXCEPTION(env);
    env->DeleteLocalRef(printWriterClass);
    CHECK_EXCEPTION(env);
    env->DeleteLocalRef(stringWriterClass);
    CHECK_EXCEPTION(env);
    throw USER_EXCEPTION(lQName, err);
  }
  return ItemSequence_t(new EmptySequence());
}

}}; // namespace zorba, exi

#ifdef WIN32
#  define DLL_EXPORT __declspec(dllexport)
#else
#  define DLL_EXPORT __attribute__ ((visibility("default")))
#endif

extern "C" DLL_EXPORT zorba::ExternalModule* createModule() {
  return new zorba::exi::EXIModule();
}