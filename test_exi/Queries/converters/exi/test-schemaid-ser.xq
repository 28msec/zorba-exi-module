import schema namespace exi-options="http://www.zorba-xquery.com/modules/converters/exi-options";

import module namespace exi = "http://www.zorba-xquery.com/modules/converters/exi";
import module namespace file = "http://expath.org/ns/file";

declare namespace exi-header = "http://www.w3.org/2009/exi";

let $options:=
<exi-options:options>
  <exi-header:header>
    <exi-header:common>
      <exi-header:schemaId>{file:path-to-native(fn:resolve-uri("movesinstitute.org/notebook/notebook.xsd"))}</exi-header:schemaId>
    </exi-header:common>
  </exi-header:header>
</exi-options:options>
return
exi:serialize(fn:doc("./sample-data/notebook.xml"), $options)