#include"LR0.h"

pair<string,string> grammar;// 单个语法
vector< pair<string, string> > inclusive_grammar;// 整个扩展语法

// Action表
map<int, map<string, pair<char, int>> > action;// 表示从状态int到一个终结符，指向一个状态/归约，bool如果是true表示指向的是状态，如果是false表示指向的是reduce，之后的int就表示指向的是什么转换状态或归约式

// GOTO表
map<int, map<string, int> > GOTO;

// 非终结符和终结符
set<string> non_term,terminator;

void init(string file_name ="./LR0.txt"){
    ifstream file;
    file.open(file_name, ios::in);
    if(!file.is_open()){
        cout<<"cannot open the grammar file"<<endl;
    }

    string left, right;
    while(!file.eof()){
        file >> left >> right;
        inclusive_grammar.push_back({left, right});
    }

    // 输出读入的表达式
    // for(int i=0;i<inclusive_grammar.size();++i){
    //     cout<<inclusive_grammar[i].first<<"->"<<inclusive_grammar[i].second<<endl;
    // }

}

void read_table(string file_name = "./LR0_table.txt"){
    ifstream table_file;
    table_file.open(file_name, ios::in);
    if(!table_file.is_open()){
        cout<<"cannot open the table file"<<endl;
    }

    int num;
    string temp;
    table_file >> num; // 读入非终结符的数量并读取非终结符
    for(int i=0; i<num; ++i){
        table_file >> temp;
        non_term.insert(temp);
    }
    table_file >> num; // 读入终结符的数量并读取终结符
    for(int i=0; i<num; ++i){
        table_file >> temp;
        terminator.insert(temp);
    }

    table_file >> num; // 读入状态数目
    int n;// 用来读取每个状态中能有多少种情况
    char if_S;// 读入转换后的是状态还是要进行归约
    int after_S;// 读入转移之后的状态
    // 开始读入action表
    for(int i=0; i<num; ++i){
        table_file >> n;
        for(int j=0; j<n; ++j){
            table_file >> temp >> if_S >> after_S;
            action[i][temp] = {if_S, after_S};
        }
    }
    // 开始读入GOTO表
    // after_S 用来读入归约后的状态
    string reduce_S;// 规约后的状态，GOTO表的横行
    for(int i=0; i<num; ++i){
        table_file >> n;
        for(int j=0; j<n; ++j){
            table_file >> reduce_S >> after_S;
            GOTO[i][reduce_S] = after_S;
        }
    }

    // print_table(num);
}

void print_table(int n){
    
    for(auto it = terminator.begin(); it!=terminator.end(); ++it){
        cout << "\t" << *it ;
    }
    cout << "\t" << "|" << "\t";
    for(auto it = non_term.begin(); it!=non_term.end(); ++it){
        if(*it != "A'")
        cout << *it << "\t" ;
    }
    cout<<endl;

    for(int i=0; i<n ;++i){
        cout << i << "\t";
        for(auto it = terminator.begin(); it!=terminator.end(); ++it){
            if(action[i][*it].first){
                cout << action[i][*it].first << action[i][*it].second ;
            }
            cout << "\t";
        }
        cout << "|";
        for(auto it = non_term.begin(); it!=non_term.end(); ++it){
            cout << "\t";
            if(GOTO[i][*it] && (*it) != "A'")
                cout << GOTO[i][*it];
        }
        cout << endl;
    }
}

void print_stack_string(stack<string> S, ofstream *file){
    vector<string> arr;
    string e;
    while(!S.empty()){
        e = S.top();
        S.pop();
        arr.insert(arr.begin(),1,e);
    }
    for(int i=0; i<arr.size(); ++i)
        *file << arr[i] ;

    *file << "\t\t\t";
}

void print_stack_int(stack<int> S, ofstream *file){
    vector<int> arr;
    int e;
    while(!S.empty()){
        e = S.top();
        S.pop();
        arr.insert(arr.begin(),1,e);
    }
    for(int i=0; i<arr.size(); ++i)
        *file << arr[i] ;

    *file << "\t\t\t";
}

bool process(string file_name = "./analysis_process_LR0.txt"){
    // 将分析过程写入这个文件中
    ofstream file;
    file.open(file_name, ios::out);
    if(!file.is_open()){
        cout<<"cannot open the analysis_process_LR0 file"<<endl;
    }

    int n;
    cout << "输入查询的语句由多少个终结符组成:";
    cin >> n;
    vector<string> sentence;
    cout << "输入查询的语句(不同终结符之间使用空格隔开): ";// 是因为在写LL1文法的时候发现如果终结符/非终结符只使用单个字符是完全不够的，使用字符串来表示，如果是从token序列中读到的话就已经将每个终结符隔开了，就不用像这里输入需要在终结符之间使用空格了
    string temp;// 暂时存储读到的终结符串
    for(int i=0;i<n;++i){
        cin >> temp;
        sentence.push_back(temp);
    }
    file << "步骤\t\t\t" << "状态栈\t\t\t" << "符号栈\t\t\t" << "输入串\t\t\t" << "ACTION\t\t\t" << "GOTO" << endl;

    sentence.push_back("#");// 输入串
    stack<string> Sym;// 符号栈
    Sym.push("#");
    stack<int> State;// 状态栈
    State.push(0);
    bool flag = false;
    int State_top;// 状态栈栈顶
    string Sym_top;// 符号栈栈顶
    string sentence_top;
    pair<char, int> to;// 做什么动作
    int step = 0;
    while(!flag){
        file << step << "\t\t\t";
        print_stack_int(State, &file);
        print_stack_string(Sym, &file);
        for(int i=0;i<sentence.size();++i){
            file << sentence[i];
        }
        file <<"\t\t\t";
        State_top = State.top();
        Sym_top = Sym.top();
        sentence_top = sentence[0];
        to = action[State_top][sentence_top];
        if(to.first == 'S'){// 如果是状态
            // 状态入栈 移进
                State.push(to.second);
                Sym.push(sentence_top);
                sentence.erase(sentence.begin(), sentence.begin()+1);// 将句子的开头符号转入符号栈中
                file << to.first <<to.second;
        }
        else if(to.first == 'R'){
            // 进行归约
            
            pair<string, string> reduc = inclusive_grammar[to.second];// 要归约成的表达式
            int len = reduc.second.length();
            for(int i=0 ;i<len;++i){
                State.pop();// 状态栈出栈,归约多少个出多少栈
                Sym.pop();  // 符号栈
            }
            Sym.push(reduc.first);// 规约后的非终结符入栈
            State_top = State.top();
            int after_state = GOTO[State_top][reduc.first];
            State.push(after_state);
            file << "\t\t\t\t"<<"R"<<after_state;
        }
        else if(to.first == 'C'){
            flag = true;
            file << "ACC";
        }
        else{
            return false;
        }
        file << endl;
        ++step;
    }
    return true;

}

int main(){
    init();
    read_table();
    if(process()){
        cout<<"语法正确"<<endl;
    }
    else{
        cout<<"语法错误"<<endl;
    }
}