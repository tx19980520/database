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
	void Free(int pos, int size);/* Free a space and add the space into manager*/
	int Malloc(int size);/* malloc space from manager */
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
		else {
			this->Deserialize();
		}
	};
	~BuddyManager() {};
	void Serialize() { serialize(); }/* the serialize interface for unit test  */ 
	void Deserialize() { deserialize(); }
	vector<pair<int, vector<int> > >& GetManager() { return manager; };
private:
	int tail;/* record the tail of the file */
	string BMpath;/* Record the BM filepath */
	vector<pair<int, vector<int> > > manager;/* core structure of BuddyManger */
	vector<int> DecToBin(int size);/* an auxiliary function used to convert decimal to binary */
	vector<MiddleNode> Slice(vector<int>slices,int pos);/*  */
	int malloc(int size);/* An auxiliary function used to segment space */
	void insert(int pos, int size);/* private insert, which is a recursive function */
	void split(int pos, int size);/* split a large space for use */
	void serialize();/* To storage the BuddyManager */
	void deserialize();/* To init the BuddyManager */
};
#endif