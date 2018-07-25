#include <iostream>
#include <fstream>
#include<algorithm>
#include <direct.h>
#include <cstdio>
#include "DataBase.h"
#include "BplusTree.h"
#include "constant.h"
using namespace std;
bool judge(const pair<int, string> a, const pair<int, string> b) {
	return a.first<b.first;
}
DataBase::DataBase(const string& name, const string& path)
{
	dbname = name;
	dbpath = path;
	BM = new BuddyManager(dbpath + "\\" + dbname + "\\BM.txt");
	index = new BplusTree(dbpath + "\\" + dbname + "\\" + dbname + ".idx");
	buffer.SetFilePath(dbpath + "\\" + dbname + "\\" + dbname + ".dat");
}

DataBase::~DataBase()
{
	this->save();
	delete BM;
	delete index;
}

int DataBase::save()
{
	this->BM->Serialize();
	this->buffer.flush();
	this->cache.flush();
	return 1;
}

int DataBase::FindOneInTree(int id)
{
	return this->index->Find(id);
}


pair<vector<pair<int, string> >, vector<int> > DataBase::FindManyInCache(vector<int> IdArray)
{
	int size = IdArray.size();
	pair<int, string>tmp;
	vector<int> rest;
	vector<pair<int, string> > Found;
	for (int i = 0; i < size; ++i)
	{
		tmp = this->FindOneInCache(IdArray[i]);
		if (tmp.second == "")
		{
			rest.push_back(IdArray[i]);
		}
		else {
			Found.push_back(tmp);
		}
	}
	return make_pair(Found, rest);
}

vector<pair<int, string> > DataBase::FindManyInFile(vector<pair<int, int> > Info)
{
	int size = Info.size();
	vector<pair<int, string> > Found;
	for (int i = 0; i < size; ++i)
	{
		Found.push_back(this->FindOneInFile(Info[i].first, Info[i].second));
	}
	return Found;
}

pair<int, string> DataBase::FindOneInCache(int id)
{
	return this->cache.Find(id);
}

pair<int, string> DataBase::FindOneInFile(int id, int pos)
{
	/* I will change the temporary setting after the system work */
	ifstream check(dbpath+"\\" + dbname + "\\"+dbname+".dat", ifstream::in | ifstream::binary);
	check.seekg(pos);
	char *tmp;
	int len;
	check.read(reinterpret_cast<char *>(&len), sizeof(len));
	tmp = new char[len + 1];
	check.read(tmp, len + 1);
	tmp[len] = '\0';
	string result = tmp;
	return make_pair(id, result);
}

pair<int, string> DataBase::FindOne(int id)
{
	pair<int, string>result = this->FindOneInCache(id);
	if (result.second != "")
		return result;
	result = make_pair(id, this->buffer.Find(id));
	if (result.second != "")
	{
		cache.Update(result);
		return result;
	}
	int pos = this->FindOneInTree(id);
	if (pos == -1)
	{
		return make_pair(-1, "");
	}
	result = this->FindOneInFile(id, pos);
	this->cache.Update(result);
	return result;
}

vector<pair<int, string>> DataBase::FindMany(int low, int high)
{
	this->buffer.flush();
	vector<pair<int, int> > offsets = this->index->FindMany(low, high);
	// first find in the cache
	vector<pair<int, string> >result;
	int s = offsets.size();
	if (s == 0)
	{
		result.clear();
		return result;
	}
	vector<pair<int, string> >FileResult;
	// If offsets is empty the FileResult is empty
	FileResult = this->FindManyInFile(offsets);
	result.insert(result.begin(), FileResult.begin(), FileResult.end());
	sort(result.begin(), result.end(), judge);
	return result;
	
}

void DataBase::close(DataBase* db)
{
	delete db;
}

void DataBase::FlushCheck(bool force)
{
	this->save();
}

void DataBase::Print(pair<int, string> data)
{
	cout << "   key          value" << endl;
	cout << "-------------------------" << endl;
	cout << "   " << data.first << "          " << data.second << endl;
}

void DataBase::Print(vector<pair<int, string> > data)
{
	cout << "   key          value" << endl;
	cout << "-------------------------" << endl;
	int len = data.size();
	for (int i = 0; i < len; ++i)
	{
		cout << "   " << data[i].first << "          " << data[i].second << endl;
	}
}
void DataBase::Dump()
{
	fstream db("C:\\Users\\ty020\\Desktop\\db\\database\\db\\Release\\db.default", ios::in | ios::trunc | ios::out);
	int len = DataBaseManager.size();
	string str;
	map<string, string>::iterator itr;
	for (itr = DataBaseManager.begin(); itr != DataBaseManager.end(); ++itr)
	{
		str += itr->first;
		str += " ";
		str += itr->second;
		str += '\n';
	}
	db << str;
	db.close();
}

void DataBase::Init()
{
	fstream db("C:\\Users\\ty020\\Desktop\\db\\database\\db\\Release\\db.default", ios::in | ios::out);
	db.seekg(0, db.end);
	int length = db.tellg();
	db.seekg(0);
	if (length == 0)
	{
		db.close();
		return;
	}
	string str;
	string key = "";
	string value = "";
	while (getline(db, str))
	{
		int len = str.size();
		int pos = 0;
		while (true)
		{
			if (str[pos] != ' ')
			{
				key += str[pos];
				pos++;
			}
			else {
				pos++;
				break;
			}
		}
		for (int i = pos; i < len; ++i)
		{
			value += str[i];
		}
		DataBaseManager[key] = value;
		key = "";
		value = "";
	}
	db.close();
}

bool DataBase::HasDataBase(const string &dbname)
{
	map<string, string>::iterator itr;
	for (itr = DataBaseManager.begin(); itr != DataBaseManager.end(); ++itr)
	{
		if (itr->first == dbname)
		{
			return true;
		}
	}
	return false;
}

void DataBase::DeleteDataBaseByName(const string& dbname)
{
	map<string, string>::iterator itr;
	for (itr = DataBaseManager.begin(); itr != DataBaseManager.end(); ++itr)
	{
		if (itr->first == dbname)
		{
			string allpath = itr->second + "\\" + dbname;
			string dat = allpath + "\\"+ dbname + ".dat";
			string idx = allpath + "\\"+ dbname + ".idx";
			remove(dat.c_str());
			remove(idx.c_str());
			_rmdir(allpath.c_str());
			DataBaseManager.erase(itr);
			return;
		}
	}
}

void DataBase::AddDataBase(const string & dbname, const string& dbpath)
{
	DataBaseManager[dbname] = dbpath;
	string dirpath = dbpath + "\\" + dbname;
	_mkdir(dirpath.c_str());
	fstream dat(dbpath + "\\"+ dbname +"\\"+dbname+".dat", ios::out);
	dat.close();
	fstream idx(dbpath + "\\" + dbname + "\\" + dbname + ".idx", ios::out);
	idx.close();
	fstream BM(dbpath + "\\" + dbname + "\\BM.txt", ios::out);
	BM.close();
}

DataBase * DataBase::GetDataBaseByName(const string &dbname)
{
	map<string, string>::iterator itr;
	for (itr = DataBaseManager.begin(); itr != DataBaseManager.end(); ++itr)
	{
		if (itr->first == dbname)
		{
			return new DataBase(itr->first, itr->second);
		}
	}
	return nullptr;
}

int DataBase::InsertOne(int id, const string &data)
{
	int pos = this->BM->Malloc(data.size() + 5);// add int + '/0'
	// refresh the buffer and cache
	buffer.Insert(id, pos, data);
	cache.Update(make_pair(id, data));
	return this->index->Insert(id, pos, data.size() + 5);
}


int DataBase::RemoveOne(int id)
{
	//   pos, size
	pair<int, int>data = this->index->Remove(id);// return the pos
	if (data.first == -1)
	{
		return -1;
	}
	this->BM->Free(data.first, data.second);
	// refresh buffer and cache
	this->buffer.Remove(id);
	this->cache.Update(make_pair(id, ""));
	return 1;
}

int DataBase::ModifyOne(int id, const string & data)
{	
	LeafNode* node = this->index->_Find(id);
	int pos = node->Find(id);
	if (pos == -1)
	{
		return -1;
	}
	this->BM->Free(node->data[pos].offset, node->data[pos].length);
	int NewPos = this->BM->Malloc(data.size() + 5);
	this->index->Modify(node, id, NewPos, data.size() + 5);
	cache.Update(make_pair(id, data));
	buffer.Insert(id, NewPos, data);
	return 1;
}


map<string, string> DataBase::DataBaseManager;
