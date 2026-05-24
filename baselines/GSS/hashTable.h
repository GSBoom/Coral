#include<string>
#include<vector>
#include<algorithm>
#include "hashfunction.h"
using namespace std;
unsigned int (*hfunc[14])(const unsigned char*, unsigned int)=
{
    BOB1,
	BOB2,
	BOB3,
	BOB4,
    PJW,
    DJB,
    CRC32,
    OCaml,
    SML,
    STL,
    FNV32,
    Hsieh,
    RSHash	
};
struct  hashvalue
{
	unsigned int key;
	int IDnum;
};
bool mycomp(const hashvalue &hv1, const hashvalue &hv2)//重载比较函数
{
	return hv1.key<hv2.key;
}
bool operator==(const hashvalue &hv1, const hashvalue &hv2)//重载等于
{
	return hv1.key==hv2.key;
}
int countjoin( vector<hashvalue> &V1, vector<hashvalue> &V2)//计算两个 vector<hashvalue> 的交集，并返回交集中 IDnum 的总和
{
 int i1=0,i2=0;
 int count=0;
 while(i1<V1.size())
 {
 	if (i2>=V2.size()) return count;
 	while(V2[i2].key<V1[i1].key)
 	{
 		i2++;
 		if (i2>=V2.size()) return count;
 	}
 	if(V2[i2].key==V1[i1].key)
 	{
 		count+=V1[i1].IDnum;
 	    i1++;
 		i2++;
 		continue;
	 }
	else if(V2[i2].key>V1[i1].key)
	{
 			i1++;
	}
  }
  return count; 
}
template<class T>
class hashTableNode
{
public:
	T value;//node本身的值
	unsigned int key;//hash值
	hashTableNode<T> *next;//指向下一个可以使用的地址 hashTableNode的 同一个hash值 但不同的node
};
template<class T>
class hashTable
{
public:
	hashTableNode<T> **table;//哈希表数组的指针，数组的每个元素是一个指向 hashTableNode 的指针
	int tableSize;//hash表大小
	hashTable(int s):tableSize(s)
	{
		table = new hashTableNode<T>*[s];//s是hash表的大小
		memset(table, NULL, tableSize * sizeof(hashTableNode<T>*));
	}
	hashTable()
	{
	}
	void init(int s)
	{
		tableSize = s;
		table = new hashTableNode<T>*[s];
		for(int i=0;i<s;i++)
			table[i]=NULL;
	//	memset(table, NULL, tableSize * sizeof(hashTableNode<T>*));
	}
	~hashTable()
	{
		cleanupHashTable();
		delete [] table;
	}
	void cleanupHashTable()
	{
		hashTableNode<T>*np, *tmp;
		for (int i = 0; i < tableSize; ++i)
		{
			if (table[i] != NULL)
			{
				np = table[i];
				while (np != NULL)
				{
					tmp = np->next;
					delete np;
					np = tmp;
				}
			}
		}
	}
	void insert(unsigned int hash, T value)//插入一个键值对到哈希表中（用链表解决哈希碰撞）
	{
		hashTableNode<T> *np;
		bool inTable;//判断是否已经在当中
		np = table[hash%tableSize];//np指向一个根据输入的hash可以放入的地址
		inTable = false;
		for (; np != NULL; np = np->next)//判断可以放入的地址中有没有存储该边
		{
			if (np->key == hash && np->value == value)
			{
				inTable = true;
				break;
			}
		}
		if (!inTable)//如果没有被存储
		{
			hashTableNode<T>* newNode = new hashTableNode<T>;
			newNode->key = hash;
			newNode->value = value;
			newNode->next = table[hash%tableSize];//相当于头插法
			table[hash%tableSize] = newNode;
		}
	}
	void getID(unsigned int hash, vector<T>&IDs) //获取指定哈希值对应的ID列表
	{
		hashTableNode<T> *np;
		np=table[hash%tableSize];
		for(;np != NULL; np=np->next)
		{
			if(np->key==hash)
			{
				IDs.push_back(np->value);
			}
		}
		return;
	}
	int countIDnums(unsigned int hash)//计算指定哈希值对应的ID数量
{
	int num=0;
	hashTableNode<T> *np;
	np=table[hash%tableSize];
	for(;np!=NULL;np=np->next)
	{
		if(np->key==hash)
			num++;
	}
	return num;
}
};

