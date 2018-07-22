#include <string>
#include "BplusTree.h"
using namespace std;

/*	
	class MiddleNode
*/
MiddleNode::MiddleNode(fstream& fs, int pos)
{
	fs.seekg(pos) ;
	int t;
	fs.read(reinterpret_cast<char *>(&t), sizeof(int));
	// specific type
	this->type = NODE_TYPE(t);
	// specific size	
	int s = 0;
	fs.read(reinterpret_cast<char *>(&s), sizeof(int));
	size = s;
	// parent
	fs.read(reinterpret_cast<char *>(&parent), sizeof(int));
	// siblings
	fs.read(reinterpret_cast<char *>(&LeftSibling), sizeof(int));
	fs.read(reinterpret_cast<char *>(&RightSibling), sizeof(int));
	// has size keys and size+1 branches
	for (int i = 0; i < s; ++i)
	{
		keys.push_back(0);
		fs.read(reinterpret_cast<char *>(&keys[i]), sizeof(int));
	}
	for (int i = 0; i <= s; ++i)
	{
		offset.push_back(0);
		fs.read(reinterpret_cast<char *>(&offset[i]), sizeof(int));
	}
	fs.seekg(pos);
}

MiddleNode::MiddleNode(vector<int> keys, vector<int> offset, int parent, int lb, int rb)
{
	this->type = NODE_TYPE(0);
	this->keys = keys;
	this->offset = offset;
	this->parent = parent;
	this->LeftSibling = lb;
	this->RightSibling = rb;
	this->size = keys.size();
}

int MiddleNode::FindSuit(int id)
{
	int low = 0;
	int high = size - 1;
	int mid;
	while (low <= high) {
		mid = low + ((high - low) >> 1);
		if (id < keys[mid]) {
			high = mid - 1;
		}
		else {
			low = mid + 1;
		}
	}
	return low <= size - 1 ? low : size - 1;
}

int MiddleNode::Borrow(fstream& fs, int pos)
{
	MiddleNode left(fs, this->LeftSibling);
	// borrow need to change the parents
	if (this->LeftSibling != 0 && this->parent == left.parent)
	{
		int LeftSize = left.GetSize();
		if (LeftSize > M/2)
		{
			MiddleNode parent(fs, this->parent);
			int ppos = parent.FindSuit(left.keys[LeftSize - 1]);
			int sm = this->offset[0];
			fs.seekg(sm);
			int type;
			fs.read(reinterpret_cast<char *>(&type), sizeof(int));
			bool done = false;
			while (!done)
			{
				switch (type)
				{
				case 1:
				{
					LeafNode small(fs, sm);
					this->keys.insert(this->keys.begin(), small.data[0].id);
					done = true;
					break;
				}
				case 0:
				{
					MiddleNode s(fs, sm);
					sm = s.offset[0];
					fs.seekg(sm);
					fs.read(reinterpret_cast<char *>(&type), sizeof(int));
					break;
				}
				}
			}
			this->offset.insert(this->offset.begin(), left.offset[left.size]);
			this->size++;
			this->NodeWrite(fs, pos);
			//refresh parent
			parent.keys[ppos] = left.keys[left.size-1];
			parent.NodeWrite(fs, this->parent);
			// refresh the borrow
			fs.seekg(left.offset[left.size]);
			fs.read(reinterpret_cast<char *>(&type), sizeof(int));
			switch (type)
			{
			case 1: {
				LeafNode borrow(fs, (int)fs.tellg() - 4);
				borrow.parent = pos;
				borrow.NodeWrite(fs, left.offset[left.size]);
				break;
			}
			case 0: {
				MiddleNode borrow(fs, (int)fs.tellg() - 4);
				borrow.parent = pos;
				borrow.NodeWrite(fs, left.offset[left.size]);
				break;
			}
			}
			// refresh the left
			left.keys.pop_back();
			left.offset.pop_back();
			left.size--;
			left.NodeWrite(fs, this->LeftSibling);
			return 1;
		}
	}
	MiddleNode right(fs, this->RightSibling);
	if (this->RightSibling != 0 && this->parent == right.parent)
	{
		int RightSize = right.GetSize();
		if (RightSize > M / 2)
		{
			MiddleNode parent(fs, this->parent);
			int ppos = parent.FindSuit(this->keys[this->size - 1]);
			int sm = right.offset[0];
			fs.seekg(sm);
			int type;
			fs.read(reinterpret_cast<char *>(&type), sizeof(int));
			bool done = false;
			while (!done)
			{
				switch (type)
				{
				case 1:
				{
					LeafNode small(fs, sm);
					this->keys.push_back(small.data[0].id);
					done = true;
					break;
				}
				case 0:
				{
					MiddleNode s(fs, sm);
					sm = s.offset[0];
					fs.seekg(sm);
					fs.read(reinterpret_cast<char *>(&type), sizeof(int));
					break;
				}
				}
			}
			this->offset.push_back(right.offset[0]);
			//refresh the borrow one's parent
			fs.seekg(right.offset[0]);
			fs.read(reinterpret_cast<char *>(&type), sizeof(int));
			switch (type)
			{
			case 1: {
				LeafNode borrow(fs, (int)fs.tellg() - 4);
				borrow.parent = pos;
				borrow.NodeWrite(fs,right.offset[0]);
				break;
			}
			case 0: {
				MiddleNode borrow(fs, (int)fs.tellg() - 4);
				borrow.parent = pos;
				borrow.NodeWrite(fs, right.offset[0]);
				break;
			}
			}
			this->size++;
			this->NodeWrite(fs, pos);
			// refresh the parent
			parent.keys[ppos] = right.keys[0];
			parent.NodeWrite(fs, this->parent);
			// refresh the left
			right.keys.erase(right.keys.begin());
			right.offset.erase(right.offset.begin());
			right.size--;
			right.NodeWrite(fs, this->RightSibling);
			return 1;
		}
	}
	return 0;
}

void MiddleNode::Merge(fstream& fs, int& root)
{
	// default LeftSibling first
	if (this->LeftSibling != 0)
	{
		MiddleNode left(fs, this->LeftSibling);
		if (this->RightSibling != 0)
		{
			MiddleNode right(fs, this->RightSibling);
			right.LeftSibling = this->LeftSibling;
			right.NodeWrite(fs, this->RightSibling);
		}
		if (left.parent == this->parent)
		{
			MiddleNode parent(fs, this->parent);
			int Down = parent.FindOffset(this->LeftSibling);
			// it is
			fs.seekg(this->offset[0]);
			int type;
			fs.read(reinterpret_cast<char *>(&type), sizeof(int));
			switch (type)
			{
			case 1: {
				for (int i = 0; i < this->offset.size(); ++i)
				{
					LeafNode tmp(fs, this->offset[i]);
					tmp.parent = this->LeftSibling;
					tmp.NodeWrite(fs, this->offset[i]);
				}
				break;
			}
			case 0: {
				for (int i = 0; i < this->offset.size(); ++i)
				{
					MiddleNode tmp(fs, this->offset[i]);
					tmp.parent = this->LeftSibling;
					tmp.NodeWrite(fs, this->offset[i]);
				}
				break;
			}
			}
			// find the right the smallest one
			int sm = this->offset[0];
			bool done = false;
			while (!done)
			{
				switch (type)
				{
				case 1:
				{
					LeafNode small(fs, sm);
					left.keys.push_back(small.data[0].id);
					done = true;
					break;
				}
				case 0:
				{
					MiddleNode s(fs, sm);
					sm = s.offset[0];
					fs.seekg(sm);
					fs.read(reinterpret_cast<char *>(&type), sizeof(int));
					break;
				}
				}
			}
			left.keys.insert(left.keys.end(), keys.begin(), keys.end());
			left.offset.insert(left.offset.end(), offset.begin(), offset.end());
			left.size += size + 1;
			left.RightSibling = RightSibling;
			left.NodeWrite(fs, this->LeftSibling);
			fs.seekg(this->parent);
			parent.Remove(parent.keys[Down], fs, -1, root);
			// refresh the parent node, it must have parent
			return;
		}
	}
	if(this->RightSibling != 0)
	{
		MiddleNode right(fs, this->RightSibling);
		if (this->LeftSibling != 0)
		{
			MiddleNode left(fs, this->LeftSibling);
			left.RightSibling = this->RightSibling;
			left.NodeWrite(fs, this->LeftSibling);
		}
		if (this->parent == right.parent)
		{
			MiddleNode parent(fs, this->parent);
			fs.seekg(offset[0]);
			int type;
			fs.read(reinterpret_cast<char *>(&type), sizeof(int));
			switch (type)
			{
			case 1: {
				for (int i = 0; i < this->offset.size(); ++i)
				{
					LeafNode tmp(fs, this->offset[i]);
					tmp.parent = this->RightSibling;
					tmp.NodeWrite(fs, this->offset[i]);
				}
				break;
			}
			case 0: {
				for (int i = 0; i < this->offset.size(); ++i)
				{
					MiddleNode tmp(fs, this->offset[i]);
					tmp.parent = this->RightSibling;
					tmp.NodeWrite(fs, this->offset[i]);
				}
				break;
			}
			}
			int sm = right.offset[0];
			bool done = false;
			while (!done)
			{
				switch (type)
				{
				case 1:
				{
					LeafNode small(fs, sm);
					right.keys.insert(right.keys.begin(), small.data[0].id);
					done = true;
					break;
				}
				case 0:
				{
					MiddleNode s(fs, sm);
					sm = s.offset[0];
					fs.seekg(sm);
					fs.read(reinterpret_cast<char *>(&type), sizeof(int));
					break;
				}
				}
			}
			int Down = parent.FindOffset(this->RightSibling) - 1;
			right.keys.insert(right.keys.begin(), keys.begin(), keys.end());
			right.offset.insert(right.offset.begin(), offset.begin(), offset.end());
			right.size += size + 1;
			right.LeftSibling = LeftSibling;
			right.NodeWrite(fs, this->RightSibling);
			fs.seekg(this->parent);
			// refresh the parent node, it must have parent
			parent.Remove(parent.keys[Down], fs, 1, root);
			return;
		}	
	}

}

int MiddleNode::Find(int id)
{
	// return offset
	// the size is the keys' size
	int low = 0;
	int high = size - 1;
	int mid;
	while (low <= high) {
		mid = low + ((high - low) >> 1);
		if (id < keys[mid]) {
			high = mid - 1;
		}
		else {
			low = mid + 1;
		}
	}
	return low <= size - 1 ? offset[low] : offset[size] ;
}

void MiddleNode::Insert(int middle ,int l, int r, fstream& fs, int& tail, int& root)
{
	// insert the new node
	bool done = false;
	int low = 0;
	int high = size - 1;
	int mid;
	while (low <= high) {
		mid = low + ((high - low) >> 1);
		if (middle < keys[mid]) {
			high = mid - 1;
		}
		else {
			low = mid + 1;
		}
	}
	if (low <= size - 1)
	{
		keys.insert(low + keys.begin(), middle);
		if(low != 0)
		offset.insert(low + offset.begin(), r);
		else {
			offset.insert(offset.begin(), r);
			offset.insert(offset.begin(), l);
		}
	}
	else {
		keys.push_back(middle);
		offset.push_back(r);
		++size;
	}
	/*for (int i = 0; i < size; ++i)
	{
		if (middle < keys[i])
		{
			keys.insert(i + keys.begin(), middle);
			size++;
			if (i > 0)
			{
				offset.insert(i + offset.begin(), r);
			}
			else {// insert first
				offset.insert(offset.begin(), r);
				offset.insert(offset.begin(), l);
			}
			done = true;
			break;
		}
	}
	if (!done)// the last one
	{
		keys.push_back(middle);
		offset.push_back(r);
		++size;
	}
	*/
}

void MiddleNode::Remove(int id, fstream& fs, int site, int& root)
{
	int pos = FindKey(id);
	keys.erase(pos + keys.begin());
	switch (site)
	{
		case 1: {
			//right be merged
			offset.erase(pos + offset.begin());
			break;
		}
		case -1: {
			offset.erase(pos + 1 + offset.begin());
			break;
		}
		default:
			break;
	}
	size--;


	if (this->parent == 0 && size == 0)
	{
		// At this time the only offset is the new root
		fs.seekg(offset[0]);
		int type;
		fs.read(reinterpret_cast<char *>(&type), sizeof(int));
		switch (type)
		{
			case 1: {
				LeafNode NewNode(fs, (int)fs.tellg() - 4);
				NewNode.parent = 0;
				NewNode.NodeWrite(fs, offset[0]);
				// root refresh in file, we can refresh in memory
				root = offset[0];
				break;
			}
			case 0: {
				MiddleNode NewNode(fs, (int)fs.tellg() - 4);
				NewNode.parent = 0;
				NewNode.NodeWrite(fs, offset[0]);
				root = offset[0];
				break;
			}
		}
	}
	else if (this->parent == 0 && size >= 1)
	{
		int seek = fs.tellg();
		NodeWrite(fs, seek);
	}
	else if (size > M / 2)
	{
		int seek = fs.tellg();
		NodeWrite(fs, seek);
	}
	else {
		// Middle Borrow
		int seek = fs.tellg();
		int result = this->Borrow(fs, seek);
		if (!result)// can't borrow, need merge
		{
			this->Merge(fs, root);
		}
	}
}

void MiddleNode::Modify(int id, int offset, int len, fstream&)
{
	
}

void MiddleNode::Split(SplitData sp, fstream& fs, int& tail, int& root)
{
	// need to add height
	this->Insert(sp.middle, sp.LeftOffset, sp.RightOffset, fs, tail, root);
	if (size > M)// overflow
	{
		// slice the array
		int half = size / 2;
		vector<int> NewKey;
		vector<int> NewOffset;
		// the i = half will go to the parent
		for (int i = half+2; i < size; ++i)
		{
			NewKey.push_back(keys[i]);
			NewOffset.push_back(offset[i]);
		}
		NewOffset.push_back(offset[size]);
		int ChildType;
		// record now fs pos
		int now = fs.tellg();
		int pos = tail;
		tail += 512;
		fs.seekg(NewOffset[0]);
		fs.read(reinterpret_cast<char *>(&ChildType), sizeof(int));
		switch (ChildType)
		{
			case 1: {
				for (int i = 0; i < NewOffset.size(); ++i)
				{
					LeafNode tmp(fs, NewOffset[i]);
					tmp.parent = pos;
					tmp.NodeWrite(fs, NewOffset[i]);
				}
				break;
			}
			case 0: {
				for (int i = 0; i < NewOffset.size(); ++i)
				{
					MiddleNode tmp(fs, NewOffset[i]);
					tmp.parent = pos;
					tmp.NodeWrite(fs, NewOffset[i]);

				}
				break;
			}
			
		}
		size = half + 1;
		MiddleNode NewNode(NewKey, NewOffset, this->parent, now, this->RightSibling);
		// refresh the RightSibling's LeftSibling pointer
		if (this->RightSibling != 0)// the RightSibiling is not empty
		{
			MiddleNode RB(fs, this->RightSibling);
			RB.LeftSibling = pos;
			RB.NodeWrite(fs, this->RightSibling);
			// return now
			fs.seekg(now);
		}
		// change the RightSibing
		this->RightSibling = pos;
		this->NodeWrite(fs, now);
		//refresh the old middle node
		NewNode.NodeWrite(fs, pos);
		fs.flush();
		SplitData s(NewNode.LeftSibling, pos, this->keys[half+1]);
		//go to parent
		// right
		if (this->parent == 0)
		{
			int NewPos = tail;
			tail += 512;
			vector<int> NewKey{ s.middle };
			vector<int> NewOffset{ s.LeftOffset, s.RightOffset };
			MiddleNode NewRoot(NewKey, NewOffset, 0, 0, 0);
			// refresh the parent
			NewRoot.NodeWrite(fs, NewPos);
			MiddleNode lc(fs, s.LeftOffset);
			lc.parent = NewPos;
			lc.NodeWrite(fs, s.LeftOffset);
			MiddleNode rc(fs, s.RightOffset);
			rc.parent = NewPos;
			rc.NodeWrite(fs, s.RightOffset);
			// refresh the root
			root = NewPos;
		}
		else {
			MiddleNode parent(fs, this->parent);
			parent.Split(s, fs, tail, root);
		}
	}
	else {
		// just write down
		this->NodeWrite(fs, (int)fs.tellg());
	}
}

int MiddleNode::FindKey(int id)
{
	int low = 0, high = size - 1, mid = 0;
	while (low <= high) {
		mid = low + ((high - low) >> 1);
		if (id == keys[mid]) {
			return mid;
		}
		else if (id < keys[mid]) {
			high = mid - 1;
		}
		else if (id > keys[mid]) {
			low = mid + 1;
		}
	}
	return -1;
}

int MiddleNode::FindOffset(int off)
{

	int low = 0, high = size, middle = 0;
	while (low <= high) {
		middle = low + ((high - low) >> 1);
		if (off == offset[middle]) {
			return middle;
		}
		else if (off < offset[middle]) {
			high = middle - 1;
		}
		else {
			low = middle + 1;
		}
	}
	return -1;
}

void MiddleNode::NodeWrite(fstream& fs, int pos)
{
	fs.seekp(pos);
	// set 0 in the block
	int zero = 0;
	for (int i = 0; i < 128; ++i)
	{
		fs.write(reinterpret_cast<char *>(&zero), sizeof(int));
	}
	fs.seekp(pos);
	// specific type
	int t = (int)this->type;
	fs.write(reinterpret_cast<char *>(&t), sizeof(int));
	// specific size
	fs.write(reinterpret_cast<char *>(&size), sizeof(int));
	// parent
	fs.write(reinterpret_cast<char *>(&parent), sizeof(int));
	// siblings
	fs.write(reinterpret_cast<char *>(&LeftSibling), sizeof(int));
	fs.write(reinterpret_cast<char *>(&RightSibling), sizeof(int));
	// has size keys and size+1 branches
	for (int i = 0; i < size; ++i)
	{
		fs.write(reinterpret_cast<char *>(&keys[i]), sizeof(int));
	}
	for (int i = 0; i <= size; ++i)
	{
		fs.write(reinterpret_cast<char *>(&offset[i]), sizeof(int));
	}

}

/*
	class LeafNode
*/
 LeafNode::LeafNode(fstream& fs, int pos)
{
	fs.seekg(pos);
	// type
	int t;
	fs.read(reinterpret_cast<char *>(&t), sizeof(int));
	this->type = NODE_TYPE(t);
	// size
	int s = 0;
	fs.read(reinterpret_cast<char *>(&s), sizeof(int));
	size = s;
	// parent
	fs.read(reinterpret_cast<char *>(&parent), sizeof(int));
	// siblings
	fs.read(reinterpret_cast<char *>(&LeftSibling), sizeof(int));
	fs.read(reinterpret_cast<char *>(&RightSibling), sizeof(int));
	// data init
	for (int i = 0; i < s; ++i)
	{
		LeafData ld;
		fs.read(reinterpret_cast<char *>(&ld.id), sizeof(int));
		fs.read(reinterpret_cast<char *>(&ld.length), sizeof(int));
		fs.read(reinterpret_cast<char *>(&ld.offset), sizeof(int));
		data.push_back(ld);
	}
	fs.seekg(pos);
}

 LeafNode::LeafNode(vector<LeafData> data, int pointer, int parent, int lb, int rb)
 {
	 this->type = NODE_TYPE(1);
	 this->data = data;
	 this->parent = parent;
	 this->size = data.size();
	 this->LeftSibling = lb;
	 this->RightSibling = rb;
 }

 int LeafNode::Borrow(fstream& fs, int pos)
{
	 if (LeftSibling != 0)
	 {
		 LeafNode left(fs, LeftSibling);
		 if (left.parent == this->parent)
		 {
			 int LeftSize = left.GetSize();
			 if (LeftSize > L / 2)
			 {
				 // move a the biggset child and need change the parent
				 this->data.insert(this->data.begin(), left.data[left.GetSize() - 1]);
				 size++;
				 left.size--;
				 left.data.pop_back();
				 left.NodeWrite(fs, LeftSibling);
				 this->NodeWrite(fs, pos);
				 MiddleNode parent(fs, this->parent);
				 int KeyPos = parent.FindKey(this->data[1].id);
				 parent.keys[KeyPos] = this->data[0].id;
				 parent.NodeWrite(fs, this->parent);
				 return 1;
			 }
		 }
		 
	 }
	 if (RightSibling != 0)
	 {
		 LeafNode right(fs, RightSibling);
		 if (this->parent == right.parent)
		 {
			 int RightSize = right.GetSize();
			 if (RightSize > L / 2)
			 {
				 // move a the biggset child and need change the parent
				 this->data.push_back(right.data[0]);
				 right.data.erase(right.data.begin());
				 size++;
				 right.size--;
				 right.NodeWrite(fs, RightSibling);
				 this->NodeWrite(fs, pos);
				 MiddleNode parent(fs, this->parent);
				 int KeyPos = parent.FindKey(this->data[size - 1].id);
				 parent.keys[KeyPos] = right.data[0].id;
				 parent.NodeWrite(fs, this->parent);
				 return 1;
			 }
		 }
	 }
	 return 0;
}

 void LeafNode::Merge(fstream& fs, int& root)
{
	 // default LeftSibling
	 if (this->LeftSibling != 0)
	 {
		 LeafNode left(fs, this->LeftSibling);
		 if (left.parent == this->parent)
		 {
			 if (this->RightSibling != 0)
			 {
				 LeafNode right(fs, this->RightSibling);
				 right.LeftSibling = this->LeftSibling;
				 right.NodeWrite(fs, this->RightSibling);
			 }
			 for (int i = 0; i < size; ++i)
			 {
				 left.data.push_back(data[i]);
			 }
			 left.size += size;
			 left.RightSibling = this->RightSibling;
			 MiddleNode parent(fs, this->parent);
			 left.NodeWrite(fs, this->LeftSibling);
			 // refresh the parent node
			 fs.seekg(this->parent);
			 parent.Remove(data[0].id, fs, -1, root);
			 return;
		 }
	 }
	 if(this->RightSibling != 0)
	 {
		 LeafNode right(fs, this->RightSibling);
		 if (this->parent == right.parent)
		 {
			 if (this->LeftSibling != 0)
			 {
				 LeafNode left(fs, this->LeftSibling);
				 left.RightSibling = this->RightSibling;
				 left.NodeWrite(fs, this->LeftSibling);
			 }
			 int r = right.data[0].id;
			 right.data.insert(right.data.begin(), this->data.begin(), this->data.end());
			 right.size += size;
			 right.LeftSibling = LeftSibling;
			 fs.flush();
			 MiddleNode parent(fs, this->parent);
			 right.NodeWrite(fs, this->RightSibling);
			 // refresh the parent node
			 fs.seekg(this->parent);
			 parent.Remove(r, fs, 1, root);
		 }
	 }

}

 int LeafNode::Find(int id)
{
	//data is order by the id;
	// we do not use binary search first
	int len = data.size();
	int low = 0;
	int high = len - 1;
	int mid = 0;
	while (low <= high) {
		mid = low + ((high - low) >> 1);
		if (id == data[mid].id) {
			return mid;
		}
		else if (id < data[mid].id) {
			high = mid - 1;
		}
		else {
			low = mid + 1;
		}
	}
	return -1;

}

 void LeafNode::Insert(int id, int index, int size, fstream& fs,int& tail, int& root)
{
	 int low = 0;
	 int high = (int)data.size() - 1;
	 int mid = 0;
	 while (low <= high) {
		 mid = low + ((high - low) >> 1);
		 if (id < data[mid].id) {
			 high = mid - 1;
		 }
		 else {
			 low = mid + 1;
		 }
	 }
	 int result = 0;
	 if (low <= size - 1)
	 {
		 result = low;
	 }
	 else {
		 result = size;
	 }
	 if (result == size)
	 {
		 data.push_back(LeafData(id, index, size));
	 }
	 else {
		 if (result == 0 && this->parent != 0)
		 {
			 int NowPos = fs.tellg();
			 MiddleNode parent(fs, this->parent);
			 int pos = parent.FindKey(this->data[0].id);
			 if (pos != -1)
			 {
				 parent.keys[pos] = id;
			 }
			 fs.seekg(NowPos);
		 }
		 data.insert(data.begin() + result, LeafData(id, index, size));
	 }
	 this->size++;
	 // check the size
	 if (this->size > L)
	 {
		 SplitData s(0, 0, 0);
		 this->Split(s, fs, tail, root);
	 }
	 else {
		 // find and the tellg is the right place
		 this->NodeWrite(fs, (int)fs.tellg());
	 }
}

 void LeafNode::Remove(int pos, fstream& fs, int site, int& root)
{
	 if (pos != -1)
	 {
		 LeafData RemoveData = this->data[pos];
		 this->data.erase(pos + this->data.begin());
		 size--;
		 int seek = fs.tellg();
		 if (this->parent != 0 && pos == 0)
		 {// delete the initial element
			 MiddleNode parent(fs, this->parent);
			 int KeyPos = parent.FindKey(RemoveData.id);
			 if (KeyPos != -1)// left left exclude
			 {
				 parent.keys[KeyPos] = this->data[0].id;
				 parent.NodeWrite(fs, this->parent);
				 MiddleNode check(fs, this->parent);
			 }
			 fs.seekg(seek);
		 }
		 // not the root and need merge or borrow
		 if (size < L / 2 && this->parent != 0)
		 {
			 // borrow first
			 int s = fs.tellg();
			 int result = this->Borrow(fs, (int)fs.tellg());
			 // the result has two situation 1(success) 0(fail)
			 if (!result)
			 {
				 int now = fs.tellg();
				 this->Merge(fs, root);// merge default left brother priority
			 }
		 }
		 else {
			 this->NodeWrite(fs, (int)fs.tellg());
		 }
	 }
}

 void LeafNode::Modify(int id, int offset, int len, fstream& fs)
{
	 int pos = this->Find(id);
	 LeafData& leaf = this->data[pos];
	 leaf.offset = offset;
	 leaf.length = len;
	 int u = fs.tellg();
	 this->NodeWrite(fs, u);
}

 void LeafNode::Split(SplitData sd, fstream& fs, int& tail, int& root)
{
	int half = size / 2;
	vector<LeafData> tmp;
	for (int i = half; i < size; ++i)// the second half is bigger
	{
		tmp.push_back(data[i]);
	}
	int pointer = 0;
	LeafNode NewNode(tmp, pointer, this->parent, (int)fs.tellg(), this->RightSibling);//int pointer, int parent, int lb, int rb
	int now = fs.tellg();
	int pos = tail;
	tail += 512;
	// change the size 
	this->size = half;
	NewNode.NodeWrite(fs, pos);
	this->RightSibling = pos;
	this->NodeWrite(fs, now);
	// init the new node
	if (this->parent == 0)
	{
		// need to new a MiddleNode as parent
		int NewPos = tail;
		tail += 512;
		vector<int> NewKey{ NewNode.data[0].id };
		vector<int> NewOffset{ now, pos };
		MiddleNode NewRoot(NewKey, NewOffset, 0, 0, 0);
		// refresh the parent
		LeafNode lc(fs, now);
		lc.parent = NewPos;
		lc.NodeWrite(fs, now);
		LeafNode rc(fs, pos);
		rc.parent = NewPos;
		rc.NodeWrite(fs, pos);
		NewRoot.NodeWrite(fs, NewPos);
		// refresh the root
		root = NewPos;
		fs.flush();
		return;
	}
	MiddleNode Parent(fs, this->parent);
	// go to  the parent;
	fs.seekg(this->parent);
	Parent.Split(SplitData(NewNode.LeftSibling, pos, NewNode.data[0].id), fs, tail, root);
}

 int LeafNode::FindRange(int id)
 {
	 int low = 0;
	 int high = size - 1;
	 int mid = 0;
	 while (low <= high) {
		 mid = low + ((high - low) >> 1);
		 if (id < data[mid].id) {
			 high = mid - 1;
		 }
		 else {
			 low = mid + 1;
		 }
	 }
	 return low <= size - 1 ? low : -1;
 }



/*
	class BplusTree
*/
int BplusTree::Insert(int id, int index, int size)
{
	LeafNode* node = this->_Find(id);
	node->Insert(id, index, size, fs, tail, root);
	return 1;
}

pair<int, int> BplusTree::Remove(int id)
{
	LeafNode* node = this->_Find(id);
	int pos = node->Find(id);
	//  pos, size
	if (pos == -1)
	{
		MiddleNode ss(fs, node->parent);
		LeafNode s(fs, node->LeftSibling);
		LeafNode f(fs, node->RightSibling);
		LeafNode sc(fs, f.RightSibling);

		return make_pair(-1, -1);

	}
	pair<int, int> Raw = make_pair(node->data[pos].offset, node->data[pos].length);
	node->Remove(pos, fs, 0, root);
	return Raw;
}

int BplusTree::Find(int id)
{
	LeafNode* l = this->_Find(id);
	int pos = l->Find(id);
	if (pos == -1)
	{
		return pos;
	}
	return l->data[pos].offset;
}

vector<pair<int, int> > BplusTree::FindMany(int low, int high)
{
	LeafNode* head =  this->_Find(low);
	int pointer = head->RightSibling;
	vector<pair<int, int> > result;
	int pos = head->FindRange(low);
	if (pos == 0)
	{
		pos = 0;
	}
	else if(pos > 0) {
		if (head->data[pos - 1].id < low)
		{
			pos = pos;
		}
		else if(head->data[pos - 1].id == low){
			pos = pos - 1;
		}
	}
	int TailPos = head->FindRange(high);
	if (TailPos == -1)// not arrive
	{
		for (int i = pos; i < head->size; ++i)
		{
			result.push_back(make_pair(head->data[i].id, head->data[i].offset));
		}
	}
	else {// just in the same node
		for (int i = pos; i < TailPos; ++i)
		{
			result.push_back(make_pair(head->data[i].id, head->data[i].offset));
		}
		return result;
	}
	// need to find in the right sibling
	while (TailPos == -1)
	{
		LeafNode *tail = new LeafNode(fs, pointer);
		TailPos = tail->FindRange(high);
		if (TailPos == -1)
		{
			for (int i = 0; i < tail->size; ++i)
			{
				result.push_back(make_pair(tail->data[i].id, tail->data[i].offset));
			}
			pointer = tail->RightSibling;
		}
		else {
			for (int i = 0; i < TailPos; ++i)
			{
				result.push_back(make_pair(tail->data[i].id, tail->data[i].offset));
			}
		}
		delete tail;
	}
	delete head;
	return result;
}

// !important
LeafNode* BplusTree::_Find(int id)
{
	fs.seekg(root);
	return FindCore(id);
}

LeafNode* BplusTree::FindCore(int id)
{
	// after init root is get and we seekg to the root;
	// get the the first int to get the type
	// if root is empty, init it.
	int t;
	int u = fs.tellg();
	fs.read(reinterpret_cast<char *>(&t), sizeof(int));
	switch (t)
	{
		case 1: {// LeafNode
			int now = fs.tellg();
			now -= 4;
			LeafNode* result = new LeafNode(fs, now);
			fs.seekg(now);
			return result;
		}
		case 0: {
			// init the MiddleNode
			// change the fs
			// call the function recursively
			MiddleNode mn(fs, (int)fs.tellg() - 4);
			int NewSeek = mn.Find(id);
			fs.seekg(NewSeek);
			return this->FindCore(id);
		}
	}
}

void LeafNode::NodeWrite(fstream &fs, int pos)
{
	fs.seekp(pos);
	// set 0 in the block
	int zero = 0;
	for (int i = 0; i < 128; ++i)
	{
		fs.write(reinterpret_cast<char *>(&zero), sizeof(int));
	}
	fs.seekp(pos);
	// type
	int t = (int)this->type;
	fs.write(reinterpret_cast<char *>(&t), sizeof(int));
	// size
	int s = size;
	fs.write(reinterpret_cast<char *>(&s), sizeof(int));
	// parent
	fs.write(reinterpret_cast<char *>(&parent), sizeof(int));
	// siblings
	fs.write(reinterpret_cast<char *>(&LeftSibling), sizeof(int));
	fs.write(reinterpret_cast<char *>(&RightSibling), sizeof(int));
	// data init
	for (int i = 0; i < s; ++i)
	{
		LeafData &ld = data[i];
		fs.write(reinterpret_cast<char *>(&ld.id), sizeof(int));
		fs.write(reinterpret_cast<char *>(&ld.length), sizeof(int));
		fs.write(reinterpret_cast<char *>(&ld.offset), sizeof(int));
	}
	fs.seekg(pos);
}

void BplusTree::Modify(int id, int offset, int len)
{
	LeafNode* leaf =  this->_Find(id);
	leaf->Modify(id, offset, len , fs);
	fs.seekg(root);
	fs.seekp(root);
}


BplusTree::BplusTree(const string& path)
{
	Bpath = path;
	fs.open(path, ios::in | ios::binary | ios::out);
	fs.seekg(0);
	fs.seekp(0);
	int now = fs.tellg();
	fs.seekg(0, fs.end);
	int pos = fs.tellg();
	if (pos == 0)
	{
		fs.seekg(0);
		fs.seekp(0);
		int r = 512;
		// record root
		fs.write(reinterpret_cast<char *>(&r), sizeof(r));
		// record the tail
		r *= 2;
		fs.write(reinterpret_cast<char *>(&r), sizeof(r));
		r = 0;
		for (int i = 0; i < 126; ++i)
		{
			fs.write(reinterpret_cast<char *>(&r), sizeof(r));
		}
		vector<int> tmp;
		tmp.push_back(1);// type
		tmp.push_back(0);// size
		tmp.push_back(0);// parent;
		tmp.push_back(0);// lb
		tmp.push_back(0);// rb
		int s = tmp.size();
		for (int i = 0; i < s; ++i)
		{
			fs.write(reinterpret_cast<char *>(&tmp[i]), sizeof(int));
		}
	}
	fs.seekg(0);
	fs.seekp(0);
	fs.read(reinterpret_cast<char *>(&root), sizeof(root));
	fs.read(reinterpret_cast<char *>(&tail), sizeof(tail));
	fs.seekg(root);
}

BplusTree::~BplusTree()
{
	fs.seekp(0);
	fs.write(reinterpret_cast<char *>(&root), sizeof(root));
	fs.write(reinterpret_cast<char *>(&tail), sizeof(tail));
	this->fs.close();
}
