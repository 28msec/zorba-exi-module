import schema namespace exi-options="http://www.zorba-xquery.com/modules/converters/exi-options";
import module namespace exi = "http://www.zorba-xquery.com/modules/converters/exi";
import module namespace file="http://expath.org/ns/file";

declare namespace exi-header = "http://www.w3.org/2009/exi";
declare namespace ann = "http://www.zorba-xquery.com/annotations";

declare function local:get-schema-file-for-exi($exiname as xs:string) as xs:string
{
  fn:replace($exiname, "\d*\.[^\.]*\.[^\.]*$", ".xsd")
};

declare function local:get-xml-for-exi($exiname as xs:string) as xs:string
{
  fn:replace($exiname, "(\.[^.]*)_[^\.]*\.exi$", "$1")
};

declare function local:get-options-from-filename($exiname as xs:string) as element(exi-options:options)
{
  <exi-options:options>
    <exi-header:header>
      <exi-header:lesscommon>
        <exi-header:uncommon>
        {
          if(fn:matches($exiname, "_BYTEALIGNED")) then
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
           <exi-header:schemaId>{fn:trace(local:get-schema-file-for-exi($exiname), "schema file")}</exi-header:schemaId>
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
  fn:trace(fn:name($elem1), "elem1");
  fn:trace(fn:name($elem2), "elem2");
  if(fn:not(fn:name($elem1) = fn:name($elem2))) then
    exit returning fn:false();
  else
    ();

  let $childs2 := $elem2/*
  for $child1 at $i in $elem1/*
  return
    if(fn:not(local:compare-xml($child1, $childs2[$i]))) then 
      exit returning fn:false();
    else ();

  if(fn:not(fn:deep-equal($elem1/@*, $elem2/@*))) then
    exit returning fn:false();
  else 
    ();

  if(fn:not(fn:trace(local:concat-xml-text($elem1/text()), "elem1 text") = fn:trace(local:concat-xml-text($elem2/text()), "elem2 text"))) then
    fn:false()
  else 
    fn:true()
};

(:
  Check encoding and decoding for this exi file.
  Return the result <check-exi file="file name" encoding="ok/fail" decoding="ok/fail"/>
:)
declare %ann:nondeterministic function local:check-exi($exiname as xs:string) as element()
{
  let $options := fn:trace(local:get-options-from-filename($exiname), "options")
  let $xml := fn:doc(fn:trace(local:get-xml-for-exi($exiname), "xml file:"))
  let $encoded_xml := exi:serialize($xml, $options)
  let $exi := file:read-binary($exiname)
  let $decoded_exi := exi:parse($exi, $options)
  return
    <check-exi file="{$exiname}" encoding="{if($exi eq $encoded_xml) then "ok" else "fail"}"
                                 decoding="{if(local:compare-xml($decoded_exi, $xml)) then "ok" else "fail"}"
    >
    <orig-xml>{$xml}</orig-xml>
    <decoded-xml>{$decoded_exi}</decoded-xml>
    </check-exi>
};


(:
for $f in file:list(fn:resolve-uri(""), fn:true(), "*.exi")
return
 local:check-exi($f)
:)

local:check-exi("E:\xquery_development\zorba_repo\z_m2\conv\test_exi\Queries\converters\exi\movesinstitute.org\notebook\notebook.xml_DEFAULT.exi")
