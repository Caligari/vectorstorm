/*
 *  VS_Image.cpp
 *  MMORPG2
 *
 *  Created by Trevor Powell on 01-02-2012.
 *  Copyright 2012 Trevor Powell.  All rights reserved.
 *
 */

#include "VS_Image.h"

#include "VS_Color.h"
#include "VS_Texture.h"
#include "VS_TextureManager.h"
#include "VS_TextureInternal.h"
#include "VS_RenderTarget.h"

#include "VS_File.h"
#include "VS_Store.h"

#include "VS_OpenGL.h"


#include "stb_image_write.h"
#include "stb_image.h"

int vsImage::s_textureMakerCount = 0;
bool vsImage::s_allowLoadFailure = false;

vsImage::vsImage():
	m_pixel(nullptr),
	m_pixelCount(0),
	m_width(0),
	m_height(0),
	m_pbo(0),
	m_sync(0)
{
}

vsImage::vsImage(unsigned int width, unsigned int height):
	m_pixel(nullptr),
	m_pixelCount(0),
	m_width(width),
	m_height(height),
	m_pbo(0),
	m_sync(0)
{
	m_pixelCount = width * height;

	m_pixel = new uint32_t[m_pixelCount];
	memset(m_pixel,0,sizeof(uint32_t)*m_pixelCount);
}

vsImage::vsImage( const vsString &filename ):
	m_pixel(nullptr),
	m_pbo(0),
	m_sync(0)
{
	vsFile img(filename, vsFile::MODE_Read);
	vsStore *s = new vsStore( img.GetLength() );
	img.Store(s);

	int w,h,n;
	unsigned char* data = stbi_load_from_memory( (uint8_t*)s->GetReadHead(), s->BytesLeftForReading(), &w, &h, &n, STBI_rgb_alpha );
	if ( !data )
		vsLog( "Failure: %s", stbi_failure_reason() );

	vsDelete(s);

	m_width = w;
	m_height = h;

	m_pixel = new uint32_t[m_width*m_height];

	for ( unsigned int v = 0; v < m_height; v++ )
	{
		for ( unsigned int u = 0; u < m_width; u++ )
		{
			int ind = PixelIndex(u,v);
			m_pixel[ind] = ((uint32_t*)(data))[ind];
		}
	}

	stbi_image_free(data);

}

vsImage::vsImage( const vsStore &filedata ):
	m_pixel(nullptr),
	m_pbo(0),
	m_sync(0)
{
	int w,h,n;
	unsigned char* data = stbi_load_from_memory( (uint8_t*)filedata.GetReadHead(), filedata.BytesLeftForReading(), &w, &h, &n, STBI_rgb_alpha );
	if ( !data )
		vsLog( "Failure: %s", stbi_failure_reason() );

	m_width = w;
	m_height = w;

	m_pixel = new uint32_t[m_width*m_height];

	for ( unsigned int v = 0; v < m_height; v++ )
	{
		for ( unsigned int u = 0; u < m_width; u++ )
		{
			int ind = PixelIndex(u,v);
			m_pixel[ind] = ((uint32_t*)(data))[ind];
		}
	}

	stbi_image_free(data);
}

vsImage::vsImage( vsTexture * texture ):
	m_pixel(nullptr),
	m_width(0),
	m_height(0),
	m_pbo(0),
	m_sync(0)
{
	Read(texture);
}

vsImage::~vsImage()
{
	if ( m_pbo != 0 )
	{
		if ( m_pixel )
			AsyncUnmap();

		glDeleteBuffers( 1, (GLuint*)&m_pbo );
		glDeleteSync( m_sync );

		vsAssert( m_pixel == nullptr, "async-mapped pixel data not cleared during destruction??" );
		m_pbo = 0;
		m_sync = 0;
	}

	vsDeleteArray( m_pixel );
}

void
vsImage::Read( vsTexture *texture )
{
	// GL_CHECK_SCOPED("vsImage");

	if ( m_width != (unsigned int)texture->GetResource()->GetWidth() ||
			m_height != (unsigned int)texture->GetResource()->GetHeight() )
	{
		vsDeleteArray(m_pixel);

		m_width = texture->GetResource()->GetWidth();
		m_height = texture->GetResource()->GetHeight();

		m_pixelCount = m_width * m_height;
		m_pixel = new uint32_t[m_pixelCount];
	}

	bool depthTexture = texture->GetResource()->IsDepth();

	// glReadPixels can align the first pixel in each row at 1-, 2-, 4- and 8-byte boundaries. We
	// have allocated the exact size needed for the image so we have to use 1-byte alignment
	// (otherwise glReadPixels would write out of bounds)
	glActiveTexture( GL_TEXTURE0 );
	glBindTexture( GL_TEXTURE_2D, texture->GetResource()->GetTexture() );
	glPixelStorei(GL_PACK_ALIGNMENT, 1);

	if ( depthTexture )
	{
		size_t imageSizeInFloats = size_t(m_width) * size_t(m_height);

		float* pixels = new float[imageSizeInFloats];

		glGetTexImage(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, GL_FLOAT, pixels);
		glBindTexture( GL_TEXTURE_2D, 0 );

		for ( unsigned int y = 0; y < m_height; y++ )
		{
			int rowStart = y * m_width;

			for ( unsigned int x = 0; x < m_width; x++ )
			{
				int rInd = rowStart + (x);
				float rVal = pixels[rInd];

				SetPixel(x, (m_height-1)-y, vsColor(rVal, rVal, rVal, 1.0f) );
			}
		}
		vsDeleteArray( pixels );
	}
	else
	{
		size_t imageSize = size_t(m_width) * size_t(m_height);
		uint32_t* pixels = new uint32_t[imageSize];
		// TODO:  THis would be faster if it was BGRA.
		glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
		glBindTexture( GL_TEXTURE_2D, 0 );

		for ( unsigned int y = 0; y < m_height; y++ )
		{
			int rowStart = y * m_width;

			for ( unsigned int x = 0; x < m_width; x++ )
			{
				int rInd = rowStart + (x);
				uint32_t pixel = pixels[rInd];

				SetRawPixel(x, (m_height-1)-y, pixel);
			}
		}
		vsDeleteArray( pixels );
	}
}

void
vsImage::PrepForAsyncRead( vsTexture *texture )
{
	if ( m_pbo == 0 )
		glGenBuffers(1, &m_pbo);

	glBindBuffer( GL_PIXEL_PACK_BUFFER, m_pbo);
	size_t width = texture->GetResource()->GetWidth();
	size_t height = texture->GetResource()->GetHeight();
	if ( width != m_width || height != m_height )
	{
		m_width = width;
		m_height = height;
		int bytes = width * height * sizeof(uint32_t);
		glBufferData( GL_PIXEL_PACK_BUFFER, bytes, nullptr, GL_DYNAMIC_READ );
	}

	glBindBuffer( GL_PIXEL_PACK_BUFFER, 0);
}

void
vsImage::AsyncRead( vsTexture *texture )
{
	PrepForAsyncRead(texture);

	if ( m_sync != 0 )
		glDeleteSync( m_sync );

	glBindBuffer( GL_PIXEL_PACK_BUFFER, m_pbo);

	glActiveTexture( GL_TEXTURE0 );
	glBindTexture( GL_TEXTURE_2D, texture->GetResource()->GetTexture() );
	glPixelStorei(GL_PACK_ALIGNMENT, 1);
	glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
	glBindTexture( GL_TEXTURE_2D, 0 );

	// GL_CHECK("glGetTexImage");
	glBindBuffer( GL_PIXEL_PACK_BUFFER, 0);

	m_sync = glFenceSync( GL_SYNC_GPU_COMMANDS_COMPLETE, 0 );
}

void
vsImage::AsyncReadRenderTarget(vsRenderTarget *target, int buffer)
{
	GL_CHECK_SCOPED("AsyncReadRenderTarget");
	PrepForAsyncRead(target->Resolve(0));
	GL_CHECK("Prepped");

	if ( m_sync != 0 )
		glDeleteSync( m_sync );
	GL_CHECK("Deleted Sync");

	glBindBuffer( GL_PIXEL_PACK_BUFFER, m_pbo);
	GL_CHECK("BindBuffer");

	target->Bind();
	int width = target->GetWidth();
	int height = target->GetHeight();
	glReadBuffer(GL_COLOR_ATTACHMENT0+buffer);
	GL_CHECK("glReadBuffer");
	glReadPixels(0,0,width,height, GL_RGBA, GL_UNSIGNED_BYTE, 0);
	GL_CHECK("glReadPixels");

	glBindBuffer( GL_PIXEL_PACK_BUFFER, 0);
	GL_CHECK("glUnbindBuffer");

	m_sync = glFenceSync( GL_SYNC_GPU_COMMANDS_COMPLETE, 0 );
	GL_CHECK("glFenceSync");
}

bool
vsImage::AsyncReadIsReady()
{
	// GL_CHECK_SCOPED("AsyncReadIsReady");
	if ( glClientWaitSync( m_sync, GL_SYNC_FLUSH_COMMANDS_BIT, 0 ) != GL_TIMEOUT_EXPIRED )
	{
		return true;
	}
	return false;
}

void
vsImage::AsyncMap()
{
	vsAssert( m_pixel == nullptr, "Non-null during pbo async mapping");
	glBindBuffer( GL_PIXEL_PACK_BUFFER, m_pbo);
	m_pixel = (uint32_t*)glMapBuffer(GL_PIXEL_PACK_BUFFER, GL_READ_ONLY);
	m_pixelCount = m_width * m_height;
	glBindBuffer( GL_PIXEL_PACK_BUFFER, 0);
}

void
vsImage::AsyncUnmap()
{
	glBindBuffer( GL_PIXEL_PACK_BUFFER, m_pbo);
	glUnmapBuffer(GL_PIXEL_PACK_BUFFER);
	glBindBuffer( GL_PIXEL_PACK_BUFFER, 0);
	m_pixel = nullptr;
}

uint32_t
vsImage::GetRawPixel(unsigned int u, unsigned int v) const
{
	vsAssert(u >= 0 && u < m_width && v >= 0 && v < m_height, "Texel out of bounds!");
	return m_pixel[ PixelIndex(u,v) ];
}

void
vsImage::SetRawPixel(unsigned int u, unsigned int v, uint32_t c)
{
	vsAssert(u >= 0 && u < m_width && v >= 0 && v < m_height, "Texel out of bounds!");
	m_pixel[ PixelIndex(u,v) ] = c;
}

vsColor
vsImage::GetPixel(unsigned int u, unsigned int v) const
{
	return vsColor::FromUInt32(GetRawPixel(u,v));
}

void
vsImage::SetPixel(unsigned int u, unsigned int v, const vsColor &c)
{
	SetRawPixel(u,v,c.AsUInt32());
}

void
vsImage::Clear( const vsColor &clearColor )
{
	uint32_t cc = clearColor.AsUInt32();
	// TODO:  This should really be being done via memset.
	for ( int i = 0; i < m_pixelCount; i++ )
	{
		m_pixel[i] = cc;
	}
}

void
vsImage::CopyTo( vsImage *other )
{
	// in CopyTo, we've already validated that 'other' can contain our data.
	int bytes = m_width * m_height * sizeof(uint32_t);
	if ( m_pbo )
	{
		glBindBuffer( GL_PIXEL_PACK_BUFFER, m_pbo);
		// glGetBufferSubData( GL_PIXEL_PACK_BUFFER, 0, bytes, other->m_pixel );
		void* ptr = glMapBuffer(GL_PIXEL_PACK_BUFFER, GL_READ_ONLY);
		memcpy(other->m_pixel, ptr, bytes);
		glUnmapBuffer(GL_PIXEL_PACK_BUFFER);
		glBindBuffer( GL_PIXEL_PACK_BUFFER, 0);
	}
	else
	{
		memcpy( other->m_pixel, m_pixel, bytes );
	}
}

void
vsImage::Copy( vsImage *other )
{
	if ( m_width != (unsigned int)other->GetWidth() ||
			m_height != (unsigned int)other->GetHeight() )
	{
		vsDeleteArray(m_pixel);

		m_width = other->GetWidth();
		m_height = other->GetHeight();

		m_pixelCount = m_width * m_height;
		m_pixel = new uint32_t[m_pixelCount];
	}
	other->CopyTo(this);
}

vsTexture *
vsImage::Bake( const vsString& name_in )
{
	vsString name(name_in);
	if ( name.empty() )
		name = vsFormatString("MakerTexture%d", s_textureMakerCount++);

	vsTextureInternal *ti = new vsTextureInternal(name, this);
	vsTextureManager::Instance()->Add(ti);

	return new vsTexture(name);
}

namespace
{
	void vsfile_write_func(void *context, void *data, int size)
	{
		vsFile* file = (vsFile*)(context);
		file->WriteBytes(data, size);
	}
};

void
vsImage::SaveJPG(int quality, const vsString& filename)
{
	vsFile file( filename, vsFile::MODE_Write );

	stbi_flip_vertically_on_write(1);
	int retval = stbi_write_jpg_to_func(vsfile_write_func, &file,
			m_width, m_height, 4,
			m_pixel, quality);

	if ( retval == 0 )
		vsLog("Failed to write jpg '%s'", filename, retval);
}

void
vsImage::SavePNG(const vsString& filename)
{
	vsFile file( filename, vsFile::MODE_Write );

	stbi_flip_vertically_on_write(1);
	int retval = stbi_write_png_to_func(vsfile_write_func, &file,
			m_width, m_height, 4,
			m_pixel, m_width * sizeof(uint32_t));

	if ( retval == 0 )
		vsLog("Failed to write png '%s'", filename, retval);
}

void
vsImage::SavePNG_FullAlpha(const vsString& filename)
{
	vsImage dup( m_width, m_height );
	for ( size_t y = 0; y < m_height; y++ )
	{
		for ( size_t x = 0; x < m_width; x++ )
		{
			vsColor c = GetPixel(x,y);
			c.a = 1.0;
			dup.SetPixel(x,y,c);
		}
	}
	dup.SavePNG(filename);
}

