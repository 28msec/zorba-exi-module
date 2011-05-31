import module namespace json = "http://www.zorba-xquery.com/modules/converters/json";
import schema namespace html-options="http://www.zorba-xquery.com/modules/converters/json-options";

json:parse('[{
     "firstName": "John",
     "lastName": "Smith",
     "address": {
         "streetAddress": "21 2nd Street",
         "city": "New York",
         "state": "NY",
         "postalCode": 10021
     },
     "phoneNumbers": [
         "212 732-1234",
         "646 123-4567"
     ]
 },
{
   "firstName": "John",
   "state": null,
   "bool": true ,
   "numbers": [1,2,3] ,
   "address": {
       "streetAddress": "21 2nd Street",
       "state": null,
       "postalCode": 10021 ,
       "literals": [true,false,null],
       "delivery": {
                "streetAddress": "StreetName",
                "city": "CityName",
                "state": "StateName"
        }} ,
   "strings": [
       "one",
       "two",
       "three",
       "four" ]
}]',<options xmlns="http://www.zorba-xquery.com/modules/converters/json-options" >
              <jsonParam name="mapping" value="simple-json" />
            </options>)