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
	map<int, string> CacheSpace;/* store the data */
public:
	int GetCacheSize()const;
	pair<int, string> Find(int);/* Firstly query a data in cache */
	void Update(pair<int, string>);/* The data entering the buffer must be refresh in Cache */ 
	void flush();/* clear the cache */
};
#endif