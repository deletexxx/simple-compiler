// #include<iostream>
// #include<fstream>
// #include<string>
// #include<vector>
// using namespace std;
#include"../head/lexical.h"

//�ؼ���
static string keywords[29]={
    "main", "void", "int", "char", "float", "double", "long", "short", 
    "bool", "if", "else", "do", "while", "for", "break", "continue", 
    "cin", "cout", "const", "return", "true", "false", "using", "namespace", 
    "class", "public", "private", "new", "delete"
};

//����
static string symbol[40]{
    "+"  ,"-"  ,"*"  ,"/"  ,"++"  ,"--"  ,">"  ,"<"  ,"="  ,"=="  ,
    "!="  ,">="  ,"<="  ,">>"  ,"<<"  ,"&"  ,"|" ,"&&" ,"||", ";",
    ","  ,":"  ,"!"  ,"?"  ,"%"  ,"#"  ,"."  ,"^","(",")","{","}",
    "[","]","\'", "\"", "//", "/*","*/","\\"
};

vector<pair<string,int>> my_token;// ����token����


//�ж��Ƿ�����ĸ
bool is_letter(char a){
    if( (a>=65 && a<=90) || (a>=97 && a<=122)) 
        return true;
    else 
        return false;
}

//�ж��Ƿ�������
bool is_number(char a){
    if(a>=48 && a<=57)
        return true;
    else 
        return false;
}

//��λ����״̬��token������ַ���
void reset_state(string* s, int* state){
    *s = "";
    *state = 1;
}

//�ж��Ƿ��ǹؼ���
int is_keyword(string word){//����ǹؼ��֣����عؼ��ֵ��ֱ�����򷵻�0

    for(int i=0; i<29; ++i){
        if(word == keywords[i]){
            return i+1;
        }
    }
    return 30;//�Ǳ�ʶ��
}

//�ж��Ƿ����ַ�
int is_symbol(string word){//����������/������������ǵ��ֱ��룬���򷵻�0
    for(int i=0; i<40; ++i){
        if(word == symbol[i]){
            return i+32;
        }
    }
    return 0;
}

//���ض�Ӧ���ֱ���
int state_to_code(string s){
    int code = 0;
    if(is_letter(s[0]) || s[0]=='_'){
        code = is_keyword(s);
    }
    else if(is_number(s[0])){
        for(int i=1;i<s.size();++i){
            if(s[i]=='.')   return 71;//���ж�,�����С�������Ϊ�������ͳ���
        }
        code = 72;//���û�о���Ϊ���γ���
    }
    else if(is_symbol(s)){
        code = is_symbol(s);
    }
    else{
        cout<<"can't find code of the state: "<< s <<endl;
        return 0;
    }
    return code;
}

//�Զ���������״̬ת�������뵱ǰ��״̬�Լ��������ַ�������ת�����ַ�
int state_change(int state, char c){
    if(state == 1){     //���Ϊ1
        if(c == ' ' || c == '\n'){
            return 1;   //������ȡ
        }
        else if(is_letter(c) || c == '_'){//�ַ���
            return 2;
        }
        else if(is_number(c)){            //����
            return 3;
        }
        else if(c=='?' || c=='%' || c=='#' || c=='.' || c==',' || c==':' || c==';' || c=='^'){
            return 35;                    //35��һ��Ĭ�ϵ�״̬�����״̬֮��һ����0״̬
        }
        else if(c=='(' || c==')' || c=='{' || c=='}' || c=='[' || c==']'){//���
            return 35;
        }
        else{
            switch (c)//������״̬ת��ͼ����
            {
            case  '\'':
                return 5;
                break;
            case '\"':
                return 7;
                break;
            case '+':
                return 10;
                break;
            case '-':
                return 12;
                break;
            case '*':
                return 14;
                break;
            case '/':
                return 17;
                break;
            case '>':
                return 20;
                break;
            case '<':
                return 23;
                break;
            case '=':
                return 26;
                break;
            case '!':
                return 28;
                break;
            case '&':
                return 30;
                break;
            case '|':
                return 32;
                break;
            case '\n':
                return 0;
            default:
                return -1;
            }
        }
    }
    else{//����״̬��ת��
        switch(state){
            case 2://�����ַ���
                if(is_letter(c) || is_number(c) || c == '_'){
                    return 2;      
                }
                else{
                    return 0;
                }
            case 3://��������
                if(is_number(c)){
                    return 3;
                }
                else if(c == '.'){
                    return 4;
                }
                else{
                    return 0;
                }
            case 4://����С��
                if(is_number(c)){
                    return 4;
                }
                else{
                    return 0;
                }
            case 5:
                if (c == '\\'){
                    return 36;
                }
                else
                    return 6;
            case 6:
                if(c == '\''){
                    return 9;
                }
                else return -1;//������֮��ֻ�ܶ�ȡһ���ַ��������״̬6����û���ҵ����ţ�����
            case 7:
                if(c == '\"')   return 8;
                else if(c == '\\') return 37;//����һ��ת���ַ����������ַ����ɶ��룬��ֹת������˫������ɻ���
                else if(c == '\n') return -1;//һ�н�������û�������ţ��ʷ�����
                else  return 7;              //������ȡ�����ڵ��ַ����ͳ���
            case 10:
                if(c == '+') return 11;
                else if(c == '=') return 34;
                else return 0;
            case 12:
                if(c == '-') return 13;
                else if(c == '=') return 16;
                else return 0;
            case 14:
                if(c == '/')    return 15;
                else return 0;
            case 17:
                if(c == '/') return 18;
                else if(c == '*') return 19;
                else return 0;
            case 20:
                if(c == '>') return 21;
                else if(c == '=') return 22;
                else return 0;
            case 23:
                if(c == '<') return 24;
                else if(c == '=') return 25;
                else return 0;
            case 26:
                if(c == '=') return 27;
                else return 0;
            case 28:
                if(c == '=') return 29;
                else return 0;
            case 30:
                if(c == '&') return 31;
                else return 0;
            case 32:
                if(c == '|') return 33;
                else return 0;
            case 36:
                return 6;
            case 37:
                return 7;
            default:
                return 0;
        }
    }

}

bool scan(ifstream *code_file, ofstream *token_file){//ɨ���ļ�
    int state = 1;//��ʼ״̬
    int before_state = 0;
    char c;
    string token;
    int code;     //�ֱ���
    bool remain = false;//�����ж���һ�ζ����ַ��Ƿ񻹿��ã�������״̬ת��ʱ�Ƿ���Ҫ��ȡһ���ַ�

    while(remain || (c=(*code_file).get())!=EOF){//�����һ������״̬ʱ��ȡ�����ַ����ã��Ͳ��ٶ�ȡ������
        remain = false;//��һ��һ������Ҫ��ȡ�ַ�

        before_state = state;
        state = state_change(state, c);//�ı�״̬

        //��������״̬���ж�
        if(state > 1){//�����1˵����Ҫ������ȡ�����ַ���ת��״̬

            //��ȡ�������ݵ��������
            if(state == 5 || (state == 7 && before_state==1)){//�յ���˫����/�����ŵ�����������ŵ�����Ϊһ��������������ڵ�������Ϊ����
                token = token + c;
                code = state_to_code(token);
                //���õ���token����д���ļ�
                *token_file << "< " << token << " , " << code << " >" << endl;
                my_token.push_back({token,code});
                token = "";//��������ȡ����������ݣ��������ַ���
            }
            else if(state == 9 || state == 8){//�������״̬9/8��˵���յ����ҵ�˫���ţ����յ����ַ�������������ٽ��ҵ�������Ϊһ��token
                code = 31;//�ַ��ͳ���
                //д���ļ�
                *token_file << "< " << token << " , " << code << " >" << endl;
                my_token.push_back({token,code});
                token = "";
                token = token + c;
            }
            else{
                token = token + c;                
                // cout<<token<<endl;
            }

        }
        else if(state == 0){ //״̬Ϊ0,��Ҫ���¶�ȡ��һ��token������
            code = state_to_code(token);//�жϱ����ַ����ֱ���
            //���ע�͵�����(������)
            if(code == 68){//����//�����
                string temp;
                getline(*code_file,temp);//������ע���˵�һ��
            }
            else if(code == 69){//��ע�ͷ�����
                //���϶�ȡ�ַ�,ֱ��ɨ�赽����ע�ͷ�*/����
                char last_char = (*code_file).get();
                char now_char = last_char;
                while(now_char!=EOF){
                    last_char = now_char;
                    now_char = (*code_file).get();
                    if(now_char == '/' && last_char == '*') break;
                }
                //���ֱ���ļ�������û����ע�ͷ�����,˵������
                if(now_char == EOF){
                    cout<<"ERROR! missing right comment"<<endl;
                    return false;
                }
            }
            else{
                //����յ����ַ�����ע�ͷ�,�ͽ��õ���token����д���ļ�
                *token_file << "< " << token << " , " << code << " >" << endl;
                my_token.push_back({token,code});
                //�жϴ˴�"��"��ȡ���ַ��Ƿ��ǿո�/����,�������(˵������������),��һ��״̬ת��ʱ��ʹ������ַ�,�������¶�ȡ
                if(c != ' ' && c!='\n'){
                    remain = true;
                }
            }
            reset_state(&token, &state);
        }
        else if(state == -1){
            cout<<"ERROR!! "<<endl;
            return false;
        }
    }
    return true;
}

void print_token(){
    cout << "token:"<<endl;
    for(auto it = my_token.begin(); it!=my_token.end();++it){
        cout<<"<"<<(*it).first<<","<<it->second<<">"<<endl;
    }
}

bool lexical_analysis(string file){
    // �ʷ�����
    // string read_file = "C://test//compiler2//demo.cpp";
    // string write_file = "C://test//compiler2//tokens.txt";
    string write_file = "../process_result/g_tokens.txt";

    //open file and read file
    ifstream code_file;
    code_file.open(file,ios::in);
    if(!code_file.is_open())
    {
        cout<<"cannot open the code file"<<endl;
    }

    //write to
    ofstream token_file;
    token_file.open(write_file, ios::out);
    if(!token_file.is_open()){
        cout<<"cannot open the token_file"<<endl;
    }

    bool if_success;
    
    if(scan(&code_file, &token_file)){
        if_success = true;
    }
    else{
        if_success = false;
    }

    //close file
    code_file.close();
    token_file.close();

    // print_token();

    return if_success;


}
