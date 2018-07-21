#pragma once
#ifndef _BPLUSTREE_H
#define _BPLUSTRR_H
#include <fstream>
#include <string>
#include <vector>
using namespace std;

/*##############constant statement and definition################*/
enum NODE_TYPE {MIDDLE, LEAF};
const int M = 61;
const int L = 40;
/*##############constant statement and definition################*/


/*
*SplitData is a supply struct for transfor the data
*It contains LeftOffset, RightOffset, middle
*/
struct SplitData {
	int LeftOffset;
	int RightOffset;
	int middle;
	SplitData() = default;
	SplitData(int l, int r, int middle) :LeftOffset(l), RightOffset(r), middle(middle) {};
	~SplitData() {};
};
/*
*TreeNode is the part of the tree
*it is the father of the LeafNode and the MiddleNode
*/
class TreeNode {
public:
	int size;
	int parent;
	int RightSibling;
	int LeftSibling;
	NODE_TYPE type;
	TreeNode() {};
	virtual void Merge(fstream& fs, int& root) = 0;
	virtual void Remove(int, fstream&, int site, int& root) = 0;
	virtual void Insert(int, int, int, fstream&, int& tail, int& root) = 0;
	virtual void Modify(int, int, int, fstream&) = 0;
	virtual int Find(int) = 0;
	virtual void Split(SplitData, fstream&, int& tail, int& root) = 0;
	virtual int Borrow(fstream& fs, int pos) = 0;
	virtual void NodeWrite(fstream& fs, int pos) = 0;
	NODE_TYPE GetType() { return type; }
};


/*
*MiddleNode is the part of the tree, which is in the middle of the tree
*has an array to save the child node and a pointer to the right sibling;
*/
class MiddleNode : TreeNode {
	friend class LeafNode;
private:
	void NodeWrite(fstream& fs, int pos);
public:
	vector<int> keys;
	vector<int> offset;
	NODE_TYPE GetType() { return this->type; };
	~MiddleNode() {};
	MiddleNode(fstream& fs, int pos);
	MiddleNode(vector<int>, vector<int>, int parent, int lb, int rb);
	int FindSuit(int id);
	int GetSize() const { return size; };
	int Borrow(fstream& fs, int pos);
	void Merge(fstream& fs, int& root);
	int Find(int);
	void Insert(int, int, int, fstream&, int& tail, int& root);
	void Remove(int, fstream&, int site, int& root);
	void Modify(int, int, int, fstream&);
	void Split(SplitData, fstream&, int& tail, int& root);
	int GetSize() { return size; }
	int FindOffset(int offset);
	int FindKey(int id);
};


/*
*LeafData is a supply struct for storage the data 
*It contains id, offset, length
*/
struct LeafData {
	int id;
	int offset;
	int length;
	LeafData() = default;
	LeafData(int id, int offset, int length) :id(id), offset(offset), length(length) {};
	~LeafData() {};
};
/*
*LeafNode is the part of the tree, which is at the bottom of the tree
*has an array to save the specific data and two pointers to point the right sibling and the left sibling
*/
class LeafNode: TreeNode {
	friend class MiddleNode;
private:
	void NodeWrite(fstream& fs, int pos);
public:
	vector<LeafData> data;
	NODE_TYPE GetType() { return this->type; };
	~LeafNode() {};
	LeafNode(fstream& fs, int pos);
	LeafNode(vector<LeafData>, int pointer, int parent, int lb, int rb);
	int Borrow(fstream& fs, int pos);
	void Merge(fstream& fs, int& root);
	int Find(int);
	void Insert(int, int, int, fstream&, int& tail, int& root);
	void Remove(int, fstream&, int site, int& root);
	void Modify(int, int, int, fstream&);
	void Split(SplitData, fstream&, int& tail, int& root);
	/* extra function */
	int GetSize() { return size; }

};



/*
*BplusTree is the core of index
*TreeNode make the BplusTree
*It has CURD methods.
*/
class BplusTree {
private:
	fstream fs;
	int root;
	LeafNode* FindCore(int id);
	int tail;
public:
	BplusTree(const string& path);
	~BplusTree();
	int Find(int);
	LeafNode * _Find(int);/* Find a node by id */
	int Insert(int id, int index, int size);/* insert a node */
	pair<int, int> Remove(int id);/* delete a node */
	void Modify(int, int, int);
	string Bpath;
	int GetRoot() { return root; }

};
#endif // !_BPLUSTRR_H
