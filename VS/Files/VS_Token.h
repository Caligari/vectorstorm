/*
 *  FS_Token.h
 *  VectorStorm
 *
 *  Created by Trevor Powell on 19/01/08.
 *  Copyright 2009 Trevor Powell. All rights reserved.
 *
 */

#ifndef FS_TOKEN_H
#define FS_TOKEN_H

#include "VS/Utils/VS_Array.h"
#include "VS/Utils/VS_StringTable.h"
class vsSerialiser;

class vsToken
{
public:
	enum Type
	{
		Type_None,
		Type_Label,
		Type_String,
		Type_Float,
		Type_Integer,
		Type_OpenBrace,
		Type_CloseBrace,
		Type_Equals,
		Type_NewLine,

		TYPE_MAX
	};
private:
	Type		m_type;
	union
	{
		char*	m_string;
		float		m_float;
		int32_t		m_int;
	};
	void SetStringField( const vsString& s );
public:

	vsToken();
	vsToken(Type t);
	vsToken(const vsToken& other);
	~vsToken();

	bool		ExtractFrom( vsString &string );
	vsString	BackToString();			// back to a string, exactly as we were extracted from.  (If we're of "String" type, this will have quotes around it)

	void		SerialiseBinaryV1( vsSerialiser *s, vsStringTable& stringTable );
	void		SerialiseBinaryV2( vsSerialiser *s );
	void		PopulateStringTable( vsStringTable& array );

	Type		GetType() const { return m_type; }
	vsString	AsString() const;			// give us our value as a string.  (If we're of string type, this will NOT have quotes around it)
	int			AsInteger();
	float		AsFloat();

	void		SetString(const vsString &value);
	void		SetLabel(const vsString &value);
	void		SetInteger(int value);
	void		SetFloat(float value);

	bool		IsType( Type type );
	bool		IsNumeric() { return IsType( Type_Float ) || IsType( Type_Integer ); }

	vsToken& operator=( const vsToken& other );
	bool operator==( const vsToken& other );
	bool operator!=( const vsToken& other ) { return ! ((*this) == other); }
};

#endif // FS_TOKEN_H

