/*
 *  VS_LocString.h
 *  VectorStorm
 *
 *  Created by Trevor Powell on 09/03/2020
 *  Copyright 2020 Trevor Powell.  All rights reserved.
 *
 */

#ifndef VS_LOCSTRING_H
#define VS_LOCSTRING_H

#include "VS/Utils/fmt/format.h"
#include <vector>
// #include "VS/Utils/VS_String.h"
// #include "VS/Utils/VS_Array.h"

class vsLocString;

struct vsLocArg;

class vsLocString
{
	void SubVars( vsString& str ) const;
public:
	vsString m_string;
	std::vector<vsLocArg> m_args;

	vsLocString();
	vsLocString( const char* str ); // non-nullptr!
	vsLocString( const vsString& key );
	vsLocString( int value );
	vsLocString( float value,int places );

	bool IsEmpty() const;
	vsString AsString() const;

	static void SetNumberThousandsSeparator(const vsString& separator);
	static void SetNumberDecimalSeparator(const vsString& separator);

	bool operator==(const vsLocString& other) const;
	bool operator!=(const vsLocString& other) const;
};

struct vsLocArg
{
	enum Type
	{
		Type_LocString,
		Type_Int,
		Type_Float,
	};

	vsString m_name;

	vsLocString m_locString;
	int m_intLiteral;
	float m_floatLiteral;
	Type m_type;

	vsLocArg(const vsString& name, float literal): m_name(name), m_intLiteral(-1), m_floatLiteral(literal), m_type(Type_Float) {}
	vsLocArg(const vsString& name, int literal): m_name(name), m_intLiteral(literal), m_type(Type_Int) {}
	vsLocArg(const vsString& name, const vsLocString& loc): m_name(name), m_locString(loc), m_type(Type_LocString) {}

	// int literal is telling us how many places to fill in
	vsLocArg(const vsString& name, float literal, int places): m_name(name), m_intLiteral(places), m_floatLiteral(literal), m_type(Type_Float) {}

	vsString AsString(const vsString& fmt = vsEmptyString) const;
	bool operator==(const vsLocArg& other) const;
	bool operator!=(const vsLocArg& other) const;
};

template <typename S, typename... Args>
vsLocString vsLocFormat(S key, Args&&... args)
{
	std::vector<vsLocArg> a = {args...};

	vsLocString result(key);
	result.m_args = a;
	return result;
}

// =========  INTEGRATION WITH FMT
//
// FMTLIB needs an operator<< in order to work with printf-style
// formatting, and needs an fmt::formatter template specialisation
// to work with Python-style formatting (which is what we're using
// for most localisation stuff, but maybe isn't going to be useful
// any more in future, since I'm writing my own localisation formatting?)
//
std::ostream& operator <<(std::ostream &s, const vsLocString &ls);

namespace fmt
{
template <>
struct formatter<vsLocString> {
	auto parse(format_parse_context& ctx) -> decltype(ctx.begin()) {
		auto it = ctx.begin();
		return it;
	}

	template <typename FormatContext>
		auto format(const vsLocString& p, FormatContext& ctx) -> decltype(ctx.out()) {
			return format_to(
					ctx.out(),
					"{}",
					p.AsString()
					);
		}
};
};

#endif // VS_LOCSTRING_H

