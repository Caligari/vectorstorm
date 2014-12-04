/*
 *  FS_Record.h
 *  VectorStorm
 *
 *  Created by Trevor Powell on 27/12/07.
 *  Copyright 2007 Trevor Powell. All rights reserved.
 *
 */

#ifndef FS_RECORD_H
#define FS_RECORD_H

#include "VS/Utils/VS_Array.h"
#include "VS/Utils/VS_LinkedListStore.h"
#include "VS/Math/VS_Quaternion.h"

class vsVector2D;
class vsColor;
class vsFile;

#include "VS_Token.h"

class vsRecord
{
	vsToken		m_label;

	vsArray<vsToken>	m_token;

	vsLinkedListStore<vsRecord>	m_childList;
	vsRecord *	m_lastChild;

	bool		m_inBlock;
	bool		m_hasLabel;
	bool		m_lineIsOpen;

	bool		m_writeType;

	float		GetArg(int i);

public:
	vsRecord();
	~vsRecord();

	void		Init();

	bool		Parse( vsFile *file );				// attempt to fill out this vsRecord from a vsString
	bool		ParseString( vsString string );
	bool		AppendToken( const vsToken &token );		// add this token to me.
	vsString	ToString( int childLevel = 0 );							// convert this vsRecord into a vsString.

	vsToken &			GetLabel() { return m_label; }
	void				SetLabel(const vsString &label);

	vsToken &			GetToken(int i);
	int					GetTokenCount() { return m_token.ItemCount(); }
	void				SetTokenCount( int count );

	vsRecord *			GetChild(int i);
	int					GetChildCount() { return m_childList.ItemCount(); }
	int					GetChildCount(const vsString& label);	// returns number of children with this label
	void				AddChild(vsRecord *record);
	//void				SetChildCount( int count );

	bool				Bool();
	int					Int();
	vsString			String();
	vsColor				Color();
	vsVector2D			Vector2D();
	vsVector3D			Vector3D();
	vsQuaternion		Quaternion();

	void				SetRect(float x, float y, float width, float height);
	void				SetInt(int value);
	void				SetFloat(float value);
};

#endif // FS_RECORD_H

