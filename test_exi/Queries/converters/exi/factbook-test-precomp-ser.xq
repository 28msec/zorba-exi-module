import schema namespace exi-options="http://www.zorba-xquery.com/modules/converters/exi-options";
import module namespace exi = "http://www.zorba-xquery.com/modules/converters/exi";
import module namespace file = "http://expath.org/ns/file";

declare namespace exi-header = "http://www.w3.org/2009/exi";

let $currdir := fn:resolve-uri("./")
let $miscdir := fn:resolve-uri("./movesinstitute.org/Miscellaneous/")
let $options:=
  <exi-options:options>
    <exi-header:header>
      <exi-header:lesscommon>
        <exi-header:uncommon>
          <exi-header:alignment>
            <exi-header:pre-compress/>
          </exi-header:alignment>
        </exi-header:uncommon>
      </exi-header:lesscommon>
      <exi-header:common>
        <exi-header:schemaId>
          { file:path-to-native(fn:resolve-uri("factbook.xsd", $miscdir)) }
        </exi-header:schemaId>
      </exi-header:common>
    </exi-header:header>
  </exi-options:options>
let $factbook-xml := fn:resolve-uri("factbook.xml", $miscdir)
let $exi_res := exi:serialize(fn:doc($factbook-xml), $options)
let $foo := file:write-binary("foo.exi", $exi_res)
let $exi_exp := file:read-binary(fn:resolve-uri("factbook-precomp.exi", $currdir))
return
  if ($exi_res = $exi_exp) then "success" else "failure"