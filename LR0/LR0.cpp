#include"LR0.h"

pair<string,string> grammar;// �����﷨
vector< pair<string, string> > inclusive_grammar;// ������չ�﷨

// Action��
map<int, map<string, pair<char, int>> > action;// ��ʾ��״̬int��һ���ս����ָ��һ��״̬/��Լ��bool�����true��ʾָ�����״̬�������false��ʾָ�����reduce��֮���int�ͱ�ʾָ�����ʲôת��״̬���Լʽ

// GOTO��
map<int, map<string, int> > GOTO;

// ���ս�����ս��
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

    // �������ı��ʽ
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
    table_file >> num; // ������ս������������ȡ���ս��
    for(int i=0; i<num; ++i){
        table_file >> temp;
        non_term.insert(temp);
    }
    table_file >> num; // �����ս������������ȡ�ս��
    for(int i=0; i<num; ++i){
        table_file >> temp;
        terminator.insert(temp);
    }

    table_file >> num; // ����״̬��Ŀ
    int n;// ������ȡÿ��״̬�����ж��������
    char if_S;// ����ת�������״̬����Ҫ���й�Լ
    int after_S;// ����ת��֮���״̬
    // ��ʼ����action��
    for(int i=0; i<num; ++i){
        table_file >> n;
        for(int j=0; j<n; ++j){
            table_file >> temp >> if_S >> after_S;
            action[i][temp] = {if_S, after_S};
        }
    }
    // ��ʼ����GOTO��
    // after_S ���������Լ���״̬
    string reduce_S;// ��Լ���״̬��GOTO��ĺ���
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
    // ����������д������ļ���
    ofstream file;
    file.open(file_name, ios::out);
    if(!file.is_open()){
        cout<<"cannot open the analysis_process_LR0 file"<<endl;
    }

    int n;
    cout << "�����ѯ������ɶ��ٸ��ս�����:";
    cin >> n;
    vector<string> sentence;
    cout << "�����ѯ�����(��ͬ�ս��֮��ʹ�ÿո����): ";// ����Ϊ��дLL1�ķ���ʱ��������ս��/���ս��ֻʹ�õ����ַ�����ȫ�����ģ�ʹ���ַ�������ʾ������Ǵ�token�����ж����Ļ����Ѿ���ÿ���ս�������ˣ��Ͳ���������������Ҫ���ս��֮��ʹ�ÿո���
    string temp;// ��ʱ�洢�������ս����
    for(int i=0;i<n;++i){
        cin >> temp;
        sentence.push_back(temp);
    }
    file << "����\t\t\t" << "״̬ջ\t\t\t" << "����ջ\t\t\t" << "���봮\t\t\t" << "ACTION\t\t\t" << "GOTO" << endl;

    sentence.push_back("#");// ���봮
    stack<string> Sym;// ����ջ
    Sym.push("#");
    stack<int> State;// ״̬ջ
    State.push(0);
    bool flag = false;
    int State_top;// ״̬ջջ��
    string Sym_top;// ����ջջ��
    string sentence_top;
    pair<char, int> to;// ��ʲô����
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
        if(to.first == 'S'){// �����״̬
            // ״̬��ջ �ƽ�
                State.push(to.second);
                Sym.push(sentence_top);
                sentence.erase(sentence.begin(), sentence.begin()+1);// �����ӵĿ�ͷ����ת�����ջ��
                file << to.first <<to.second;
        }
        else if(to.first == 'R'){
            // ���й�Լ
            
            pair<string, string> reduc = inclusive_grammar[to.second];// Ҫ��Լ�ɵı��ʽ
            int len = reduc.second.length();
            for(int i=0 ;i<len;++i){
                State.pop();// ״̬ջ��ջ,��Լ���ٸ�������ջ
                Sym.pop();  // ����ջ
            }
            Sym.push(reduc.first);// ��Լ��ķ��ս����ջ
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
        cout<<"�﷨��ȷ"<<endl;
    }
    else{
        cout<<"�﷨����"<<endl;
    }
}