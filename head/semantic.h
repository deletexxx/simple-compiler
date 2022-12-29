#ifndef SEMANTIC_H
#define SEMANTIC_H

#include"LL1.h"
#include"symbol.h"


struct Quat{
    string op;// 操作符
    string a, b, res;   // 操作数名字
    char a_type,b_type,res_type;    // 三个操作数的类型
    bool a_if_act, b_if_act, res_if_act;    // 保存活跃信息
    int block_id;   // 基本块号
    int id; // 四元式的标号
};

bool generate_quat(); // 进行语义分析
bool get_Quat(ofstream *file);// 生成四元式
void PRINT_Quat(ofstream *file);// 打印四元式
void Print_a_Quat(ofstream *file);// 打印最新生成的四元式
string i_to_s(int i);// 将整数t_id转换为string类型的变量名
// 动作
void GEQ(int *tid);// 参数是临时变量的标号
void PUSH_i(string i, char type);//, int type);
void ASSI();
void IF();
void ELSE();
void ELIF();
void ENDIF();
void WE();
void WH();
void DO();

#endif