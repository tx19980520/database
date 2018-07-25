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
	static void Print(pair<int, string>);// print a single data
	static void Print(vector<pair<int, string> >);// print plenty of data
	static void Dump();// dump the DataBaseManager to db.default
	static void Init();// read db.default and init the system
	static bool HasDataBase(const string&);// check whether the db exists;
	static void DeleteDataBaseByName(const string&);// delete a DataBase
	static void AddDataBase(const string&, const string&);// add a DataBase
	static DataBase* GetDataBaseByName(const string&);/* static function to realize the multiton*/
	static void close(DataBase*);/* close whole DB */
	int InsertOne(int, const string&); /* insert one data */
	int RemoveOne(int);/* delete by key */
	int ModifyOne(int, const string&); /* modify a data by primary key */
	pair<int, string> FindOne(int);/* find a data by primary key */
	vector<pair<int, string> > FindMany(int low, int high);/* find data by the rule of string */
	void FlushCheck(bool force = false);/* check both buffer and cache flush */
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


	/*   FIND START   */
	pair<vector<pair<int, string> >,vector<int> > FindManyInCache(vector<int>);
	/* find data in cache, many times some data still can't be find in the cache,so the 2ed parameter has output the ids can't find*/
	vector<pair<int, string> > FindManyInFile(vector<pair<int,int> >);/* open the file */
	int FindOneInTree(int a);/* find pointer in B+ */
	pair<int, string> FindOneInCache(int);/* find data in cache */
	pair<int, string> FindOneInFile(int, int);/* open the file */


};
#endif // !_DATABASE_H

