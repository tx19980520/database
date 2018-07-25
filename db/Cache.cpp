#include "Cache.h"
#include <string>
#include <map>
using namespace std;
int Cache::GetCacheSize() const
{
	return this->CacheSpace.size();
}
pair<int, string> Cache::Find(int id)
{
	if (this->CacheSpace.size() == 0)
	{
		return make_pair(0, "");
	}
	map<int, string>::iterator result = this->CacheSpace.find(id);
	if (result != this->CacheSpace.end())
	{
		return *result;
	}
	else 
	{
		return make_pair(0, "");// tell the db cache has missed;
	}
}
/* premature optimization is the root of all evil */

/* extension Find and Remove */
void Cache::Update(pair<int, string> single)
{
	if (single.second == "")
	{
		this->CacheSpace.erase(single.first);
	}
	else 
	{
		map<int, string>::iterator tmp = this->CacheSpace.find(single.first);
		/* modify, Remove effect does not enter buffer */
		if (tmp != this->CacheSpace.end())
		{
			this->CacheSpace[single.first] = single.second;
		}
		else 
		{/* append */
			if ((int)(this->CacheSpace.size()) > BufferSize)
			{
				this->flush();
			}
			this->CacheSpace[single.first] = single.second;
		}
	}
}

void Cache::flush()
{
	this->CacheSpace.clear();
}

