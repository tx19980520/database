// db.cpp: 定义控制台应用程序的入口点。
//
#include <ctime>
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
	/*
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
	*/
	/*clock_t start, end;
	fstream vv("C:\\DataBase\\darling\\darling.idx", ios::out | ios::trunc);
	vv.close();
	fstream sf("C:\\DataBase\\darling\\BM.txt", ios::out | ios::trunc);
	sf.close();
	fstream sss("C:\\DataBase\\darling\\darling.dat", ios::out | ios::trunc);
	sss.close();
	DataBase::Init();
	DataBase *db = DataBase::GetDataBaseByName("darling");
	start = clock();
	for (int i = 0; i < 100000; ++i)
	{
		db->InsertOne(i, "fuck" + to_string(i));
	}
	end = clock();
	cout << "Insert" << endl;
	cout << (double)(end - start) / CLOCKS_PER_SEC << endl;
	start = clock();
	for (int i = 0; i < 100000; ++i)
	{
		db->RemoveOne(i);
	}
	end = clock();
	cout << "Remove" << endl;
	cout << (double)(end - start) / CLOCKS_PER_SEC << endl;
	start = clock();
	for (int i = 0; i < 100000; ++i)
	{
		db->InsertOne(i, "shit" + to_string(i));
	}
	end = clock();
	cout << "Insert Again" << endl;
	cout << (double)(end - start) / CLOCKS_PER_SEC << endl;
	start = clock();
	for (int i = 0; i < 100000; ++i)
	{
		db->FindOne(i).second;
	}
	end = clock();
	cout << "Find" << endl;
	cout << (double)(end - start) / CLOCKS_PER_SEC << endl;
	start = clock();
	DataBase::close(db);
	DataBase::Dump();
	return 0;
	*/



	fstream vv("D:\\DataBase\\darling\\darling.idx", ios::out | ios::trunc);
	vv.close();
	fstream sf("D:\\DataBase\\darling\\BM.txt", ios::out | ios::trunc);
	sf.close();
	fstream sss("D:\\DataBase\\darling\\darling.dat", ios::out | ios::trunc);
	sss.close();
	DataBase::Init();
	DataBase *db = DataBase::GetDataBaseByName("darling");
	map<int, string> TestSet;
	vector<int> keys;
	for (int i = 0; i < 10000; ++i)
	{
		TestSet[i] = "fuck" + to_string(i);
		keys.push_back(i);
		db->InsertOne(i, "fuck" + to_string(i));
	}
	for (int i = 0; i < 100000; ++i)
	{
		srand((unsigned)time(NULL));
		int f = rand() % (int)keys.size();
		if(db->FindOne(keys[f]).second  != TestSet[keys[f]])
		{
			return 0;
		}
		if (i % 37 == 0 && i != 0)
		{
			int f = rand() % (int)keys.size();
			db->RemoveOne(keys[f]);
			TestSet.erase(keys[f]);
			keys.erase(keys.begin() + f);
		}
		else if (i % 11 == 0 && i != 0)
		{
			int f = rand() % 10000 + 10001;
			keys.push_back(f);
			db->InsertOne(f, "fuck" + to_string(f));
			TestSet[f] = "fuck" + to_string(f);
			if (TestSet[f] !=db->FindOne(f).second)
			{
				return 0;
			}
		}
		else if (i % 17 == 0 && i != 0)
		{
			int f = rand() % (int)keys.size();
			TestSet[keys[f]] = "shit" + to_string(keys[f]);
			db->ModifyOne(keys[f], TestSet[keys[f]]);
		}

	}
	return 0;
}

