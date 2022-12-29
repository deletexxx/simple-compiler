#include"../head/symbol.h"

// 对已经读好的token序列进行文法分析
extern vector<pair<string,int>> my_token;// 从其他文件中去找
struct SYM_TABLE symbol_table;
int arr_table_num = 0;// 记录数组表 表项的个数
int type_table_num = 0;// 记录数组表 表项的个数
int sym_table_num = 0;// 记录符号表 表项的个数


// 判断是否是数组，忘记了我的词法分析中是将数组的名字和个数分开保存的，因此这个函数无用
/*
bool IS_arr(string token_name, string* name, int* num){
    char c;
    string num_;
    *num = 0;
    for(int i=0; i<token_name.size(); ++i){
        if(token_name[i] != '['){
            name->push_back(token_name[i]);
        }
        else{// 读取数字
            for(int j=i+1 ; token_name[j] != ']' && j<token_name.size() ; ++j){
                num_.push_back(token_name[j]);
            }
            for(int j=0;j<num_.size();++j){
                *num = (*num)*10+(num_[i]-'0');
            }// 字符串转换为数字
            return true;
        }
    }
    return false;
}
*/
bool in_symbol(string a,int *synbl_id){// 不在符号表中说明是个常量
    vector<SYNBL>::iterator it;
    for(it = symbol_table.synbl.begin(); it != symbol_table.synbl.end();++it){
        if(a==(*it).name){
            *synbl_id = (*it).id;
            return true;
        }
    }
    return false;
}

char find_type(string a, int token_num){
    if(token_num == 30){// 如果是标识符
        vector<SYNBL>::iterator it;
        for(it = symbol_table.synbl.begin(); it != symbol_table.synbl.end(); ++it){
            if((*it).name == a){    // 有匹配的
                // cout<< (*it).type_point->t_val<<endl;
                return (*it).type_point->t_val; // 输出对应的类型
            }
        }
    
        cout << "ERROR:can't find " << a <<" in symbol table"<<endl;
    }
    else{// 不是标识符，是其他类型的常量
        if(token_num == 31) {return 'c';}// 字符型常量
        else if(token_num == 71) {return 'f';}// 单精度浮点型
        else if(token_num == 72) {return 'i';} // 整型变量
        else{
            cout << "error: unrecognized data type of " << a <<endl;
        }
    }
    return 0;
}

// 类型匹配
bool type_match(char a_type, char b_type, string op, char *res_type){
    if(a_type == b_type){
        if(op == "==" || op == ">=" || op == ">" || op == "<=" || op == "<" || op == "!=" || op == "&&" || op == "||")
            *res_type = 'b';// 返回布尔值，是判断类型的
        else{
            *res_type = a_type;
        }
        return true; 
    }    
    else return false;  
    
}

// 根据类型得到它的大小
int get_size(char type){
    switch(type){
        case 'i':// int
            return 4;
        case 'c':// char 
            return 1;
        case 'f':// float
            return 4;
        case 'd':// double
            return 8;
        case 'b':// bool
            return 1;
        default:
            return 0;
    }
}

// 根据种别码获取数据类型
char get_type(int token_num){
    switch(token_num){
        case 3:
            return 'i';
        case 4:
            return 'c';
        case 5:
            return 'f';
        case 6:
            return 'd';
        case 9:
            return 'b';
        default:
            return 0;
    }
}

void Init_fun(){
    struct PEINFL fun_table;
    fun_table.entry = 0;
    fun_table.FN = 0;
    fun_table.level = 0;
    fun_table.off = 0;
    fun_table.param = NULL;// 没有参数指向空

    symbol_table.fun_table.push_back(fun_table);
}

// 打印符号表
void PRINT_SYM_TABLE(string file_name){
    // cout<<symbol_table.type_table[1].arr_point->id<<endl;
    // cout<<symbol_table.type_table[2].arr_point->id<<endl;
    //open file ,将符号表写入这个文件
    ofstream file;
    file.open(file_name,ios::out);
    if(!file.is_open())
    {
        file<<"cannot open the file"<<endl;
    }

    file<<"SYNBL"<<endl;
    file << "id\t\tname\t\t" << "TYPE(id)\t\t" << "CAT\t\t" << "ADDR\t\t"<<"is_temp_var"<<endl;
    for(int i=0;i<symbol_table.synbl.size();++i){
        file <<i<<"\t\t"<< symbol_table.synbl[i].name <<"\t\t";
        // file << "type_table:" <<(int)symbol_table.synbl[i].type_point->id<<"\t\t";
        file << "type_table:" <<i<<"\t\t";
        file << symbol_table.synbl[i].CAT <<"\t\t";
        if(symbol_table.synbl[i].CAT == "f"){
            file<<"在函数表的："<<symbol_table.synbl[i].function_point->id<<"\t\t";
        }
        else{
            file<<symbol_table.synbl[i].addr<<"\t\t";
        }
        file << symbol_table.synbl[i].is_temp_var << endl;
    }

    file<<endl<<"TYPEL"<<endl;
    file<< "id\t\tTYPEL\t\t"<<"TPOINT"<<endl;
    int n=0;
    for(int i=0;i<symbol_table.type_table.size();++i){
        file<<i<<"\t\t"<<symbol_table.type_table[i].t_val<<"\t\t";
        if(symbol_table.type_table[i].t_val == 'a'){// 如果是数组，输出指向的数组表项的id
            int temp = (symbol_table.type_table[i].arr_point->id == n )? symbol_table.type_table[i].arr_point->id : n;
            n++;
            file<<"arr_table:"<< temp <<endl;
            // cout<<"arr_table:"<<symbol_table.type_table[i].arr_point->id<<endl;
            // cout<<i<<endl;
        }
        else if(symbol_table.type_table[i].t_val == 'D'){
            file << "struct_table"<<symbol_table.type_table[i].rinf_point->id<<endl;
        }
        else{
            file<<"NULL"<<endl;
        }
    }

    file<<endl<<"AINFL"<<endl;
    file<<"id\t\tLOW"<<"\t\tUP"<<"\t\tCTP"<<"\t\tlen"<<endl;
    for(int i=0;i<symbol_table.arr_table.size();++i){
        file << i << "\t\t"<<symbol_table.arr_table[i].low<<"\t\t";
        file << symbol_table.arr_table[i].up << "\t\t";
        file << symbol_table.arr_table[i].CTP << "\t\t";
        file << symbol_table.arr_table[i].clen <<endl;
    }

    file<<endl<<"RINF"<<endl;
    file<<"id\t\t"<<"name\t\t"<<"off\t\t"<<"TPOINT"<<endl;
    for(int i=0;i<symbol_table.stru_table.size();++i){
        file<<i<<"\t\t"<<symbol_table.stru_table[i].name<<"\t\t";
        file<<symbol_table.stru_table[i].off<<"\t\t";
        file<<"arr_table:"<<symbol_table.stru_table[i].type_point->id<<endl;
    }

    file<<endl<<"PEINFL"<<endl;
    file<<"id\t\t"<<"level\t\t"<<"off\t\t"<<"FN\t\t"<<"entry\t\t"<<"PARAM"<<endl;
    for(int i=0;i<symbol_table.fun_table.size();++i){
        file<<i<<"\t\t"<<symbol_table.fun_table[i].level<<"\t\t";
        file<<symbol_table.fun_table[i].off<<"\t\t";
        file<<symbol_table.fun_table[i].FN<<"\t\t";
        file<<symbol_table.fun_table[i].entry<<"\t\t";
        if(symbol_table.fun_table[i].param != NULL)
            file<<"arr_table:"<<symbol_table.fun_table[i].param->id<<endl;
        else
            file<<"NULL"<<endl;
    }
    
    file.close();
}

bool add_item(char type, string name,bool is_fun,bool is_const,bool is_array, int arr_num, bool if_temp_var){
    // string name;
    int num = arr_num;// 数组的元素个数
    // bool is_array = IS_arr(token_name, &name, &num);
    // if(!is_array){
    //     name = token_name;
    // }
    // 判断已有符号表中是否有同名变量,即是否被重复定义
    for(int i=0;i<symbol_table.synbl.size();++i){
        if(symbol_table.synbl[i].name == name){
            cout<<"ERROR :  '"<<name<<"'  Duplicate definition!"<<endl;
            return false;
        }
    }
    struct SYNBL synbl_item;
    synbl_item.id = sym_table_num++;
    synbl_item.name = name;
    // 指向类型表
    struct TYPEL type_table_item;
    type_table_item.id = type_table_num++;
    // cout<< type_table_item.id<<endl;
    if(is_array){// 如果考虑结构在这里加类型
        type_table_item.t_val = 'a';// 数组型
        // 构建一个数组表项
        struct AINFL arr_table_item;
        arr_table_item.id = arr_table_num++;
        arr_table_item.low = 0;
        arr_table_item.up = num-1;
        arr_table_item.CTP = type;
        arr_table_item.clen = get_size(type)*num;
        symbol_table.arr_table.push_back(arr_table_item);// 将这个数组项加入符号表
        int len = symbol_table.arr_table.size();
        type_table_item.arr_point = &symbol_table.arr_table[len-1];// 指向符号表的数组表的对应项中
        type_table_item.rinf_point = NULL;// 结构表指针为空，是结构项再创建结构项
    }
    else{
        type_table_item.t_val = type;
        type_table_item.arr_point = NULL;
        type_table_item.rinf_point = NULL;
    }
    symbol_table.type_table.push_back(type_table_item);// 将这个类型表项加入符号表的类型表中
    synbl_item.type_point = &symbol_table.type_table[type_table_num-1];// 将
    // if(symbol_table.type_table[type_table_num-1].arr_point !=NULL)
    //     cout<<symbol_table.type_table[type_table_num-1].arr_point->id<<endl;

    // 种类
    if(is_fun){
        synbl_item.CAT = "f";
        // 创建一个函数表项，并加入符号表的函数表中，并让synbl_item里的函数表指针指向这个对应的符号表的函数表项上
    }
    else if(is_const){
        synbl_item.CAT = "c";
        synbl_item.addr = get_size(type);
    }
    else if(is_array){
        synbl_item.CAT = "t";
        synbl_item.addr = get_size(type)*num;
        // cout<<get_size(type)<<" "<<num<<endl;
    }
    else{
        synbl_item.CAT = "v";
        synbl_item.addr = get_size(type);
    }
    synbl_item.is_temp_var = if_temp_var;
    
    // 最后将符号表项写入符号表中
    symbol_table.synbl.push_back(synbl_item);
    return true;
}


// 创建符号表,主函数
bool GET_SYMTABLE(){
    vector<pair<string,int>>::iterator it;
    for(it = my_token.begin(); it!=my_token.end(); ++it){
        if((*it).second == 1){
            Init_fun();// 初始化函数表
        }
        else if((*it).second == 3 || (*it).second == 4 || (*it).second == 5 || (*it).second == 6 || (*it).second == 9 || (*it).second == 19){// 最后一个表示是常量
            bool is_fun,is_const,is_arr;
            if((*it).second == 19){
                is_const = true;
                ++it;
            }
            else{
                is_const = false;
            }
            if((*(it+1)).second != 1){// 不是主函数, it+1 指向的是标识符
                int arr_num = 0;
                // 但我忘了我的文法中是没有实现常数类型的，所以没有办法验证常数
                if((*(it+2)).second == 60){// 用标识符后面是否是括号来判断是否是函数 这里虽然判断了是函数，但对齐后续的参数并未能将其写进函数表中（即函数表并未完善）
                    is_fun = true;
                    is_arr = false;
                }
                if((*(it+2)).second == 64){// 用标识符后面是否是括号来判断是否是数组
                    is_fun = false;
                    is_arr = true;
                    string num_ = (*(it+3)).first;
                    for(int i = 0; i<num_.size(); ++i){
                        arr_num = arr_num*10 + (num_[i]-'0');
                    }
                }
                else
                    is_fun = is_arr = false;

                char type = get_type((*it).second);
                if(!add_item(type, (*(it+1)).first, is_fun, is_const,is_arr,arr_num, false)){// 判断增加表项时是否出现错误
                    return false;
                }
                ++it;// 这个标识符被识别，之后就不用重复识别了
            }
            
        }
        else if((*it).second == 30){// 是标识符，但不被定义，检测是否已经被定义过
            // 判断已有符号表中是否已存在,即是否被定义
            bool if_def = false;
            for(int i=0;i<symbol_table.synbl.size();++i){
                if(symbol_table.synbl[i].name == (*it).first){
                    if_def = true;
                    break;// 
                }
            }
            if(!if_def){// 有未被定义的变量出现
                cout<<"ERROR:  \""<<(*it).first<< "\"  was not declared in this scope"<<endl;
                return false;
            }
        }
    }

    return true;
}