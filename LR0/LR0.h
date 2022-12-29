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

void init(string file_name);// ��ʼ��������д�õ��ķ�������
void read_table(string file_name); // ��ȡLR(0)������
bool process(string file_name);// LR0��������
void print_table(int num);// ��ӡLR0������
void print_stack_string(stack<string> S, ofstream *file);// ��˳���ӡջ�����Ϣ
void print_stack_int(stack<int> S, ofstream *file);

#endif