#include "Buffer.h"
#include <fstream>
#include <iostream>
#include <map>
#include "string.h"
#include "constant.h"

using namespace std;

void Buffer::Wrtite(const string & filepath)
{
	fstream file(filepath, ios::out| ios::in |ios::binary);
	if (file.fail())
	{
		cout << "file is wrong" << endl;
		return;
	}
	map<int, pair<int, string> >::iterator itr;
	for (itr = this->BufferSpace.begin(); itr != this->BufferSpace.end(); ++itr)
	{
		file.seekp(itr->second.first);// change the right position
		int size = itr->second.second.size();
		file.write(reinterpret_cast<char *>(&size), sizeof(size));
		string tmp = itr->second.second;
		file.write(tmp.c_str(), size + 1);
	}
	file.close();
}

int Buffer::GetBufferSize() const
{
	return this->BufferSpace.size();
}

void Buffer::Insert(int index, int pointer, const string & str)// just add
{
	this->BufferSpace[index] = make_pair(pointer,str);
	if (this->BufferSpace.size() > BufferSize)
	{
		this->flush();
	}
}

void Buffer::flush()
{
	this->Wrtite(filepath);
	this->BufferSpace.clear();
}

void Buffer::flush(const string & path)
{
	this->Wrtite(path);
	this->BufferSpace.clear();
}

void Buffer::Remove(int id)
{
	this->BufferSpace.erase(id);
}

string Buffer::Find(int id)
{
	map<int, pair<int, string>>::iterator itr = this->BufferSpace.begin();
	while (itr != this->BufferSpace.end())
	{
		if (itr->first == id)
		{
			return itr->second.second;
		}
		++itr;
	}
	return "";
}

map<int, pair<int,string> >& Buffer::GetBufferSpace()
{
	return this->BufferSpace;
}
