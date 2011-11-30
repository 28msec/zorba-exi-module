import schema namespace exi-options="http://www.zorba-xquery.com/modules/converters/exi-options";

import module namespace exi = "http://www.zorba-xquery.com/modules/converters/exi";

declare namespace exi-header = "http://www.w3.org/2009/exi";

declare variable $input-doc external;

let $options:=
<exi-options:options>
  <exi-header:header>
    <exi-header:lesscommon>
      <exi-header:preserve>
        <exi-header:dtd/>
        <exi-header:prefixes/>
        <exi-header:lexicalValues/>
        <exi-header:comments/>
        <exi-header:pis/>
      </exi-header:preserve>
    </exi-header:lesscommon>
  </exi-header:header>
</exi-options:options>
return
exi:parse(exi:serialize($input-doc, $options), $options)