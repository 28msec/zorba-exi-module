package com.zorbaxquery.exi;

//import java.io.File;
//import java.io.FileInputStream;
//import java.io.FileOutputStream;
import java.io.ByteArrayInputStream;
import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.io.StringBufferInputStream;

import javax.xml.transform.Transformer;
import javax.xml.transform.TransformerException;
import javax.xml.transform.TransformerFactory;
import javax.xml.transform.sax.SAXSource;
import javax.xml.transform.stream.StreamResult;

import org.xml.sax.ContentHandler;
import org.xml.sax.InputSource;
import org.xml.sax.SAXException;
import org.xml.sax.XMLReader;
import org.xml.sax.helpers.XMLReaderFactory;

import com.siemens.ct.exi.EXIFactory;
import com.siemens.ct.exi.EncodingOptions;
import com.siemens.ct.exi.FidelityOptions;
import com.siemens.ct.exi.CodingMode;
import com.siemens.ct.exi.GrammarFactory;
import com.siemens.ct.exi.api.sax.EXIResult;
import com.siemens.ct.exi.api.sax.EXISource;
import com.siemens.ct.exi.exceptions.UnsupportedOption;
import com.siemens.ct.exi.grammar.Grammar;
import com.siemens.ct.exi.helpers.DefaultEXIFactory;

public final class exificient_stub {

	private static void set_options(EXIFactory exiFactory, exificient_options options) throws UnsupportedOption
	{
		if(options == null)
			return;
		EncodingOptions	encoding_options = EncodingOptions.createDefault();
		if(options.include_cookie)
			encoding_options.setOption(EncodingOptions.INCLUDE_COOKIE);
		if(options.include_insignificant_xsi_nil)
			encoding_options.setOption(EncodingOptions.INCLUDE_INSIGNIFICANT_XSI_NIL);
		if(options.include_insignificant_xsi_type)
			encoding_options.setOption(EncodingOptions.INCLUDE_INSIGNIFICANT_XSI_TYPE);
		if(options.include_options)
			encoding_options.setOption(EncodingOptions.INCLUDE_OPTIONS);
		if(options.include_xsi_schemalocation)
			encoding_options.setOption(EncodingOptions.INCLUDE_XSI_SCHEMALOCATION);
		if(options.preserve_entity_references)
			encoding_options.setOption(EncodingOptions.RETAIN_ENTITY_REFERENCE);
		if(options.schema_location != null && !options.schema_location.isEmpty())
			encoding_options.setOption(EncodingOptions.INCLUDE_SCHEMA_ID);
		
		exiFactory.setEncodingOptions(encoding_options);

		FidelityOptions fidelity_options = FidelityOptions.createDefault();
		fidelity_options.setFidelity(FidelityOptions.FEATURE_COMMENT ,options.preserve_comments);
		fidelity_options.setFidelity(FidelityOptions.FEATURE_DTD ,options.preserve_dtds);
		fidelity_options.setFidelity(FidelityOptions.FEATURE_LEXICAL_VALUE ,options.preserve_lexical_values);
		fidelity_options.setFidelity(FidelityOptions.FEATURE_PI ,options.preserve_pis);
		fidelity_options.setFidelity(FidelityOptions.FEATURE_PREFIX ,options.preserve_prefixes);
		fidelity_options.setFidelity(FidelityOptions.FEATURE_SC ,options.selfContained != null && options.selfContained.length > 0);
		fidelity_options.setFidelity(FidelityOptions.FEATURE_STRICT ,options.strict_schema);
		exiFactory.setFidelityOptions(fidelity_options);
		
		if(options.coding == 1)
			exiFactory.setCodingMode(CodingMode.BYTE_PACKED);
		if(options.coding == 2)
			exiFactory.setCodingMode(CodingMode.COMPRESSION);
		if(options.coding == 3)
			exiFactory.setCodingMode(CodingMode.PRE_COMPRESSION);
		
		if(options.selfContained != null && options.selfContained.length > 0)
			exiFactory.setSelfContainedElements(options.selfContained);
		
		if(options.blockSize > 0)
			exiFactory.setBlockSize(options.blockSize);
		if(options.valueMaxLength > 0)
			exiFactory.setValueMaxLength(options.valueMaxLength);
		if(options.valuePartitionCapacity > 0)
			exiFactory.setValuePartitionCapacity(options.valuePartitionCapacity);
		
		if(options.dtrMapTypes != null && options.dtrMapTypes.length > 0)
			exiFactory.setDatatypeRepresentationMap(options.dtrMapTypes, options.dtrMapRepresentations);
		
		if(options.is_fragment)
			exiFactory.setFragment(true);
	}
	
	public static byte[] encodeSchemaInformed(String xmlstr, 
											  exificient_options options
											  ) throws Exception {

		EXIFactory exiFactory = DefaultEXIFactory.newInstance();
		set_options(exiFactory, options);
		EXIResult exiResult;
		// create default factory and EXI grammar for schema
		if(options != null && options.schema_location != null && !options.schema_location.isEmpty()){
			GrammarFactory grammarFactory = GrammarFactory.newInstance();
			Grammar g = grammarFactory.createGrammar(options.schema_location);
			exiFactory.setGrammar(g);
		}
		exiResult = new EXIResult(exiFactory);

		ByteArrayOutputStream exiOS = new ByteArrayOutputStream();
		exiResult.setOutputStream(exiOS);
		XMLReader xmlReader = XMLReaderFactory.createXMLReader();
		xmlReader.setContentHandler(exiResult.getHandler());

		// parse xml file
		InputStream xmlIS = new StringBufferInputStream(xmlstr);
		xmlReader.parse(new InputSource(xmlIS));
		return exiOS.toByteArray();
	}

	public static String decodeSchemaInformed(byte exibin[], 
											  exificient_options options
											  ) throws Exception {

		EXIFactory exiFactory = DefaultEXIFactory.newInstance();
		set_options(exiFactory, options);
		EXISource saxSource;
		if(options != null && options.schema_location != null && !options.schema_location.isEmpty()){
			// create default factory and EXI grammar for schema
			GrammarFactory grammarFactory = GrammarFactory.newInstance();
			Grammar g = grammarFactory.createGrammar(options.schema_location);
			exiFactory.setGrammar(g);
		}
		saxSource = new EXISource(exiFactory);

		XMLReader exiReader = saxSource.getXMLReader();
		TransformerFactory tf = TransformerFactory.newInstance();
		Transformer transformer = tf.newTransformer();

		InputStream exiIS = new ByteArrayInputStream(exibin);
		SAXSource exiSource = new SAXSource(new InputSource(exiIS));
		exiSource.setXMLReader(exiReader);

		ByteArrayOutputStream os = new ByteArrayOutputStream();
		transformer.transform(exiSource, new StreamResult(os));
		if(exiFactory.isFragment())
			return "fragment"+os.toString("UTF8");
		else
			return os.toString("UTF8");
	}

}
