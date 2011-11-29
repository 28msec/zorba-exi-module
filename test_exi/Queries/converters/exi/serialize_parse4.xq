import schema namespace exi-options="http://www.zorba-xquery.com/modules/converters/exi-options";

import module namespace exi = "http://www.zorba-xquery.com/modules/converters/exi";

declare namespace exi-header = "http://www.w3.org/2009/exi";

let $options-serialize:=
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
                      <exi-options:cookie/>
                      <exi-options:add-options/>
                    </exi-options:options>
let $options-parse:=
            <exi-options:options>
            </exi-options:options>
let $doc:=
          <elem1>
            <nr:elem2 xmlns:nr="urn:schemas-microsoft-com:rowset">
              text2<nr:a/>
              <nr:elem3 attr3="value3">
                <a attr1="value1"></a>
              </nr:elem3>
              <?php?>
            </nr:elem2>
            <![CDATA[long text]]>
            <!-- second comment-->
          </elem1>

return
exi:parse(exi:serialize($doc, $options-serialize), $options-parse)