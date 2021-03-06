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
#include "constant.h"
using namespace std;

int main()
{
	DataBase::Init();
	DataBase* db = NULL;
	cout << "welcome!" << endl;
	while (true)
	{
		cout << "$ ";
		string command;
		cin >> command;
		if (command == "add")
		{
			string dbname;
			cin >> dbname;
			if (DataBase::HasDataBase(dbname))
			{
				cout << "The db having the same name has existed." << endl;
			}
			else 
			{
				cout << "Input the absolute path for the DB(C:\\xxxx):";
				string dbpath;
				cin >> dbpath;
				DataBase::AddDataBase(dbname, dbpath);
				cout << "success!" << endl;
			}
			continue;
		}
		if (command == "delete")
		{
			string dbname;
			cin >> dbname;
			if (DataBase::HasDataBase(dbname))
			{
				DataBase::DeleteDataBaseByName(dbname);
				cout << "success!" << endl;
			}
			else 
			{
				cout << "The db using the name doesn't exist." << endl;
			}
			continue;
		}
		if (command == "enter")
		{
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
			DataBase::Dump();
			break;
		}
		cout << "wrong command" << endl;
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
			else 
			{
				DataBase::Print(result);
			}
			continue;
		}
		if (sentence == "findmany")
		{
			int low;
			int high;
			cin >> low;
			cin >> high;
			vector<pair<int, string> > result = db->FindMany(low, high);
			DataBase::Print(result);
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
			int result = db->ModifyOne(id, data);
			if (result == -1)
			{
				cout << "the primary key is not found." << endl;
			}
			continue;
		}
		if (sentence == "remove")
		{
			int id;
			cin >> id;
			int result = db->RemoveOne(id);
			if(result == -1)
			{
				cout << "the primary key is not found." << endl;
			}

			continue;
		}
		if (sentence == "flush")
		{
			db->FlushCheck();
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
	system("Pause");
	return 0;

	/*
	clock_t start, end;
	fstream vv("C:\\DataBase\\darling\\darling.idx", ios::out | ios::trunc);
	vv.close();
	fstream sf("C:\\DataBase\\darling\\BM.txt", ios::out | ios::trunc);
	sf.close();
	fstream sss("C:\\DataBase\\darling\\darling.dat", ios::out | ios::trunc);
	sss.close();
	DataBase::Init();
	DataBase *db = DataBase::GetDataBaseByName("darling");
	for (int i = 0; i < 200000; ++i)
	{
		db->InsertOne(i, "fuck" + to_string(i));
	}
	start = clock();
	for (int i = 200000; i < 1000 + 200000; ++i)
	{
		db->InsertOne(i, "fuck" + to_string(i));
	}
	end = clock();
	cout << "Insert" << endl;
	cout << (double)(end - start) / CLOCKS_PER_SEC << endl;


	start = clock();
	for (int i = 0; i < 1000; ++i)
	{
		db->ModifyOne(i, "mun");
	}
	end = clock();
	cout << "modify" << endl;
	cout << (double)(end - start) / CLOCKS_PER_SEC << endl;

	start = clock();
	for (int i = 0; i < 1000; ++i)
	{
		db->FindOne(i);
	}
	end = clock();
	cout << "find" << endl;
	cout << (double)(end - start) / CLOCKS_PER_SEC << endl;

	start = clock();
	for (int i = 0; i < 1000; ++i)
	{
		db->RemoveOne(i);
	}
	end = clock();
	cout << "Remove" << endl;
	cout << (double)(end - start) / CLOCKS_PER_SEC << endl;


	DataBase::close(db);
	DataBase::Dump();
	system("Pause");
	return 0;
	*/
	/*
	fstream vv("C:\\DataBase\\darling\\darling.idx", ios::out | ios::trunc);
	vv.close();
	fstream sf("C:\\DataBase\\darling\\BM.txt", ios::out | ios::trunc);
	sf.close();
	fstream sss("C:\\DataBase\\darling\\darling.dat", ios::out | ios::trunc);
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
	*/
	/*
	fstream vv("C:\\DataBase\\darling\\darling.idx", ios::out | ios::trunc);
	vv.close();
	fstream sf("C:\\DataBase\\darling\\BM.txt", ios::out | ios::trunc);
	sf.close();
	fstream sss("C:\\DataBase\\darling\\darling.dat", ios::out | ios::trunc);
	sss.close();
	DataBase::Init();
	DataBase *db = DataBase::GetDataBaseByName("darling");
	
	int turns = 20000;
	clock_t start, end;
	start = clock();
	for (int i = 0; i < turns; i += 1)
	{
		db->InsertOne(i, "fuck" + to_string(i));
	}
	end = clock();
	double result2 = (double)(end - start) / CLOCKS_PER_SEC;
	cout << "insert:" << result2 << endl;


	start = clock();
	for (int i = turns - 1; i >= turns - 100; --i)
	{
		db->RemoveOne(i);
	}
	end = clock();
	result2 = (double)(end - start) / CLOCKS_PER_SEC;
	cout << "Remove:" << result2 << endl;
	
	
	DataBase::close(db);
	system("Pause");
	return 0;
	clock_t start, end;
	double total = 0;
	int check = 5000;
	for (int n = 3584; n < 4608; n += 32)
	{
		total = 0;
		for (int i = 0; i < 5000; i+= n)
		{
			string c = "";
			for (int i = 0; i < n; ++i)
			{
				c += "a";
			}
			start = clock();
			fstream sss("D:\\DataBase\\darling\\darling.dat", ios::out | ios::trunc | ios::binary);
			sss.write(c.c_str(), c.size() + 1);
			sss.flush();
			sss.close();
			end = clock();
			total += (double)(end - start);
		}
		cout << "n = " << n << " average:" << total / 5000 << endl;
	}
	system("Pause");
	return 0;
	*/
}

