/*
 *  VS_HalfImage.cpp
 *  VectorStorm
 *
 *  Created by Trevor Powell on 27/04/2020
 *  Copyright 2020 Trevor Powell.  All rights reserved.
 *
 */

#include "VS_HalfIntImage.h"

#include "VS_Color.h"
#include "VS_Texture.h"
#include "VS_TextureManager.h"
#include "VS_TextureInternal.h"
#include "VS_RenderTarget.h"

#include "VS_File.h"
#include "VS_Store.h"

#if !TARGET_OS_IPHONE
#include <SDL2/SDL_image.h>
#include <png.h>
#include "VS_OpenGL.h"

#ifndef _WIN32
#include <zlib.h>
#endif // _WIN32
#endif // TARGET_OS_IPHONE

int vsHalfIntImage::s_textureMakerCount = 0;

vsHalfIntImage::vsHalfIntImage(unsigned int width, unsigned int height):
	m_pixel(NULL),
	m_pixelCount(0),
	m_width(width),
	m_height(height)
{
	m_pixelCount = width * height;

	m_pixel = new uint64_t[m_pixelCount];
	memset(m_pixel,0,sizeof(uint64_t)*m_pixelCount);
}

vsHalfIntImage::~vsHalfIntImage()
{
	vsDeleteArray( m_pixel );
}

uint64_t
vsHalfIntImage::GetRawPixel(unsigned int u, unsigned int v) const
{
	vsAssert(u >= 0 && u < m_width && v >= 0 && v < m_height, "Texel out of bounds!");
	return m_pixel[ PixelIndex(u,v) ];
}

void
vsHalfIntImage::SetRawPixel(unsigned int u, unsigned int v, uint64_t c)
{
	vsAssert(u >= 0 && u < m_width && v >= 0 && v < m_height, "Texel out of bounds!");
	m_pixel[ PixelIndex(u,v) ] = c;
}

vsColor
vsHalfIntImage::GetPixel(unsigned int u, unsigned int v) const
{
	return vsColor::FromUInt64(GetRawPixel(u,v));
}

void
vsHalfIntImage::SetPixel(unsigned int u, unsigned int v, const vsColor &c)
{
	SetRawPixel(u,v,c.AsUInt64());
}

vsTexture *
vsHalfIntImage::Bake()
{
	vsString name = vsFormatString("HalfIntMakerTexture%d", s_textureMakerCount++);

	vsTextureInternal *ti = new vsTextureInternal(name, this);
	vsTextureManager::Instance()->Add(ti);

	return new vsTexture(name);
}
