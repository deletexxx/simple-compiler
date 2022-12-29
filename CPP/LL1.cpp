#include"../head/LL1.h"
// #include"source.h"

// 产生式
// 使用set的好处就是可以去掉手工判断重复元素
map<char, set<string>> BNF;

// 终结符和非终结符(是不会重复的，大写字母表示非终结符)
set<char> terminator,non_Terminator;

// 文法开始符
char start;

// FIRST、FOLLOW、select集
map<char, string> FIRST,FOLLOW,FOLLOW_nonter,SELECT;// FOLLOW_nonter,SELECT是用来暂时存储的
map<char, bool> if_done_first;// 用于判断是否已经找到了first集合
map<char, set<pair<string,string>> > select;


// 预测分析表
map<char, map<char,string>> pre_table;

// 判断是否能推出空
map<char, bool> empty_set;

// 分析栈
stack<char> S;

// 对已经读好的token序列进行文法分析
extern vector<pair<string,int>> my_token;// 从其他文件中去找


// 带动作的BNF，
map<char, set<string>> BNF_action;
// 一个带动作的select集
map<char, set<pair<string,string>> > select_action;
// 一个左端非终结符与一个右端表达式，有一个对应的带动作的右端表达式
map<pair<int, string>, string> LL1_to_action;
// 带动作的预测分析表
map<char, map<char,string>> pre_table_action;


void init_action(string read_file){

    //open file and read file
    ifstream grammar_file;
    grammar_file.open(read_file,ios::in);
    if(!grammar_file.is_open())
    {
        cout<<"cannot open the grammar_ file"<<endl;
    }

    //open file and read file
    ifstream grammar_file_action;
    grammar_file_action.open("../assert/LL1_trans.txt",ios::in);
    if(!grammar_file_action.is_open())
    {
        cout<<"cannot open the translate grammar file"<<endl;
    }

    // 读取文法产生式
    char left_temp, left_temp_a;
    string right_temp, right_temp_a;
    string temp, temp_a;
    int start_nonter = 1;
    while(!grammar_file.eof()){
        grammar_file >> left_temp ;
        grammar_file_action >> left_temp_a;
        non_Terminator.insert(left_temp); // 保存非终结符
        if_done_first[left_temp] = false;// 还没有得到first集合
        if(start_nonter) {
            start_nonter = 0;
            start = left_temp;// 第一个非终结符作为开始符号
        }

        // cout<<"nont:"<<left_temp <<endl;

        grammar_file >> right_temp;//读入右端表达式
        grammar_file_action >> right_temp_a;
        int start , end;
        start = end = 0;
        int start_a, end_a;
        start_a = end_a = 0;
        if(left_temp != 'V')// 因为V在进行||，会被误判，因此不进行分割，写得时候就已经分开写了。
            while(right_temp.find('|',start)!= string::npos){
                end = right_temp.find('|',start);//从发现|之后开始找到下一个分隔符
                temp = right_temp.substr(start,end-start);
                BNF[left_temp].insert(temp);// 将分割符隔开的右端表达式保存
                // 同时读取带动作的文法表达式
                end_a = right_temp_a.find('|',start_a);
                temp_a = right_temp_a.substr(start_a,end_a-start_a);
                LL1_to_action[{left_temp_a,temp}] = temp_a;// 对应关系
                // 判断并保存非终结符
                for(int i=0;i<(int)temp.length();++i){
                    if(!is_capital(temp[i])) terminator.insert(temp[i]);
                }
                // cout<<"line:"<<temp<<endl;
                start = end+1; 
                start_a = end_a + 1; 
            }
        if(end+1 == (int)right_temp.length() || right_temp=="~"){// 判断最后是否能推出空，如果可以就将空的字符串也写入
            temp = "";
            empty_set[left_temp] = true;// 能推出空
        }
        else{
            empty_set[left_temp] = false;// 推不出空
            temp = right_temp.substr(start,right_temp.length()-start);
            
            for(int i=0;i<temp.length();++i){
                if(!is_capital(temp[i])) terminator.insert(temp[i]);
            }// 判断并保存终结符
        }        
        BNF[left_temp].insert(temp);// 保存文法
        // 带动作的文法表达式的每一行里最后一个分割符后的表达式
        if(end_a+1 == (int)right_temp_a.length() || right_temp_a=="~"){
            temp_a = "";
        }
        else{
            temp_a = right_temp_a.substr(start_a,right_temp_a.length()-start_a); 
        }      
        LL1_to_action[{left_temp_a,temp}] = temp_a;// 对应关系
        
        
    }
    
    

    //close file
    grammar_file.close();
    // token_file.close();
}

void make_pre_table_action(){
    for(auto it = select_action.begin(); it!=select_action.end(); ++it){
        for(auto i = it->second.begin(); i!=it->second.end(); ++i){
            string right = (*i).second;
            for(int l=0;l<right.size();++l)
            if(i->first.empty())    pre_table_action[it->first][right[l]] = " ";
            else    pre_table_action[it->first][right[l]] = i->first;
        }
    }
}

void print_BNF_action(){
    // 输出带动作的文法表达式
    cout<<"BNF_action:"<<endl;
    for(auto it = BNF_action.begin();it!=BNF_action.end();++it){
        cout<<it->first<<"->";
        set<string>::iterator temp;
        for(temp=(it->second).begin();temp!=(it->second).end();) {
            string a = *temp;
            if(!a.empty()) cout<<(*temp);
            else cout<<"empty";
            if(++temp !=(it->second).end()) cout<<"|";
        }
        cout<<endl;
    }
    cout<<endl;
}

void print_select_action(){
    cout<<"SELECT SET:"<<endl;
    for(auto it = BNF_action.begin(); it!=BNF_action.end(); ++it){
        for(auto j=select_action[it->first].begin();j!=select_action[it->first].end();++j){
            cout<<"select("<<it->first<<"->"<<(*j).first<<")={"<<(*j).second<<"}    ";
        }
        cout<<endl;
    }
    cout<<endl;
}

bool is_capital(char temp){
    if(temp>='A' && temp<='Z') return true;
    else return false;
}

void init(string read_file){
    //open file and read file
    ifstream grammar_file;
    grammar_file.open(read_file,ios::in);
    if(!grammar_file.is_open())
    {
        cout<<"cannot open the grammar file"<<endl;
    }

    // 读取文法产生式
    char left_temp;
    string right_temp;
    string temp;
    int start_nonter = 1;
    while(!grammar_file.eof()){
        grammar_file >> left_temp ;
        non_Terminator.insert(left_temp); // 保存非终结符
        if_done_first[left_temp] = false;// 还没有得到first集合
        if(start_nonter) {
            start_nonter = 0;
            start = left_temp;// 第一个非终结符作为开始符号
        }

        // cout<<"nont:"<<left_temp <<endl;

        grammar_file >> right_temp;//读入右端表达式
        int start , end;
        start = end = 0;
        if(left_temp != 'V')// 因为V在进行||，会被误判，因此不进行分割，写得时候就已经分开写了。
            while(right_temp.find('|',start)!= string::npos){
                end = right_temp.find('|',start);//从发现|之后开始找到下一个分隔符
                temp = right_temp.substr(start,end-start);
                BNF[left_temp].insert(temp);// 将分割符隔开的右端表达式保存
                // 判断并保存非终结符
                for(int i=0;i<(int)temp.length();++i){
                    if(!is_capital(temp[i])) terminator.insert(temp[i]);
                }
                // cout<<"line:"<<temp<<endl;
                start = end+1; 
            }
        if(end+1 == (int)right_temp.length() || right_temp=="~"){// 判断最后是否能推出空，如果可以就将空的字符串也写入
            temp = "";
            BNF[left_temp].insert(temp);
            empty_set[left_temp] = true;// 能推出空
        }
        else{
            empty_set[left_temp] = false;// 推不出空
            temp = right_temp.substr(start,right_temp.length()-start);
            BNF[left_temp].insert(temp);
            
            for(int i=0;i<temp.length();++i){
                if(!is_capital(temp[i])) terminator.insert(temp[i]);
            }// 判断并保存终结符
        }
        
    }
    //close file
    grammar_file.close();
    // token_file.close();
}



string find_first(char start){
    if(!is_capital(start)){// 如果是终结符，则first就是
        string ans = "";
        ans.push_back(start);
        return ans;
    }
    else{
        if(if_done_first[start])
            return FIRST[start];// 如果非终结符的first集合已经获得，就直接返回
        for(auto it = BNF[start].begin(); it != BNF[start].end(); ++it){// 判断这个非终结符的所有右端表达式

            string right = *it;
            if(right.empty()){// 如果能推出空串（轮到的这个右端是空），就将空格加入first集合中，这里是他自己能直接推出空
                FIRST[start].append(" ");
            }
            else{
                int len = right.size();
                for(int i=0; i<len; ++i){

                    if(is_capital(right[i])){// 如果是非终结符
                        auto head = find_first(right[i]);
                        if(empty_set[right[i]] && (head.find(" ")!=head.size()))  head.erase(head.find(' '), 1);// 如果能推出空，要先将空剔除，因为虽然这个非终结符能推出空，但如果start的右端，在这个非终结符之后还有非空的first，那么start的first里就没有空串
                        // 补充first集
                        for (int j=0;j<(int)head.size();++j){
                            if(FIRST[start].find(head[j])==FIRST[start].npos) //将空去除
                                FIRST[start].push_back(head[j]);// 将head中的first加入
                        }
                        if(!empty_set[right[i]]){// 如果当前非终结符不能推出空，就跳出（即如果有空才能取推断表达式右端的下一个字符
                            break;
                        }
                        else if(i == len-1){
                            FIRST[start].push_back(' ');// 如果当前非终结符能推出空，而且是最后一个非终结符，那么原非终结符就能推出空，first集合里就能推出空
                        }
                    }
                    else{ // 若为终结符，则first集就是它自身
                        FIRST[start].push_back(right[i]);
                        break;
                    }

                }
            }
        }// 每个右端都查了一遍
        if_done_first[start] = true;
        return FIRST[start];// 返回这个非终结符的first集合
    }
}

void first_set(){
    string temp;
    for(auto it = non_Terminator.begin(); it!=non_Terminator.end();++it) {
        // cout<<"q"<<endl;
        temp = find_first(*it);
        // cout<<"FIRST("<<*it<<")={"<<temp<<"}"<<endl;
    }
}

void follow_set(){
    FOLLOW[start].push_back('#'); // 把#加入文法的开始符号的follow集合中
    string temp;
    for(auto it=BNF.begin(); it!=BNF.end();++it){//查看每个非终结符的每个表达式
        for(auto i=it->second.begin();i!=it->second.end();++i){// 非终结符的所有右端表达式
            string right = *i;// 一个右端表达式
            // cout<<"right: "<<right<<endl;
            int len = right.size();
            for(int j=0;j<len;++j){
                if(is_capital(right[j]) && j!=len-1){// 如果是非终结符且不是最后一个字符的话
                    if(is_capital(right[j+1])){// 如果后一个字符是非终结符
                        // *******
                        /* 实现
                         如果有 A->\alpha B\beta，则FIRST(\beta)/\{\varepsilon\}（这是除去\varepsilon的意思）加入FOLLOW(B);
                         \beta 指的不止B后的第一个非终结符/终结符，如果后面的是非终结符且这个非终结符能推出空
                         那么也就要看B后第2个非终结符/终结符，同理可能也看第3、4……个
                         */
                        // 将follow中的first集合加入
                        string next_first = FIRST[right[j+1]];
                        int next_len = next_first.size();
                        for(int k=0;k<next_len;++k){
                            if(FOLLOW[right[j]].find(next_first[k]) == FOLLOW[right[j]].npos && next_first[k]!=' ')// 将空取出，并手动去重
                                FOLLOW[right[j]].push_back(next_first[k]);
                        }
                        
                        // 如果是j是倒数第二个有下一个if去对它的空做处理,这里是说如果j后面的非终结符能推出空，就要看非终结符后面的“first”作为j个非终结符的follow
                        if(empty_set[right[j+1]] && j+1 < len-1){
	                        int next_next = j + 2;
        	                while(next_next <= len-1){
    	                        if(is_capital(right[next_next])){
                                    next_first = FIRST[right[next_next]];
                        			next_len = next_first.size();
			                        for(int k=0;k<next_len;++k){
            			                if(FOLLOW[right[j]].find(next_first[k]) == FOLLOW[right[j]].npos && next_first[k]!=' ')
                        	            // 将空取出，并手动去重
                            	        FOLLOW[right[j]].push_back(next_first[k]);
                        			}
                                    if(!empty_set[right[next_next]]) break;
                                    // 如果这个非终结符不能推出空就中止
                                }
                                else{
                                    FOLLOW[right[j]].push_back(right[next_next]);
                                    break;
                                }
                                ++next_next;
	                        }
                        }
                        // *******

                        if(empty_set[right[j+1]] && j+1==len-1){// 如果下一个非终结符能推出空，并且它是最后一个，就将左边非终结符的follow集合加入，这里只是将他的非终结符加入，之后再写入
                            if(FOLLOW_nonter[right[j]].find(it->first) == FOLLOW_nonter[right[j]].npos)// 手动查重
                                FOLLOW_nonter[right[j]].push_back(it->first);
                        }
                    }
                    else{// 右边的(下一个字符)为终结符，直接将它加到follow集中
                        if(FOLLOW[right[j]].find(right[j+1]) == FOLLOW[right[j]].npos)
                        FOLLOW[right[j]].push_back(right[j+1]);
                    }
                }
                else if(is_capital(right[j])){// 如果是非终结符且是最后一个字符，就要将左端的follow加入这个非终结符的follow集中
                    if(FOLLOW_nonter[right[j]].find(it->first) == FOLLOW_nonter[right[j]].npos)// 手动查重
                        FOLLOW_nonter[right[j]].push_back(it->first);
                }
            }
        }
    }
    // 每个非终结符的follow都查完了，就把它如果出现在其他非终结符的follow中就进行更新插入
    int flag=0;
    do{
        flag = 0;
        for(auto it = FOLLOW_nonter.begin(); it != FOLLOW_nonter.end();++it){// 遍历每一个follow集中包含其他非终结符的follow集
            int len = FOLLOW[it->first].size();
            string follow_nonter_right = it->second;
            for(int i=0; i<follow_nonter_right.size(); ++i){
                for(int j=0; j<FOLLOW[follow_nonter_right[i]].size() ;++j){
                    char temp = FOLLOW[follow_nonter_right[i]][j];
                    if(FOLLOW[it->first].find(temp) == FOLLOW[it->first].npos){
                        FOLLOW[it->first].push_back(temp);
                    }
                }
            }
            if(FOLLOW[it->first].size() > len) flag=1;// 如果更新后的follow集的长度大于原来的长度，说明有新的终结符加入，说明可能还有没有加入的
        }
    }while(flag);

    // for(auto it = FOLLOW.begin();it!=FOLLOW.end();++it){
    //     cout<<"FOLLOW("<<it->first<<")={"<<it->second<<"}"<<endl;
    // }
}

bool select_set_judge(){//根据select判断是否是LL1文法
    for(auto it = BNF.begin(); it!=BNF.end(); ++it){
        for(auto i=it->second.begin(); i!=it->second.end(); ++i){// 遍历右端所有表达式
            string right = *i;
            string temp_select;// 某一个右端表达式的select集合
            bool if_empty = false;// 这个产生式右部能否推出空
            for(int l=0; l<right.size(); ++l){
                if(!is_capital(right[l])){// 如果是终结符，加入select，然后结束
                    temp_select.push_back(right[l]);
                    break;
                }
                else{// 是非终结符
                    temp_select.append(FIRST[right[l]]);
                    if(!empty_set[right[l]]){// 如果不能推出空，就结束，否则继续找first
                        break;
                    }
                    else if(l == right.size()-1){// 如果能推到最后一个非终结符并且最后一个非终结符也能推出空，那么这个产生式右端也能是空
                        if_empty = true;
                    }
                }
            }
            if(right.empty()) if_empty = true;// 如果右端的表达式为空，说明能推出空

            // temp_select里已经是右端产生式的first了
            if(if_empty){
                // 去掉空
                for(int l=0;l<temp_select.size();++l){
                    if(temp_select[l]==' '){
                        int pos = temp_select.find(' ');
                        temp_select.erase(pos,1);// 删去空,从first集中得来的，只可能有一个空，不会重复
                        break;
                    }
                }
                string temp_follow = FOLLOW[it->first];
                
                for(int l = temp_follow.size()-1;l>=0;--l){// 删除相同的，进行并操作
                    for(int l2 = temp_select.size()-1; l2>=0;--l2){
                        if(temp_follow[l] == temp_select[l2]){
                            temp_follow.erase(l,1);
                        }
                    }
                }
                temp_select.append(temp_follow);
            }

            // cout<<"select("<<it->first<<"->"<<right<<")={"<<temp_select<<"}    ";
            select[it->first].insert({right,temp_select});
            select_action[it->first].insert({LL1_to_action[{(*it).first,right}],temp_select});// 对应的带有动作的select集合
            
            // 进行具有相同左部的select的并
            for(int l=0;l<temp_select.size();++l){
                for(int l2=0;l2<SELECT[it->first].size();++l2){
                    if(temp_select[l] == SELECT[it->first][l2]){// 说明并集非空
                        cout<<"not LL1"<<endl;
                        cout << it->first << "\t" << temp_select<<"\t"<<SELECT[it->first]<<endl;
                        return false;
                    }
                }
            }
            SELECT[it->first].append(temp_select);
        }
        // 输出select集合
        // for(auto j=select[it->first].begin();j!=select[it->first].end();++j){
        //     cout<<"select("<<it->first<<"->"<<(*j).first<<")={"<<(*j).second<<"}    ";
        // }
        // cout<<endl;
        // cout<<"SELECT("<<it->first<<")={"<<SELECT[it->first]<<"}"<<endl;
    }
    return true;
}


void make_pre_table(){
    for(auto it = select.begin(); it!=select.end(); ++it){
        for(auto i = it->second.begin(); i!=it->second.end(); ++i){
            string right = (*i).second;
            for(int l=0;l<right.size();++l)
            if(i->first.empty())    pre_table[it->first][right[l]] = " ";
            else    pre_table[it->first][right[l]] = i->first;
        }
    }
}


char find_c(int token_code){
    switch(token_code)
    {
        case 0:
            return '#';// 结束符号
        case 71:
            return 'n';
        case 72:
            return 'n'; // 浮点数常量和整数常量
        case 30:
            return 'i';// 标识符id
        case 1:
            return 'm';// main
        case 10:
            return 'j';// if
        case 11:
            return 'e';// else
        case 13:
            return 'w';//while
        case 14:
            return 'f';//for
        case 17:
            return 'c';//cin
        case 18:
            return 'o';//cout;
        default:
            if(token_code >=2 && token_code <= 9) return 'd';
            // 是一些需要拆开的符号
            if(token_code == 36 || token_code==37) return 'z';
            if(token_code >= 41 && token_code<=46) return 'z';
            if(token_code == 49 || token_code==50) return 'z';
            else return 'x';// 不知道是啥
    }
}

void print_stack(stack<char> S, ofstream *file){
    string arr;
    char e;
    while(!S.empty()){
        e = S.top();
        S.pop();
        arr.insert(0,1,e);
    }
    *file<<arr<<"\t\t\t";
}

bool check_grammar(ofstream *file){
    *file << "栈\t\t\t"  << "当前符号\t\t\t" << "栈操作" <<endl;

    S.push('#');
    S.push(start);

    vector< pair<string,int> > token = my_token;
    token.push_back({"#",0});// 结束符号

    pair<string,int> temp_token = token[0];
    token.erase(token.begin(), token.begin()+1);// 删除头部元素

    while((!token.empty() || !S.empty())){// 如果同时为空说明语法匹配，结束了
        // 输出信息
        print_stack(S, file);
        *file<<temp_token.first<<"\t\t\t";

        char temp_S = S.top();// 此时的栈顶
        char c=find_c(temp_token.second); // 判断当前符号对应的终结符是什么
        if(c=='x') c = temp_token.first[0];
        else if(c=='z') {// 返回z说明这个字符是由两个字符组成的将他们分开进行查验文法
            string token1,token2;// 复合字符的第一二个字符
            token2.push_back(temp_token.first[1]);// 复合的两个字符里的第二个
            token.insert(token.begin(),{token2,state_to_code(token2)});// 把第二个加入token中，下一次读取
            
            token1.push_back(temp_token.first[0]);// 复合的两个字符里的第一个
            temp_token = {token1, state_to_code(token1)};
            c = temp_token.first[0];// c是
        }
        if(is_capital(temp_S)){// 判断栈顶是否是非终结符
            // 查找分析表
            string right = pre_table[temp_S][c];
            // 输出信息
            *file << temp_S << "->" << right << endl;
            // 逆序入栈
            S.pop();// 出栈
            if(right!=" "){// 如果表达式右端推出的不是空，就逆序入栈
                for(int i=right.size()-1;i>=0;--i){
                    S.push(right[i]);
                }
            }

            if(token.empty() && pre_table[temp_S]['#']!=" ") return false;
        }
        else{
            if(temp_S == c){
                *file<<"delete " << c << endl;
                S.pop();// 消去
                temp_token = token[0];//再读token里的首位
                if(!token.empty())
                    token.erase(token.begin(), token.begin()+1);// 删除头部元素
            }
            else {//if(S.empty()){// 如果非终结符没有和最后一个字符匹配上，就不会继续了
                *file<<"mismatch " << c << endl;
                return false;
            }
        }
        
    }
    return true;
    
}

void print_first(){
    cout<<"FIRST SET:"<<endl;
    for(auto it = FIRST.begin(); it!=FIRST.end();++it) {
        cout<<"FIRST("<<(*it).first<<")={"<<it->second<<"}"<<endl;
    }
    cout<<endl;
}

void print_follow(){
    cout<<"FOLLOW SET:"<<endl;
    for(auto it = FOLLOW.begin();it!=FOLLOW.end();++it){
        cout<<"FOLLOW("<<it->first<<")={"<<it->second<<"}"<<endl;
    }
    cout<<endl;
}

void print_select(){
    cout<<"SELECT SET:"<<endl;
    for(auto it = BNF.begin(); it!=BNF.end(); ++it){
        for(auto j=select[it->first].begin();j!=select[it->first].end();++j){
            cout<<"select("<<it->first<<"->"<<(*j).first<<")={"<<(*j).second<<"}    ";
        }
        cout<<endl;
    }
    cout<<endl;
}



void print_pre_table(map<char, map<char,string>> pre_table){
    // 输出预测分析表
    cout<<endl;
    cout<<"Predictive Analysis Form:"<<endl;
    cout << "         " << "\t";
    for(auto it = terminator.begin(); it!=terminator.end();++it){// �ս������
        cout << *it <<"\t";
    }
    cout<<"#"<<endl;
    for(auto it = non_Terminator.begin(); it!=non_Terminator.end(); ++it){
        cout<<"         "<<*it<<"\t";
        for(auto i = terminator.begin(); i!=terminator.end(); ++i){
            if(pre_table[*it][*i]!=" ") cout<<pre_table[*it][*i] <<"\t";
            else cout<<"~"<<"\t";
        }
        if(pre_table[*it]['#']!=" ") cout<<pre_table[*it]['#']<<endl;
        else cout<<"~"<<endl;
    }
    cout<<endl;
}

void print_BNF(){
    // 输出读入的表达式
    cout<<"BNF:"<<endl;
    for(auto it = BNF.begin();it!=BNF.end();++it){
        cout<<it->first<<"->";
        set<string>::iterator temp;
        for(temp=(it->second).begin();temp!=(it->second).end();) {
            string a = *temp;
            if(!a.empty()) cout<<(*temp);
            else cout<<"empty";
            if(++temp !=(it->second).end()) cout<<"|";
        }

        cout<<endl;
    }
    cout<<endl;
}

void print_terminator(){
    // 输出终结符与非终结符的集合
    set<char>::iterator temp;
    cout<<"Terminator set:"<<endl;
    for(temp=terminator.begin();temp!=terminator.end();++temp) {
        cout<<(*temp)<<" ";
    }
    cout<<endl<<"non_Terminator set:"<<endl;
    for(temp=non_Terminator.begin();temp!=non_Terminator.end();++temp) {
        cout<<(*temp)<<" ";
    }
    cout<<endl;
}

void save_information(){
    string write_file = "../process_result/pre_table.txt";

    ofstream table_file;
    table_file.open(write_file, ios::out);
    if(!table_file.is_open()){
        cout<<"cannot open the table_file"<<endl;
    }

    table_file << start << endl;

    table_file << non_Terminator.size() << "\t\t";
    for(auto it = non_Terminator.begin(); it!=non_Terminator.end(); ++it){
        table_file << *it <<"\t";
    }
    table_file << endl << terminator.size() << "\t\t";
    for(auto it = terminator.begin(); it!=terminator.end(); ++it){
        table_file << *it <<"\t";
    }
    table_file << endl;

    for(auto it = pre_table.begin(); it != pre_table.end(); ++it){
        table_file << it->first <<"\t";
        table_file << it->second.size()<<"\t\t";
        for(auto i = it->second.begin(); i!=it->second.end(); ++i){
            table_file << i->first <<" ";

            if(i->second == " ") table_file << "~" <<"\t\t";
            else table_file << i->second <<"\t\t";

        }
        
        table_file << endl;
    }

    table_file.close();
}

bool read_pre_table(){
    string read_file = "./pre_table.txt";

    //open file and read file
    ifstream table_file;
    table_file.open(read_file,ios::in);
    if(!table_file.is_open())
    {
        cout<<"cannot open the table file"<<endl;
        return false;
    }

    table_file >> start;// 将文法开始符号读入

    int num_non, num_ter;// 非终结符和终结符的数量
    table_file >> num_non;
    char non_term, ter; // 读到的非终结符，终结符
    for(int i=0; i<num_non; ++i){
        table_file >> non_term;
        non_Terminator.insert(non_term);
    }
    table_file >> num_ter;
    for(int i=0; i<num_ter; ++i){
        table_file >> ter;
        terminator.insert(ter);
    }

    string right; // 对应的右端表达式
    // num_ter是每个非终结符对应的终结符的个数
    for(int i=0;i<num_non;++i){
        table_file >> non_term; //获得非终结符
        table_file >> num_ter;
        for(int j=0; j<num_ter; ++j){
            table_file >> ter >> right;
            if(right == "~") right = " ";// 如果读入的是符号~，说明推出空
            pre_table[non_term][ter] = right;
        }
        cout<< endl;
    }
    

    table_file.close();
    return true;
}

bool creat_pre_table(string grammar){
    // 语法分析
    // init(grammar);// 读入文法
    init_action(grammar);// 同时读入带动作和不带动作的文法
    // print_BNF();
    // print_BNF_action();
    first_set();// 得到FIRST集合
    follow_set();// 得到FOLLOW集合
    bool if_LL1 = select_set_judge();// 得到SELECT集合并判断是否是LL1文法
    if(if_LL1){
        cout<<"Productions with the same left part have disjoint sets of choices, The grammar is LL(1) grammar."<<endl;

        make_pre_table();

        save_information();

        // 输出相关信息
        // print_first();
        // print_follow();
        // print_select();
        // print_select_action();
        // print_pre_table();

        return true;
    }
    else{
        cout<<"The grammar isn't LL(1) grammar."<<endl;
        return false;
    }
}


// 语法分析的主函数
bool grammar_analyze(string grammar){
    if (creat_pre_table(grammar)) { // 如果根据文法创建预测表的话就调用这个函数
    // if(read_pre_table()){ // 如果之前已经用文法创建好了预测表，那么只需要读入就可以
        make_pre_table_action();        
        // print_pre_table(pre_table_action);// 输出带动作的预测分析表
        // print_pre_table(pre_table); // 输出不带动作的预测分析表
        string file_name = "../process_result/Analysis_process_LL1.txt";

        //open file ,将分析过程写入这个文件
        ofstream file;
        file.open(file_name,ios::out);
        if(!file.is_open())
        {
            cout<<"cannot open the file"<<endl;
        }

        bool if_grammar = check_grammar(&file);// 判断输入的语句是否合法

        file.close();//关闭文件

        if (if_grammar) return true;
        else return false;

    }
    else return false;
    
}