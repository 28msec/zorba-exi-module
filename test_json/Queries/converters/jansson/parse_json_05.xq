import module namespace json = "http://www.zorba-xquery.com/modules/converters/json";
import schema namespace html-options="http://www.zorba-xquery.com/modules/converters/json-options";

json:parse(('[ "ul",
                  [ "li",
                    true],
                  [ "li",
                    {"href":"driving.html", "title":"Driving"},
                    "Second item"],
                  [ "li", null],
                  [ "li", -14e12]
'),<options xmlns="http://www.zorba-xquery.com/modules/converters/json-options" >
              <jsonParam name="mapping" value="simple-json" />
            </options>)