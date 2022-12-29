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

bool is_capital(char temp);// 判断是否是非终结符。用大写字母来表示非终结符，因此判断是否是大写字母也是用来判断是否是非终结符
void init();// 初始化，读入写好的文法并保存
string find_first(char start);// 计算一个非终结符的first集合，因为会被递归调用，因此单独写出来
void first_set();// 得到first集合
void follow_set();// 得到follow集合
bool select_set_judge();// 得到每个右端表达式的select集合并保存，并且每得到一个非终结符的全部右端表达式的select集合就进行判断是否满足LL1文法，如果都满足最后返回true，否则返回false
void make_pre_table();// 根据select集合得到预测分析表
bool creat_pre_table();// 这个函数用来从0开始创造预测表，即在这个函数中调用上面的几个函数：first_set()-> follow_set()-> select_set_judge()-> make_pre_table()并且最后将得到的预测表保存到txt文件中通过调用函数save_information()
void save_information();// 在文件中保存信息：如开始符，终结符，非终结符以及预测表
//  函数creat_pre_table()和save_information()只需要调用一个就可以了
bool read_pre_table();  // 不根据设计好的LL1文法创建预测表，而是直接从之前创建好之后保存的文件中读取一些信息（开始符，终结符，非终结符以及预测表），然后进行语法分析，判断是否满足需求
bool check_grammar(ofstream *file);// 检查读好的经过了词法分析器的token是否满足我们设计好的文法，并将分析过程写入txt文件中并保存


// 都是用来进行输出信息的函数，从名字上可以看出来打印的是什么信息
void print_first();
void print_follow();
void print_select();
void print_pre_table(map<char, map<char,string>> pre_table);
void print_BNF();
void print_terminator();
void print_stack(stack<char> S, ofstream *file);

// 用来进行语法分析，相当于LL1.cpp中的main函数
bool grammar_analyze(string grammar);
char find_c(int token_code);// token与终结符的对应关系，也是由于设置的终结符是字符型无法保存if else这样的字符串型终结符，无法进行直接比较，可以用这个函数作为一个转换关系作为一个桥梁

// 应为增加的动作新增的函数
void make_pre_table_action();// 生成带动作的预测分析表
void print_select_action();// 输出带有动作的select集合
void init_action(string read_file);// 同时读取原本的文法以及带有动作的翻译文法，保存他们的对应关系

#endif
