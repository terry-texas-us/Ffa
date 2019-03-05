#pragma once

#include "irrXML.h"
#include "irrArray.h"
#include "fast_atof.h"

namespace xml
{

int findFirst(CString str, char c)
{
	for(int i=0; i<str.GetLength(); ++i)
	{
		if(str[i] == c)
			return i;
	}

	return -1;
}

int findNext(CString str, char c, int startPos)
{
	for(int i=startPos; i<str.GetLength(); ++i)
		if (str[i] == c)
			return i;

	return -1;
}

CString subString(CString str, int nBegin, int nLength)
{
	return str.Mid(nBegin, nLength);
}

//! implementation of the IrrXMLReader
class CXMLReaderImpl : public IrrXMLReader
{
public:

	//! Constructor
	CXMLReaderImpl(IFileReadCallBack* callback, bool deleteCallBack = true)
		: TextData(0), P(0), TextSize(0), TextBegin(0), CurrentNodeType(EXN_NONE)
	{
		if (!callback)
			return;

		// read whole xml file
		readFile(callback);
		
		// clean up
		if (deleteCallBack)
			delete callback;

		// create list with special characters
		createSpecialCharacterList();

		// set pointer to text begin
		P = TextBegin;
	}

	//! Destructor
	virtual ~CXMLReaderImpl()
	{
		delete [] TextData;
	}

	//! Reads forward to the next xml node. 
	//! \return Returns false, if there was no further node. 
	virtual bool read()
	{
		// if not end reached, parse the node
		if (P && (unsigned int)(P - TextBegin) < TextSize - 1 && *P != 0)
		{
			parseCurrentNode();
			return true;
		}

//		_IRR_IMPLEMENT_MANAGED_MARSHALLING_BUGFIX;
		return false;
	}

	//! Returns the type of the current XML node.
	virtual EXML_NODE getNodeType() const
	{
		return CurrentNodeType;
	}

	//! Returns attribute count of the current XML node.
	virtual int getAttributeCount() const
	{
		return Attributes.size();
	}

	//! Returns name of an attribute.
	virtual const char* getAttributeName(int idx) const
	{
		if (idx < 0 || idx >= (int)Attributes.size())
			return 0;

		return Attributes[idx].Name.GetString();
	}

	//! Returns the value of an attribute. 
	virtual const char* getAttributeValue(int idx) const
	{
		if (idx < 0 || idx >= (int)Attributes.size())
			return 0;

		return Attributes[idx].Value.GetString();
	}

	//! Returns the value of an attribute. 
	virtual const char* getAttributeValue(const char* name) const
	{
		const SAttribute* attr = getAttributeByName(name);
		if (!attr)
			return 0;

		return attr->Value.GetString();
	}

	//! Returns the value of an attribute
	virtual const char* getAttributeValueSafe(const char* name) const
	{
		const SAttribute* attr = getAttributeByName(name);
		if (!attr)
			return EmptyString.GetString();

		return attr->Value.GetString();
	}

	//! Returns the value of an attribute as integer. 
	int getAttributeValueAsInt(const char* name) const
	{
		return (int)getAttributeValueAsFloat(name);
	}


	//! Returns the value of an attribute as integer. 
	int getAttributeValueAsInt(int idx) const
	{
		return (int)getAttributeValueAsFloat(idx);
	}


	//! Returns the value of an attribute as float. 
	float getAttributeValueAsFloat(const char* name) const
	{
		const SAttribute* attr = getAttributeByName(name);
		if (!attr)
			return 0;

		CString c = attr->Value.GetString();
		return fast_atof(c.GetString());
	}


	//! Returns the value of an attribute as float. 
	float getAttributeValueAsFloat(int idx) const
	{
		const char* attrvalue = getAttributeValue(idx);
		if (!attrvalue)
			return 0;

		CString c = attrvalue;
		return fast_atof(c.GetString());
	}


	//! Returns the name of the current node.
	virtual const char* getNodeName() const
	{
		return NodeName.GetString();
	}


	//! Returns data of the current node.
	virtual const char* getNodeData() const
	{
		return NodeName.GetString();
	}


	//! Returns if an element is an empty element, like <foo />
	virtual bool isEmptyElement() const
	{
		return IsEmptyElement;
	}

private:

	// Reads the current xml node
	void parseCurrentNode()
	{
		char* start = P;

		// more forward until '<' found
		while(*P != L'<' && *P)
			++P;

		if (!*P)
			return;

		if (P - start > 0)
		{
			// we found some text, store it
			if (setText(start, P))
				return;
		}

		++P;

		// based on current token, parse and report next element
		switch(*P)
		{
		case L'/':
			parseClosingXMLElement(); 
			break;
		case L'?':
			ignoreDefinition();	
			break;
		case L'!':
			if (!parseCDATA())
				parseComment();	
			break;
		default:
			parseOpeningXMLElement();
			break;
		}
	}


	//! sets the state that text was found. Returns true if set should be set
	bool setText(char* start, char* end)
	{
		// check if text is more than 2 characters, and if not, check if there is 
		// only white space, so that this text won't be reported
		if (end - start < 3)
		{
			char* p = start;
			for(; p != end; ++p)
				if (!isWhiteSpace(*p))
					break;

			if (p == end)
				return false;
		}

		// set current text to the parsed text, and replace xml special characters
		CString s(start, (int)(end - start));
		NodeName = replaceSpecialCharacters(s);

		// current XML node type is text
		CurrentNodeType = EXN_TEXT;

		return true;
	}



	//! ignores an xml definition like <?xml something />
	void ignoreDefinition()
	{
		CurrentNodeType = EXN_UNKNOWN;

		// move until end marked with '>' reached
		while(*P != L'>')
			++P;

		++P;
	}


	//! parses a comment
	void parseComment()
	{
		CurrentNodeType = EXN_COMMENT;
		P += 1;

		char *pCommentBegin = P;

		int count = 1;

		// move until end of comment reached
		while(count)
		{
			if (*P == L'>')
				--count;
			else
			if (*P == L'<')
				++count;

			++P;
		}

		P -= 3;
		NodeName = CString(pCommentBegin+2, (int)(P - pCommentBegin-2));

		P += 3;
	}


	//! parses an opening xml element and reads attributes
	void parseOpeningXMLElement()
	{
		CurrentNodeType = EXN_ELEMENT;
		IsEmptyElement = false;
		Attributes.clear();

		// find name
		const char* startName = P;

		// find end of element
		while(*P != L'>' && !isWhiteSpace(*P))
			++P;

		const char* endName = P;

		// find Attributes
		while(*P != L'>')
		{
			if (isWhiteSpace(*P))
				++P;
			else
			{
				if (*P != L'/')
				{
					// we've got an attribute

					// read the attribute names
					const char* attributeNameBegin = P;

					while(!isWhiteSpace(*P) && *P != L'=')
						++P;

					const char* attributeNameEnd = P;
					++P;

					// read the attribute value
					// check for quotes and single quotes, thx to murphy
					while( (*P != L'\"') && (*P != L'\'') && *P) 
						++P;

					if (!*P) // malformatted xml file
						return;

					const char attributeQuoteChar = *P;

					++P;
					const char* attributeValueBegin = P;
					
					while(*P != attributeQuoteChar && *P)
						++P;

					if (!*P) // malformatted xml file
						return;

					const char* attributeValueEnd = P;
					++P;

					SAttribute attr;
					attr.Name = CString(attributeNameBegin, 
						(int)(attributeNameEnd - attributeNameBegin));

					CString s(attributeValueBegin, 
						(int)(attributeValueEnd - attributeValueBegin));

					attr.Value = replaceSpecialCharacters(s);
					Attributes.push_back(attr);
				}
				else
				{
					// tag is closed directly
					++P;
					IsEmptyElement = true;
					break;
				}
			}
		}

		// check if this tag is closing directly
		if (endName > startName && *(endName-1) == L'/')
		{
			// directly closing tag
			IsEmptyElement = true;
			endName--;
		}
		
		NodeName = CString(startName, (int)(endName - startName));

		++P;
	}

	//! parses an closing xml tag
	void parseClosingXMLElement()
	{
		CurrentNodeType = EXN_ELEMENT_END;
		IsEmptyElement = false;
		Attributes.clear();

		++P;
		const char* pBeginClose = P;

		while(*P != L'>')
			++P;

		NodeName = CString(pBeginClose, (int)(P - pBeginClose));
		++P;
	}

	//! parses a possible CDATA section, returns false if begin was not a CDATA section
	bool parseCDATA()
	{
		if (*(P+1) != L'[')
			return false;

		CurrentNodeType = EXN_CDATA;

		// skip '<![CDATA['
		int count=0;
		while( *P && count<8 )
		{
			++P;
			++count;
		}

		if (!*P)
			return true;

		char *cDataBegin = P;
		char *cDataEnd = 0;

		// find end of CDATA
		while(*P && !cDataEnd)
		{
			if (*P == L'>' && 
			   (*(P-1) == L']') &&
			   (*(P-2) == L']'))
			{
				cDataEnd = P - 2;
			}

			++P;
		}

		if ( cDataEnd )
			NodeName = CString(cDataBegin, (int)(cDataEnd - cDataBegin));
		else
			NodeName = "";

		return true;
	}

	// structure for storing attribute-name pairs
	struct SAttribute
	{
		CString Name;
		CString Value;
	};

	// finds a current attribute by name, returns 0 if not found
	const SAttribute* getAttributeByName(const char* name) const
	{
		if (!name)
			return 0;

		CString n = name;

		for (int i=0; i<(int)Attributes.size(); ++i)
			if (Attributes[i].Name == n)
				return &Attributes[i];

		return 0;
	}

	// replaces xml special characters in a string and creates a new one
	CString replaceSpecialCharacters(CString &origstr)
	{
		int pos = findFirst(origstr, '&');//origstr.findFirst(L'&');
		int oldPos = 0;

		if (pos == -1)
			return origstr;

		CString newstr;

		while(pos != -1 && pos < origstr.GetLength()-2)
		{
			// check if it is one of the special characters

			int specialChar = -1;
			for (int i=0; i<(int)SpecialCharacters.size(); ++i)
			{
				//const char* p = &origstr.GetString()[pos]+1;

				//if (equalsn(&(SpecialCharacters[i][1]), p, SpecialCharacters[i].size()-1))
				//{
				//	specialChar = i;
				//	break;
				//}
			}

			if (specialChar != -1)
			{
				newstr.Append(origstr.Mid(oldPos, pos - oldPos));
				newstr.AppendChar(SpecialCharacters[specialChar][0]);
				pos += SpecialCharacters[specialChar].GetLength();
			}
			else
			{
				newstr.Append(subString(origstr,oldPos, pos - oldPos + 1));
				pos += 1;
			}

			// find next &
			oldPos = pos;
			pos = findNext(origstr, '&', pos);		
		}

		if (oldPos < origstr.GetLength()-1)
			newstr.Append(subString(origstr, oldPos, origstr.GetLength()-oldPos));

		return newstr;
	}

	//! reads the xml file and converts it into the wanted character format.
	bool readFile(IFileReadCallBack* callback)
	{
		int size = callback->getSize();		
		size += 1; // We need two terminating 0's at the end.
		           // For ASCII we need 1 0's, for UTF-16 2, for UTF-32 4.

		char* data8 = new char[size];

		if (!callback->read(data8, size-1))
		{
			delete [] data8;
			return false;
		}

		data8[size-1] = 0;
		convertTextData(data8, data8, size);

		return true;
	}

	//! converts the text file into the desired format.
	//! \param source: begin of the text (without byte order mark)
	//! \param pointerToStore: pointer to text data block which can be
	//! stored or deleted based on the nesessary conversion.
	//! \param sizeWithoutHeader: Text size in characters without header
	template<class src_char_type>
	void convertTextData(src_char_type* source, char* pointerToStore, int sizeWithoutHeader)
	{
		TextBegin = (char*)source;
		TextData = (char*)pointerToStore;
		TextSize = sizeWithoutHeader;
	}

	//! converts whole text buffer to little endian
	template<class src_char_type>
	void convertToLittleEndian(src_char_type* t)
	{
		if (sizeof(src_char_type) == 4) 
		{
			// 32 bit

			while(*t)
			{
				*t = ((*t & 0xff000000) >> 24) |
				     ((*t & 0x00ff0000) >> 8)  |
				     ((*t & 0x0000ff00) << 8)  |
				     ((*t & 0x000000ff) << 24);
				++t;
			}
		}
		else
		{
			// 16 bit 

			while(*t)
			{
				*t = (*t >> 8) | (*t << 8);
				++t;
			}
		}
	}

	//! returns true if a character is whitespace
	inline bool isWhiteSpace(char c)
	{
		return (c==' ' || c=='\t' || c=='\n' || c=='\r');
	}


	//! generates a list with xml special characters
	void createSpecialCharacterList()
	{
		// list of strings containing special symbols, 
		// the first character is the special character,
		// the following is the symbol string without trailing &.

		SpecialCharacters.push_back("&amp;");
		SpecialCharacters.push_back("<lt;");
		SpecialCharacters.push_back(">gt;");
		SpecialCharacters.push_back("\"quot;");
		SpecialCharacters.push_back("'apos;");
		
	}

	//! compares the first n characters of the strings
	bool equalsn(const char* str1, const char* str2, int len)
	{
		int i;
		for(i=0; str1[i] && str2[i] && i < len; ++i)
			if (str1[i] != str2[i])
				return false;

		// if one (or both) of the strings was smaller then they
		// are only equal if they have the same lenght
		return (i == len) || (str1[i] == 0 && str2[i] == 0);
	}

	// instance variables:

	char* TextData;         // data block of the text file
	char* P;                // current point in text to parse
	char* TextBegin;        // start of text to parse
	unsigned int TextSize;       // size of text to parse in characters, not bytes

	EXML_NODE CurrentNodeType;   // type of the currently parsed node

	CString NodeName;
	CString EmptyString;

	bool IsEmptyElement;       // is the currently parsed node empty?

	array< CString > SpecialCharacters; // see createSpecialCharacterList()

	array<SAttribute> Attributes; // attributes of current element
	
}; // end CXMLReaderImpl

}