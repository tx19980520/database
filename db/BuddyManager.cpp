#include "BuddyManager.h"
#include <iostream>
#include <fstream>
#include <sstream>


using namespace std;

vector<int> BuddyManager::DecToBin(int size)
{
	vector<int> bins;	
	do{
		int tmp = size % 2;
		bins.push_back(tmp);
		size /= 2;
	} while (size >= 2);
	if(size != 0)
	bins.push_back(size);
	return bins;
}

vector<BuddyManager::MiddleNode> BuddyManager::Slice(vector<int> slices, int pos)
{
	int size = slices.size();
	int m = 1;
	vector<MiddleNode>split;
	for (int i = 0; i < size; ++i)
	{
		if (slices[i] == 1)
		{
			split.push_back(MiddleNode(m, pos, i));
			pos += m;
			m *= 2;
		}
		else 
		{
			m *= 2;
		}
	}
	return split;
}

void BuddyManager::Free(int pos, int size)
{
	insert(pos, size);
}

int BuddyManager::Malloc(int size)
{
	return malloc(size);
}

int BuddyManager::malloc(int size)
{
	int OverSize = 1;
	int count = 0;
	while (OverSize < size)
	{
		count += 1;
		OverSize *= 2;
	}
	int ManageSize = manager.size();
	if (ManageSize - 1 < count)
	{
		int ExpandSize = 1;
		int ExpandCount = 0;
		while (ExpandCount != count + 1)
		{
			ExpandSize *= 2;
			ExpandCount += 1;
			if (ExpandCount >= ManageSize)
			{
				vector<int>empty;
				manager.push_back(make_pair(ExpandSize, empty));
			}
		}
	}
	vector<int>& specific = this->manager[count].second;/* storage address */
	if (specific.size() != 0)/* has suitable size */
	{
		int pos = specific[0];
		specific.erase(specific.begin());
		if (size <= OverSize)
		{
			if (size < OverSize)
			{
				split(pos+size, OverSize - size);/* split the rest space */
			}
			return pos;
		}
	}
	else 
	{/* the most specific space has nothing rest */
		while (true)
		{
			count++;
			if (count == manager.size())/* have no rest space to reuse */
			{
				int pos = tail;
				tail += size;
				return pos;
			}
			vector<int>& search = manager[count].second;
			int CurrSize = search.size();
			if (CurrSize > 0)
			{
				int pos = search[0];
				search.erase(search.begin());
				int NewPos = pos + size;
				split(NewPos, manager[count].first-size);
				return pos;
			}
		}
	}
}

void BuddyManager::insert(int pos, int size)
{
	vector<int> spaces = DecToBin(size);
	
	/* expand the top of manager */
	int index = spaces.size() - 1;
	int OriginIndex = manager.size() - 1;/* OriginIndex >= 0 */
	if (index > OriginIndex)//
	{
		int NowMax = manager[OriginIndex].first;
		while (OriginIndex < index)
		{
			NowMax *= 2;
			vector<int> empty;
			manager.push_back(make_pair(NowMax, empty));
			OriginIndex++;
		}
	}



	vector<MiddleNode> slices = Slice(spaces, pos);
	int SliceSize = slices.size();
	for (int i = 0; i < SliceSize; ++i)
	{
		MiddleNode &node = slices[i];
		vector<int>& specific = manager[node.vec_pos].second;/* storage the address */
		int &space = manager[slices[i].vec_pos].first;
		int SpecificSize = specific.size();
		if (SpecificSize != 0 && specific[0] > node.pos)
		{
			if (1 <= SpecificSize && specific[0] == node.pos + space)
			{
				specific.erase(specific.begin());
				insert(node.pos, size * 2);
				continue;
			}
			specific.insert(specific.begin(), node.pos);
			continue;
		}
		for (int i = 0; i < SpecificSize; ++i)/* now O(n) */
		{
			if (specific[i] < node.pos)
			{
				/* check buddy */
				int SpecificSize = specific.size();
				if (specific[i] == node.pos - space)
				{
					specific.erase(specific.begin() + i);
					insert(node.pos - space, node.size * 2);
					break;
				}
				else if (i + 1 < SpecificSize && specific[i + 1] == node.pos + space)
				{
					specific.erase(specific.begin() + i);
					insert(node.pos, size * 2);
					break;
				}
				else 
				{
					specific.insert(specific.begin() + i, node.pos);
					break;
				}
			}
			else 
			{
				continue;
			}
		}
		/* the vector is empty */
		if (SpecificSize == 0)
		{
			specific.insert(specific.begin(), node.pos);
		}
	}
}

void BuddyManager::split(int pos,int size)
{
	vector<int> bins = DecToBin(size);
	int len = bins.size();
	int space = 1;
	for (int i = 0; i < len; ++i)
	{
		if (bins[i] != 0)
		{
			insert(pos, space);
			pos += space;
		}
		space *= 2;
	}


}

void BuddyManager::serialize()
{
	int size = manager.size();
	string serial = "";
	serial += to_string(tail) + "&";
	for (int i = 0; i < size; ++i)
	{
		vector<int>& specific = manager[i].second;
		int SpecSize = specific.size();
		serial += to_string(manager[i].first);
		serial += '%';
		for (int j = 0; j < SpecSize; ++j)
		{
			serial += to_string(specific[j]);
			serial += "*";
		}
	}
	serial += '!';
	ofstream fs(BMpath, ios::trunc | ios::out);
	if (fs.fail())
	{
		cout << "BM error";
	}
	fs << serial;
	fs.flush();
	fs.close();
}

void BuddyManager::deserialize()
{
	fstream fs(BMpath,ios::in);
	if (fs.fail())
	{
		cout << "deserialize error";
		return;
	}

	string contents;
	fs >> contents;
	int pos = 0;
	int size = contents.size();
	string SmallBuffer = "";
	vector<int>* Current = NULL;
	while (pos < size)
	{
		if (contents[pos] == '!')
		{
			break;
		}
		if ((int)contents[pos] >= 48 && (int)contents[pos] <= 57)
		{
			SmallBuffer += contents[pos];
		}
		else 
		{
			if (contents[pos] == '!')/* end */
			{
				break;
			}
			if (contents[pos] == '%')/* buffer is size */
			{
				vector<int> empty;
				stringstream steam(SmallBuffer);
				int num;
				steam >> num;
				manager.push_back(make_pair(num, empty));
				Current = &manager[manager.size() - 1].second;
			}
			if (contents[pos] == '*')/* buffer is specific pos */
			{
				stringstream steam(SmallBuffer);
				int num;
				steam >> num;
				Current->push_back(num);
			}
			if (contents[pos] == '&')/* buffer is tail */
			{
				stringstream steam(SmallBuffer);
				steam >> tail;
			}	
			SmallBuffer = "";
		}
		pos++;
	}
}
