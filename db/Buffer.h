#pragma once
#ifndef _BUFFER_H
#define _BUFFER_H
#include <map>
#include <string>
#include <fstream>
#include "constant.h"
using namespace std;
/* premature optimization is the root of all evil */
/************************
class Buffer is a write buffer, which is used to reduce the number of open files.
the core structure is a map<int, pair<int,string>>
************************/
class Buffer {
private:
	string filepath;
	map<int, pair<int,string>> BufferSpace;
	/* public interface for write through */
	void Wrtite(const string& dbname);
public:
	void SetFilePath(const string& filepath);
	/* monitor the size of buffer */
	int GetBufferSize()const;
	/* insert a data in buffer */
	void Insert(int index, int pointer, const string& str);
	/* flush the data when Size maxmizing */
	void flush();
	void flush(const string& path);
	void Remove(int id);
	string Find(int id);
	/*get Buffer for unit test*/
	map<int, pair<int,string> >& GetBufferSpace();
};
inline void Buffer::SetFilePath(const string& path)
{
	filepath = path;
}
#endif