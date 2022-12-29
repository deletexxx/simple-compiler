#include"../head/semantic.h"
// 加了动作的预测分析表
extern map<char, map<char,string>> pre_table_action;
// 对已经读好的token序列进行翻译文法分析
extern vector<pair<string,int>> my_token;// 从其他文件中去找
// 将临时变量加入符号表
extern struct SYM_TABLE symbol_table;


stack<char> S_;// 分析栈
stack<string> operate;// 符号栈
stack<string> SEM;
stack<char> SEM_type;// SEM对应的类型
vector<Quat> quat;
// int t_id = 0;// 新的变量的id,临时变量
int Q_id = 0;// 四元式的计数
extern char start;// 文法开始符号

// 将栈中的信息打印
void Print_stack(stack<string> S,ofstream *file){
    string arr;
    string e;
    while(!S.empty()){
        e = S.top();
        S.pop();
        arr = e+arr;
    }
    *file<<"SEM="<<arr<<"\t\t\t";
}

// 打印刚生成的四元式
void Print_a_Quat(ofstream *file){
    Quat q = quat[Q_id-1];
    *file <<"<" << q.op <<",\t" ;
    if(q.a!=""){
        *file << q.a <<",\t" ;
    }
    else{
        *file <<"_,\t" ;
    }
    if(q.b!=""){
        *file <<q.b <<",\t";
    }
    else{
        *file << "_,\t" ;
    }
    if(q.res!=""){
        *file << q.res << "\t>" << endl;
    }
    else{
        *file << "_\t>" << endl;
    }
}

string i_to_s(int i){
    string t = "";
    int temp_t = i;
    string temp_id;
    while(temp_t != 0){
        temp_id = "";
        temp_id.push_back(temp_t % 10 + '0');
        temp_t = temp_t/10;
        t = temp_id + t;
    }
    t = "t" + t;
    return t;
}

void GEQ(int *t_id){
    struct Quat temp_q;
    temp_q.op = operate.top();  
    if(!operate.empty())
        operate.pop();
    else
        cout << "ERROR! stack is empty!" << endl;
    // 这里一定是先出b后出a，不然会发生顺序上的问题
    temp_q.b = SEM.top();   SEM.pop();
    char b_type = SEM_type.top();   SEM_type.top();
    temp_q.a = SEM.top();   SEM.pop();  
    char a_type = SEM_type.top();   SEM_type.top();
    // 将t_id转换为字符
    temp_q.res = i_to_s(*t_id);
    char res_type;
    if(type_match(a_type, b_type, temp_q.op, &res_type)){// 如果类型成功匹配，将其加入符号表
        add_item(res_type, temp_q.res, false, false, false, 0, true);
        // cout<<temp_q.a<<" "<<a_type<<" "<<temp_q.b<<" "<<b_type << " " << temp_q.res <<a_type<<endl;
    }
    else {
        cout << "ERROR:" << temp_q.a << "(" << a_type <<  ") and " << temp_q.b << "(" << b_type << ") type match failed" << endl;
    }
    // add_item()
    (*t_id)++;

    SEM.push(temp_q.res);
    SEM_type.push(a_type);

    temp_q.id = Q_id;
    Q_id++;

    quat.push_back(temp_q);
}

void PUSH_i(string i, char type){//, int type){
    SEM.push(i);
    SEM_type.push(type);
}

void ASSI(){
    struct Quat temp_q;
    temp_q.op = "=";
    temp_q.a = SEM.top();   SEM.pop();
    char a_type = SEM_type.top(); SEM_type.pop();
    temp_q.res = SEM.top(); SEM.pop();
    char res_type = SEM_type.top(); SEM_type.pop();
    temp_q.b = "";
    temp_q.id = Q_id;
    Q_id++;
    quat.push_back(temp_q);
    
    // // 参数类型的匹配
    // cout<<temp_q.a<<" "<< temp_q.res <<endl;
    char type;
    // if(!type_match(temp_q.a_type, temp_q.res_type, "=", &res_type)){
    //     cout << temp_q.a << "(" << a_type <<  ") and " << temp_q.b << "(" << res_type << ") type match failed" << endl;
    // }
    if(!type_match(a_type, res_type, "=", &res_type)){
        cout << "ERROR:" << temp_q.a << "(" << a_type <<  ") and " << temp_q.res << "(" << res_type << ") type match failed" << endl;
    }
}


void IF(){
    struct Quat temp_q;
    temp_q.op = "if";
    temp_q.a = SEM.top();   SEM.pop();  // SEM_type.pop();
    SEM_type.pop();
    temp_q.b = "";
    temp_q.res = "";
    temp_q.id = Q_id;
    Q_id++;
    quat.push_back(temp_q);
}

void ELSE(){
    struct Quat temp_q;
    temp_q.op = "el";
    temp_q.a = "";
    temp_q.b = "";
    temp_q.res = "";
    temp_q.id = Q_id;
    Q_id++;
    quat.push_back(temp_q);
}

void ELIF(){
    struct Quat temp_q;
    temp_q.op = "ei";
    temp_q.a = SEM.top();   SEM.pop();  // SEM_type.pop();
    SEM_type.pop();
    temp_q.b = "";
    temp_q.res = "";
    temp_q.id = Q_id;
    Q_id++;
    quat.push_back(temp_q);
}

void ENDIF(){
    struct Quat temp_q;
    temp_q.op = "ie";
    temp_q.a = "";
    temp_q.b = "";
    temp_q.res = "";
    temp_q.id = Q_id;
    Q_id++;
    quat.push_back(temp_q);
}

void WE(){
    struct Quat temp_q;
    temp_q.op = "we";
    temp_q.a = "";
    temp_q.b = "";
    temp_q.res = "";
    temp_q.id = Q_id;
    Q_id++;
    quat.push_back(temp_q);    
}

void WH(){
    struct Quat temp_q;
    temp_q.op = "wh";
    temp_q.a = "";
    temp_q.b = "";
    temp_q.res = "";
    temp_q.id = Q_id;
    Q_id++;
    quat.push_back(temp_q);
    // cout<<":< "<<temp_q.op<<" , "<<temp_q.a << " , "<<temp_q.b <<" , "<<temp_q.res<<" >"<<endl;
}

void DO(){
    struct Quat temp_q;
    temp_q.op = "do";
    temp_q.a = SEM.top();   SEM.pop();  // SEM_type.pop();
    SEM_type.pop();
    temp_q.b = "";
    temp_q.res = "";
    temp_q.id = Q_id;
    Q_id++;
    quat.push_back(temp_q);
}

void PRINT_Quat(ofstream *file){
    vector<Quat>::iterator it = quat.begin();
    for(it; it<quat.end();++it){
        *file << (*it).id <<"\t< "<<(*it).op <<",\t" ;
        if((*it).a!=""){
            *file << (*it).a <<",\t" ;
        }
        else{
            *file <<"_,\t" ;
        }
        if((*it).b!=""){
            *file <<(*it).b <<",\t";
        }
        else{
            *file << "_,\t" ;
        }
        if((*it).res!=""){
            *file << (*it).res << "\t>" << endl;
        }
        else{
            *file << "_\t>" << endl;
        }
    }
}


bool get_Quat(ofstream *file){
    int t_id = 0;
    *file << "栈\t\t\t"  << "当前符号\t\t\t" << "栈操作\t\t\t" << "SEM" << "动作及生成的四元式" << endl;

    S_.push('#');
    S_.push(start);

    vector< pair<string,int> > token = my_token;
    token.push_back({"#",0});// 结束符号
    string temp_ident;// 存储被消去的标识符，为了之后的push
    char temp_ident_type;// 对应的类型
    // int temp_ident_type;

    pair<string,int> temp_token = token[0];
    token.erase(token.begin(), token.begin()+1);// 删除头部元素
    
    int if_add_op = 0;// 因为我之前把两个符号拆开的操作，很影响后面的识别，因此这里判断第二次是否需要加入操作符
    string temp_operate = "";
    while((!token.empty() || !S_.empty())){// 如果同时为空说明语法匹配，结束了
        // 输出信息
        print_stack(S_, file);
        
        *file<<temp_token.first<<"\t\t\t";
        

        char temp_S = S_.top();// 此时的栈顶
        
        char c=find_c(temp_token.second); // 判断当前符号对应的终结符是什么
        // if(temp_token.second==9) cout<<temp_token.first<<" "<<c<<endl;
        if(c=='x') c = temp_token.first[0];
        else if(c=='z') {// 返回z说明这个字符是由两个字符组成的将他们分开进行查验文法
            temp_operate = temp_token.first;// 暂存，当轮到查验这个终结符时，再入操作符栈，否则入栈的符号位置会错乱
            if_add_op = 1;
            if(temp_token.first == "++" || temp_token.first == "--"){// ++ --时另一个操作符默认为1
                SEM.push("1");
                SEM_type.push('i');// 作为整型
                // SEM_type.push(72);// 整型常量
            }// 这里还是有点问题的，不应该在这里将1push，而是要在分析到++的时候再push

            string token1,token2;// 复合字符的第一二个字符
            token2.push_back(temp_token.first[1]);// 复合的两个字符里的第二个
            token.insert(token.begin(),{token2,state_to_code(token2)});// 把第二个加入token中，下一次读取
            
            token1.push_back(temp_token.first[0]);// 复合的两个字符里的第一个
            temp_token = {token1, state_to_code(token1)};
            c = temp_token.first[0];// c是
        }
        
        if(is_capital(temp_S)){// 判断栈顶是否是非终结符
            // 查找分析表
            string right = pre_table_action[temp_S][c];
            
            // 输出信息
            *file << temp_S << "->\"" << right << "\"" ;
            // SEM信息
            (*file) <<"\t\t\t";
            Print_stack(SEM, file);
            (*file) << endl;
            // 逆序入栈
            S_.pop();// 出栈
            if(right!=" "){// 如果表达式右端推出的不是空，就逆序入栈
                for(int i=right.size()-1;i>=0;--i){
                    S_.push(right[i]);
                }
            }

            if(token.empty() && pre_table_action[temp_S]['#']!=" ") return false;
            
        }

        else if(is_number(temp_S)){// 栈顶是动作符号
            (*file) <<"\t\t\t";
            Print_stack(SEM, file);
            (*file) << "\t\t\t";
            
            int action_id = temp_S - '0';
            S_.pop();
            switch (action_id){
                case 0:
                    (*file) << "GEQ("<< operate.top() <<")";
                    GEQ(&t_id);
                    Print_a_Quat(file);
                    break;
                case 1:{
                    (*file) << "PUSH("<<temp_ident<<")"<<endl;
                    char type = find_type(temp_ident, temp_ident_type);
                    PUSH_i(temp_ident, type);// , temp_ident_type);
                    break;
                }
                case 2:
                    (*file) << "ASSI";
                    ASSI();
                    Print_a_Quat(file);
                    break;
                case 3:
                    (*file) << "IF";
                    IF();
                    Print_a_Quat(file);
                    break;
                case 4:
                    (*file) << "ELIF";
                    ELIF();
                    Print_a_Quat(file);
                    break;
                case 5:
                    (*file) << "ELSE";
                    ELSE();
                    Print_a_Quat(file);
                    break;
                case 6:
                    (*file) << "ENDIF";
                    ENDIF();
                    Print_a_Quat(file);
                    break;
                case 7:
                    (*file) << "WH";
                    WH();
                    Print_a_Quat(file);
                    break;
                case 8:
                    (*file) << "DO";
                    DO();
                    Print_a_Quat(file);
                    break;
                case 9:
                    (*file) << "WE";
                    WE();
                    Print_a_Quat(file);
                    break;
            }
            // int len = quat.size();
            // if(action_id!=1)// 输出进行GEQ的四元式
            //     (*file)<<"<"<<quat[len-1].op<<","<<quat[len-1].a<<","<<quat[len-1].b<<","<<quat[len-1].res<<">"<<endl;
                        
        }
        else{// 栈顶是终结符，查看有无匹配，或者对操作符进行入栈
            if(temp_S == c){
                if((temp_token.second>=32 && temp_token.second<=39) || (temp_token.second>=47 && temp_token.second<=48)) {                    
                    if(if_add_op != 1 && if_add_op != 2){
                        operate.push(temp_token.first);
                    }
                    else if(if_add_op == 1){
                        operate.push(temp_operate);// 先将这个操作符写入操作符栈中
                        if_add_op++;
                    }
                    else
                        if_add_op = 0;// 下一个就要输入了
                }
                else if_add_op = 0;
                if(temp_token.second == 30 || temp_token.second == 31 || temp_token.second == 71|| temp_token.second == 72) {
                    temp_ident = temp_token.first;
                    temp_ident_type = temp_token.second;
                    // temp_ident_type = temp_token.second;
                }
                *file<<"delete " << c ;
                S_.pop();// 消去
                temp_token = token[0];//再读token里的首位
                if(!token.empty())
                    token.erase(token.begin(), token.begin()+1);// 删除头部元素
                // 打印SEM信息
                (*file) <<"\t\t\t";
                Print_stack(SEM, file);
                (*file) << endl;
            }
            else {//if(S_.empty()){// 如果非终结符没有和最后一个字符匹配上，就不会继续了
                *file<<"mismatch " << c << endl;
                return false;
            }
        }
        
        // Print_stack(operate, file); *file<<if_add_op;
        
    }
    return true;
    
}


bool generate_quat(){
    ofstream file;
    file.open("../process_result/Quat_trans_process.txt",ios::out);
    if(!file.is_open())
    {
        cout<<"cannot open the file"<<endl;
        return false;
    }

    if(get_Quat(&file)){
        ofstream file_quat;
        file_quat.open("../process_result/Quat.txt",ios::out);
        if(!file_quat.is_open())
        {   
            cout<<"cannot open the Quat file"<<endl;
            return false;
        }
        PRINT_Quat(&file_quat);
        return true;
    }
    else 
        return false;
}