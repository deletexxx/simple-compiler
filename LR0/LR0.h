#ifndef LR0_H
#define LR0_H

#include<fstream>
#include<cstdio>
#include<vector>
#include<string>
#include<iostream>
#include<map>
#include<set>
#include<stack>
#include<queue>
using namespace std;

void init(string file_name);// 初始化，读入写好的文法并保存
void read_table(string file_name); // 读取LR(0)分析表
bool process(string file_name);// LR0分析过程
void print_table(int num);// 打印LR0分析表
void print_stack_string(stack<string> S, ofstream *file);// 按顺序打印栈里的信息
void print_stack_int(stack<int> S, ofstream *file);

#endif