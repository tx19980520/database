#pragma once
#ifndef _DATABASE_H
#define _DATABASE_H
#include <string>
#include <map>
#include <vector>
#include <cstdio>
#include "Cache.h"
#include "Buffer.h"
#include "BuddyManager.h"
#include "BplusTree.h"
class DataBase{
public:
	/* print a single data*/
	static void Print(pair<int, string>);
	/* print plenty of data */
	static void Print(vector<pair<int, string> >);
	/* dump the DataBaseManager to db.default */
	static void Dump();
	/* read db.default and init the system */
	static void Init();
	/* check whether the db exists */
	static bool HasDataBase(const string&);
	/* delete a DataBase */
	static void DeleteDataBaseByName(const string&);
	/* add a DataBase */
	static void AddDataBase(const string&, const string&);
	/* static function to realize the multiton */
	static DataBase* GetDataBaseByName(const string&);
	/* close whole DB */
	static void close(DataBase*);
	/* insert one data */
	int InsertOne(int, const string&);
	/* delete by key */
	int RemoveOne(int);
	/* modify a data by primary key */
	int ModifyOne(int, const string&);
	/* find a data by primary key */
	pair<int, string> FindOne(int);
	/* find data by the rule of string */
	vector<pair<int, string> > FindMany(int low, int high);
	/* check both buffer and cache flush */
	void FlushCheck(bool force = false);
private:
	/* DB init */
	DataBase(const string& name, const string& path);
	/* DB delete */
	~DataBase();
	/* B+ tree */
	BplusTree * index;
	/* store dbpath in memory */
	static map<string, string> DataBaseManager;
	/* buffer controls the modify and insert added */
	Buffer buffer;
	/* cache stores the find and is updated by DB and buffer */
	Cache cache;
	/* BM manages the fragements waiting for reusing */
	BuddyManager *BM;
	/* DataBaseManager store the path of the specific store */
	string dbname;
	string dbpath;
	int save();

	/* find data in cache, many times some data still can't be find in the cache,so the 2ed parameter has output the ids can't find*/
	pair<vector<pair<int, string> >,vector<int> > FindManyInCache(vector<int>);
	/* open the file */
	vector<pair<int, string> > FindManyInFile(vector<pair<int,int> >);
	/* find pointer in B+ */
	int FindOneInTree(int a);
	/* find data in cache */
	pair<int, string> FindOneInCache(int);
	/* open the file */
	pair<int, string> FindOneInFile(int, int);
};
#endif // !_DATABASE_H

