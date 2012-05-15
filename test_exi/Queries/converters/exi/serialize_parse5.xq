import schema namespace exi-options="http://www.zorba-xquery.com/modules/converters/exi-options";

import module namespace exi = "http://www.zorba-xquery.com/modules/converters/exi";

declare namespace exi-header = "http://www.w3.org/2009/exi";

let $options-serialize:=
                    <exi-options:options>
                      <exi-header:header>
                        <exi-header:lesscommon>
                          <exi-header:preserve>
                            <exi-header:prefixes/>
                          </exi-header:preserve>
                        </exi-header:lesscommon>
                        <exi-header:common>
                          <exi-header:fragment/>
                        </exi-header:common>
                      </exi-header:header>
                      <exi-options:cookie/>
                      <exi-options:add-options/>
                    </exi-options:options>
let $options-parse:=
            <exi-options:options>
            </exi-options:options>
let $doc:=(<elem1/>, 
           <elem1>aaa<elem2/>bbb</elem1>,
           <elem3 attr1="value1"/>
           )

return
exi:parse(exi:serialize($doc, $options-serialize), $options-serialize)