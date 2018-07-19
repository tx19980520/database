#pragma once

#ifndef _TYDB_H
#define _TYDB_H
#include <string>
#include <map>
#include "DataBase.h"
using namespace std;
/************************
class TyDB is a DB controller, which is used for managing Multiple databases.
mainly it has a map to find the path by the dbname and connect to the db; 
************************/
class TyDB{
public:
	void CreateDB(const string& DbName);/* create a db and related files */
	DataBase* CreateNewConnect(const string& DbName);/* create a connect to a specific db */
private:
	map<string, string> controller;/* record db data */
	DataBase* InitDB(const string& dbname, const string& path);/* init a db from file */
};

#endif // !_TYDB_H

