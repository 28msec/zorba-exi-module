xquery version "3.0";

(:
 : Copyright 2006-2009 The FLWOR Foundation.
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
 : Parse a EXI binary and convert to XML document.<br/>
 : @param $csv the string containing the csv or fixed size text.
 : @param $options this parameter is validated against "http://www.zorba-xquery.com/modules/converters/csv-options" schema. 
 :    If this parameter is not specified, the row name is by default "row" and the column name is by default "column". 
 : @return a sequence of row elements, one for each line in csv
 : @error csv:CSV001 if the input string is streamable string and cannot be rewinded
 : @error csv:WrongInput if the input string has lines with variable number of items, and the csv has headers and
 :         the options do not specify the ignore-foreign-input attribute
 : @error err:XQDY0027 if $options can not be validated against the csv-options schema
 : @error err:XQDY0084 if the options parameter doesn't have the name "csv-options:options".
 : @example test/Queries/converters/csv/csv_parse1.xq
 : @example test/Queries/converters/csv/csv_parse2.xq
 : @example test/Queries/converters/csv/csv_parse3.xq
 : @example test/Queries/converters/csv/csv_parse6.xq
 : @example test/Queries/converters/csv/csv_parse11.xq
 : @example test/Queries/converters/csv/csv_parse_utf8_11.xq
 : @example test/Queries/converters/csv/txt_parse5.xq
 : @example test/Queries/converters/csv/txt_parse8.xq
:)
declare function exi:parse($exi as xs:base64Binary,
                           $options as element(exi-options:options)?) as node()+
{
  let $validated-options :=
  if(empty($options)) then
    $options
  else
  if(schemaOptions:is-validated($options)) then
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
 : @param $xml a sequence of elements, each element representing a row. The name of each row element is ignored.
 :     The childs of each row are the column fields.
 : @param $options The options parameter. See the function description for details. 
 : This parameter is validated against "http://www.zorba-xquery.com/modules/converters/exi-options" schema.
 : @return the csv or fixed size text as string containing all the lines
 : @error csv:CSV003 if the serialize output is streamable string and cannot be reset
 : @error csv:ForeignInput if there are input elements in subsequent rows that do not match the headers,
 :    and the options specify first-row-is-header and do not specify the ignore-foreign-input attribute
 : @error err:XQDY0027 if $options can not be validated against csv-options schema
 : @error err:XQDY0084 if the options parameter doesn't have the name "csv-options:options".
 : @example test/Queries/converters/csv/csv_serialize1.xq
 : @example test/Queries/converters/csv/csv_serialize2.xq
 : @example test/Queries/converters/csv/csv_serialize3.xq
 : @example test/Queries/converters/csv/csv_serialize5.xq
 : @example test/Queries/converters/csv/csv_serialize6.xq
 : @example test/Queries/converters/csv/csv_parse_serialize6.xq
 : @example test/Queries/converters/csv/txt_serialize6.xq
 : @example test/Queries/converters/csv/txt_parse_serialize6.xq
:)
declare function exi:serialize($xml as node()+,
									             $options as element(exi-options:options)?) as xs:base64Binary
{
  let $validated-options :=
  if(empty($options)) then
    $options
  else
  if(schemaOptions:is-validated($options)) then
    $options
  else
    validate{$options}
  return
    exi:serialize-internal($xml, $validated-options)
};
																		
declare %private function exi:serialize-internal($xml as node()+,
									$options as element(exi-options:options, exi-options:optionsType)?) as xs:base64Binary external;
