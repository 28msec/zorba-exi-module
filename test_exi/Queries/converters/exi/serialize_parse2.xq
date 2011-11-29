import schema namespace exi-options="http://www.zorba-xquery.com/modules/converters/exi-options";

import module namespace exi = "http://www.zorba-xquery.com/modules/converters/exi";

declare namespace exi-header = "http://www.w3.org/2009/exi";

let $options:=
<exi-options:options>
  <exi-header:header>
    <exi-header:lesscommon>
      <exi-header:uncommon>
        <ns2:other-tag xmlns:ns2="http://uri1"/>
      </exi-header:uncommon>
    </exi-header:lesscommon>
  </exi-header:header>
</exi-options:options>
return
exi:parse(exi:serialize(fn:doc("test.xml"), $options), $options)