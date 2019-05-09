/*
 *  VS_Color.h
 *  VectorStorm
 *
 *  Created by Trevor Powell on 17/03/07.
 *  Copyright 2007 Trevor Powell. All rights reserved.
 *
 */

#ifndef VS_COLOR_H
#define VS_COLOR_H

#include "Math/VS_Math.h"

class vsColorHSL;
class vsColorHSV;
class vsColorPacked;

class vsColor
{
public:
	float r, g, b, a;

	vsColor(float red=0.f, float green=0.f, float blue=0.f, float alpha=1.f) { r=red; g=green; b=blue; a=alpha; }
	vsColor(const vsColorPacked& packed);
	vsColor(const vsColorHSV& hsv);
	vsColor(const vsColorHSL& hsl);

	static vsColor FromHSV(float hue, float saturation, float value);
	static vsColor FromHSL(float hue, float saturation, float lightness);
	static vsColor FromBytes(unsigned char r, unsigned char g, unsigned char b, unsigned char a);
	static vsColor FromUInt32(uint32_t rgba);
	float GetHue() const;
	float GetSaturation() const;
	float GetValue() const;
	vsColorHSV GetHSV() const;
	uint32_t AsUInt32() const;

	vsColor  operator+( const vsColor &o ) const {return vsColor(r+o.r,g+o.g,b+o.b,a+o.a);}
	vsColor  operator-( const vsColor &o ) const {return vsColor(r-o.r,g-o.g,b-o.b,a-o.a);}
	vsColor & operator+=( const vsColor &o ) {r+=o.r; g+=o.g; b+=o.b; a+=o.a; return *this;}
	vsColor  operator*( float scalar ) const {return vsColor(r*scalar,g*scalar,b*scalar, a*scalar);}
	vsColor &  operator*=( float scalar ) {r*=scalar; g*=scalar; b*=scalar; a*=scalar; return *this;}
	vsColor  operator*( const vsColor &o ) const {return vsColor(r*o.r,g*o.g,b*o.b, a*o.a);}
	vsColor & operator*=( const vsColor &o ) {r*=o.r; g*=o.g; b*=o.b; a*=o.a; return *this;}

	bool	operator==( const vsColor &o ) const { return (r==o.r && b==o.b && g==o.g && a==o.a); }
	bool	operator!=( const vsColor &o ) const { return !(*this==o); }

	float	Magnitude() { return vsSqrt( r*r + g*g + b*b + a*a ); }

	void Set(float red=0.f, float green=0.f, float blue=0.f, float alpha=1.f) { r=red; g=green; b=blue; a=alpha; };

	static const vsColor PureWhite;
	static const vsColor White;
	static const vsColor Grey;
	static const vsColor Blue;
	static const vsColor Green;
	static const vsColor LightGreen;
	static const vsColor Yellow;
	static const vsColor Orange;
	static const vsColor LightBlue;
	static const vsColor DarkBlue;
	static const vsColor Red;
	static const vsColor Purple;
	static const vsColor Black;
	static const vsColor Clear;
};

class vsColorPacked
{
public:
	uint8_t r, g, b, a;

	vsColorPacked(uint8_t red=0, uint8_t green=0, uint8_t blue=0, uint8_t alpha=1) { r=red; g=green; b=blue; a=alpha; }
	vsColorPacked(const vsColor& c);
	vsColorPacked(const vsColorHSV& hsv);

	static vsColor FromHSV(float hue, float saturation, float value);
	static vsColor FromBytes(unsigned char r, unsigned char g, unsigned char b, unsigned char a);
	static vsColor FromUInt32(uint32_t rgba);
	float GetHue() const;
	float GetSaturation() const;
	float GetValue() const;
	vsColorHSV GetHSV() const;
	uint32_t AsUInt32() const;

	vsColor  operator+( const vsColor &o ) const;
	vsColor  operator-( const vsColor &o ) const;
	vsColor & operator+=( const vsColor &o );
	vsColor  operator*( float scalar );
	vsColor &  operator*=( float scalar );
	vsColor  operator*( const vsColor &o ) const;
	vsColor & operator*=( const vsColor &o );

	bool	operator==( const vsColorPacked &o ) const { return (r==o.r && b==o.b && g==o.g && a==o.a); }
	bool	operator!=( const vsColorPacked &o ) const { return !(*this==o); }

	float	Magnitude();

	void Set(float red=0.f, float green=0.f, float blue=0.f, float alpha=1.f) { r=red*255; g=green*255; b=blue*255; a=alpha*255; };
};

class vsColorHSV
{
public:
	float h, s, v, a;

	vsColorHSV();
	vsColorHSV( float h, float s, float v, float a );
	vsColorHSV( const vsColor& other );
};

class vsColorHSL
{
public:
	float h, s, l, a;

	vsColorHSL();
	vsColorHSL( float h, float s, float l, float a );
	vsColorHSL( const vsColor& other );

	// Technically, our 'S' value both here in the vsColorHSL and
	// vsColorHSV classes are actually storing "chroma" values;  scaled
	// within the value range of saturation levels for the specified
	// lightness/value (because otherwise, it would be possible to go
	// outside the representable RGB color space).  If we need an
	// absolute saturation value, we can use these conversion functions.
	//
	// Again, in order to cause maximum confusion, the 's' value on
	// this class is actually storing chroma, and "saturation" is traditionally
	// the 'true' non-scaled value.  So to get that true non-scaled value,
	// pass that 's' value into "ChromaToSaturation".
	static float SaturationToChroma(float saturation, float lightness);
	static float ChromaToSaturation(float chroma, float lightness);
};

extern const vsColor c_white;
extern const vsColor c_grey;
extern const vsColor c_blue;
extern const vsColor c_green;
extern const vsColor c_lightGreen;
extern const vsColor c_yellow;
extern const vsColor c_orange;
extern const vsColor c_lightBlue;
extern const vsColor c_darkBlue;
extern const vsColor c_red;
extern const vsColor c_purple;
extern const vsColor c_black;
extern const vsColor c_clear;

vsColor  operator*( float scalar, const vsColor &color );
vsColor vsInterpolate( float alpha, const vsColor &a, const vsColor &b );
vsColor vsInterpolateHSV( float alpha, const vsColor &a, const vsColor &b );

#endif // VS_COLOR_H
