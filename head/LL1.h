#ifndef LL1_H
#define LL1_H
#include<stdio.h>
#include<iostream>
#include<fstream>
#include<string.h>
#include<map>
#include<set>
#include<stack>
#include"lexical.h"
using namespace std;

bool is_capital(char temp);// �ж��Ƿ��Ƿ��ս�����ô�д��ĸ����ʾ���ս��������ж��Ƿ��Ǵ�д��ĸҲ�������ж��Ƿ��Ƿ��ս��
void init();// ��ʼ��������д�õ��ķ�������
string find_first(char start);// ����һ�����ս����first���ϣ���Ϊ�ᱻ�ݹ���ã���˵���д����
void first_set();// �õ�first����
void follow_set();// �õ�follow����
bool select_set_judge();// �õ�ÿ���Ҷ˱��ʽ��select���ϲ����棬����ÿ�õ�һ�����ս����ȫ���Ҷ˱��ʽ��select���Ͼͽ����ж��Ƿ�����LL1�ķ��������������󷵻�true�����򷵻�false
void make_pre_table();// ����select���ϵõ�Ԥ�������
bool creat_pre_table();// �������������0��ʼ����Ԥ���������������е�������ļ���������first_set()-> follow_set()-> select_set_judge()-> make_pre_table()������󽫵õ���Ԥ����浽txt�ļ���ͨ�����ú���save_information()
void save_information();// ���ļ��б�����Ϣ���翪ʼ�����ս�������ս���Լ�Ԥ���
//  ����creat_pre_table()��save_information()ֻ��Ҫ����һ���Ϳ�����
bool read_pre_table();  // ��������ƺõ�LL1�ķ�����Ԥ�������ֱ�Ӵ�֮ǰ������֮�󱣴���ļ��ж�ȡһЩ��Ϣ����ʼ�����ս�������ս���Լ�Ԥ�����Ȼ������﷨�������ж��Ƿ���������
bool check_grammar(ofstream *file);// �����õľ����˴ʷ���������token�Ƿ�����������ƺõ��ķ���������������д��txt�ļ��в�����


// �����������������Ϣ�ĺ������������Ͽ��Կ�������ӡ����ʲô��Ϣ
void print_first();
void print_follow();
void print_select();
void print_pre_table(map<char, map<char,string>> pre_table);
void print_BNF();
void print_terminator();
void print_stack(stack<char> S, ofstream *file);

// ���������﷨�������൱��LL1.cpp�е�main����
bool grammar_analyze(string grammar);
char find_c(int token_code);// token���ս���Ķ�Ӧ��ϵ��Ҳ���������õ��ս�����ַ����޷�����if else�������ַ������ս�����޷�����ֱ�ӱȽϣ����������������Ϊһ��ת����ϵ��Ϊһ������

// ӦΪ���ӵĶ��������ĺ���
void make_pre_table_action();// ���ɴ�������Ԥ�������
void print_select_action();// ������ж�����select����
void init_action(string read_file);// ͬʱ��ȡԭ�����ķ��Լ����ж����ķ����ķ����������ǵĶ�Ӧ��ϵ

#endif
