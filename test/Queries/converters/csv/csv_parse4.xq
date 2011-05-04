import schema namespace csv-options="http://www.zorba-xquery.com/modules/converters/csv-options";
import module namespace zorba-csv = "http://www.zorba-xquery.com/modules/converters/csv";
import module namespace file="http://expath.org/ns/file";

zorba-csv:parse(file:read-text(fn:resolve-uri("ME_1_2008_v08.txt")), 

validate{
<csv-options:options>
  <first-row-is-header/>
  <csv separator="&#009;"
       quote-char=""
       quote-escape=""/>
  <xml-nodes>
    <row/>
  </xml-nodes>
</csv-options:options>}
)
