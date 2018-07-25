#pragma once
#ifndef _CACHE_H
#define _CACHE_H
#include <string>
#include <map>
#include "Buffer.h"
using namespace std;
/************************
class Cahe is a cache for query, which is used to optimize query speed.
The core structure is  a map<int, string>
************************/
class Cache {
private:
	/* store the data */
	map<int, string> CacheSpace;
public:
	int GetCacheSize() const;
	/* find a data in cache */
	pair<int, string> Find(int);
	/* The data entering the buffer must be refresh in Cache */
	void Update(pair<int, string>);
	/* clear the cache */
	void flush();
};
#endif