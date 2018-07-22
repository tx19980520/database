#include "stdafx.h"
#include "CppUnitTest.h"
#include <set>
#include<map>
#include<string>
#include<cstdlib>
#include<ctime>
#include<iostream>
#include <sstream>
#include<fstream>
#include <algorithm> 
#include "string.h"
#include "../db/Buffer.h"
#include "../db/Buffer.cpp"
#include "../db/Cache.h"
#include "../db/Cache.cpp"
#include "../db/BuddyManager.h"
#include "../db/BuddyManager.cpp"
#include "../db/BplusTree.h"
#include "../db/BplusTree.cpp"
#include "../db/DataBase.h"
#include "../db/DataBase.cpp"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTest
{		
	TEST_CLASS(SMALL_COMPONENT_TEST)
	{
	public:
		
		TEST_METHOD(BUFFER_SINGLE)
		{
			Buffer buffer;
			string str1 = "hi";
			string str2 = "fuck";
			buffer.Insert(0,0, str1);
			buffer.flush("./fuck.dat");
			ifstream check("./fuck.dat", ifstream::in | ifstream::binary);
			if (check.fail())
			{
				cout << "文件读取失败" << endl;
			}
			int len = 0;
			char *tmp;
			check.read(reinterpret_cast<char *>(&len), sizeof(len));
			tmp = new char[len + 1];
			check.read(tmp, len + 1);
			Assert::AreEqual("hi", tmp);
		}

		TEST_METHOD(BUFFER_MULTI)
		{
			Buffer buffer;
			string str1 = "hi";
			string str2 = "fuck";
			buffer.Insert(0,0, str1);
			buffer.Insert(1,7, str2);
			buffer.flush("./fuck.dat");
			ifstream check("./fuck.dat", ifstream::in | ifstream::binary);
			if (check.fail())
			{
				cout << "文件读取失败" << endl;
			}
			int len = 0;
			char *tmp;
			check.read(reinterpret_cast<char *>(&len), sizeof(len));
			tmp = new char[len + 1];
			check.read(tmp, len + 1);
			delete[] tmp;

			char *tmp2;
			int len2 = 0;
			check.read(reinterpret_cast<char *>(&len2), sizeof(len2));
			tmp2 = new char[len2 + 1];
			check.read(tmp2, len2 + 1);
			Assert::AreEqual("fuck", tmp2);
			delete[] tmp2;
		}

		TEST_METHOD(CACHE_UPDATE_SEARCH)
		{
			Buffer buffer;
			string str1 = "hi";
			string str2 = "fuck";
			buffer.Insert(0,0, str1);
			buffer.Insert(1,7, str2);
			buffer.flush("./fuck.dat");
			ifstream check("./fuck.dat", ifstream::in | ifstream::binary);
			if (check.fail())
			{
				cout << "file error" << endl;
			}
			int len = 0;
			char *tmp;
			check.read(reinterpret_cast<char *>(&len), sizeof(len));
			tmp = new char[len + 1];
			check.read(tmp, len + 1);

			char *tmp2;
			int len2 = 0;
			check.read(reinterpret_cast<char *>(&len2), sizeof(len2));
			tmp2 = new char[len2 + 1];
			check.read(tmp2, len2 + 1);
			cout << tmp2;
			Cache cache;
			cache.Update(make_pair(0, tmp));
			cache.Update(make_pair(1, tmp2));
			string stmp1 = tmp;
			string stmp2 = tmp2;
			Assert::AreEqual("hi", cache.Find(0).second.c_str());
			Assert::AreEqual("fuck", cache.Find(1).second.c_str());
		}

		TEST_METHOD(CACHE_BUFFER_UPDATE)
		{
			Buffer buffer;
			string str1 = "hi";
			string str2 = "fuck";
			buffer.Insert(0,0, str1);
			buffer.Insert(1,7, str2);
			Cache cache;
			cache.Update(make_pair(0, str1));
			cache.Update(make_pair(1, str2));
			const char* p = cache.Find(0).second.c_str();
			Assert::AreEqual("hi", cache.Find(0).second.c_str());
			Assert::AreEqual("fuck", cache.Find(1).second.c_str());
		}

		TEST_METHOD(BM_SIMPLE_MALLOC)
		{
			BuddyManager BM("BMtestEmpty.txt");
			BM.Free(0, 4);
			BM.Free(7, 4);
			int pos = BM.Malloc(4);
			
			Assert::AreEqual(7, pos);
		}
		TEST_METHOD(BM_SIMPLE_FREE)
		{
			BuddyManager BM("BMtestEmpty.txt");
			BM.Free(0, 4);
			BM.Free(7, 4);
			vector<pair<int, vector<int> > >& manager = BM.GetManager();
			Assert::AreEqual(manager[0].first, 1);
			Assert::AreEqual(manager[1].first, 2);
			Assert::AreEqual(manager[2].first, 4);
			Assert::AreEqual((int)(manager[2].second.size()), 2);
		}
		TEST_METHOD(BM_ANNOYING_FREE)
		{
			BuddyManager BM("BMtestEmpty.txt");
			BM.Free(0, 9);
			BM.Free(12, 5);
			vector<pair<int, vector<int> > >& manager = BM.GetManager();
			Assert::AreEqual((int)manager[0].second.size(), 2);
			Assert::AreEqual((int)manager[1].second.size(), 0);
			Assert::AreEqual((int)manager[2].second.size(), 1);
			Assert::AreEqual((int)manager[3].second.size(), 1);
		}
		TEST_METHOD(BM_COMPLEX_FREE)
		{
			BuddyManager BM("BMtestEmpty.txt");
			BM.Free(0, 1);
			BM.Free(2, 2);
			BM.Free(4, 4);
			BM.Free(1, 1);
			BM.Free(12, 4);
			BM.Free(8, 4);
			vector<pair<int, vector<int> > >& manager = BM.GetManager();
			Assert::AreEqual((int)manager[0].second.size(), 0);
			Assert::AreEqual((int)manager[1].second.size(), 0);
			Assert::AreEqual((int)manager[2].second.size(), 0);
			Assert::AreEqual((int)manager[4].second[0], 0);
			Assert::AreEqual((int)manager[4].second.size(), 1);
		}
		TEST_METHOD(BM_ANNOYING_MALLOC)
		{
			BuddyManager BM("BMtestEmpty.txt");
			BM.Free(0, 4);
			BM.Malloc(2);
			BM.Malloc(8);
			int pos = BM.Malloc(8);
			vector<pair<int, vector<int> > >& manager = BM.GetManager();
			Assert::AreEqual((int)manager[1].second[0], 2);
			Assert::AreEqual(pos, 8);
		}

		TEST_METHOD(BM_COMPLEX_MALLOC)
		{
			BuddyManager BM("BMtestEmpty.txt");
			BM.Free(0, 4);
			BM.Malloc(2);
			BM.Malloc(8);
			int pos = BM.Malloc(8);
			vector<pair<int, vector<int> > >& manager = BM.GetManager();
			Assert::AreEqual((int)manager[1].second.size(), 1);
			Assert::AreEqual(pos, 8);
		}

		TEST_METHOD(BM_DESERIALIZE)
		{
			BuddyManager BM("BMtestD.txt");
			vector<pair<int, vector<int> > >& manager = BM.GetManager();
			Assert::AreEqual((int)manager[1].second.size(), 1);
			Assert::AreEqual((int)manager[2].second.size(), 2);
		}

		TEST_METHOD(BM_SERIALIZE)
		{
			fstream c("BMtest.txt", ios::in | ios::out | ios::trunc);
			c.close();
			BuddyManager BM("BMtest.txt");
			BM.Free(0, 4);
			BM.Free(7, 4);
			BM.Free(11, 2);
			BM.Serialize();
			ifstream fs("BMtest.txt", ios::in);
			if (fs.fail())
			{
				cout << "error";
				Assert::AreEqual("1%2%11*4%7*0*!", "");
			}
			stringstream buffer;
			buffer << fs.rdbuf();
			string contents(buffer.str());
			Assert::AreEqual("0&1%2%11*4%7*0*!", contents.c_str());
			fs.close();
			fstream clean("BMtest.txt", ios::out | ios::trunc);
			clean.close();
		}

		TEST_METHOD(BM_IDEAL_FREE)
		{
			BuddyManager BM("BMtestEmpty.txt");
			for (int i = 0; i < 100; ++i)
			{
				BM.Free(i*4, 4);
			}
			for (int i = 0; i < 100; ++i)
			{
				int d = BM.Malloc(4);
			}
			vector<pair<int, vector<int> > >& manager = BM.GetManager();
			for (int i = 0; i < 7; ++i)
			{
				Assert::AreEqual((int)manager[7].second.size(), 0);
			}
			
		}

		TEST_METHOD(BM_BUFFER_SIMPLE_COMBINATION)
		{
			BuddyManager BM("BMtestEmpty.txt");
			vector<pair<int, vector<int> > >& manager = BM.GetManager();
			int pos1 = BM.Malloc(8);
			Buffer buffer;
			buffer.Insert(8, pos1, "dog");// word + int pos1 = 0
			int pos2 = BM.Malloc(9);
			buffer.Insert(9, pos2, "duck");// pos2 = 8
			int pos3 = BM.Malloc(13);
			buffer.Insert(13, pos3, "bilibili");// pos3 = 17
			buffer.flush("./fuck.dat");
			Assert::AreEqual(pos1, 0);
			Assert::AreEqual(pos2, 8);
			Assert::AreEqual(pos3, 17);
			BM.Free(pos1, 8);
			int pos4 = BM.Malloc(6);
			Assert::AreEqual(pos4, 0);// pos4 = 0
			buffer.Insert(6, pos4, "m");
			Assert::AreEqual((int)manager[1].second.size(), 1);
			buffer.flush("./fuck.dat");
			//file input
			ifstream check("./fuck.dat", ifstream::in | ifstream::binary);
			if (check.fail())
			{
				cout << "file error" << endl;
			}
			int len = 0;
			char *tmp;
			check.seekg(0);
			check.read(reinterpret_cast<char *>(&len), sizeof(len));
			tmp = new char[len + 1];
			check.read(tmp, len + 1);
			Assert::AreEqual("m", tmp);
			int len2 = 0;
			char *tmp2;
			check.seekg(17);
			check.read(reinterpret_cast<char *>(&len2), sizeof(len2));
			tmp2 = new char[len2 + 1];
			check.read(tmp2, len2 + 1);
			Assert::AreEqual("bilibili", tmp2);
			delete[] tmp2;
			delete[] tmp;
			check.close();
		}



	};
	TEST_CLASS(BPLUSTREE)
	{
		TEST_METHOD(BPLUSTREE_LEAFNODE_INOUT)
		{
			fstream fs("testLeaf.dat", ios::in | ios::out | ios::binary);
			fs.seekp(0);
			vector<int> tmp;
			tmp.push_back(1);
			tmp.push_back(3);
			tmp.push_back(0);
			tmp.push_back(-1);
			tmp.push_back(1);
			tmp.push_back(1);
			tmp.push_back(3);
			tmp.push_back(0);
			tmp.push_back(2);
			tmp.push_back(5);
			tmp.push_back(8);
			tmp.push_back(3);
			tmp.push_back(6);
			tmp.push_back(9);
			int s = tmp.size();
			for (int i = 0; i< s; ++i)
				fs.write(reinterpret_cast<char *>(&tmp[i]), sizeof(int));
			LeafNode ln(fs, 0);
			Assert::AreEqual((int)ln.data.size(), 3);
			fs.close();
		}

		TEST_METHOD(BPLUSTREE_MIDDLENODE_INOUT)
		{
			fstream fs("testMiddle.dat", ios::in | ios::out | ios::binary | ios::trunc);
			fs.seekp(0);
			fs.seekg(0);
			vector<int> tmp;
			tmp.push_back(0);
			tmp.push_back(3);
			tmp.push_back(0);
			tmp.push_back(-1);
			tmp.push_back(1);
			tmp.push_back(15);
			tmp.push_back(20);
			tmp.push_back(25);
			tmp.push_back(30);
			tmp.push_back(34);
			tmp.push_back(32);
			tmp.push_back(223);
			int s = tmp.size();
			for (int i = 0; i< s; ++i)
				fs.write(reinterpret_cast<char *>(&tmp[i]), sizeof(int));
			MiddleNode mn(fs, 0);
			Assert::AreEqual(mn.offset[1], 34);
			Assert::AreEqual(mn.offset[3], 223);
			Assert::AreEqual(mn.GetSize(), 3);
			Assert::AreEqual(mn.keys[1], 20);
			fs.close();
		}

		TEST_METHOD(BPLUSTREE_SIMPLE_FIND)
		{
			fstream fs("FindSimple.idx", ios::in | ios::out | ios::binary);
			int r = 512;
			fs.write(reinterpret_cast<char *>(&r), sizeof(r));
			r *= 512;
			fs.write(reinterpret_cast<char *>(&r), sizeof(r));
			r = 0;
			for (int i = 0; i < 126; ++i)
			{
				fs.write(reinterpret_cast<char *>(&r), sizeof(r));
			}
			vector<int> tmp;
			tmp.push_back(1);// type
			tmp.push_back(3);// size
			tmp.push_back(0);// parent
			tmp.push_back(0);// lb
			tmp.push_back(0);// rb
			tmp.push_back(1);// id1
			tmp.push_back(3);
			tmp.push_back(0);
			tmp.push_back(7);// id2
			tmp.push_back(5);
			tmp.push_back(8);
			tmp.push_back(13);// id3
			tmp.push_back(6);
			tmp.push_back(9);
			int s = tmp.size();
			for (int i = 0; i < s; ++i)
			{
				fs.write(reinterpret_cast<char *>(&tmp[i]), sizeof(int));
			}
			fs.close();
			BplusTree tree("FindSimple.idx");
			LeafNode* result = tree._Find(7);
			Assert::AreEqual(result->data[2].id, 13);
			Assert::AreEqual(result->data[1].id, 7);
		}

		TEST_METHOD(BPLUSTREE_DOUBLE_FIND)
		{
			fstream fs("FindSimple.idx", ios::in | ios::out | ios::binary);
			int r = 512;
			fs.write(reinterpret_cast<char *>(&r), sizeof(r));
			r *= 3;
			fs.write(reinterpret_cast<char *>(&r), sizeof(r));
			r = 0;
			for (int i = 0; i < 126; ++i)
			{
				fs.write(reinterpret_cast<char *>(&r), sizeof(r));
			}
			vector<int> tmp;
			tmp.push_back(0);// type
			tmp.push_back(3);// size
			tmp.push_back(0);// parent;
			tmp.push_back(0);// lb
			tmp.push_back(0);// rb
			tmp.push_back(5);// key1
			tmp.push_back(10);// key2
			tmp.push_back(15);// key3
			tmp.push_back(1024);// offset1 [0, 5)
			tmp.push_back(1536);// offset2 [5, 10)
			tmp.push_back(2056);// offset3 [10, 15)
			tmp.push_back(2560);// offset4 [15, +00)
			int s = tmp.size();
			for (int i = 0; i < s; ++i)
			{
				fs.write(reinterpret_cast<char *>(&tmp[i]), sizeof(int));
			}
			int zero = 0;
			for (int i = s; i < 128; ++i)// 0
			{
				fs.write(reinterpret_cast<char *>(&zero), sizeof(int));
			}
			tmp.clear();
			tmp.push_back(1);// type
			tmp.push_back(3);// size
			tmp.push_back(512);// parent
			tmp.push_back(0);// lb
			tmp.push_back(0);// rb
			tmp.push_back(1);// id1
			tmp.push_back(3);
			tmp.push_back(0);
			tmp.push_back(2);// id2
			tmp.push_back(5);
			tmp.push_back(8);
			tmp.push_back(3);// id3
			tmp.push_back(6);
			tmp.push_back(9);
			s = tmp.size();
			for (int i = 0; i < s; ++i)
			{
				fs.write(reinterpret_cast<char *>(&tmp[i]), sizeof(int));
			}
			for (int i = s; i < 128; ++i)// 0
			{
				fs.write(reinterpret_cast<char *>(&zero), sizeof(int));
			}
			fs.close();
			BplusTree tree("FindSimple.idx");
			LeafNode* result = tree._Find(3);
			Assert::AreEqual(result->Find(3), 2);
			delete result;
		}

		TEST_METHOD(BPLUSTREE_INSERT_REMOVE)
		{
			fstream fs("FindSimple.idx", ios::in | ios::out | ios::binary | ios::trunc);
			BplusTree tree("FindSimple.idx");
			vector<int> keysss;
			for (int i = 0; i < 100000;i += 17)
			{
				tree.Insert(i, 0, 0);
				keysss.push_back(i);
			}
			LeafNode f(fs, 512);
			LeafNode s(fs, 1024);
			for (int i = 0; i < 100000; i += 17)
			{
				srand((unsigned)time(NULL));
				int f = rand() % (int)keysss.size();
				int m = keysss[f];
				tree.Remove(m);
				keysss.erase(keysss.begin() + f);
				Assert::AreEqual(tree.Find(m), -1);
			}
			Assert::AreEqual(1, 1);
			fs.close();
		}

		TEST_METHOD(BPLUSTREE_SIMPLE_REMOVE)
		{
			fstream fs("remove.idx", ios::in | ios::out | ios::binary);
			int r = 512;
			fs.write(reinterpret_cast<char *>(&r), sizeof(r));
			r *= 2;
			fs.write(reinterpret_cast<char *>(&r), sizeof(r));
			r = 0;
			for (int i = 0; i < 126; ++i)
			{
				fs.write(reinterpret_cast<char *>(&r), sizeof(r));
			}
			vector<int> tmp;
			tmp.push_back(1);// type
			tmp.push_back(3);// size
			tmp.push_back(0);// parent
			tmp.push_back(0);// lb
			tmp.push_back(0);// rb
			tmp.push_back(1);// id1
			tmp.push_back(3);
			tmp.push_back(0);
			tmp.push_back(7);// id2
			tmp.push_back(5);
			tmp.push_back(8);
			tmp.push_back(13);// id3
			tmp.push_back(6);
			tmp.push_back(9);
			int s = tmp.size();
			for (int i = 0; i < s; ++i)
			{
				fs.write(reinterpret_cast<char *>(&tmp[i]), sizeof(int));
			}
			fs.close();
			BplusTree tree("remove.idx");
			tree.Remove(7);
			LeafNode *node = tree._Find(13);
			Assert::AreEqual((int)node->data.size(), 2);
		}

		TEST_METHOD(BPLUSTREE_COMPLEX_MERGE_REMOVE)
		{
			fstream fs("remove.idx", ios::in | ios::out | ios::binary | ios::trunc);
			fs.flush();
			BplusTree tree("remove.idx");
			for (int i = 0; i < 1266; ++i)
			{
				tree.Insert(i + 1, 0, 0);
			}
			// two leafNode and a MiddleNode
			LeafNode ss(fs, 32768);
			MiddleNode root(fs, 33792);
			LeafNode *t = tree._Find(1256);
			fs.flush();
			Assert::AreEqual(tree.GetRoot(), 33792);
			Assert::AreEqual(t->GetSize(), 26);
			delete t;
			t = tree._Find(1);
			Assert::AreEqual(t->GetSize(), 20);
			delete t;
			t = tree._Find(33);
			tree.Remove(33);
			delete t;
			MiddleNode sroot(fs, 33280);
			t = tree._Find(33);
			tree.Remove(35);
			t = tree._Find(37);
			Assert::AreEqual(t->GetSize(), 38);
			fs.close();
			delete t;
		}

		TEST_METHOD(BPLUSTREE_LEAF_MERGE_REMOVE)
		{
			fstream fs("remove.idx", ios::in | ios::out | ios::binary | ios::trunc);
			BplusTree tree("remove.idx");
			for (int i = 0; i < L + 1; ++i)
			{
				tree.Insert(i + 1, 0, 0);
			}
			LeafNode *ss = tree._Find(2);
			delete ss;
			// two leafNode and a MiddleNode
			LeafNode *t = tree._Find(33);
			Assert::AreEqual(t->GetSize(), 21);
			delete t;
			tree.Remove(33);
			LeafNode sx3(fs, 512);
			LeafNode xx3(fs, 1024);
			MiddleNode sxv3(fs, 1536);
			tree.Remove(35);
			LeafNode sx5(fs, 512);
			LeafNode xx5(fs, 1024);
			MiddleNode sxv5(fs, 1536);
			t = tree._Find(37);
			Assert::AreEqual(t->GetSize(), 39);
			delete t;
			fs.close();
		}

		TEST_METHOD(BPLUSTREE_LEAF_BORROW_REMOVE)
		{
			fstream fs("remove.idx", ios::in | ios::out | ios::binary | ios::trunc);
			fs.close();
			BplusTree tree("remove.idx");
			for (int i = 0; i < L + 3; ++i)
			{
				tree.Insert(i + 1, 0 , 0);
			}
			// two leafNode and a MiddleNode
			LeafNode *t = tree._Find(41);
			Assert::AreEqual(t->GetSize(), 23);
			tree.Insert(0, 0, 0);
			delete t;
			t = tree._Find(0);
			Assert::AreEqual(t->GetSize(), 21);
			tree.Remove(33);
			tree.Remove(35);
			delete t;
			t = tree._Find(37);
			Assert::AreEqual(t->GetSize(), 21);
			tree.Remove(37);
			tree.Remove(39);
			delete t;
			t = tree._Find(40);
			Assert::AreEqual(t->GetSize(), 20);
		}

		TEST_METHOD(BPLUSTREE_BUDDYMANAGER_REMOVE)
		{
			fstream fs("combine.idx", ios::in | ios::out | ios::binary | ios::trunc);
			BplusTree tree("combine.idx");
			fstream sf("BMtestEmpty.txt", ios::in | ios::out | ios::binary | ios::trunc);
			sf.close();
			BuddyManager BM("BMtestEmpty.txt");
			//init
			for (int i = 0; i < 50; ++i)
			{
				tree.Insert(i, i * 4, 4);
			}
			MiddleNode i(fs, 1536);
			fs.close();
			for (int i = 0; i < 50; ++i)
			{
				pair<int, int>empty =  tree.Remove(i);
				BM.Free(empty.first, empty.second);
			}
			vector<pair<int, vector<int> > >& manager = BM.GetManager();
			Assert::AreEqual((int)manager[7].second.size(), 1);
			Assert::AreEqual((int)manager[6].second.size(), 1);
			Assert::AreEqual((int)manager[3].second.size(), 1);
		}


		TEST_METHOD(BPLUSTREE_SIMPLE_MODIFY)
		{
			fstream fs("FindSimple.idx", ios::in | ios::out | ios::binary);
			int r = 512;
			fs.write(reinterpret_cast<char *>(&r), sizeof(r));
			r *= 2;
			fs.write(reinterpret_cast<char *>(&r), sizeof(r));
			int zero = 0;
			for (int i = 0; i < 126; ++i)
			{
				fs.write(reinterpret_cast<char *>(&zero), sizeof(zero));
			}
			vector<int> tmp;
			tmp.push_back(1);// type
			tmp.push_back(3);// size
			tmp.push_back(0);// parent;
			tmp.push_back(0);// lb
			tmp.push_back(0);// rb
			for (int i = 0; i < 3; ++i)
			{
				tmp.push_back(i);// id1
				tmp.push_back(i*2);
				tmp.push_back(i*3);
			}
			int s = tmp.size();
			for (int i = 0; i < s; ++i)
			{
				fs.write(reinterpret_cast<char *>(&tmp[i]), sizeof(zero));
			}
			fs.close();
			BplusTree tree("FindSimple.idx");
			tree.Modify(0, 2, 3);
			LeafNode* leaf= tree._Find(0);
			Assert::AreEqual(leaf->data[0].offset, 2);
			Assert::AreEqual(leaf->data[0].length, 3);
		}

		TEST_METHOD(BPLUSTREE_SIMPLE_INSERT)
		{
			fstream fs("FindSimple.idx", ios::in | ios::out | ios::binary);
			int r = 512;
			fs.write(reinterpret_cast<char *>(&r), sizeof(r));
			r *= 4;
			fs.write(reinterpret_cast<char *>(&r), sizeof(r));
			int zero = 0;
			for (int i = 0; i < 126; ++i)
			{
				fs.write(reinterpret_cast<char *>(&zero), sizeof(zero));
			}
			// Middle Node
			vector<int> tmp;
			tmp.push_back(0);// type
			tmp.push_back(1);// size
			tmp.push_back(0);// parent;
			tmp.push_back(0);// lb
			tmp.push_back(0);// rb
			tmp.push_back(3);// key1
			tmp.push_back(1024);// offset1 [0, 3)
			tmp.push_back(1536);// offset2 [3, +00)
			int s = tmp.size();
			for (int i = 0; i < s; ++i)
			{
				fs.write(reinterpret_cast<char *>(&tmp[i]), sizeof(int));
			}
			for (int i = s; i < 128; ++i)// 0
			{
				fs.write(reinterpret_cast<char *>(&zero), sizeof(int));
			}
			tmp.clear();
			tmp.push_back(1);// type
			tmp.push_back(2);// size
			tmp.push_back(512);// parent
			tmp.push_back(0);// lb
			tmp.push_back(1536);// rb
			tmp.push_back(1);// id1
			tmp.push_back(3);
			tmp.push_back(0);
			tmp.push_back(2);// id2
			tmp.push_back(5);
			tmp.push_back(8);
			s = tmp.size();
			for (int i = 0; i < s; ++i)
			{
				fs.write(reinterpret_cast<char *>(&tmp[i]), sizeof(int));
			}
			for (int i = s; i < 128; ++i)// 0
			{
				fs.write(reinterpret_cast<char *>(&zero), sizeof(int));
			}

			tmp.clear();
			tmp.push_back(1);// type
			tmp.push_back(2);// size
			tmp.push_back(512);// parent
			tmp.push_back(1024);// lb
			tmp.push_back(0);// rb
			tmp.push_back(3);// id1
			tmp.push_back(6);
			tmp.push_back(9);
			tmp.push_back(5);// id2
			tmp.push_back(9);
			tmp.push_back(8);
			s = tmp.size();
			for (int i = 0; i < s; ++i)
			{
				fs.write(reinterpret_cast<char *>(&tmp[i]), sizeof(int));
			}
			for (int i = s; i < 128; ++i)// 0
			{
				fs.write(reinterpret_cast<char *>(&zero), sizeof(int));
			}
			fs.close();
			BplusTree tree("FindSimple.idx");
			tree.Insert(6, 500, 2);
			LeafNode* node = tree._Find(6);
			Assert::AreEqual(node->GetSize(), 3);
			Assert::AreEqual(node->Find(6), 2);
		}

		TEST_METHOD(BPLUSTREE_LEAF_SPLIT_INSERT)
		{
			fstream fs("FindSimple.idx", ios::in | ios::out | ios::binary|ios::trunc);
			int r = 512;
			fs.write(reinterpret_cast<char *>(&r), sizeof(r));
			r *= 4;
			fs.write(reinterpret_cast<char *>(&r), sizeof(r));
			int zero = 0;
			for (int i = 0; i < 126; ++i)
			{
				fs.write(reinterpret_cast<char *>(&zero), sizeof(zero));
			}
			// Middle Node
			vector<int> tmp;
			tmp.push_back(0);// type
			tmp.push_back(1);// size
			tmp.push_back(0);// parent;
			tmp.push_back(0);// lb
			tmp.push_back(0);// rb
			tmp.push_back(200);// key1
			tmp.push_back(1024);// offset1 [0, 3)
			tmp.push_back(1536);// offset2 [3, +00)
			int s = tmp.size();
			for (int i = 0; i < s; ++i)
			{
				fs.write(reinterpret_cast<char *>(&tmp[i]), sizeof(int));
			}
			for (int i = s; i < 128; ++i)// 0
			{
				fs.write(reinterpret_cast<char *>(&zero), sizeof(int));
			}
			tmp.clear();
			tmp.push_back(1);// type
			tmp.push_back(L);// size
			tmp.push_back(512);// parent
			tmp.push_back(0);// lb
			tmp.push_back(1536);// rb
			for (int i = 0; i < L; ++i)
			{
				tmp.push_back(i);// id1
				tmp.push_back(3);
				tmp.push_back(0);
			}// 1024 full
			s = tmp.size();
			for (int i = 0; i < s; ++i)
			{
				fs.write(reinterpret_cast<char *>(&tmp[i]), sizeof(int));
			}
			for (int i = s; i < 128; ++i)// 0
			{
				fs.write(reinterpret_cast<char *>(&zero), sizeof(int));
			}



			tmp.clear();
			tmp.push_back(1);// type
			tmp.push_back(30);// size
			tmp.push_back(512);// parent
			tmp.push_back(1024);// lb
			tmp.push_back(0);// rb
			for (int i = 200; i < 230; ++i)
			{
				tmp.push_back(i);// id1
				tmp.push_back(3);
				tmp.push_back(0);
			}// 1536 has 30 elements
			s = tmp.size();
			for (int i = 0; i < s; ++i)
			{
				fs.write(reinterpret_cast<char *>(&tmp[i]), sizeof(int));
			}
			for (int i = s; i < 128; ++i)// 0
			{
				fs.write(reinterpret_cast<char *>(&zero), sizeof(int));
			}
			fs.close();
			BplusTree tree("FindSimple.idx");
			tree.Insert(199, 500, 2);
			LeafNode* node = tree._Find(199);
			Assert::AreEqual(node->GetSize(), L/2+1);
			Assert::AreEqual(node->Find(199), L/2);
		}


	};
	TEST_CLASS(DB_TEST)
	{
		TEST_METHOD(DB_ADD)
		{
			// fresh db 
			fstream fs("D:\\DataBase\\mongo\\mongo.idx", ios::out|ios::trunc);
			fs.close();
			fstream sf("D:\\DataBase\\mongo\\BM.txt", ios::out |ios::trunc);
			sf.close();
			fstream sss("D:\\DataBase\\mongo\\mongo.dat", ios::out |ios::trunc);
			sss.close();
			DataBase::Init();
			DataBase* db = NULL;
			db = DataBase::GetDataBaseByName("mongo");
			Assert::AreNotEqual((int)db, NULL);
			db->InsertOne(1, "fuck");
			pair<int ,string>result = db->FindOne(1);
			Assert::AreEqual(result.second.c_str(), "fuck");
			DataBase::Dump();
			DataBase::close(db);
			BplusTree ss("D:\\DataBase\\mongo\\mongo.idx");
			Assert::AreEqual(ss.GetRoot(), 512);
		}


		TEST_METHOD(DB_MODIFY)
		{
			fstream fs("D:\\DataBase\\mongo\\mongo.idx", ios::out | ios::trunc);
			fs.close();
			fstream sf("D:\\DataBase\\mongo\\BM.txt", ios::out | ios::trunc);
			sf.close();
			fstream sss("D:\\DataBase\\mongo\\mongo.dat", ios::out | ios::trunc);
			sss.close();
			DataBase::Init();
			DataBase* db = NULL;
			db = DataBase::GetDataBaseByName("mongo");
			db->InsertOne(1, "fuck");
			db->ModifyOne(1, "shit");
			Assert::AreEqual(db->FindOne(1).second.c_str(), "shit");
			
		}

		TEST_METHOD(DB_SIMPLE_TEST)
		{
			fstream vv("C:\\DataBase\\darling\\darling.idx", ios::out | ios::trunc);
			vv.close();
			fstream sf("C:\\DataBase\\darling\\BM.txt", ios::out | ios::trunc);
			sf.close();
			fstream sss("C:\\DataBase\\darling\\darling.dat", ios::out | ios::trunc);
			sss.close();
			DataBase::Init();
			DataBase *db = DataBase::GetDataBaseByName("darling");
			Assert::AreNotEqual((int)db, NULL);
			for (int i = 0; i < 101; ++i)
			{
				db->InsertOne(i, "fuck" + to_string(i));
			}
			BplusTree tree("C:\\DataBase\\darling\\darling.idx");
			LeafNode *f = tree._Find(1);
			fstream fs("C:\\DataBase\\darling\\darling.idx", ios::in | ios::out | ios::binary);
			MiddleNode cc(fs, 1536);
			LeafNode ss(fs, 1024);
			for (int i = 0; i < 101; ++i)
			{
				db->RemoveOne(i);
			}
			for (int i = 0; i < 101; ++i)
			{
				db->InsertOne(i, "fuck" + to_string(i));
			}
			for (int i = 0; i < 101; ++i)
			{
				Assert::AreEqual(db->FindOne(i).second, "fuck"+ to_string(i));
			}
			DataBase::close(db);
			DataBase::Dump();
			
		}

		TEST_METHOD(DB_ANNOYING_TEST)
		{
			fstream vv("C:\\DataBase\\darling\\darling.idx", ios::out | ios::trunc);
			vv.close();
			fstream sf("C:\\DataBase\\darling\\BM.txt", ios::out | ios::trunc);
			sf.close();
			fstream sss("C:\\DataBase\\darling\\darling.dat", ios::out | ios::trunc);
			sss.close();
			DataBase::Init();
			DataBase *db = DataBase::GetDataBaseByName("darling");
			Assert::AreNotEqual((int)db, NULL);
			for (int i = 0; i < 200; ++i)
			{
				db->InsertOne(i, "abcdefg");
			}
			
			for (int i = 0; i < 200; ++i)
			{
				db->ModifyOne(i, "gfedcba");
			}
			for (int i = 0; i < 200; ++i)
			{
				string t = db->FindOne(i).second;
				Assert::AreEqual(t, string()+"gfedcba");
			}
			DataBase::close(db);
			DataBase::Dump();
		}

		TEST_METHOD(DB_COMPLEX_TEST)
		{
			fstream vv("C:\\DataBase\\darling\\darling.idx", ios::out | ios::trunc);
			vv.close();
			fstream sf("C:\\DataBase\\darling\\BM.txt", ios::out | ios::trunc);
			sf.close();
			fstream sss("C:\\DataBase\\darling\\darling.dat", ios::out | ios::trunc);
			sss.close();
			DataBase::Init();
			DataBase *db = DataBase::GetDataBaseByName("darling");
			Assert::AreNotEqual((int)db, NULL);
			for (int i = 0; i < 10000; ++i)
			{
				db->InsertOne(i, "fuck" + to_string(i));
			}
			for (int i = 0; i < 10000; ++i)
			{
				db->ModifyOne(i, "shit" + to_string(i));
			}
			for (int i = 0; i < 10000; ++i)
			{
				string t = db->FindOne(i).second;
				Assert::AreEqual(t, "shit" + to_string(i));
			}
			DataBase::close(db);
			DataBase::Dump();
		}

		TEST_METHOD(DB_MANAGER_ADD_AND_REMOVE)
		{
			DataBase::Init();
			DataBase::AddDataBase("testDB", "D:\\DataBase");
			fstream _file;
			_file.open("D:\\DataBase\\testDB\\testDB.idx", ios::in);
			Assert::AreEqual(_file.fail(), false);
			int root;
			_file.read(reinterpret_cast<char *>(&root), sizeof(root));
			Assert::AreEqual(root, 512);// root init 
			_file.close();
			DataBase::DeleteDataBaseByName("testDB");
			_file.open("D:\\DataBase\\testDB\\testDB.idx", ios::in);
			Assert::AreEqual(_file.fail(), true);
			_file.close();
			DataBase::Dump();
		}

		TEST_METHOD(DB_STANDARD_TEST)
		{
			fstream vv("C:\\DataBase\\darling\\darling.idx", ios::out | ios::trunc);
			vv.close();
			fstream sf("C:\\DataBase\\darling\\BM.txt", ios::out | ios::trunc);
			sf.close();
			fstream sss("C:\\DataBase\\darling\\darling.dat", ios::out | ios::trunc);
			sss.close();
			DataBase::Init();
			DataBase *db = DataBase::GetDataBaseByName("darling");
			Assert::AreNotEqual((int)db, NULL);
			map<int, string> TestSet;
			vector<int> keys;
			vector<int>Re;
			set<int> hadInsert;
			for (int i = 0; i < 1000000; i+=5)
			{
				TestSet[i] = "fuck" + to_string(i);
				keys.push_back(i);
				db->InsertOne(i, "fuck" + to_string(i));
			}
			
			for (int i = 0; i < 1000000; ++i)
			{
				srand((unsigned)time(NULL));
				int f = rand() % (int)keys.size();
				Assert::AreEqual(db->FindOne(keys[f]).second, TestSet[keys[f]]);
				if (i % 37 == 0 && i != 0)
				{
					int f = rand() % (int)keys.size();
					db->RemoveOne(keys[f]);
					TestSet.erase(keys[f]);
					Re.push_back(keys[f]);
					int m = keys[f];
					keys.erase(keys.begin() + f);
					Assert::AreEqual(db->FindOne(m).first, -1);
				}
				if (i % 11 == 0 && i != 0)
				{
					set<int>::iterator itr;
					int f = rand() % 10000000 + 10000000;
					itr = hadInsert.find(f);
					if (itr == hadInsert.end())
					{
						hadInsert.insert(f);
						keys.push_back(f);
						db->InsertOne(f, "fuck" + to_string(f));
						TestSet[f] = "fuck" + to_string(f);
						Assert::AreEqual(TestSet[f], db->FindOne(f).second);
					}
				}
				if (i % 17 == 0 && i != 0)
				{
					int f = rand() % (int)keys.size();
					TestSet[keys[f]] = "shit" + to_string(keys[f]);
					db->ModifyOne(keys[f], TestSet[keys[f]]);
					Assert::AreEqual(db->FindOne(keys[f]).second, TestSet[keys[f]]);
				}
			}
		}


	};
};