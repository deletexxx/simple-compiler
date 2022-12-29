#ifndef DAGOPTIMIZE_H
#define DAGOPTIMIZE_H
#include"semantic.h"
#include<list>

// 一个节点
struct node
{
    int type;
    node *left,*right;  // 左子树和右子树 
    list<string> signlist;// 下标
    string op;// 操作符
    int Value;
    bool isNumber;
    node():type(0),left{NULL},right(NULL),Value(0),isNumber(false){}// 初始化
};

// 先将中间代码四元式划分成基本块，给四元式加上块的信息
void div_base_block(vector<Quat> *quat, int *block_num, vector<int> *per_block_quat_num);

// 节点操作
// 以下都是存在，就返回该节点；不存在，就返回空
node *find(string name, map<int,node*> NodeMap); // 查找是否存在相同名字的node
node * create(string name); // find node为空(不存在已有节点，进行创建)之后再创建新节点
node * findopyz(string op,node *nodey,node *nodez, map<int,node*> NodeMap); // 是否存在相同操作有相同左右子树的节点
node * findxopy(string x,string op,node *nodey, map<int,node*> NodeMap);    // 是否存在节点xopy

// 绘制图（一步一步加）
void add_3_op(string x,string y,string op,string z, map<int,node*> NodeMap); //给图中加上 x=y op z,三个操作数
void add_2_op(string x,string op,string y, map<int,node*> NodeMap);           //给图中加上 x=op y,两个操作数
void add_1_op(string x,string y, map<int,node*> NodeMap);                       //图中加上x:=y,赋值操作
void show();// 打印信息
string printnode(node *nodex); // 节点信息的打印
void print_map(map<int,node*> NodeMap); // 打印图的信息

// 主函数相当于
bool DAG_op();

#endif