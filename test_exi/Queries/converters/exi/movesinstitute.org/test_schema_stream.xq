import schema namespace exi-options="http://www.zorba-xquery.com/modules/converters/exi-options";
import module namespace exi = "http://www.zorba-xquery.com/modules/converters/exi";
import module namespace file="http://expath.org/ns/file";
import module namespace doc="http://www.zorba-xquery.com/modules/store/dynamic/documents";

declare namespace exi-header = "http://www.w3.org/2009/exi";
declare namespace ann = "http://www.zorba-xquery.com/annotations";
declare namespace err="http://www.w3.org/2005/xqt-errors";

declare %ann:assignable variable $last-xml := "";

declare function local:compare-exi-without-version($exi1 as xs:base64Binary, $exi2 as xs:base64Binary) as xs:boolean
{
  let $strexi1 := fn:string($exi1)
  let $strexi2 := fn:string($exi2)
  return
    fn:substring($strexi1, 3) = fn:substring($strexi2, 3)
};

let $exi_file := fn:resolve-uri("notebook/notebook.xml_DEFAULT.exi")
let $schema_file := fn:resolve-uri("notebook/notebook.xsd")
let $exi-content := file:read-binary($exi_file)
let $xml := fn:doc("notebook/notebook.xml")
let $schema-content := file:read-text($schema_file)
let $encoded-exi := exi:serialize($xml, 
                                <exi-options:options>
                                  <exi-options:schema-content>{$schema-content}</exi-options:schema-content>
                                </exi-options:options>
                                )
return
  local:compare-exi-without-version($encoded-exi, $exi-content)
