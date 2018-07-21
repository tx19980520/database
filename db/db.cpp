// db.cpp: 定义控制台应用程序的入口点。
//
#include <iostream>
#include <sstream>
#include <string>
#include "DataBase.h" 
#include "Cache.h"
#include "Buffer.h"
#include "BuddyManager.h"
#include "stdafx.h"
#include "direct.h"
using namespace std;

int main()
{
	DataBase::Init();
	DataBase* db = NULL;
	while (true)
	{
		cout << "please enter a db name:";
		cout.flush();
		string dbname;
		string answer;
		cin >> dbname;
		db = DataBase::GetDataBaseByName(dbname);
		if (db == NULL)
		{
			cout << "Are you sure you want to create a new database(Y/N)";
			cout.flush();
			cin >> answer;
			int len = answer.size();
			if (len == 1 && (answer == "Y" || answer == "y"))
			{
				cout << "Input the absolute path for the DB(C:\\xxxx):";
				cout.flush();
				cin >> answer;
				DataBase::AddDataBase(dbname, answer);
				db = DataBase::GetDataBaseByName(dbname);
				cout << "enter DB " << dbname << endl;
				break;
			}
			continue;
		}
		cout << "enter DB " << dbname << endl;
		break;
	}
	while (db)
	{
		string sentence;
		cout << ">>";
		cout.flush();
		cin >> sentence;
		if (sentence == "find")
		{
			int id;
			cin >> id;
			pair<int, string> result = db->FindOne(id);
			if (result.first == -1)
			{
				cout << "the primary key is not found." << endl;
			}
			else {
				DataBase::Print(result);
			}
			continue;
		}
		if (sentence == "insert")
		{
			int id;
			string data;
			cin >> id;
			cin >> data;
			db->InsertOne(id, data);
			continue;
		}
		if (sentence == "modify")
		{
			int id;
			string data;
			cin >> id;
			cin >> data;
			db->ModifyOne(id, data);
			continue;
		}
		if (sentence == "remove")
		{
			int id;
			cin >> id;
			db->RemoveOne(id);
			continue;
		}
		if (sentence == "-q")
		{
			DataBase::close(db);
			DataBase::Dump();
			return 0;
		}
		cout << "wrong command" << endl;
		continue;
	}
	return 0;
}

