import schema namespace exi-options="http://www.zorba-xquery.com/modules/converters/exi-options";
import module namespace exi = "http://www.zorba-xquery.com/modules/converters/exi";
import module namespace file="http://expath.org/ns/file";
import module namespace doc="http://www.zorba-xquery.com/modules/store/dynamic/documents";

declare namespace exi-header = "http://www.w3.org/2009/exi";
declare namespace ann = "http://www.zorba-xquery.com/annotations";
declare namespace err="http://www.w3.org/2005/xqt-errors";

declare %ann:assignable variable $last-xml := "";

declare function local:get-schema-file-for-exi($exiname as xs:string) as xs:string
{
  file:path-to-native(fn:replace($exiname, "\d*\.[^\.]*\.[^\.]*$", ".xsd"))
};

declare %ann:sequential function local:get-xml-for-exi($exiname as xs:string) as xs:string
{
  variable $new-xml := fn:trace(fn:replace($exiname, "(\.[^_]*)_[^\.]*\.exi$", "$1"), "xml name");
  if(fn:not($last-xml eq "") and fn:not($last-xml eq $new-xml)) then
  {
    doc:remove(fn:resolve-uri($last-xml));
    $last-xml := $new-xml;
  }
  else ();
  $new-xml
};

declare function local:get-options-from-filename($exiname as xs:string) as element(exi-options:options)
{
  <exi-options:options>
    <exi-header:header>
      <exi-header:lesscommon>
        <exi-header:uncommon>
        {
          if(fn:matches($exiname, "_BYTEALIGN")) then
            <exi-header:alignment><exi-header:byte/></exi-header:alignment>
          else
            if(fn:matches($exiname, "_PRECOMPRESSION")) then
              <exi-header:alignment><exi-header:pre-compress/></exi-header:alignment>
            else
              ()
        }
        </exi-header:uncommon>
        {if(fn:matches($exiname, "_LEXICAL")) then
          <exi-header:preserve><exi-header:lexicalValues/></exi-header:preserve>
         else ()
        }
      </exi-header:lesscommon>
      <exi-header:common>
      {(
        if(fn:matches($exiname, "_COMPRESSED")) then
          <exi-header:compression/>
        else (),
        if(fn:matches($exiname, "_FRAGMENT")) then
          <exi-header:fragment/>
        else (),
        if(fn:matches($exiname, "_STRICT") or fn:matches($exiname, "_SCHEMA") or 
           fn:matches($exiname, "_DEFAULT") or fn:matches($exiname, "_PRECOMPRESSION")) then
           <exi-header:schemaId>{local:get-schema-file-for-exi($exiname)}</exi-header:schemaId>
        else ()
      )}
      </exi-header:common>
      {
        if(fn:matches($exiname, "_STRICT") or fn:matches($exiname, "_PRECOMPRESSION")) then
          <exi-header:strict/>
        else ()
      }
    </exi-header:header>
    {
      if(fn:matches($exiname, "_HEADER")) then 
        <exi-options:add-options/>
      else ()
    (:
    if(fn:matches($exiname, "_STRICT")) then (<schema>{local:get-schema-file-for-exi($exiname)}</schema>,<strict/>) else (),
    if(fn:matches($exiname, "_SCHEMA")) then <schema>{local:get-schema-file-for-exi($exiname)}</schema> else (),
    if(fn:matches($exiname, "_DEFAULT")) then <schema>{local:get-schema-file-for-exi($exiname)}</schema> else (),
    if(fn:matches($exiname, "_HEADER")) then <header/> else (),
    if(fn:matches($exiname, "_BYTEALIGNED")) then <bytealigned/> else (),
    if(fn:matches($exiname, "_COMPRESSED")) then <compress/> else (),
    if(fn:matches($exiname, "_PRECOMPRESSION")) then (<schema>{local:get-schema-file-for-exi($exiname)}</schema>,<strict/>,<precompress/>) else (),
    if(fn:matches($exiname, "_LEXICAL")) then <stringvalues/> else (),
    if(fn:matches($exiname, "_FRAGMENT")) then <fragment/> else ()
    :)
    }
    <exi-options:add-insignificant-xsi-nil/>
  </exi-options:options>
};

declare function local:concat-xml-text($texts as xs:string*) as xs:string
{
  fn:string-join(
     for $t in $texts
     return fn:normalize-space($t))
};

declare %ann:sequential function local:compare-xml( $elem1 as node(), $elem2 as node() ) as xs:boolean
{
  (: fn:trace(fn:name($elem1), "elem1"); :)
  (: fn:trace(fn:name($elem2), "elem2"); :)
  if(fn:not(fn:name($elem1) = fn:name($elem2))) then
    exit returning fn:false();
  else
    ();

  let $childs2 := $elem2/*
  let $childs2_count := fn:count($childs2)
  for $child1 at $i in $elem1/*
  return
    if(($childs2_count lt $i) or 
      fn:not(local:compare-xml($child1, $childs2[$i]))) then 
      exit returning fn:false();
    else ();

  if(fn:not(fn:deep-equal($elem1/@*, $elem2/@*))) then
    exit returning fn:false();
  else 
    ();
  (: fn:trace("attrs ok", "compare attrs"); :)
  if(fn:not(local:concat-xml-text($elem1/text()) = local:concat-xml-text($elem2/text()))) then
    fn:false()
  else 
    fn:true()
};

declare function local:compare-exi-without-version($exi1 as xs:base64Binary, $exi2 as xs:base64Binary) as xs:boolean
{
  let $strexi1 := fn:string($exi1)
  let $strexi2 := fn:string($exi2)
  return
    fn:substring($strexi1, 3) = fn:substring($strexi2, 3)
};

declare function local:display-file-name($fname as xs:string?) as xs:string?
{
  fn:replace(fn:replace($fname, ".*(test_exi.*)$", "$1"), "\\", "/")
};

(:
  Check encoding and decoding for this exi file.
  Return the result <check-exi file="file name" encoding="ok/fail" decoding="ok/fail"/>
:)
declare %ann:sequential function local:check-exi($exiname as xs:string) as element()
{
  let $options := local:get-options-from-filename($exiname)
  let $xmlname := local:get-xml-for-exi($exiname)
  let $exi := file:read-binary($exiname)
  return
    <check-exi>
     <file>{local:display-file-name($exiname)}</file>
     <xml>{local:display-file-name($xmlname)}</xml>
     <schema>{local:display-file-name(fn:string($options/exi-header:header/exi-header:common/exi-header:schemaId))}</schema>
     <encoding>{
       try{
         let $xml := fn:doc($xmlname)
         let $encoded_xml := exi:serialize($xml, $options)
         return
         fn:trace(if(local:compare-exi-without-version($exi, $encoded_xml)) then "ok" else "fail", "encoding")
        }catch *
        {
          (<err-code>{$err:code}</err-code>,
          <err-description>{fn:trace($err:description, "error encoding")}</err-description>,
          <err-value>{$err:value}</err-value>,
          <err-module>{$err:module}</err-module>,
          <err-line>{$err:line-number}</err-line>)
        }
     }</encoding>
     <decoding>{
       try{
         let $xml := fn:doc($xmlname)
         let $decoded_exi := exi:parse($exi, $options)
         return
         fn:trace(if(local:compare-xml($decoded_exi[1], $xml/element())) then "ok" else "fail", "decoding")
        }catch *
        {
          (<err-code>{$err:code}</err-code>,
          <err-description>{fn:trace($err:description, "error decoding")}</err-description>,
          <err-value>{$err:value}</err-value>,
          <err-module>{$err:module}</err-module>,
          <err-line>{$err:line-number}</err-line>)
        }
     }</decoding>
    </check-exi>
};




let $current-dir := fn:resolve-uri("./")
for $f in file:list($current-dir, fn:true(), "*.exi")
order by $f
return
 local:check-exi(fn:trace(fn:concat($current-dir, $f), "exi file"))


(:
local:check-exi("E:\xquery_development\zorba_repo\z_m2\conv\test_exi\Queries\converters\exi\movesinstitute.org\notebook\notebook.xml_DEFAULT.exi")
:)

(:
let $exiname := "E:\xquery_development\zorba_repo\z_m2\conv\test_exi\Queries/converters/exi/movesinstitute.org/notebook\notebook.xml_STRICT_COMPRESSED.exi"
let $options := local:get-options-from-filename($exiname)
let $exi := file:read-binary($exiname)
let $decoded_exi := exi:parse($exi, $options)
return $decoded_exi
:)

(:
let $exiname := "E:\xquery_development\zorba_repo\z_m2\conv\test_exi\Queries/converters/exi/movesinstitute.org/notebook\notebook.xml_DEFAULT_HEADER.exi"
let $options := local:get-options-from-filename($exiname)
let $xmlname := local:get-xml-for-exi($exiname)
let $exi := file:read-binary($exiname)
let $xml := fn:doc($xmlname)
let $encoded_xml := exi:serialize($xml, $options)
return (fn:trace($exi, "orig exi"), 
        fn:trace(xs:hexBinary($exi), "orig exi hex"),
       fn:trace($encoded_xml, "encoded xml"),
       fn:trace(xs:hexBinary($encoded_xml), "encoded xml hex"))
:)