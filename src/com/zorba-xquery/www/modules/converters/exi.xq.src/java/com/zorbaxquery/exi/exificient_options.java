package com.zorbaxquery.exi;

public class exificient_options {
	public boolean include_cookie = false;
	public boolean include_insignificant_xsi_nil = false;
	public boolean include_insignificant_xsi_type = false;
	public boolean include_options = false;
	public boolean include_xsi_schemalocation = false;
	
	public boolean preserve_entity_references = false;
	public boolean preserve_comments = false;
	public boolean preserve_dtds = false;
	public boolean preserve_lexical_values = false;
	public boolean preserve_pis = false;
	public boolean preserve_prefixes = false;
	
	public boolean strict_schema = false;
	public String  schema_location = "";
	public String  schema_content = "";
	
	public javax.xml.namespace.QName[] selfContained;//also sets SELF_CONTAINED in fidelity options
	
	public int blockSize = -1;
	public int valueMaxLength = -1;
	public int valuePartitionCapacity = -1;
	
	public int coding = 0;//alignment: 0=bit packed; 1=byte-aligned; 2=compression; 3=pre-compression
	
	public javax.xml.namespace.QName[] dtrMapTypes;//DataTypeRepresentationMap
	public javax.xml.namespace.QName[] dtrMapRepresentations;
	
	public boolean is_fragment = false;
}
