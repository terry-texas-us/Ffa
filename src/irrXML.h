#pragma once

#include <stdio.h>

namespace xml
{
	//! Enumeration for all xml nodes which are parsed by IrrXMLReader
	enum EXML_NODE
	{
		//! No xml node. This is usually the node if you did not read anything yet.
		EXN_NONE,

		//! A xml element, like <foo>
		EXN_ELEMENT,

		//! End of an xml element, like </foo>
		EXN_ELEMENT_END,

		//! Text within a xml element: <foo> this is the text. </foo>
		EXN_TEXT,

		//! An xml comment like &lt;!-- I am a comment --&gt; or a DTD definition.
		EXN_COMMENT,

		//! An xml cdata section like &lt;![CDATA[ this is some CDATA ]]&gt;
		EXN_CDATA,

		//! Unknown element.
		EXN_UNKNOWN
	};

	//! Callback class for file read abstraction. 
	/** With this, it is possible to make the xml parser read in other things 
	than just files. The Irrlicht engine is using this for example to 
	read xml from compressed .zip files. To make the parser read in 
	any other data, derive a class from this interface, implement the 
	two methods to read your data and give a pointer to an instance of
	your implementation when calling createIrrXMLReader(), 
	createIrrXMLReaderUTF16() or createIrrXMLReaderUTF32() */
	class IFileReadCallBack
	{
	public:

		//! virtual destructor
		virtual ~IFileReadCallBack() {};

		//! Reads an amount of bytes from the file.
		/** \param buffer: Pointer to buffer where to read bytes will be written to.
		\param sizeToRead: Amount of bytes to read from the file.
		\return Returns how much bytes were read. */
		virtual int read(void* buffer, int sizeToRead) = 0;

		//! Returns size of file in bytes
		virtual int getSize() = 0;
	};

	//! Empty class to be used as parent class for IrrXMLReader.
	/** If you need another class as base class for the xml reader, you can do this by creating
	the reader using for example new CXMLReaderImpl<char, YourBaseClass>(yourcallback);
	The Irrlicht Engine for example needs IUnknown as base class for every object to
	let it automaticly reference countend, hence it replaces IXMLBase with IUnknown.
	See irrXML.cpp on how this can be done in detail. */
	class IXMLBase
	{
	};	

	class IrrXMLReader : public IXMLBase
	{
	public:

		//! Destructor
		virtual ~IrrXMLReader() {};

		//! Reads forward to the next xml node. 
		/** \return Returns false, if there was no further node.  */
		virtual bool read() = 0;

		//! Returns the type of the current XML node.
		virtual EXML_NODE getNodeType() const = 0;

        //! Returns attribute count of the current XML node. 
		/** This is usually
		non null if the current node is EXN_ELEMENT, and the element has attributes.
		\return Returns amount of attributes of this xml node. */
		virtual int getAttributeCount() const = 0;

		//! Returns name of an attribute. 
		/** \param idx: Zero based index, should be something between 0 and getAttributeCount()-1.
		\return Name of the attribute, 0 if an attribute with this index does not exist. */
		virtual const char* getAttributeName(int idx) const = 0;

		//! Returns the value of an attribute. 
		/** \param idx: Zero based index, should be something between 0 and getAttributeCount()-1.
		\return Value of the attribute, 0 if an attribute with this index does not exist. */
		virtual const char* getAttributeValue(int idx) const = 0;

		//! Returns the value of an attribute. 
		/** \param name: Name of the attribute.
		\return Value of the attribute, 0 if an attribute with this name does not exist. */
		virtual const char* getAttributeValue(const char* name) const = 0;

		//! Returns the value of an attribute in a safe way.
		/** Like getAttributeValue(), but does not 
		return 0 if the attribute does not exist. An empty string ("") is returned then.
		\param name: Name of the attribute.
		\return Value of the attribute, and "" if an attribute with this name does not exist */
		virtual const char* getAttributeValueSafe(const char* name) const = 0;

		//! Returns the value of an attribute as integer. 
		/** \param name Name of the attribute.
		\return Value of the attribute as integer, and 0 if an attribute with this name does not exist or
		the value could not be interpreted as integer. */
		virtual int getAttributeValueAsInt(const char* name) const = 0;

		//! Returns the value of an attribute as integer. 
		/** \param idx: Zero based index, should be something between 0 and getAttributeCount()-1.
		\return Value of the attribute as integer, and 0 if an attribute with this index does not exist or
		the value could not be interpreted as integer. */
		virtual int getAttributeValueAsInt(int idx) const = 0;

		//! Returns the value of an attribute as float. 
		/** \param name: Name of the attribute.
		\return Value of the attribute as float, and 0 if an attribute with this name does not exist or
		the value could not be interpreted as float. */
		virtual float getAttributeValueAsFloat(const char* name) const = 0;

		//! Returns the value of an attribute as float. 
		/** \param idx: Zero based index, should be something between 0 and getAttributeCount()-1.
		\return Value of the attribute as float, and 0 if an attribute with this index does not exist or
		the value could not be interpreted as float. */
		virtual float getAttributeValueAsFloat(int idx) const = 0;

		//! Returns the name of the current node. 
		/** Only non null, if the node type is EXN_ELEMENT.
		\return Name of the current node or 0 if the node has no name. */
		virtual const char* getNodeName() const = 0;

		//! Returns data of the current node. 
		/** Only non null if the node has some
		data and it is of type EXN_TEXT or EXN_UNKNOWN. */
		virtual const char* getNodeData() const = 0;

		//! Returns if an element is an empty element, like <foo />
		virtual bool isEmptyElement() const = 0;
	};

	//! Creates an instance of an UFT-8 or ASCII character xml parser.
	/** This means that all character data will be returned in 8 bit ASCII or UTF-8. 
	The file to read can be in any format, it will be converted to UTF-8 if it is not in this format.
	If you are using the Irrlicht Engine, it is better not to use this function but
	IFileSystem::createXMLReaderUTF8() instead.
	\param filename: Name of file to be opened.
	\return Returns a pointer to the created xml parser. This pointer should be 
	deleted using 'delete' after no longer needed. Returns 0 if an error occured
	and the file could not be opened. */
	IrrXMLReader* createIrrXMLReader(const char* filename);
} 