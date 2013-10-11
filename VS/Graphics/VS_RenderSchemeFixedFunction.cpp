/*
 *  VS_RenderSchemeFixedFunction.cpp
 *  VectorStorm
 *
 *  Created by Trevor Powell on 17/03/07.
 *  Copyright 2007 Trevor Powell. All rights reserved.
 *
 */

#include "VS_DisplayList.h"
#include "VS_RenderSchemeFixedFunction.h"
#include "VS_Screen.h"

#include "VS_OpenGL.h"

vsRenderSchemeFixedFunction::vsRenderSchemeFixedFunction( vsRenderer *renderer ):
	vsRenderScheme(renderer)
{
}

vsRenderSchemeFixedFunction::~vsRenderSchemeFixedFunction()
{
}

void
vsRenderSchemeFixedFunction::PreRender(const vsRenderer::Settings &s)
{
	// give us thicker lines
	glLineWidth( 2.0f );
}

void
vsRenderSchemeFixedFunction::RenderDisplayList( vsDisplayList *list )
{
	Parent::RenderDisplayList(list);
}

void
vsRenderSchemeFixedFunction::PostRender()
{
	Parent::PostRender();
}

