#ifndef GSS_H
#define GSS_H


#include<iostream>
#include<string>
#include<vector>
#include<queue>
#include<set>
#include<map>
#include<cmath>
#include<stdlib.h>
#include<bitset>
#include<memory.h>
#ifndef HASHTABLE_H
#define HASHTABLE_H
#include "GraphUndirected.h" 
#endif // HASHTABLE_H
using namespace std;
#define prime 739
#define bigger_p 1048576
#define timer 5
#define M 80000


#define Roomnum 4// This is the parameter to controll the maximum number of rooms in a bucket. 

struct cellinfo{
	unsigned int fingerprint;
	uint16_t index;
};

struct basket
{
	unsigned int src[Roomnum];
	unsigned int dst[Roomnum];
	short  weight[Roomnum];
	long long idx; // need to change to unsigned long long if Roomnum is larger than 4.
};
struct mapnode
{
	unsigned int h;
	unsigned short g;
};
struct linknode
{
	long long key;
	short weight;
	linknode* next;
};


class GSS
{
private:
	hashTable<string> mapTable;
	
	
	bool useT;
	int tablesize;//定义矩阵X的大小
	
	

	public:
	    basket* value;
        int s;
		int w;
		int p;
		int differentedge;
		int f;
		vector<linknode*> buffer;//邻接链表B
		map<unsigned int, int> index;
		int n;
		int edge_num; // count the number of edges in the buffer to assist buffer size analysis. Self loop edge is not included as it does not use additional memory.
	    int r;
		unsigned int datainbuffer;
		GSS(int width, int range, int p_num, int size,int f_num, bool usetable, int tablesize); // table size is set approximately the the number of nodes in the graph.
		~GSS()
		{
			delete[] value;
			cleanupBuffer();

		 }
		 void insert(string s1, string s2,int weight);
		 void insertc(string s1,string s2,int weight);
		 void cleanupBuffer();
		 int edgeQuery(string s1, string s2);
		 int edgeQueryc(string s1, string s2);
		 bool query(string s1, string s2);
		 int nodeValueQuery(string s1, int type);//src_type = 0 dst_type = 1
		 int nodeDegreeQuery(string s1, int type);//src_type = 0 dst_type = 1
		 void nodeSuccessorQuery(string s1, vector<string> &IDs);
		 void nodeSuccessorQuery(vector<string> &s1, vector<string> &IDs);
		 void nodePrecursorQuery(string s2, vector<string> &IDs); 
		 void nodePrecursorQuery(vector<string> &s2, vector<string> &IDs); 
		 int TriangleCounting();
		 bool reachabilityQuery(string s1,string s2);
		 int findnearbyvertex(string s1,int k);
		 void TopKheavyhitter(vector<string> &lotsnode);
};

GSS::GSS(int width, int range, int p_num, int size,int f_num, bool usehashtable, int TableSize)//the side length of matrix, the length of hash addtress list, the number of candidate bucekt
// the number of rooms, whether to use hash table, and the size of the table.
// Hash table which stores the original nodes can be omitted if not needed. For nodequery, 
//  reachability, edgequery not needed. But needed for triangel counting, degree query, and successor / precursor queries.
{
	datainbuffer=0;
	differentedge=0;
	w = width;//矩阵的边长 论文里面的m
	r = range; /* r x r mapped baskets */
	p = p_num; /*candidate buckets*///候选桶的数量
	s = size; /*multiple rooms*/
	f = f_num; /*finger print lenth*/
	n = 0;
	edge_num = 0;
	value = new basket[w*w];//用于储存边的信息对应矩阵X
	useT=usehashtable;//是否使用hash table
	tablesize=TableSize;//大小
	memset(value, 0, sizeof(basket)*w*w);
	if(usehashtable) 
	mapTable.init(tablesize);//相当于用tablesize初始化了一个hashTable
}

void GSS::cleanupBuffer()
{
	vector<linknode*>::iterator IT = buffer.begin();
	linknode* e, *tmp;
	for (; IT != buffer.end(); ++IT)
	{
		e = *IT;
		while (e != NULL)
		{
			tmp = e->next;
			delete e;
			e = tmp;
		}
	}
}


void GSS::insert(string s1, string s2,int weight)// s1 is the ID of the source node, s2 is the ID of the destination node, weight is the edge weight.
{		
		unsigned int hash1 = (*hfunc[0])((unsigned char*)(s1.c_str()), s1.length());//用第一个hash函数来生成值
		unsigned int hash2 = (*hfunc[0])((unsigned char*)(s2.c_str()), s2.length());
		unsigned int tmp = pow(2,f)-1;//指纹最大的值2的f次方-1
		unsigned short g1 = hash1 & tmp;//算指纹
		if(g1==0) g1+=1;
		unsigned int h1 = (hash1>>f)%w;//矩阵X的位置
		unsigned short g2 = hash2 & tmp;
		if(g2==0) g2+=1;
		unsigned int h2 = (hash2>>f)%w;
		
		unsigned int k1 = (h1<<f)+g1;
		unsigned int k2 = (h2<<f)+g2;
	
	//更新hashTable
		if(useT){
		mapTable.insert(k1, s1);//k1是hash值 s1是value
		mapTable.insert(k2, s2);
		}
		
		int* tmp1 = new int[r];//要生成r个可以映射的地址
		int* tmp2 = new int[r];
		tmp1[0] = g1;
		tmp2[0] = g2;
		for(int i=1;i<r;i++)
		{
			tmp1[i]=(tmp1[i-1]*timer+prime)%bigger_p;//像论文里面那样计算可以插入的r个位置 多重hash的中介变量
			tmp2[i]=(tmp2[i-1]*timer+prime)%bigger_p;
		}
		bool inserted=false;
		long key = g1+g2; 
		for(int i=0;i<p;i++)
		{
			key = (key*timer+prime)%bigger_p;
			int index = key%(r*r);
			int index1 = index/r;
			int index2 = index%r; 
			int p1 = (h1+tmp1[index1])%w;
			int p2 = (h2+tmp2[index2])%w;


			int pos = p1*w + p2;//插入的位置
			for (int j = 0; j < s; j++)
			{
				//如果已经在此bucket中的第j个room中存了 将权重加上即可
				int a=( ((value[pos].idx>>(j<<3))&((1<<8)-1)) == (index1|(index2<<4)) );
				if ( (value[pos].src[j]== g1) && (value[pos].dst[j] == g2) )
				{
					value[pos].weight[j] += weight;
					inserted = true;
					break;
				}
				//如果该room是空的 可以存在这里
				if (value[pos].src[j] == 0)
				{
					value[pos].idx |= ((index1 | (index2 << 4)) << (j<<3));
					value[pos].src[j] = g1;
					value[pos].dst[j] = g2;
					value[pos].weight[j] = weight;
					differentedge++;
					inserted = true;//标致这已经插入成功
					break;
				}
			}
			if(inserted)
				break;
		} 
		//如果这里还没插入 则说明X矩阵可以插入的地方都满了 插入buffer
		if(!inserted)
		{
			
		    	
			map<unsigned int, int>::iterator it = index.find(k1);//找到k1对应的索引号
			if(it!=index.end())
			{
				int tag = it->second;
				linknode* node = buffer[tag];
				while(true)
				{
					if (node->key == k2)
					{   
						node->weight += weight;
						break;
					}
					if(node->next==NULL)
					{
						differentedge++;
						datainbuffer++;
						linknode* ins = new linknode;
						ins->key = k2;
						ins->weight = weight;
						ins->next = NULL;
						node->next = ins;
						edge_num++;
						break;
					}
					node = node->next;
				}
			}//下面的情况说明查找不到k1对应的索引号 就需要新插入一个
			else
			{
				differentedge++;
				datainbuffer++;
				index[k1] = n;
				n++;
				linknode* node = new linknode;
				node->key = k1;
				node->weight = 0;
				if (k1 != k2)//k1==k2 means loop
				{
					linknode* ins = new linknode;
					ins->key = k2;
					ins->weight = weight;
					ins->next = NULL;
					node->next = ins;
					edge_num++;
				}
				else
				{ 
					node->weight += weight;
					node->next = NULL;
				}
				buffer.push_back(node); 
			}	
		}
		delete [] tmp1;
		delete [] tmp2;
	return;
}


void GSS::insertc(string s1, string s2,int weight)// s1 is the ID of the source node, s2 is the ID of the destination node, weight is the edge weight.
{		
		unsigned int hash1 = (*hfunc[0])((unsigned char*)(s1.c_str()), s1.length());//用第一个hash函数来生成值
		unsigned int hash2 = (*hfunc[0])((unsigned char*)(s2.c_str()), s2.length());
		unsigned int tmp = pow(2,f)-1;//指纹最大的值2的f次方-1
		unsigned short g1 = hash1 & tmp;//算指纹
		if(g1==0) g1+=1;
		unsigned int h1 = (hash1>>f)%w;//矩阵X的位置
		unsigned short g2 = hash2 & tmp;
		if(g2==0) g2+=1;
		unsigned int h2 = (hash2>>f)%w;
		
		unsigned int k1 = (h1<<f)+g1;
		unsigned int k2 = (h2<<f)+g2;
		cout<<value[5570705].weight[0]<<endl;
	
	//更新hashTable
		if(useT){
		mapTable.insert(k1, s1);//k1是hash值 s1是value
		mapTable.insert(k2, s2);
		}
		
		int* tmp1 = new int[r];//要生成r个可以映射的地址
		int* tmp2 = new int[r];
		tmp1[0] = g1;
		tmp2[0] = g2;
		for(int i=1;i<r;i++)
		{
			tmp1[i]=(tmp1[i-1]*timer+prime)%bigger_p;//像论文里面那样计算可以插入的r个位置 多重hash的中介变量
			tmp2[i]=(tmp2[i-1]*timer+prime)%bigger_p;
		}

		cout<<value[5570705].weight[0]<<endl;
		bool inserted=false;
		long key = g1+g2; 
		for(int i=0;i<p;i++)
		{
			key = (key*timer+prime)%bigger_p;
			int index = key%(r*r);
			int index1 = index/r;
			int index2 = index%r; 
			int p1 = (h1+tmp1[index1])%w;
			int p2 = (h2+tmp2[index2])%w;


			int pos = p1*w + p2;//插入的位置
			cout<<"pos: "<<pos<<endl;
			cout<<value[5570705].weight[0]<<endl;
			for (int j = 0; j < s; j++)//s=roomnum
			{
				cout<<"j::"<<j<<value[5570705].weight[0]<<endl;
				//如果已经在此bucket中的第j个room中存了 将权重加上即可
				if ( ( ((value[pos].idx>>(j<<3))&((1<<8)-1)) == (index1|(index2<<4)) ) && (value[pos].src[j]== g1) && (value[pos].dst[j] == g2) )
				{
					value[pos].weight[j] += weight;
					inserted = true;
					cout<<"already stored in pos:: "<<pos<<endl;
					break;
				}
				//如果该room是空的 可以存在这里
				if (value[pos].src[j] == 0)
				{
					//value[pos].idx |= ((index1 | (index2 << 4)) << (j<<3));
					value[pos].src[j] = g1;
					value[pos].dst[j] = g2;
					value[pos].weight[j] = weight;
					cout<<"find an available place::"<<j<<endl;
					inserted = true;//标致这已经插入成功
					break;
				}
			}
			if(inserted)
			break;
		} 


		
		//如果这里还没插入 则说明X矩阵可以插入的地方都满了 插入buffer
		if(!inserted)
		{
		    cout<<"need insert in buffer"<<endl;
			map<unsigned int, int>::iterator it = index.find(k1);//找到k1对应的索引号
			if(it!=index.end())
			{
				int tag = it->second;
				linknode* node = buffer[tag];
				while(true)
				{
					if (node->key == k2)
					{   
						node->weight += weight;
						break;
					}
					if(node->next==NULL)
					{
						linknode* ins = new linknode;
						ins->key = k2;
						ins->weight = weight;
						ins->next = NULL;
						node->next = ins;
						edge_num++;
						break;
					}
					node = node->next;
				}
			}//下面的情况说明查找不到k1对应的索引号 就需要新插入一个
			else
			{
				index[k1] = n;
				n++;
				linknode* node = new linknode;
				node->key = k1;
				node->weight = 0;
				if (k1 != k2)//k1==k2 means loop
				{
					linknode* ins = new linknode;
					ins->key = k2;
					ins->weight = weight;
					ins->next = NULL;
					node->next = ins;
					edge_num++;
				}
				else
				{ 
					node->weight += weight;
					node->next = NULL;
				}
				buffer.push_back(node); 
			}	
		}
		delete [] tmp1;
		delete [] tmp2;
		cout<<value[5570705].weight[0]<<endl;
	return;
}

//s1是要查询的点 IDs储存结果
void GSS::nodeSuccessorQuery(string s1, vector<string>&IDs)// query the successors of a node, s1 is the ID of the queried node. results are put in the vector, hash table needed.
{
	unsigned int hash1 = (*hfunc[0])((unsigned char*)(s1.c_str()), s1.length());
	int tmp=pow(2,f)-1;//f是指纹的位数
	unsigned short g1=hash1 & tmp;//hash1低位部分 论文里面的finger point
	if(g1==0) g1+=1;
	unsigned int h1 = (hash1>>f)%w;//hash1的高位部分
	unsigned int k1 = (h1 << f) + g1;//组合
	int* tmp1 = new int[r];
	tmp1[0] = g1;//为了求出s1节点所有可能在的地方 要一组hash

	//求可能在的位置
	for (int i = 1; i < r; i++)
	{
		tmp1[i] = (tmp1[i - 1] * timer + prime) % bigger_p;
	}

	for (int i = 0; i < r; i++)
	{ 
		int p1 = (h1 + tmp1[i]) % w;
		for (int k = 0; k < w; k++)
		{
			int pos = p1*w + k;//这个就是可能在矩阵X的位置 也就是代码里面的value[pos]
			for (int j = 0; j < s; ++j)
			{
				if ((((value[pos].idx >> ((j << 3)))&((1 << 4) - 1)) == i) && (value[pos].src[j] == g1))//这个地方还用了多个房间 在第二个条件验证指纹
				{
					     int tmp_g = value[pos].dst[j];
						 int tmp_s = ((value[pos].idx >> ((j << 3) + 4))&((1 << 4) - 1));
				
						 int shifter = tmp_g;
						 for (int v = 0; v < tmp_s; v++)
							 shifter = (shifter*timer + prime) % bigger_p;
						 int tmp_h = k;
						 while (tmp_h < shifter)
							 tmp_h += w;
						 tmp_h -= shifter;
						 int val = (tmp_h << f) + tmp_g;//得到了hash值 要还原出答案节点
						 mapTable.getID(val, IDs);//要把答案存入
				}
			}
		}	
	}

	//矩阵X的部分上面已经查找完成了 现在要查找缓冲区B的部分
		map<unsigned int, int>::iterator it = index.find(k1);
		if (it != index.end())
		{
			int tag = it->second;
			linknode* node = buffer[tag];
			if(node->weight!=0)
				mapTable.getID(k1, IDs); // self-loop
			node = node->next;
			while (node != NULL)
			{
				mapTable.getID(node->key, IDs);
				node=node->next;
			}
		}
		delete []tmp1;//删除临时数组 
		return; 
}


void GSS::nodeSuccessorQuery(vector<string> &sall, vector<string>&IDs)// query the successors of a node, s1 is the ID of the queried node. results are put in the vector, hash table needed.
{
	for(string s1:sall){
	unsigned int hash1 = (*hfunc[0])((unsigned char*)(s1.c_str()), s1.length());
	int tmp=pow(2,f)-1;//f是指纹的位数
	unsigned short g1=hash1 & tmp;//hash1低位部分 论文里面的finger point
	if(g1==0) g1+=1;
	unsigned int h1 = (hash1>>f)%w;//hash1的高位部分
	unsigned int k1 = (h1 << f) + g1;//组合
	int* tmp1 = new int[r];
	tmp1[0] = g1;//为了求出s1节点所有可能在的地方 要一组hash

	//求可能在的位置
	for (int i = 1; i < r; i++)
	{
		tmp1[i] = (tmp1[i - 1] * timer + prime) % bigger_p;
	}

	for (int i = 0; i < r; i++)
	{ 
		int p1 = (h1 + tmp1[i]) % w;
		for (int k = 0; k < w; k++)
		{
			int pos = p1*w + k;//这个就是可能在矩阵X的位置 也就是代码里面的value[pos]
			for (int j = 0; j < s; ++j)
			{
				if ((((value[pos].idx >> ((j << 3)))&((1 << 4) - 1)) == i) && (value[pos].src[j] == g1))//这个地方还用了多个房间 在第二个条件验证指纹
				{
					     int tmp_g = value[pos].dst[j];
						 int tmp_s = ((value[pos].idx >> ((j << 3) + 4))&((1 << 4) - 1));
				
						 int shifter = tmp_g;
						 for (int v = 0; v < tmp_s; v++)
							 shifter = (shifter*timer + prime) % bigger_p;
						 int tmp_h = k;
						 while (tmp_h < shifter)
							 tmp_h += w;
						 tmp_h -= shifter;
						 int val = (tmp_h << f) + tmp_g;//得到了hash值 要还原出答案节点
						 mapTable.getID(val, IDs);//要把答案存入
				}
			}
		}	
	}

	//矩阵X的部分上面已经查找完成了 现在要查找缓冲区B的部分
		map<unsigned int, int>::iterator it = index.find(k1);
		if (it != index.end())
		{
			int tag = it->second;
			linknode* node = buffer[tag];
			if(node->weight!=0)
				mapTable.getID(k1, IDs); // self-loop
			node = node->next;
			while (node != NULL)
			{
				mapTable.getID(node->key, IDs);
				node=node->next;
			}
		}
		delete []tmp1;//删除临时数组 
		}
		return;
}


void GSS::nodePrecursorQuery(string s1, vector<string>&IDs) // same as successor query
{

	
	unsigned int hash1 = (*hfunc[0])((unsigned char*)(s1.c_str()), s1.length());
	int tmp=pow(2,f)-1;
	unsigned short g1=hash1 & tmp;
	unsigned int h1 = (hash1>>f)%w;
	if(g1==0) g1+=1;
	int* tmp1 = new int[r];
	tmp1[0] = g1;
	unsigned int k1 = (h1 << f) + g1;
	
	for (int i = 1; i < r; i++)
	{
		tmp1[i] = (tmp1[i - 1] * timer + prime) % bigger_p;
	}
	for (int i = 0; i < r; i++)
	{
		int p1 = (h1 + tmp1[i]) % w;
		for (int k = 0; k < w; k++)
		{
			int pos = p1 + k*w;
			for (int j = 0; j < s; ++j)
			{
				if ((((value[pos].idx >> ((j << 3)+4))&((1 << 4) - 1)) == i) && (value[pos].dst[j] == g1))
				{
					     int tmp_g = value[pos].src[j];
						 int tmp_s = ((value[pos].idx >> (j << 3))&((1 << 4) - 1));
				
						 int shifter = tmp_g;
						 for (int v = 0; v < tmp_s; v++)
							 shifter = (shifter*timer + prime) % bigger_p;
						 int tmp_h = k;
						 while (tmp_h < shifter)
							 tmp_h += w;
						 tmp_h -= shifter;
						 
						 int val = (tmp_h << f) + tmp_g;
						 mapTable.getID(val, IDs);
				}
			}
		}	
	}
	
		map<unsigned int, int>::iterator it = index.find(k1);
		if(it!=index.end())
		{
			if(buffer[it->second]->weight!=0)
				mapTable.getID(k1, IDs);
		}
	
			for (map<unsigned int, int>::iterator it = index.begin(); it != index.end(); ++it)
		{
			int tag = it->second;
			int src = it->first;
			linknode* node = buffer[tag];
			node = node->next;
			while (node != NULL)
			{
				if(node->key == k1)
				{
					mapTable.getID(src, IDs);
					break;	
				}
				node = node->next;
			}
		}
		delete []tmp1;
		return;
}


void GSS::nodePrecursorQuery(vector<string> &sall, vector<string>&IDs) // same as successor query
{
	for(string s1:sall){
	unsigned int hash1 = (*hfunc[0])((unsigned char*)(s1.c_str()), s1.length());
	int tmp=pow(2,f)-1;
	unsigned short g1=hash1 & tmp;
	unsigned int h1 = (hash1>>f)%w;
	if(g1==0) g1+=1;
	int* tmp1 = new int[r];
	tmp1[0] = g1;
	unsigned int k1 = (h1 << f) + g1;
	for (int i = 1; i < r; i++)
	{
		tmp1[i] = (tmp1[i - 1] * timer + prime) % bigger_p;
	}
	for (int i = 0; i < r; i++)
	{
		int p1 = (h1 + tmp1[i]) % w;
		for (int k = 0; k < w; k++)
		{
			int pos = p1 + k*w;
			for (int j = 0; j < s; ++j)
			{
				if ((((value[pos].idx >> ((j << 3)+4))&((1 << 4) - 1)) == i) && (value[pos].dst[j] == g1))
				{
					     int tmp_g = value[pos].src[j];
						 int tmp_s = ((value[pos].idx >> (j << 3))&((1 << 4) - 1));
				
						 int shifter = tmp_g;
						 for (int v = 0; v < tmp_s; v++)
							 shifter = (shifter*timer + prime) % bigger_p;
						 int tmp_h = k;
						 while (tmp_h < shifter)
							 tmp_h += w;
						 tmp_h -= shifter;
						 
						 int val = (tmp_h << f) + tmp_g;
						 mapTable.getID(val, IDs);
				}
			}
		}	
	}
	
		map<unsigned int, int>::iterator it = index.find(k1);
		if(it!=index.end())
		{
			if(buffer[it->second]->weight!=0)
				mapTable.getID(k1, IDs);
		}
	
			for (map<unsigned int, int>::iterator it = index.begin(); it != index.end(); ++it)
		{
			int tag = it->second;
			int src = it->first;
			linknode* node = buffer[tag];
			node = node->next;
			while (node != NULL)
			{
				if(node->key == k1)
				{
					mapTable.getID(src, IDs);
					break;	
				}
				node = node->next;
			}
		}
		delete []tmp1;
		
	}
	return ;
}


int GSS::edgeQuery(string s1, string s2)// s1 is the ID of the source node, s2 is the ID of the destination node, return the weight of the edge
{
	unsigned int hash1 = (*hfunc[0])((unsigned char*)(s1.c_str()), s1.length());
	unsigned int hash2 = (*hfunc[0])((unsigned char*)(s2.c_str()), s2.length());
	int tmp = pow(2, f) - 1;
	unsigned short g1 = hash1 & tmp;
	if (g1 == 0) g1 += 1;
	unsigned int h1 = (hash1 >> f) % w;
	unsigned short g2 = hash2 & tmp;
	if (g2 == 0) g2 += 1;
	unsigned int h2 = (hash2 >> f) % w;
	int* tmp1 = new int[r];
	int* tmp2 = new int[r];
	tmp1[0] = g1;
	tmp2[0] = g2;
	for (int i = 1; i<r; i++)//生成一系列中间的地址
	{
		tmp1[i] = (tmp1[i - 1] * timer + prime) % bigger_p;
		tmp2[i] = (tmp2[i - 1] * timer + prime) % bigger_p;
	}
	long key = g1 + g2;

	for (int i = 0; i<p; i++)
	{
		key = (key * timer + prime) % bigger_p;
		int index = key % (r*r);
		int index1 = index / r;
		int index2 = index%r;
		int p1 = (h1 + tmp1[index1]) % w;
		int p2 = (h2 + tmp2[index2]) % w;
		int pos = p1*w + p2;
		for (int j = 0; j<s; j++)
		{
		
			int a=((value[pos].idx >> (j << 3))&((1 << 8) - 1));
			int b=(index1 | (index2 << 4));
			if ((value[pos].src[j] == g1) && (value[pos].dst[j] == g2))//找到这条边
			{
				delete []tmp1;
				delete []tmp2;
				return value[pos].weight[j];
			}
		}
		
	}
	unsigned int k1 = (h1 << f) + g1;//还原成原本的hash
	unsigned int k2 = (h2 << f) + g2;
	map<unsigned int, int>::iterator it = index.find(k1);
	if (it != index.end())//在buffer查找
	{
		int tag = it->second;
		linknode* node = buffer[tag];
		while (node!=NULL)
		{
			if (node->key == k2)
			{
				delete []tmp1;
				delete []tmp2;
				return node->weight;
			}
			node = node->next;
		}
	}
		delete []tmp1;
		delete []tmp2;
		return 0;



	
	
}


int GSS::edgeQueryc(string s1, string s2)// s1 is the ID of the source node, s2 is the ID of the destination node, return the weight of the edge
{
	unsigned int hash1 = (*hfunc[0])((unsigned char*)(s1.c_str()), s1.length());
	unsigned int hash2 = (*hfunc[0])((unsigned char*)(s2.c_str()), s2.length());
	int tmp = pow(2, f) - 1;
	unsigned short g1 = hash1 & tmp;
	if (g1 == 0) g1 += 1;
	unsigned int h1 = (hash1 >> f) % w;
	unsigned short g2 = hash2 & tmp;
	if (g2 == 0) g2 += 1;
	unsigned int h2 = (hash2 >> f) % w;
	int* tmp1 = new int[r];
	int* tmp2 = new int[r];
	tmp1[0] = g1;
	tmp2[0] = g2;
	for (int i = 1; i<r; i++)//生成一系列中间的地址
	{
		tmp1[i] = (tmp1[i - 1] * timer + prime) % bigger_p;
		tmp2[i] = (tmp2[i - 1] * timer + prime) % bigger_p;
	}
	long key = g1 + g2;

	for (int i = 0; i<p; i++)
	{
		key = (key * timer + prime) % bigger_p;
		int index = key % (r*r);
		int index1 = index / r;
		int index2 = index%r;
		int p1 = (h1 + tmp1[index1]) % w;
		int p2 = (h2 + tmp2[index2]) % w;
		int pos = p1*w + p2;
		cout<<"pos::"<<pos<<endl;
		for (int j = 0; j<s; j++)
		{
			cout<<"search in "<<j<<" rooms"<<endl;
			cout<<"((value[pos].idx >> (j << 3))&((1 << 8) - 1))::"<<((value[pos].idx >> (j << 3))&((1 << 8) - 1))<<endl;
			cout<<"(index1 | (index2 << 4))::"<<(index1 | (index2 << 4))<<endl;

			if ((((value[pos].idx >> (j << 3))&((1 << 8) - 1)) == (index1 | (index2 << 4))) && (value[pos].src[j] == g1) && (value[pos].dst[j] == g2))//找到这条边
			{
				cout<<"find it"<<endl;
				delete []tmp1;
				delete []tmp2;
				return value[pos].weight[j];
			}
		}
		
	}
	unsigned int k1 = (h1 << f) + g1;//还原成原本的hash
	unsigned int k2 = (h2 << f) + g2;
	map<unsigned int, int>::iterator it = index.find(k1);
	if (it != index.end())//在buffer查找
	{
		int tag = it->second;
		linknode* node = buffer[tag];
		while (node!=NULL)
		{
			if (node->key == k2)
			{
				delete []tmp1;
				delete []tmp2;
				return node->weight;
			}
			node = node->next;
		}
	}
		delete []tmp1;
		delete []tmp2;
		return 0;
}


bool GSS::query(string s1, string s2) // s1 is the ID of the source node, s2 is the ID of the destination node, return whether reachable.
 {
	 unsigned int hash1 = (*hfunc[0])((unsigned char*)(s1.c_str()), s1.length());
	 unsigned int hash2 = (*hfunc[0])((unsigned char*)(s2.c_str()), s2.length());
	 int tmp = pow(2, f) - 1;
	 unsigned short g1 = hash1 & tmp;
	 if (g1 == 0) g1 += 1;
	 unsigned int h1 = (hash1 >> f) % w;
	 unsigned short g2 = hash2 & tmp;
	 if (g2 == 0) g2 += 1;
	 unsigned int h2 = (hash2 >> f) % w;
	 int pos;
	 map<unsigned int, bool> checked;
	 queue<mapnode> q;
	 mapnode e;
	 e.h = h1;
	 e.g = g1;
	 q.push(e);
	 checked[((h1 << f) + g1)] = true;
	 map<unsigned int, bool>::iterator	IT;

	 while (!q.empty())
	 {
		 e = q.front();
		 h1 = e.h;
		 g1 = e.g;
		 int* tmp1 = new int[r];
		 int* tmp2 = new int[r];
		 tmp2[0] = g2;
		 tmp1[0] = g1;
		 for (int i = 1; i<r; i++)
		 {
			 tmp1[i] = (tmp1[i - 1] * timer + prime) % bigger_p;
			 tmp2[i] = (tmp2[i - 1] * timer + prime) % bigger_p;
		 }
		 for (int i1 = 0; i1<r; i1++)
		 {
			 int p1 = (h1 + tmp1[i1]) % w;
			 for (int i2 = 0; i2<r; i2++)
			 {
				 int p2 = (h2 + tmp2[i2]) % w;
				 int pos = p1*w + p2;
				 for (int i3 = 0; i3<s; i3++)
				 {
				
					int a=((value[pos].idx >> (i3 << 3))&((1 << 8) - 1));
					int b=(i1 | (i2 << 4));
					 if ((value[pos].src[i3] == g1) && (value[pos].dst[i3] == g2))
					 {
					 	delete []tmp1;
					 	delete []tmp2;
						 return true;
					 }
				 }
			 }
		 }
		 unsigned int k1 = (h1 << f) + g1;
	//	 bool find = true;
	//	 if (find)
	//	 {
			 map<unsigned int, int>::iterator it = index.find(k1);
			 if (it != index.end())
			 {
				 int tag = it->second;
				 linknode* node = buffer[tag];
				 while (node != NULL)
				 {
					 if (node->key != k1)
					 {
						 unsigned int val = node->key;
						 unsigned int temp_h = (val) >> f;
						 unsigned int tmp = pow(2, f);
						 unsigned short temp_g = (val%tmp);
						 if ((temp_h == h2) && (temp_g == g2))
						{
							delete []tmp1;
					 		delete []tmp2;
							return true;
						}
				
						 IT = checked.find(val);
						 if (IT == checked.end())
						 {
							 mapnode temp_e;
							 temp_e.h = temp_h;
							 temp_e.g = temp_g;
							 q.push(temp_e);
							 checked[val] = true;;
						 }
					 }
					 node = node->next;
				 }
			 }
	//	 }
		 for (int i1 = 0; i1<r; i1++)
		 {
			 int p1 = (h1 + tmp1[i1]) % w;
			 for (int i2 = 0; i2<w; i2++)
			 {
				 int pos = p1*w + i2;
				 for (int i3 = 0; i3<s; i3++)
				 {
		
				
					//&& (((value[pos].idx >> (i3 << 3))&((1 << 4) - 1)) == i1)
					 if (value[pos].src[i3] == g1 )
					 {
						 int tmp_g = value[pos].dst[i3];
						 int tmp_s = ((value[pos].idx >> ((i3 << 3) + 4))&((1 << 4) - 1));
				
						 int shifter = tmp_g;
						 for (int v = 0; v < tmp_s; v++)
							 shifter = (shifter*timer + prime) % bigger_p;
						 int tmp_h = i2;
						 while (tmp_h < shifter)
							 tmp_h += w;
						 tmp_h -= shifter;


						 unsigned int val = (tmp_h << f) + tmp_g;
					 
						 IT = checked.find(val);
						 if (IT == checked.end())
						 {
						
							 mapnode tmp_e;
							 tmp_e.h = tmp_h;
							 tmp_e.g = tmp_g;
							 q.push(tmp_e);
							 checked[val] = true;
						
						 }
					 }
					
				 }
			 }
		 }
		 delete[] tmp1;
		 delete[] tmp2;
		 q.pop();
	 }
	 return false;
 }
/*type 0 is for successor query, type 1 is for precusor query*/


int GSS::nodeValueQuery(string s1, int type) // s1 is the ID of the queried node, function for node query.//type决定了查找出度还是入度
{
	int weight = 0;
	unsigned int hash1 = (*hfunc[0])((unsigned char*)(s1.c_str()), s1.length());
	int tmp = pow(2, f) - 1;
	unsigned short g1 = hash1 & tmp;
	if (g1 == 0) g1 += 1;
	unsigned int h1 = (hash1 >> f) % w;
	int* tmp1 = new int[r];
	tmp1[0] = g1;
	for (int i = 1; i < r; i++)
	{
		tmp1[i] = (tmp1[i - 1] * timer + prime) % bigger_p;
	}
	for (int i = 0; i < r; i++)
	{
		int p1 = (h1 + tmp1[i]) % w;
		for (int k = 0; k < w; k++)
		{
			if (type == 0)/*successor query*/
			{
				int pos = p1*w + k;
				for (int j = 0; j < s; ++j)//s是房间的个数
				{
					
					int b=((value[pos].idx >> ((j << 3)))&((1 << 4) - 1));
					if (type == 0 && (value[pos].src[j] == g1))
					{
						weight += value[pos].weight[j];
					}
				}
			}
			else if (type == 1)/*precursor query*/
			{
				int pos = p1 + k*w;
				for (int j = 0; j < s; ++j)
				{

					int m=((value[pos].idx >> ((j << 3) + 4))&((1 << 4) - 1));
					if (type == 1&& (value[pos].dst[j] == g1))
					{
						weight += value[pos].weight[j];
					}
				}
			}
		}
	}
	if (type == 0)
	{
		unsigned int k1 = (h1 << f) + g1;
		map<unsigned int, int>::iterator it = index.find(k1);
		if (it != index.end())
		{
			int tag = it->second;
			linknode* node = buffer[tag];
			weight += node->weight;
			node = node->next;
			while (node != NULL)
			{
				weight += node->weight;
				node = node->next;
			}
		}
	}
	else if (type==1)
	{
		unsigned int k1 = (h1 << f) + g1;
		map<unsigned int, int>::iterator it = index.find(k1);
		if(it!=index.end())
			weight += buffer[it->second]->weight;
		for (map<unsigned int, int>::iterator it = index.begin(); it != index.end(); ++it)
		{
			int tag = it->second;
			linknode* node = buffer[tag];			
			node = node->next;
			while (node != NULL)
			{
				if(node->key == k1)
					weight += node->weight;
				node = node->next;
			}
		}
	}
	delete []tmp1;
	return weight;
}

/*type 0 is for successor query, type 1 is for precusor query*/
int GSS::nodeDegreeQuery(string s1, int type) // s1 is the ID of the queried node, return the in/out degree
{
	int degree = 0;
	unsigned int hash1 = (*hfunc[0])((unsigned char*)(s1.c_str()), s1.length());
	int tmp = pow(2, f) - 1;
	unsigned short g1 = hash1 & tmp;
	if (g1 == 0) g1 += 1;
	unsigned int h1 = (hash1 >> f) % w;
	int* tmp1 = new int[r];
	tmp1[0] = g1;
	for (int i = 1; i < r; i++)
	{
		tmp1[i] = (tmp1[i - 1] * timer + prime) % bigger_p;
	}
	for (int i = 0; i < r; i++)
	{
		int p1 = (h1 + tmp1[i]) % w;
		for (int k = 0; k < w; k++)
		{
			if (type == 0)/*successor query*/
			{
				int pos = p1*w + k;
				for (int j = 0; j < s; ++j)
				{
					if ((((value[pos].idx >> ((j << 3)))&((1 << 4) - 1)) == i) && (value[pos].src[j] == g1))
					{
						 int tmp_g = value[pos].dst[j];
						 int tmp_s = ((value[pos].idx >> ((j << 3) + 4))&((1 << 4) - 1));
				
						 int shifter = tmp_g;
						 for (int v = 0; v < tmp_s; v++)
							 shifter = (shifter*timer + prime) % bigger_p;
						 int tmp_h = k;
						 while (tmp_h < shifter)
							 tmp_h += w;
						 tmp_h -= shifter;
						 unsigned int val = (tmp_h << f) + tmp_g;
						 
						 
						 degree+=mapTable.countIDnums(val);//这个地方加的是hash碰撞 该hash值的全部节点的个数
					}
				}
			}
			else if (type == 1)/*precursor query*/
			{
				int pos = p1 + k*w;
				for (int j = 0; j < s; ++j)
				{
					if ((((value[pos].idx >> ((j << 3) + 4))&((1 << 4) - 1)) == i) && (value[pos].dst[j] == g1))
					{
						 int tmp_g = value[pos].src[j];
						 int tmp_s = ((value[pos].idx >> (j << 3))&((1 << 4) - 1));
				
						 int shifter = tmp_g;
						 for (int v = 0; v < tmp_s; v++)
							 shifter = (shifter*timer + prime) % bigger_p;
						 int tmp_h = k;
						 while (tmp_h < shifter)
							 tmp_h += w;
						 tmp_h -= shifter;
						 unsigned int val = (tmp_h << f) + tmp_g;
						 
						degree+=mapTable.countIDnums(val);
					}
				}
			}
		}
	}

	if (type == 0)
	{
		unsigned int k1 = (h1 << f) + g1;
		map<unsigned int, int>::iterator it = index.find(k1);
		if (it != index.end())
		{
			int tag = it->second;
			linknode* node = buffer[tag];
			if(node->weight!=0);
				degree += mapTable.countIDnums(k1); // address self-loops first
			node = node->next;
			while (node != NULL)
			{
				degree+=mapTable.countIDnums(node->key);
				node = node->next;
			}
		}
	}
	else if (type == 1)
	{
		unsigned int k1 = (h1 << f) + g1;
		map<unsigned int, int>::iterator it = index.find(k1); // address self-loops first
		if(it!=index.end())
		{
			if(buffer[it->second]->weight!=0)
				degree += mapTable.countIDnums(k1);
		}
		for (map<unsigned int, int>::iterator it = index.begin(); it != index.end(); ++it)
		{
			int tag = it->second;
			linknode* node = buffer[tag];
			unsigned int src=node->key;
			node = node->next;
			while (node != NULL)
			{
				if (node->key == k1)
				{
					degree+=mapTable.countIDnums(src);
					break;
				 } 
				node = node->next;
			}
		}
	}
	delete[]tmp1;
	return degree;
}

void hvinsert(unsigned int hash, std::vector<hashvalue> &v)//将一个hash值插入到向量
{
	bool find=false;
	for(int i=0;i<v.size();i++)
	{
		if (hash==v[i].key)
		{
			v[i].IDnum++;//插入新hash值的时候 这个hash值对应的IDnum+1
			find=true;
			break;
		}
	}
	if(!find)
	{
		hashvalue hv;
		hv.key=hash;
		hv.IDnum=1;
		v.push_back(hv);
	}
	return;
}

int GSS::TriangleCounting()
{
	GSketch* gs = new GSketch;
	for(int i=0;i<tablesize;i++)
	{
		hashTableNode<string> *np=mapTable.table[i];
		vector<hashvalue> nodes;
		for(;np!=NULL;np=np->next)//遍历哈希表 mapTable 的每个桶
		 	hvinsert(np->key,nodes);//把mapTable里面的值插入到gs里 对于每个桶中的节点，调用 hvinsert 函数将节点的键值插入到 nodes 向量中
		int nodenum=nodes.size();
		for(int j=0;j<nodenum;j++)
		{
			//cout<<nodes[j].key<<' '<<nodes[j].IDnum<<endl;
			gs->insert_node(nodes[j].key, nodes[j].IDnum);//将 nodes 向量中的节点插入到 gs 中
		}
		nodes.clear();
	}
	for(int i=0;i<w;i++)//w是一开始论文里X矩阵的宽度和长度
	{
		int row=i*w;//论文里的X矩阵在代码里实际使用value数组存储的
		for(int k=0;k<w;k++)
		{
			int pos=row+k;
			for(int j=0;j<s;j++)//一个位置有s个房间
			{
				if(value[pos].src[j]>0)
				{
					unsigned int tmp_g1 = value[pos].src[j];
					unsigned int tmp_s1 = ((value[pos].idx >> (j << 3))&((1 << 4) - 1));
					
					int shifter = tmp_g1;
					for (int v = 0; v < tmp_s1; v++)
						shifter = (shifter*timer + prime) % bigger_p;
					int tmp_h1 = i;
					while (tmp_h1 < shifter)
						tmp_h1 += w;
					tmp_h1 -= shifter;
						 
					unsigned int val1 = (tmp_h1 << f) + tmp_g1;
					
					 int tmp_g2 = value[pos].dst[j];
					 int tmp_s2 = ((value[pos].idx >> ((j << 3) + 4))&((1 << 4) - 1));
				
					 shifter = tmp_g2;
					 for (int v = 0; v < tmp_s2; v++)
						shifter = (shifter*timer + prime) % bigger_p;
					  int tmp_h2 = k;
					  while (tmp_h2 < shifter)
							 tmp_h2 += w;
					  tmp_h2 -= shifter;
					  unsigned int val2 = (tmp_h2 << f) + tmp_g2;
					  gs->insert_edge(val1,val2);//遍历二维数组 value，提取边的源节点和目标节点，计算其哈希值并插入到 gs 中
				}
			}
		}
	}
	for(int i=0;i<buffer.size();i++)
	{
		linknode* np=buffer[i];
		unsigned int src=np->key;
		np=np->next;
		for(;np!=NULL;np=np->next)
		{
		  unsigned int dst=np->key;
		  gs->insert_edge(src, dst);//遍历 buffer 中的链表，提取边的源节点和目标节点
		}
	}
	gs->clean();
	return gs->countTriangle();//计算图中的三角形数量
	delete gs;
}

bool GSS::reachabilityQuery(string s1,string s2){
	if(edgeQuery(s1,s2)==0)return false;
	return true;
}


int GSS::findnearbyvertex(string s1,int k){

	vector<string> result;
	if(k<1){
		cout<<"illegal query "<<endl;
		return 0;
	}
	
	nodePrecursorQuery(s1,result);
	
	if(k==1)return 0;
	
	for(int i=0;i<(k-2);i++){
		
		vector<string> tem;

		for(string s:result){
			
			nodePrecursorQuery(s,tem);
		}
		result.insert(result.end(),tem.begin(),tem.end());
		tem.clear();
	}

	return result.size();
}

void GSS::TopKheavyhitter(vector<string> &lotsnode){
	string maxid;
	int max=0;
	for(string now:lotsnode){
		int res=nodeValueQuery(now,1);
		if(res>max){
			max=res;
			maxid=now;

		}
	}
	return ;
}
#endif
