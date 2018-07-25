#pragma once
#ifndef _BUDDYMANAGER_H
#define _BUDDYMANAGER_H
#include <string>
#include <vector>
#include <stack>
using namespace std;
/************************
class BuddyManager is a fragmentation space manager.
Optimize file storage space
************************/
class BuddyManager{
public:
	/************************
	struct MiddleNode is a middle layer information.
	Convenient organization of information
	************************/
	struct MiddleNode {
		int size;
		int pos;
		int vec_pos;
		MiddleNode(int s, int p, int v) :size(s), pos(p), vec_pos(v) {};
	};
	/* Free a space and add the space into manager*/
	void Free(int pos, int size);
	/* malloc space from manager */
	int Malloc(int size);
	BuddyManager() = default;
	BuddyManager(string path) :BMpath(path) {
		tail = 0;
		fstream fs(BMpath, ios::in | ios::out);
		fs.seekg(0, fs.end);
		int length = fs.tellg();
		if (length <= 0)
		{
			vector<int>empty; 
			manager.push_back(make_pair(1, empty));
		}
		else 
		{
			this->Deserialize();
		}
	};
	~BuddyManager() {};
	void Serialize() { serialize(); }/* the serialize interface for unit test  */ 
	void Deserialize() { deserialize(); }
	vector<pair<int, vector<int> > >& GetManager() { return manager; };
private:
	/* record the tail of the file */
	int tail;
	/* Record the BM filepath */
	string BMpath;
	/* core structure of BuddyManger */
	vector<pair<int, vector<int> > > manager;
	/* an auxiliary function used to convert decimal to binary */
	vector<int> DecToBin(int size);
	vector<MiddleNode> Slice(vector<int>slices,int pos);
	/* An auxiliary function used to segment space */
	int malloc(int size);
	/* private insert, which is a recursive function */
	void insert(int pos, int size);
	/* split a large space for use */
	void split(int pos, int size);
	/* To storage the BuddyManager */
	void serialize();
	/* To init the BuddyManager */
	void deserialize();
};
#endif