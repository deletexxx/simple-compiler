#ifndef GENERATE_CODE_H
#define GENERATE_CODE_H
#include"DAGoptimize.h"
extern vector<Quat> quat;

// 结构体

// 每一个变量的活跃信息
struct act_symbl{
    string name;
    bool if_act;
};

// 每个mips指令
struct mips_one_code{
    mips_one_code():var_num(0){}// 这个值只有作为标签时写作1
    string action;
    string a;// 共三个操作数，注意这里的a，b和四元式中的a，b表示的不是一个意思
    string b;
    string c;
    int var_num;
};

// 给每个四元式加上信息是否是活跃的

// 关于活跃信息
void gen_act_inf();// 生成活跃信息
void add_block_act_symbl(string name, vector<act_symbl> *block_act_symbl); // 判断标识符能否被加入每个block的symbl表里
bool find_var_in_act(string name, vector<act_symbl> block_act_symbl, int *id); // 查找这个变量是否在块的活跃信息表中
bool in_block_symbl(string name, vector<act_symbl> block_act_symbl);// 判断标识符是否在块的活跃信息表内

// 寄存器的管理
int alloc_reg(string name, bool if_act, int block_id, bool is_var);// 给传入的变量生成寄存器来存放
int free_reg(int block_id);// 释放寄存器, 返回值表示是否释放了活跃变量所在寄存器
int empty_reg(); // 判断是否有寄存器是空着的，有就返回寄存器的下标号
int find_var_reg(string name); // 查找变量是否在寄存器中被保存

// 生成asm的过程
bool get_code(); //生成代码块的mips指令
bool op_find_mips(string op, string *mips_op); //根据操作符朝朝对应的mips指令中的操作
bool get_asm(); // 生成完整的mips指令，包括变量还有代码块

// 主函数
bool generate_object_code();// 相当于生成目标代码这部分的主函数

// 终端打印信息
void PRINT_quat_num(); // 将四元式以及它的block的信息打印
void PRINT_mips();// 打印代码块的mips信息
void PRINT_block_act_symbl(); // 输出每个block的活跃信息

#endif