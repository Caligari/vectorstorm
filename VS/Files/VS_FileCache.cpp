/*
 *  VS_FileCache.cpp
 *  VectorStorm
 *
 *  Created by Trevor Powell on 08/11/2019
 *  Copyright 2019 Trevor Powell.  All rights reserved.
 *
 */

#include "VS_FileCache.h"
#include "VS_HashTable.h"
#include "VS_Store.h"

static vsHashTable<vsStore> *s_cache = NULL;

void
vsFileCache::Startup()
{
	s_cache = new vsHashTable<vsStore>(128);
}

void
vsFileCache::Shutdown()
{
	vsDelete( s_cache );
}

void
vsFileCache::Purge()
{
	Shutdown();
	Startup();
}

bool
vsFileCache::IsFileInCache(const vsString& filename)
{
	vsStore *s = s_cache->FindItem(filename);

	return NULL != s;
}

vsStore*
vsFileCache::GetFileContents(const vsString& filename)
{
	vsStore *s = s_cache->FindItem(filename);
	return s;
}

void
vsFileCache::SetFileContents(const vsString& filename, const vsStore &store)
{
	s_cache->AddItemWithKey(store, filename);
}

