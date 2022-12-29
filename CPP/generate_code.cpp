#include"../head/generate_code.h"

extern vector<Quat> quat;
extern struct SYM_TABLE symbol_table;
vector<vector<act_symbl>> Act_symbl;// 每个vector里保存的是一个块的symbl信息（又用vector保存）

int block_num; // 生成的基本块的数量
vector<int> per_block_num;// 每个基本块有多少个四元式
vector< vector<mips_one_code> > code; //每个块设置一个mips vector
stack<pair<int,int>> need_add;// 登记带反填的目标地址信息,block_id和block里的codeid

// 标签号
int while_label = 0;// 循环块的个数，用来重命名label
int else_label = 0;// 命名else相关label
int else_num = 0;// 根据个数pop反填地址，可能有多个elseif
int end_else_label = 0;// end else 的个数


// 10个寄存器
string registers[10] = { "$t0", "$t1", "$t2", "$t3", "$t4", "$t5", "$t6", "$t7", "$t8", "$t9"};
string RVALUE[10];   //每个寄存器中，当前的变量是什么
bool reg_var_act[10];   // 存储信息，寄存器中的变量是否活跃 


// 判断是否有寄存器是空着的，有就返回寄存器的下标号
int empty_reg(){
    int i;
    for(i=0;i<10;++i){
        if(RVALUE[i]==""){
            return i;
        }
    }
    return i;
}

// 将四元式以及它的block的信息打印
void PRINT_quat_num(){
    vector<Quat>::iterator it = quat.begin();
    for(it; it<quat.end();++it){
        cout << (*it).id <<"\t< "<<(*it).op <<",\t" ;
        if((*it).a!=""){
            cout << (*it).a <<"(" << (*it).a_if_act <<"),\t\t" ;
        }
        else{
            cout <<"_,\t\t" ;
        }

        if((*it).b!=""){
            cout <<(*it).b <<"(" << (*it).b_if_act <<"),\t\t";
        }
        else{
            cout << "_,\t\t" ;
        }

        if((*it).res!=""){
            cout << (*it).res <<"(" << (*it).res_if_act <<")\t>" << "\t\t" << (*it).block_id << endl;
        }
        else{
            cout << "_\t>" <<"\t\t"<< (*it).block_id << endl;
        }
    }
}

// 判断变量是否在块的活跃信息表中
bool in_block_symbl(string name, vector<act_symbl> block_act_symbl){
    for(int i = 0; i<(int)block_act_symbl.size(); ++i){
        if(block_act_symbl[i].name == name){
            return true;
        }
    }
    return false;
}

// 判断标识符能否被加入每个block的symbl表里
void add_block_act_symbl(string name, vector<act_symbl> *block_act_symbl){
    int synbl_id;
    if(name!="" && in_symbol(name, &synbl_id)){// 如果不为空且在符号表内
        // 判断symbl表中这个变量是否是临时变量
        if(!in_block_symbl(name, *block_act_symbl)){// 还未被写入表内
            struct act_symbl temp_act;
            temp_act.name = name;
            if(symbol_table.synbl[synbl_id].is_temp_var){// 是临时变量,不活跃              
                temp_act.if_act = false;    
            }
            else{
                temp_act.if_act = true;    
            }
            (*block_act_symbl).push_back(temp_act);
        }  
    }
}

// 输出每个block的活跃信息
void PRINT_block_act_symbl(){
    for(int i=0;i<(int)Act_symbl.size();++i){
        cout<<i<<":"<<endl;
        for(int j=0;j<(int)Act_symbl[i].size();++j){
            cout<<"name:"<<Act_symbl[i][j].name<<" \t\t\tif_act:"<<Act_symbl[i][j].if_act<<endl;
        }
        cout<<endl;
    }
}

// 查找这个变量是否在块的活跃信息表中
bool find_var_in_act(string name, vector<act_symbl> block_act_symbl, int *id){
    for(int i=0; i<(int)block_act_symbl.size(); ++i){
        if(name == block_act_symbl[i].name){
            *id = i;
            return true;
        }
    }
    return false;
}


void gen_act_inf(){
    // 先对每个块内的活跃信息表初始化
    int quat_id = 0;
    for(int block_id = 0; block_id < block_num; ++block_id){
        vector<act_symbl> block_act_symbl; // 每个块里的活跃信息表
        // 遍历每个块内的四元式
        for(int block_quat_id = 0; block_quat_id < per_block_num[block_id]; ++block_quat_id){
            // 先对第一个操作数判断，如果不为空，且在符号表中
            add_block_act_symbl(quat[quat_id].a, &block_act_symbl);
            // 第二个操作数
            add_block_act_symbl(quat[quat_id].b, &block_act_symbl);
            // 第三个操作数
            add_block_act_symbl(quat[quat_id].res, &block_act_symbl);
            quat_id++;
        }
        Act_symbl.push_back(block_act_symbl);
    }

    // 写入每个四元式的活跃信息
    // 先遍历每个块
    int quat_id_block = 0;
    quat_id = 0;
    // cout<<"输出当前活跃信息的转变情况，分别表示:四元式的id，变量名，当前活跃信息,这个变量在当前块的活跃信息表中转变后的活跃信息"<<endl;
    // cout<<"quat_id\t var_name \t var_act \t var_now_in_act_sym"<<endl;
    for(int block_id = 0; block_id < block_num; ++block_id){
        // 倒着遍历每个块内的每个四元式
        quat_id = quat_id_block;

        for(int block_quat_id = per_block_num[block_id] - 1; block_quat_id >= 0; --block_quat_id){// block_quat_id是以一个块为单位，四元式的id
            quat_id = quat_id_block + block_quat_id;  // 在四元式中的实际id

            // 给四元式加入信息
            // 一个操作数找一次,考虑到先找定义点，再找应用点，还是按顺序将三个操作数进行遍历查找
            int act__block_id;// 块内活跃信息表中的id
            // 先获取定义点的活跃信息
            if(find_var_in_act(quat[quat_id].res, Act_symbl[block_id], &act__block_id)){
                quat[quat_id].res_if_act = Act_symbl[block_id][act__block_id].if_act;
                // 变不活跃信息
                Act_symbl[block_id][act__block_id].if_act = false;
                // 输出当前活跃信息的转变情况，分别表示基本块号，变量名，当前活跃信息和活跃信息在当前块的活跃信息表中转变为什么
                // cout<<quat_id<<" "<<quat[quat_id].res<<" "<<quat[quat_id].res_if_act<<" "<<Act_symbl[block_id][act__block_id].if_act<<endl;
            }

            // 获取应用点的活跃信息
            if(find_var_in_act(quat[quat_id].a, Act_symbl[block_id], &act__block_id)){
                quat[quat_id].a_if_act = Act_symbl[block_id][act__block_id].if_act;
                // 变为活跃信息
                Act_symbl[block_id][act__block_id].if_act = true;
                // 输出活跃信息的转变情况
                // cout<<quat_id<<" "<<quat[quat_id].a<<" "<<quat[quat_id].a_if_act<<" "<<Act_symbl[block_id][act__block_id].if_act<<endl;
            }
            else{
                quat[quat_id].a_if_act = false; // 可能是常量，这里输出为不活跃信息，但他没有这个性质，加上是因为打印信息的时候会打印出随机值（没有设置初始变量）
            }

            // 第二个应用点
            if(find_var_in_act(quat[quat_id].b, Act_symbl[block_id], &act__block_id)){
                quat[quat_id].b_if_act = Act_symbl[block_id][act__block_id].if_act;
                // 变活跃信息
                Act_symbl[block_id][act__block_id].if_act = true;
                // 输出当前活跃信息的转变情况
                // cout<<quat_id<<" "<<quat[quat_id].b<<" "<<quat[quat_id].b_if_act<<" "<<Act_symbl[block_id][act__block_id].if_act<<endl;
            }
            else{
                quat[quat_id].b_if_act = false; // 可能是常量，这里输出为不活跃信息，但他没有这个性质，加上是因为打印信息的时候会打印出随机值（没有设置初始变量）
            }

        }
        
        // cout<<endl;// 一个块遍历结束
        quat_id_block = quat_id_block + per_block_num[block_id];    // 记下来下一个块的开始的位置
    }
    
}

void PRINT_mips(){
    for(int i=0;i <block_num; ++i){
        for(int j=0; j<(int)code[i].size(); ++j){
            if(code[i][j].var_num == 1){
                cout << code[i][j].action <<endl;
            }
            else if(code[i][j].action == "li" || code[i][j].action == "sw" || code[i][j].action == "lw" || code[i][j].action == "move"){
                cout << code[i][j].action << " " << code[i][j].a <<", "<<code[i][j].b<<endl;
            }
            else if(code[i][j].action == "beq"){
                cout << code[i][j].action << " " << code[i][j].a <<", "<<code[i][j].b <<", " << code[i][j].c <<endl;
            }
            else if(code[i][j].action == "j"){
                cout << code[i][j].action << " " << code[i][j].a <<endl;
            }
            else{
                cout << code[i][j].action << " " << code[i][j].a <<", "<<code[i][j].b<<", "<< code[i][j].c <<endl;
            }
        }
    }
}

// 查找变量是否在寄存器中被保存
int find_var_reg(string name){
    int i;
    for(i=0;i<10;++i){
        if(RVALUE[i]==name){
            return i;
        }
    }
    return i;
}

// 释放寄存器, 返回值表示是否释放了活跃变量所在寄存器
int free_reg(int block_id){
    int i;
    for(i=0;i<10;++i){
        // 如果是不活跃的就直接释放
        if(!reg_var_act[i]){
            RVALUE[i] = ""; // 直接置空
            return i;
        }
    }
    // 如果都是活跃信息，就选择一个，将其数据存回,这里默认存入第一个寄存器，因为默认它存储的时间是最长的，但这并不准确
    mips_one_code sw_code;
    sw_code.action = "sw";
    sw_code.b = RVALUE[0];// 存入变量名
    sw_code.a = registers[0]; // 数据保存的位置
    code[block_id].push_back(sw_code);  // 存入mips指令中
    RVALUE[0] = ""; // 置空
    return 0;
    
}

int alloc_reg(string name, bool if_act, int block_id, bool is_var){
    // 首先查看变量是否在寄存器内
    int reg_id = find_var_reg(name);
        if(reg_id == 10){ // 超出寄存器的范围，说明不在寄存器内
            // 分配寄存器
            reg_id = empty_reg();
            // 如果没有空的寄存器了，就进行释放操作,并返回释放的寄存器的id
            if(reg_id == 10){
                reg_id = free_reg(block_id);
            }

            // 是否是变量，是变量就需要取值
            if(is_var){
                mips_one_code one_code;
                one_code.action = "lw";// 取值
                one_code.a = registers[reg_id]; // 放入寄存器中
                one_code.b = name;
                code[block_id].push_back(one_code);
            }
            
        }    
        // 否则说明已经存在于寄存器中
        // 分配好寄存器以后更新活跃信息 和 寄存器保存的变量

        reg_var_act[reg_id] = if_act;
        RVALUE[reg_id] = name;        
        return reg_id;
        
}

bool op_find_mips(string op, string *mips_op){
    if(op == "+"){
        *mips_op = "add";
    }
    else if(op == "-"){
        *mips_op = "sub";
    }
    else if(op == "*"){
        *mips_op = "mul";
    }
    else if(op == "/"){
        *mips_op = "div";
    }
    else if(op == "<"){
        *mips_op = "slt";
    }
    else if(op == "<="){
        *mips_op = "sle";
    }
    else if(op == "=="){
        *mips_op = "seq";
    }
    else if(op == "!="){
        *mips_op = "sne";
    }
    else if(op == ">="){
        *mips_op = "sge";
    }
    else if(op == ">"){
        *mips_op = "sgt";
    }
    else if(op == "&&"){
        *mips_op = "and";
    }
    else if(op == "||"){
        *mips_op = "or";
    }
    else{
        return false;
    }
    return true;
}

bool get_code(){
    int quat_id = 0;
    for(int block_id = 0; block_id < block_num; ++block_id){
        vector<mips_one_code> block_code;
        code.push_back(block_code);

        if(block_id == 0){
                // 保存标签
                mips_one_code label;
                label.action = "main:";
                label.var_num = 1;
                code[block_id].push_back(label);
        }

        for(int block_quat_id = 0; block_quat_id < per_block_num[block_id]; ++block_quat_id){
            Quat quat_one = quat[quat_id];
            // 刚开始所有变量默认值为0

            // 如果是赋值操作
            if(quat_one.op == "="){
                // 给第三个操作数分配寄存器，已存在就不会重复分配, 如果不在就需要取值
                int reg_id_res = alloc_reg(quat_one.res, quat_one.res_if_act, block_id, true);
                // 查看赋的值是否是常量，即是否在符号表内
                int symbol_id;
                if(in_symbol(quat_one.a, &symbol_id)){
                    // 如果第一个操作数在符号表中,分配寄存器
                    int reg_id_a = alloc_reg(quat_one.a, quat_one.a_if_act, block_id, true);    
                    mips_one_code one_code;
                    // 赋值操作的mips
                    one_code.action = "move";
                    // movee a to b
                    one_code.a = registers[reg_id_res];
                    one_code.b = registers[reg_id_a];
                    code[block_id].push_back(one_code);
                }
                else{
                    // 如果不在符号表内，说明是常量，“立即加载”
                    mips_one_code one_code;
                    one_code.action = "li";
                    one_code.a = registers[reg_id_res];
                    one_code.b = quat_one.a;    // 将数值a存入寄存器b中
                    code[block_id].push_back(one_code);
                }
            }

            // 如果是一些双操作数
            if(quat_one.op == "+" || quat_one.op == "-" || quat_one.op == "*" || quat_one.op == "/" || quat_one.op == "&&" || quat_one.op == "||" \
                || quat_one.op == ">" || quat_one.op == ">=" || quat_one.op == "<" || quat_one.op == "<=" || quat_one.op == "=="){
                // 给第三个操作数分配寄存器，已存在就不会重复分配
                int reg_id_res = alloc_reg(quat_one.res, quat_one.res_if_act, block_id, true);
                int reg_id_a;
                int reg_id_b;
                // 查看第一个操作数是否是常量，即是否在符号表内
                int symbol_id;
                if(in_symbol(quat_one.a, &symbol_id)){
                    // 如果在符号表中,分配寄存器
                    reg_id_a = alloc_reg(quat_one.a, quat_one.a_if_act, block_id, true);    
                }
                else{
                    // 如果不在符号表内，说明是常量，“进行立即加载”，也要申请一个寄存器
                    // 以他自己命名，活跃信息设置为不活跃，如果需要释放，会直接释放掉不会有“保存”这个操作
                    reg_id_a = alloc_reg(quat_one.a, false, block_id, false);
                    mips_one_code one_code;
                    one_code.action = "li";
                    one_code.a = registers[reg_id_a];
                    one_code.b = quat_one.a;
                    code[block_id].push_back(one_code);
                }

                // 第3个操作数，同样进行判断
                if(in_symbol(quat_one.b, &symbol_id)){
                    // 如果在符号表中,分配寄存器
                    reg_id_b = alloc_reg(quat_one.b, quat_one.b_if_act, block_id, true);    
                }
                else{
                    // 如果不在符号表内，说明是常量，“进行立即加载”，也要申请一个寄存器
                    // 以他自己命名，活跃信息设置为不活跃，如果需要释放，会直接释放掉不会有“保存”这个操作
                    reg_id_b = alloc_reg(quat_one.b, false, block_id, false);
                    mips_one_code one_code;
                    one_code.action = "li";
                    one_code.a = registers[reg_id_b];
                    one_code.b = quat_one.b;
                    code[block_id].push_back(one_code);
                }

                mips_one_code one_code;
                // one_code.action = quat_one.op;
                string mips_op;
                // 查找对应操作符指令
                if(op_find_mips(quat_one.op, &mips_op)){
                    one_code.action = mips_op;
                }
                else{
                    cout << "ERROR: the instruction corresponding to the operator could not be found"<<endl;
                    return false;
                }
                one_code.a = registers[reg_id_res];
                one_code.b = registers[reg_id_a];
                one_code.c = registers[reg_id_b];
                code[block_id].push_back(one_code);
            }

            // 条件语句if 和else-if do也是条件跳转
            if(quat_one.op == "if" || quat_one.op == "ei" || quat_one.op == "do"){
                mips_one_code one_code;
                one_code.action = "beq";

                int reg_id_a;
                // 查看第一个操作数是否是常量，即是否在符号表内, 要作为比较数
                int symbol_id;
                if(in_symbol(quat_one.a, &symbol_id)){
                    // 如果在符号表中,分配寄存器
                    reg_id_a = alloc_reg(quat_one.a, quat_one.a_if_act, block_id, true);    
                }
                else{
                    // 如果不在符号表内，说明是常量，“进行立即加载”，也要申请一个寄存器
                    // 以他自己命名，活跃信息设置为不活跃，如果需要释放，会直接释放掉不会有“保存”这个操作
                    reg_id_a = alloc_reg(quat_one.a, false, block_id, false);
                    mips_one_code one_code;
                    one_code.action = "li";
                    one_code.a = registers[reg_id_a];
                    one_code.b = quat_one.a;
                    code[block_id].push_back(one_code);
                }
                // 存储需要反填地址的 块号和mips指令的id号，还未将这一条指令加进去，因此不需要-1
                pair<int,int> temp_inf = {block_id, (int)code[block_id].size()};
                need_add.push(temp_inf);
                // 存储跳转指令信息
                one_code.a = registers[reg_id_a];
                one_code.b = "$zero";   // 如果等于0就跳转，相当于，如果不满足条件就不能继续运行
                code[block_id].push_back(one_code);
            }

            // 如果是while
            if(quat_one.op == "wh"){
                // 保存标签
                mips_one_code label;
                label.action = "WHILE_" + to_string(while_label) + ":";
                label.var_num = 1;
                code[block_id].push_back(label);
                pair<int,int> temp_inf = {block_id, (int)code[block_id].size()};
                need_add.push(temp_inf);
            }


            // we, 需要反填两个地址
            if(quat_one.op == "we"){
                // 反填第一个地址：是do的跳转地址
                string next_step_name = "EXIT";
                pair<int,int> to = need_add.top();
                need_add.pop();
                code[to.first][to.second].c = next_step_name;

                // 反填下一个跳转地址
                mips_one_code one_code;
                one_code.action = "j";
                one_code.a = "WHILE_" + to_string(while_label);
                while_label += 1;
                code[block_id].push_back(one_code);

                // 将下一步的地址写入
                mips_one_code next_label;
                next_label.action = next_step_name+":";
                next_label.var_num = 1;
                code[block_id].push_back(next_label);
            }

            // label，el之后,反填地址，并增加label
            if(quat_one.op == "el"){
                // 反填地址
                string next_step_name = "ELSE_" + to_string(else_label);
                else_label += 1;
                else_num += 1;
                pair<int,int> to = need_add.top();
                need_add.pop();
                code[to.first][to.second].c = next_step_name;

                // jump
                mips_one_code jump;
                jump.action = "j";
                code[block_id].push_back(jump);
                pair<int,int> jump_local = {block_id, (int)code[block_id].size()-1};
                need_add.push(jump_local);

                // 增加label
                mips_one_code next_label;
                next_label.action = next_step_name+":";
                next_label.var_num = 1;
                code[block_id].push_back(next_label);
            }

            // 增加结束的地址，反填else的终结位置
            if(quat_one.op == "ie"){
                // 反填地址
                string next_step_name = "END_ELSE_" + to_string(end_else_label);
                end_else_label += 1;
                // 反填跳转到分支终点的语句
                for(int i=0 ;i<else_num; ++i){
                    pair<int,int> to = need_add.top();
                    need_add.pop();
                    code[to.first][to.second].a = next_step_name;   
                }
                // 清空计数，下一次遇到再进行重新计数
                else_num = 0;

                // 将下一步的地址写入
                mips_one_code next_label;
                next_label.action = next_step_name+":";
                next_label.var_num = 1;
                code[block_id].push_back(next_label);
            }

            // 如果是else语句，就说明有跳转的位置，需要开始反填地址
            quat_id += 1;
        }
    }
    // 最后一个基本块结束以后把未被写回的非临时变量都写回
    for(int var_id = 0; var_id < (int)symbol_table.synbl.size(); ++var_id){
        if(!symbol_table.synbl[var_id].is_temp_var){
            // 不是临时变量
            int reg_id = find_var_reg(symbol_table.synbl[var_id].name);
            if(reg_id < 10){// 在寄存器内
                // 加在最后一个块里
                mips_one_code sw_code;
                sw_code.action = "sw";
                sw_code.b = RVALUE[reg_id];// 存入变量名
                sw_code.a = registers[reg_id]; // 数据保存的位置
                code[block_num-1].push_back(sw_code);  // 存入mips指令中
                RVALUE[reg_id] = ""; // 置空
            }
        }
    }
    return true;
}

bool get_asm(){
    ofstream file;
    file.open("../process_result/object_code.asm",ios::out);
    if(!file.is_open())
    {
        cout<<"cannot open the file"<<endl;
        return false;
    }

    // 变量信息
    file << ".data" << endl;
    for(int var_id = 0; var_id < (int)symbol_table.synbl.size(); ++var_id){
        file << "\t" << symbol_table.synbl[var_id].name << ":";
        if(symbol_table.synbl[var_id].type_point->t_val == 'f'){// 如果是浮点数
            file << "  .float 0" << endl;
        }
        else{
            file << "  .word 0" << endl;
        }// 初始默认值都为0
    }

    file << endl << endl << ".text" << endl;// 开始输出代码
    for(int i=0;i <block_num; ++i){
        for(int j=0; j<(int)code[i].size(); ++j){
            if(code[i][j].var_num == 1){
                file << code[i][j].action <<endl;
            }
            else if(code[i][j].action == "li" || code[i][j].action == "sw" || code[i][j].action == "lw" || code[i][j].action == "move"){// 两个操作数
                file << "\t" << code[i][j].action << " " << code[i][j].a <<", "<<code[i][j].b<<endl;
            }
            else if(code[i][j].action == "j"){// 一个操作数
                file << "\t" << code[i][j].action << " " << code[i][j].a <<endl;
            }
            else{// 3个操作数
                file << "\t" << code[i][j].action << " " << code[i][j].a <<", "<<code[i][j].b<<", "<< code[i][j].c <<endl;
            }
        }
    }
    

    return true;
}




bool generate_object_code(){
    div_base_block(&quat, &block_num, &per_block_num);// 划分基本块
    gen_act_inf();
    cout << "got liveness information for the variables in the quaternion" << endl;
    // PRINT_block_act_symbl(); // 输出活跃信息
    if(get_code() && get_asm()){
        // PRINT_mips();
        // PRINT_quat_num();// 打印四元式信息，包括值
        return true;
    }
    else{
        return false;
    }
}