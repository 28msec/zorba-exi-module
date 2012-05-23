import schema namespace exi-options="http://www.zorba-xquery.com/modules/converters/exi-options";
import module namespace exi = "http://www.zorba-xquery.com/modules/converters/exi";
import module namespace file = "http://expath.org/ns/file";

declare namespace exi-header = "http://www.w3.org/2009/exi";

let $options:=
<exi-options:options>
  <exi-header:header>
	  <exi-header:common>
        <exi-header:schemaId>D:/zorba/code/zorba_modules/exi-module/test_exi/Queries/converters/exi/movesinstitute.org/Miscellaneous/factbook.xsd</exi-header:schemaId>
      </exi-header:common>
	  <exi-header:strict/>
  </exi-header:header>
</exi-options:options>
let $exi_res := exi:serialize(fn:doc("D:/zorba/code/zorba_modules/exi-module/test_exi/Queries/converters/exi/movesinstitute.org/Miscellaneous/factbook.xml"), $options)
(: let $fret1 := file:write-binary("factbook-test.exi", $exi_res) :)
let $exi_exp := file:read-binary("D:/zorba/code/zorba_modules/exi-module/test_exi/Queries/converters/exi/factbook-schemaid-strict.exi")
return if ($exi_res = $exi_exp) then "success" else "failure"