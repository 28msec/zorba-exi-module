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

import javax.xml.stream.XMLInputFactory;
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
import com.siemens.ct.exi.exceptions.EXIException;
import com.siemens.ct.exi.exceptions.UnsupportedOption;
import com.siemens.ct.exi.grammar.Grammar;
import com.siemens.ct.exi.helpers.DefaultEXIFactory;
import com.siemens.ct.exi.api.stream.StAXEncoder;
import javax.xml.stream.XMLEventReader;

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
	
	private static Grammar create_grammar(exificient_options options) throws EXIException
	{
		if(options == null)
			return null;
		GrammarFactory grammarFactory = GrammarFactory.newInstance();
		if(options.schema_content != null && !options.schema_content.isEmpty())
		{
			InputStream schemaContentIS = new StringBufferInputStream(options.schema_content);
			return grammarFactory.createGrammar(schemaContentIS);
		}
		else if(options.schema_location != null && !options.schema_location.isEmpty())
			return grammarFactory.createGrammar(options.schema_location);
		else
			return null;
	}
	
	public static byte[] encodeSchemaInformed(String[] xmlstr, 
											  exificient_options options
											  ) throws Exception {

		EXIFactory exiFactory = DefaultEXIFactory.newInstance();
		set_options(exiFactory, options);
		// create default factory and EXI grammar for schema
		Grammar g = create_grammar(options);
		if(g != null)
			exiFactory.setGrammar(g);
/*		
  		EXIResult exiResult = new EXIResult(exiFactory);

		ByteArrayOutputStream exiOS = new ByteArrayOutputStream();
		exiResult.setOutputStream(exiOS);
		XMLReader xmlReader = XMLReaderFactory.createXMLReader();
		xmlReader.setContentHandler(exiResult.getHandler());

		// parse xml file
		InputStream xmlIS = new StringBufferInputStream(xmlstr);
		xmlReader.parse(new InputSource(xmlIS));
		return exiOS.toByteArray();
*/
/*		StAXEncoder stax_encoder = new StAXEncoder(exiFactory);
		ByteArrayOutputStream exiOS = new ByteArrayOutputStream();
		stax_encoder.setOutputStream(exiOS);
		XMLInputFactory factory = XMLInputFactory.newInstance();
		InputStream xmlIS = new StringBufferInputStream(xmlstr);
		XMLEventReader	xmlReader = factory.createXMLEventReader(xmlIS);
		stax_encoder.encode(xmlReader);
		return exiOS.toByteArray();
*/
  		EXIResult exiResult = new EXIResult(exiFactory);
  		MyFragmentContentHandler fragmentHandler = new MyFragmentContentHandler(exiResult.getHandler());

		ByteArrayOutputStream exiOS = new ByteArrayOutputStream();
		exiResult.setOutputStream(exiOS);
		XMLReader xmlReader = XMLReaderFactory.createXMLReader();
		xmlReader.setContentHandler(fragmentHandler);

		// parse xml file
		for(int i=0;i<xmlstr.length;i++)
		{
			InputStream xmlIS = new StringBufferInputStream(xmlstr[i]);
			xmlReader.parse(new InputSource(xmlIS));
		}
		exiResult.getHandler().endDocument();
		return exiOS.toByteArray();
	}

	public static String decodeSchemaInformed(byte exibin[], 
											  exificient_options options
											  ) throws Exception {

		EXIFactory exiFactory = DefaultEXIFactory.newInstance();
		set_options(exiFactory, options);
		EXISource saxSource;
		Grammar g = create_grammar(options);
		if(g != null)
			exiFactory.setGrammar(g);
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
