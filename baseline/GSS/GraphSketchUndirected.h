#include<iostream>
#include<string>
#include<vector>
#include<queue>
#include<set>
#include<map>
#include<cmath>
#include<stdlib.h>
#ifndef HASHTABLE_H
#define HASHTABLE_H
#include "hashTable.h"
#endif // HASHTABLE_H
struct GSnode
{
	unsigned int h;
	GSnode* next;
};
struct degree_node
{
	unsigned int h;
	int degree;
};
bool comp_degree(const degree_node &dn1, const degree_node &dn2)
{
	return dn1.degree<dn2.degree;
}
class GSketch
{
	private:
	map<unsigned int, int> degree; //存储每个节点度数
	map<unsigned int, int> index;//存储每个节点的索引
	map<unsigned int, int> idnums;//存储每个节点的 ID 
	vector<degree_node> sorted_nodes;//存储按度数排序的节点
	int nodenum;
	int triangle_num;
	int edge_num;
	vector<GSnode*> sketch;//存储图的邻接链表
	public:
	GSketch(){nodenum=0;
	edge_num=0;
	triangle_num=0; 
	};
	~GSketch(){
	degree.clear();
	for(int i=0;i<nodenum;i++)1
	{
		GSnode* np=sketch[i];
		GSnode* tmp;
		while(np!=NULL)
		{
			tmp=np->next;
			delete np;
			np=tmp;
		}
	}
	sketch.clear(); 
	index.clear();  
	idnums.clear();
	sorted_nodes.clear(); };
	void get_neighbors(unsigned int h, vector<hashvalue>& hv);
	void insert_edge(unsigned int h1, unsigned int h2);
	void insert_node(unsigned int h1, int IDnum);
	void clean();
	int countTriangle();	
};
void GSketch::insert_node(unsigned int h1, int IDnum)//插入新的节点
{
	index[h1]=nodenum;//新节点放在nodenum位置 因为从0开始记位置
	degree[h1]=0;
	idnums[h1]=IDnum;//记录新节点的ID
	GSnode* np=new GSnode;
	np->h=h1;
	np->next=NULL;
	sketch.push_back(np);//将新节点加入邻接链表
	nodenum++;
}
void GSketch::insert_edge(unsigned int h1, unsigned int h2)//插入边
{
	degree[h1]++;
	degree[h2]++;
	GSnode* np = sketch[index[h1]];//找到邻接链表中该节点的位置 在h1的表后面接上h2
	for(;np!=NULL;np=np->next)
	{
		if(np->h==h2)
			break;
		if(np->next==NULL)
		{
			GSnode* newn=new GSnode;
			newn->h=h2;
			newn->next=NULL;
			np->next=newn;
			edge_num++;
			break;
		}
	}
	np = sketch[index[h2]];
	for(;np!=NULL;np=np->next)//在h2的节点后面接上h1
	{
		if(np->h==h1)
			break;
		if(np->next==NULL)
		{
			GSnode* newn=new GSnode;
			newn->h=h1;
			newn->next=NULL;
			np->next=newn;
			edge_num++;
			break;
		}
	}
}
void GSketch::clean()
{
	map<unsigned int, int> sequence;
	map<unsigned int, int>::iterator it;
	for(it=degree.begin();it!=degree.end();it++)
	{
		degree_node dn;
		dn.h=it->first;
		dn.degree=it->second;
		sorted_nodes.push_back(dn);
	}
	sort(sorted_nodes.begin(),sorted_nodes.end(),comp_degree);
	for(int i=0;i<nodenum;i++)
		sequence[sorted_nodes[i].h]=i;
	for(int i=0;i<nodenum;i++)
	{
		GSnode* np=sketch[i];
		int seq_d=sequence[np->h];
		GSnode* parent=np;
		np=np->next;
		while(np!=NULL)
		{
			if(sequence[np->h]<=seq_d)
			{
				parent->next=np->next;
				delete np;
				np=parent->next;
				edge_num--;
			}
			else
			{
				parent=np;
				np=np->next;
			}
		}
	}
 }
 void GSketch::get_neighbors(unsigned int h, vector<hashvalue>& hv)// 用于获取指定节点的所有邻居节点
 {
 	map<unsigned int, int>::iterator it;
 	it=index.find(h);//也就是找到了sketch中的h节点的信息
 	if(it==index.end()) return;//说明不存在h节点 也就是不存在查找这个说法
 	GSnode* np=sketch[it->second];//it->second才是index号
 	np=np->next;//获得第一个邻居
 	for(;np!=NULL;np=np->next)
 	{
 	//	cout<<"in"<<endl;
 		if(np->h==h)
 			cout<<"loop!"<<endl;
 		hashvalue new_hv;
 		new_hv.key=np->h;
 		new_hv.IDnum=idnums[np->h];
 		hv.push_back(new_hv);
	 }
	 return;
  } 
  int GSketch::countTriangle()//用于计算图中的三角形数量。它通过遍历每个节点及其邻居节点，计算共同邻居的数量来统计三角形
  {
  	int count=0;
  	for(int i=0;i<nodenum;i++)
  	{
  		unsigned int h1=sorted_nodes[i].h;
  		int num1=idnums[h1];
  		vector<hashvalue> set1;
  		get_neighbors(h1, set1);
  		sort(set1.begin(),set1.end(),mycomp);
  	//	cout<<set1.size()<<endl;
  		for(int j=0;j<set1.size();j++)
  		{
  			unsigned int h2=set1[j].key;
  			int num2=set1[j].IDnum;
  			vector<hashvalue> set2;
  			get_neighbors(h2,set2);
  			sort(set2.begin(),set2.end(),mycomp);
  			int num3=countjoin(set1,set2);
  		//	cout<<num1<<' '<<num2<<' '<<num3<<endl;
  			count+=num1*num2*num3;
		  }
	  }
	  return count;
  }
