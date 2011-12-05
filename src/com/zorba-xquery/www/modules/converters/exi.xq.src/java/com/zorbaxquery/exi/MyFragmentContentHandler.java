package com.zorbaxquery.exi;

import org.xml.sax.Attributes;
import org.xml.sax.ContentHandler;
import org.xml.sax.Locator;
import org.xml.sax.SAXException;

public class MyFragmentContentHandler implements ContentHandler {

	private ContentHandler exi_handler;
	private boolean started = false;
	
	public MyFragmentContentHandler(ContentHandler _exi_handler)
	{
		exi_handler = _exi_handler;
	}
	
	@Override
	public void characters(char[] ch, int start, int length)
			throws SAXException {
		exi_handler.characters(ch, start, length);
	}

	@Override
	public void endDocument() throws SAXException {
		//do nothing
	}

	@Override
	public void endElement(String uri, String localName, String qName)
			throws SAXException {
		exi_handler.endElement(uri, localName, qName);
	}

	@Override
	public void endPrefixMapping(String prefix) throws SAXException {
		exi_handler.endPrefixMapping(prefix);
	}

	@Override
	public void ignorableWhitespace(char[] ch, int start, int length)
			throws SAXException {
		exi_handler.ignorableWhitespace(ch, start, length);
	}

	@Override
	public void processingInstruction(String target, String data)
			throws SAXException {
		exi_handler.processingInstruction(target, data);
	}

	@Override
	public void setDocumentLocator(Locator locator) {
		exi_handler.setDocumentLocator(locator);
	}

	@Override
	public void skippedEntity(String name) throws SAXException {
		exi_handler.skippedEntity(name);
	}

	@Override
	public void startDocument() throws SAXException {
		if(!started)
			exi_handler.startDocument();
		started = true;
	}

	@Override
	public void startElement(String uri, String localName, String qName,
			Attributes atts) throws SAXException {
		exi_handler.startElement(uri, localName, qName, atts);
	}

	@Override
	public void startPrefixMapping(String prefix, String uri)
			throws SAXException {
		exi_handler.startPrefixMapping(prefix, uri);
	}

}
