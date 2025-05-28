#include<iostream>
#include<map>
#include<vector>
#include<set> 
#include<string>
#include<algorithm>
using namespace std;
struct edge
{
	public:
	int node;
	set<int> label;
	edge(int n, int l)//这是int L 不是1 构造函数
	{
		node=n;
		label.insert(l);
	}
};
class node
{
	public:
	int id;
	int label;
	vector<edge> pre;//前驱边列表
	vector<edge> suc;//后继边列表
	node(int i, int l)
	{
		id=i;
		label=l;
	}
	void insertp(int n, int l)//插入一个前驱边 如果目标节点已经存在，则添加标签；否则，创建一个新的边。
	{
		bool find=false;
		for(int i=0;i<pre.size();i++)
		{
			if(pre[i].node==n)
			{
				find=true;
				pre[i].label.insert(l);
			}	
		}
		if(!find)
		{
			edge e(n,l);
			pre.push_back(e);
		}
	}
	void inserts(int n, int l)//插入一个后继边。如果目标节点已经存在，则添加标签；否则，创建一个新的边。
	{
		bool find=false;
		for(int i=0;i<suc.size();i++)
		{
			if(suc[i].node==n)
			{
				find=true;
				suc[i].label.insert(l);
			}	
		}
		if(!find)
		{
			edge e(n,l);
			suc.push_back(e);
		}
	}
	void has_pre(int n,set<int> &P)//检查是否有指定的前驱边
	{
		for(int i=0;i<pre.size();i++)
		{
			if(pre[i].node==n)
			{	
			 P.insert(pre[i].label.begin(),pre[i].label.end());//并将其标签添加到集合 P 中
			 return;
			}
		}
		return;
	}
	void has_suc(int n,set<int> &S)//检查是否有指定的后继边，
	{
		for(int i=0;i<suc.size();i++ )
		{
			if(suc[i].node==n)
			{	
			 S.insert(suc[i].label.begin(),suc[i].label.end());//并将其标签添加到集合 S 中
			 return;
			}
		}
		return;
	}
 };
 class graph
 {
 	public:
 	vector<node> Node;
 	map<int, int> nodemap;
 	int nodenum;
 	graph(){
 		nodenum=0;
	 };
	 void insert_node(int n, int l)
	 {
	 	map<int,int>::iterator it=nodemap.find(n);
	 	if(it==nodemap.end())
	 	{
		    node N(n, l);
	 		Node.push_back(N);
	 		nodemap[n]=nodenum;
	 		nodenum++;
	 	}
	 }
	 void insert_edge(int s, int d, int l)
	 {
	 	for(int i=0;i<Node.size();i++)
	 	{
	 		if(Node[i].id==d)
	 			Node[i].insertp(s,l);//添加d节点的先驱节点为s 标签为l
	 		if(Node[i].id==s)
	 			Node[i].inserts(d,l);//添加s节点的后继节点为d 标签为l
		 }
	}
  } ;
