
#ifndef SYMBOL_H
#define SYMBOL_H

#include"lexical.h"

struct SYNBL{// 符号表
    string name;
    struct TYPEL* type_point;// 指向类型表
    string CAT;// f/p 函数，c 常量，t 类型，d 域名， v、vn、vf变量、换名形参，赋值形参
    int addr;// 这里对一般变量写的是偏移量
    struct PEINFL* function_point;
    int id;// 表示是第几项
    bool is_temp_var;// 是否是临时变量
};

struct TYPEL{// 类型表
    // i整型，r实型，c字符型，b布尔型，a数组型，D结构型，f单精度浮点数型，d双精度浮点型
    char t_val;// 类型
    struct AINFL* arr_point;// 数组表指针
    struct RINF* rinf_point;// 结构表指针
    int id;// 在表里是第几个表项（方便查看符号表指向类型表的位置
};

struct AINFL{// 数组表
    int low;
    int up;
    char CTP;// 数组的数据类型
    int clen;// 成分类型长度
    int id;// 和类型表中的id同理
};

struct RINF{// 结构表
    string name;// 域名
    int off;// 相对值单元首地址的偏移地址
    struct TYPEL* type_point;// 指向类型表
    int id;// 和类型表中的id同理
};

struct PEINFL{// 函数表
    int level;// 层次号
    int off;
    int FN;// 参数个数
    int entry;// 入口地址，目标函数首地址
    struct TYPEL* param;// 指向参数的位置，是一个形参表
    int id;// 和类型表中的id同理
};

struct SYM_TABLE{
    vector<SYNBL> synbl;
    vector<TYPEL> type_table;
    vector<AINFL> arr_table;
    vector<RINF> stru_table;
    vector<PEINFL> fun_table;
};

bool GET_SYMTABLE();// 创建符号表
// bool IS_arr(string token_name, string* name, int* num);// 判断是否是数组
int get_size(char type);// 根据类型得到它的大小
char get_type(int token_num);// 根据种别码获取数据类型
void PRINT_SYM_TABLE(string file_name);// 打印符号表,将其写入文件中
void Init_fun();// 初始化函数表
bool add_item(char type, string name,bool is_fun,bool is_const,bool is_array, int arr_num, bool if_temp_var);// 符号表增加一项，传入标识符的名字，暂时没有考虑增加函数表项的情况

bool type_match(char a_type, char b_type, string op, char *type);
char find_type(string a, int token_num);   // 根据标识符的名字，查找对应的类型
bool in_symbol(string a,int *synbl_id);// 查看字符是否在符号表中

#endif