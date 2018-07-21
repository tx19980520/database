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
/* BUFFER_LIMIT_SIZE is the upper bound to deal with it and clear it */
#define BUFFER_LIMIT_SIZE 30;
/* CACHE_LIMIT_SIZE is the upper bound to deal with it and clear it */
#define CACHE_LIMIT_SIZE 30;
class DataBase{
public:
	/* save the current state to the file */
	static void Print(pair<int, string>);
	static void Print(vector<pair<int, string> >);
	static void Dump();
	static void Init();
	static map<string, string> DataBaseManager;
	static void DeleteDataBaseByName(const string&);
	static void AddDataBase(const string&, const string&);
	static DataBase* GetDataBaseByName(const string&);/* static function to realize the multiton*/
	static void close(DataBase*);/* close whole DB */
	int InsertOne(int, const string&); /* insert one data */
	int InsertMany(vector<string>&); /* insert many data */
	int RemoveOne(int);/* delete by key */
	int RemoveMany(vector<int>);/* delete a data by primary key */
	int ModifyOne(int, const string&); /* modify a data by primary key */
	int ModifyMany(vector<pair<int, string> >&);/* modify data by primary key */
	pair<int, string> FindOne(int);/* find a data by primary key */
	vector<pair<int, string>> FindMany(int low, int high);/* find data by the rule of string */
	void FlushCheck(bool force = false);/* check both buffer and cache flush */
private:
	/* DB init */
	DataBase(const string& name, const string& path);
	/* DB delete */
	~DataBase();
	/* buffer controls the modify and insert added */
	Buffer buffer;
	/* cache stores the find and is updated by DB and buffer */
	Cache cache;
	/* BM manages the fragements waiting for reusing */
	BuddyManager *BM;
	/* DataBaseManager store the path of the specific store */
	BplusTree* index;
	string dbname;
	string dbpath;
	int save();


	/*   FIND START   */
	vector<int> FindManyInTree(vector<int>);/* find pointer in B+ */
	pair<vector<pair<int, string> >,vector<int> > FindManyInCache(vector<int>);
	/* find data in cache, many times some data still can't be find in the cache,so the 2ed parameter has output the ids can't find*/
	vector<pair<int, string> > FindManyInFile(vector<pair<int,int> >);/* open the file */
	int FindOneInTree(int a);/* find pointer in B+ */
	pair<int, string> FindOneInCache(int);/* find data in cache */
	pair<int, string> FindOneInFile(int, int);/* open the file */
	/*   FIND END   */

	/*   MODIFY START   */
	/*   MODIFY END   */

	/*   REMOVE START   */
	/*   REMOVE END   */

	/*   ADD START   */
	/*   ADD END   */


};
#endif // !_DATABASE_H

