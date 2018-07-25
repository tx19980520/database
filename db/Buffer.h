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
	void Wrtite(const string& dbname);/* public interface for write through */
public:
	void SetFilePath(const string& filepath);
	int GetBufferSize()const;/* monitor the size of buffer */
	void Insert(int index, int pointer, const string& str);/* insert a data in buffer */
	void flush();/* flush the data when Size maxmizing */
	void flush(const string& path);
	void Remove(int id);
	map<int, pair<int,string> >& GetBufferSpace();/*get Buffer for unit test*/
};
inline void Buffer::SetFilePath(const string& path)
{
	filepath = path;
}
#endif