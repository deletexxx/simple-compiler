#include"../head/DAGoptimize.h"


extern vector<Quat> quat;// 四元式
int block_num_opt; 
vector<int> per_block_quat_num;// 每个块里四元式的数量

// 主函数相当于
bool DAG_op(){
    div_base_block(&quat, &block_num_opt, &per_block_quat_num);
    int quat_id = 0;
    for(int block_id = 0; block_id < block_num_opt; ++block_id){
        // 每个块分别进行优化
        map<int,node*> NodeMap;// 所有节点组成的DAG图
        for(int j = 0; j < per_block_quat_num[block_id]; ++j){
            if(quat[quat_id].op == "="){
                add_1_op(quat[quat_id].b, quat[quat_id].a, NodeMap);
            }
            else{
                if(quat[quat_id].res == ""){
                    add_2_op(quat[quat_id].b, quat[quat_id].op, quat[quat_id].a, NodeMap);
                }
                else if(quat[quat_id].res != ""){
                    add_3_op(quat[quat_id].b, quat[quat_id].a, quat[quat_id].op, quat[quat_id].res, NodeMap);
                }
            }
        }
        quat_id += 1;
        print_map(NodeMap);// 打印DAG图
        cout<<endl;
    }
    return true;
}


// 给四元式中使用一个整数来给代码划分块
void div_base_block(vector<Quat> *quat, int *block_num_opt, vector<int> *per_block_quat_num){
    *block_num_opt = 0;
    bool if_next = false;
    vector<Quat>::iterator it = (*quat).begin();
    int per_block_num_opt = 0;
    for(it; it<(*quat).end();++it){
        if((*it).op == "if" || (*it).op == "el" || (*it).op == "ie" || (*it).op == "do" || (*it).op == "do" || (*it).op == "we"){
            (*it).block_id = *block_num_opt;
            per_block_num_opt++;
            
            // cout<<per_block_num_opt<<endl;
            // 更新信息
            *block_num_opt += 1;
            (*per_block_quat_num).push_back(per_block_num_opt);
            per_block_num_opt = 0;// 开始重新计数
            if_next = false;
        }
        else{
            per_block_num_opt++;
            (*it).block_id = *block_num_opt;
            if_next = true;
        }
        // cout<<(*it).block_id<<endl;
    }
    // 识别所有四元式读完后是否块数多加了1
    if(if_next){
        (*block_num_opt) --;
    }
}



// 查找节点是否已存在
node *find(string name, map<int,node*> NodeMap)
{
    node *tmp;
    map<int,node* >::iterator it;
    for(it=NodeMap.begin();it!=NodeMap.end();it++)
    {
        tmp=it->second;
        list<string>::iterator i;
        for (i=tmp->signlist.begin();i!=tmp->signlist.end();i++)
        {
            if ((*i).compare(name)==0)
                return tmp;
            if(tmp->isNumber)
            {
                if(tmp->Value==atoi(name.c_str()))
                    return tmp;
            }
        }
    }
    return NULL;
}

// 如果节点不存在，创建新节点
// find node为空之后再创建新节点
node * create(string name){
    node *newnode=new node();
    
    if(isdigit(name[0]))
    {
        newnode->isNumber=true;
        newnode->Value=atoi(name.c_str());
        cout<<"Number="<<newnode->Value<<endl;
    }
    else
        newnode->signlist.push_front(name);

    // cout<<"create Name="<<name<<endl;
    return newnode;
    
}

node * findopyz(string op,node *nodey,node *nodez, map<int,node*> NodeMap){
    node *tmp;
    map<int,node* >::iterator it;
    for(it=NodeMap.begin();it!=NodeMap.end();it++)
    {
        tmp=it->second;
        list<string>::iterator i;
        for (i=tmp->signlist.begin();i!=tmp->signlist.end();i++)
        {
            if(tmp->op==op && tmp->left==nodey && tmp->right==nodez)
                return tmp;
        }
    }
    return NULL;
}

node * findxopy(string x,string op,node *nodey, map<int,node*> NodeMap){ 
    node *tmp;
    map<int,node* >::iterator it;
    for(it=NodeMap.begin();it!=NodeMap.end();it++)
    {
        tmp=it->second;
        list<string>::iterator i;
        for (i=tmp->signlist.begin();i!=tmp->signlist.end();i++)
        {
            if ((*i).compare(x)==0)
                if(tmp->op==op && tmp->left==nodey && tmp->right==NULL)
                    return tmp;
        }
    }
    return NULL;
}

void add_3_op(string x,string y,string op,string z, map<int,node*> NodeMap)//x:=y op z
{
    // ...
}


void add_2_op(string x,string op,string y, map<int,node*> NodeMap)//x:=op y
{
    node *nodey,*nodex;
    nodey=find(y, NodeMap);
    if(nodey==NULL)
    {
        nodey=create(y);
    }
    else
    {
        nodex=findxopy(x,op,nodey, NodeMap);
        if(nodex!=NULL)
            return;
    }
    
    list<string>::iterator i;
    nodex=find(x,NodeMap);
    if(nodex!=NULL)
    {
        for(i=nodex->signlist.begin();i!=nodex->signlist.end();i++)
            if((*i).compare(x)==0)
                nodex->signlist.erase(i);
    }
        
    nodex=create(x);
    if(nodey->isNumber==true){   
        if(op=="+"){
            nodex->isNumber=true;
            nodex->Value=nodey->Value;
        }
        if(op=="-"){
            nodex->isNumber=true;
            nodex->Value=-nodey->Value;
        }
        
        node *tmp;
        
        if(tmp!=NULL)
            tmp->signlist.push_back(x);
        else
            NodeMap.insert(map<int,node*> :: value_type(NodeMap.size(),nodex) );        
        return;
    }
    else{
        cout<<"nodey is not number"<<endl;
        nodex->op=op;
        nodex->left=nodey;
        nodex->right=NULL;
        NodeMap.insert(map<int,node*> :: value_type(NodeMap.size(),nodex) );
        NodeMap.insert(map<int,node*> :: value_type(NodeMap.size(),nodey) );
    }
}

void add_1_op(string x,string y, map<int,node*> NodeMap)//x:=y
{
    node *nodex,*nodey;
    nodey=find(y, NodeMap);
    if(nodey==NULL)
    {
        nodey=create(y);
        NodeMap.insert(map<int,node*> :: value_type(NodeMap.size(),nodey));
    }
    
    list<string>::iterator i;
    nodex=find(x, NodeMap);
    if(nodex!=NULL){
        for(i=nodex->signlist.begin();i!=nodex->signlist.end();i++)
            if((*i).compare(x)==0)
                i=nodex->signlist.erase(i);
    }
    nodey->signlist.push_back(x);   
}


// 一个节点的信息
string printnode(node *nodex)
{
    string str;
    if(nodex==NULL)    return str;   

    if(nodex->isNumber)
        cout<<nodex->Value;
    else
        str=nodex->signlist.front();
    return str;
}

// 打印图的信息
void print_map(map<int,node*> NodeMap)
{
    string op,x,y,z;
    node *tmp;
    map<int,node* >::iterator it;
    for(it=NodeMap.begin();it!=NodeMap.end();it++)
    {
        tmp=it->second;
        
        if(tmp->left==NULL && tmp->right==NULL)
        {
            if(tmp->isNumber)
            {
                list<string>::iterator i;
                for(i=tmp->signlist.begin();i!=tmp->signlist.end();i++)
                {
                    cout<<*i<<" = "<<tmp->Value<<endl;
                }
            }
            continue;
        }
        else
        {
            if(tmp->right==NULL)
                cout<<printnode(tmp)<<" = "<<tmp->op<<" "<<printnode(tmp->left)<<endl;
            else
            {
                cout<<printnode(tmp)<<" = "<<printnode(tmp->left)<<" "<<tmp->op<<" "<<printnode(tmp->right)<<endl;
            }
        }
        list<string>::iterator i;
        for(i=tmp->signlist.begin();i!=tmp->signlist.end();i++)
        {
            if(i==tmp->signlist.begin())
                continue;
            else
                cout<<*i<<"="<<tmp->signlist.front()<<endl;
        }
    }
}

