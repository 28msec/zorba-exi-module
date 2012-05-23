xquery version "3.0";

(:
 : Copyright 2006-2011 The FLWOR Foundation.
 :
 : Licensed under the Apache License, Version 2.0 (the "License");
 : you may not use this file except in compliance with the License.
 : You may obtain a copy of the License at
 :
 : http://www.apache.org/licenses/LICENSE-2.0
 :
 : Unless required by applicable law or agreed to in writing, software
 : distributed under the License is distributed on an "AS IS" BASIS,
 : WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 : See the License for the specific language governing permissions and
 : limitations under the License.
:)

(:~
 : Function library providing converters from XML to EXI (binary XML) and back.
 : Implemented using EXIficient, a GPL licensed library written in Java. 
 : The Exificient library is loaded at first call of any function parse or serialize.
 :
 : @author Daniel Turcanu
 : @project data processing/data converters
 :)
module namespace exi = "http://www.zorba-xquery.com/modules/converters/exi";

(:~
 : Import schema module for checking if exi options element is validated.
 :)
import module namespace schemaOptions = "http://www.zorba-xquery.com/modules/schema";

(:~
 : Contains the definitions of the exi options element.
 :)
import schema namespace exi-options = "http://www.zorba-xquery.com/modules/converters/exi-options";

declare namespace ver = "http://www.zorba-xquery.com/options/versioning";
declare option ver:module-version "1.0";

(:~
 : Parse a EXI binary and convert to XML document or element nodes if it deals with xml fragment.<br/>
 : @param $exi the base64 binary containing the exi.
 : @param $options this parameter is validated against "http://www.zorba-xquery.com/modules/converters/exi-options" 
 :      and "http://www.w3.org/2009/exi" schemas. <br/>
 :      Possible values of options are:<br/><br/>
 :      &lt;exi-options:options xmlns:exi-options="http://www.zorba-xquery.com/modules/converters/exi-options"> <br/>
 :        &lt;exi-header:header xmlns:exi-header="http://www.w3.org/2009/exi"> <br/>
 :          &lt;!-- ... header options defined in EXI spec, see http://www.w3.org/TR/2011/REC-exi-20110310/#options 
 :            and http://www.w3.org/TR/2011/REC-exi-20110310/#optionsSchema --> <br/>
 :        &lt;/exi-header:header> <br/>
 :      &lt;/exi-options:options> <br/> <br/>
 :      If EXI has the options in the header, then you don't need to specify the options again, except maybe for the schema location.
 :      Schema location is specified in the options at position exi-options:options/exi-header:header/exi-header:common/exi-header:schemaId .
 : @return a document node, or a sequence of elements if the exi is a fragment xml
 : @error exi:JAR-NOT-FOUND if exificient_stub.jar is not found
 : @error exi:VM001 if java VM could not be started, probably because jar files are missing, 
 :        or possibly because of not enough memory (JVM is started with a maximum heap set to 700 MB)
 : @error exi:JAVA-EXCEPTION java error raised from the jars
 : @error exi:XQDY0027 if $options can not be validated against the exi-options schema
 : @error err:XQDY0084 if the options parameter doesn't have the name "exi-options:options".
 : @example test_exi/Queries/converters/exi/serialize_parse3.xq
:)
declare function exi:parse($exi as xs:base64Binary,
                           $options as element(exi-options:options)?) as node()*
{
  let $validated-options :=
    if(empty($options)) then
      $options
    else if(schemaOptions:is-validated($options)) then
      $options
    else
      validate{$options}
  return
    exi:parse-internal($exi, $validated-options)
};
                                 
declare %private function exi:parse-internal($exi as xs:base64Binary,
                                 $options as element(exi-options:options, exi-options:optionsType)?) as node()+ external;
                                 
(:~
 : Convert XML into binary EXI.
 :
 :
 : @param $xml a document or a sequence of elements if they will be saved as xml fragment.
 :    If saving a fragment, then you have to specify this in the options at position exi-options:options/exi-header:header/exi-header:common/exi-header:fragment .
 : @param $options this parameter is validated against "http://www.zorba-xquery.com/modules/converters/exi-options" 
 :      and "http://www.w3.org/2009/exi" schemas. <br/>
 :      Possible values of options are:<br/><br/>
 :      &lt;exi-options:options xmlns:exi-options="http://www.zorba-xquery.com/modules/converters/exi-options"> <br/>
 :        &lt;exi-header:header xmlns:exi-header="http://www.w3.org/2009/exi"> <br/>
 :          &lt;!-- ... header options defined in EXI spec, see http://www.w3.org/TR/2011/REC-exi-20110310/#options 
 :            and http://www.w3.org/TR/2011/REC-exi-20110310/#optionsSchema --> <br/>
 :        &lt;/exi-header:header> <br/>
 :        &lt;exi-options:cookie/> &lt;!-- to start the EXI with the $EXI cookie--> <br/>
 :        &lt;exi-options:add-options/>  &lt;!-- to save the options in the EXI header--> <br/>
 :        &lt;exi-options:add-insignificant-xsi-nil/>  &lt;!-- EXIficient specific option. See EncodingOptions in EXIficient doc--> <br/>
 :        &lt;exi-options:add-insignificant-xsi-type/>  &lt;!-- EXIficient specific option. See EncodingOptions in EXIficient doc--> <br/>
 :        &lt;exi-options:add-xsi-schemalocation/>  &lt;!-- EXIficient specific option. See EncodingOptions in EXIficient doc--> <br/>
 :        &lt;exi-options:preserve-entity-references/>  &lt;!-- EXIficient specific option. See EncodingOptions in EXIficient doc--> <br/>
 :      &lt;/exi-options:options> <br/> <br/>
 :      Schema location is specified in the options at position exi-options:options/exi-header:header/exi-header:common/exi-header:schemaId .
 : @return binary EXI encoded base64
 : @error exi:JAR-NOT-FOUND if exificient_stub.jar is not found
 : @error exi:VM001 if java VM could not be started, probably because jar files are missing, 
 :        or possibly because of not enough memory (JVM is started with a maximum heap set to 700 MB)
 : @error exi:JAVA-EXCEPTION java error raised from the jars
 : @error exi:XQDY0027 if $options can not be validated against the exi-options schema
 : @error err:XQDY0084 if the options parameter doesn't have the name "exi-options:options".
 : @example test_exi/Queries/converters/exi/serialize_parse4.xq
:)
declare function exi:serialize($xml as node()+,
  $options as element(exi-options:options)?) as xs:base64Binary
{
  let $validated-options :=
    if(empty($options)) then
      $options
    else if(schemaOptions:is-validated($options)) then
      $options
    else
      validate{$options}
  return
    exi:serialize-internal($xml, $validated-options)
};

declare %private function exi:serialize-internal($xml as node()+,
  $options as element(exi-options:options, exi-options:optionsType)?) as xs:base64Binary external;
