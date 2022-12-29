// #include<iostream>
// #include<fstream>
// #include<string>
// #include<vector>
// using namespace std;
#include"../head/lexical.h"

//关键字
static string keywords[29]={
    "main", "void", "int", "char", "float", "double", "long", "short", 
    "bool", "if", "else", "do", "while", "for", "break", "continue", 
    "cin", "cout", "const", "return", "true", "false", "using", "namespace", 
    "class", "public", "private", "new", "delete"
};

//符号
static string symbol[40]{
    "+"  ,"-"  ,"*"  ,"/"  ,"++"  ,"--"  ,">"  ,"<"  ,"="  ,"=="  ,
    "!="  ,">="  ,"<="  ,">>"  ,"<<"  ,"&"  ,"|" ,"&&" ,"||", ";",
    ","  ,":"  ,"!"  ,"?"  ,"%"  ,"#"  ,"."  ,"^","(",")","{","}",
    "[","]","\'", "\"", "//", "/*","*/","\\"
};

vector<pair<string,int>> my_token;// 保存token序列


//判断是否是字母
bool is_letter(char a){
    if( (a>=65 && a<=90) || (a>=97 && a<=122)) 
        return true;
    else 
        return false;
}

//判断是否是数字
bool is_number(char a){
    if(a>=48 && a<=57)
        return true;
    else 
        return false;
}

//复位，将状态和token保存的字符串
void reset_state(string* s, int* state){
    *s = "";
    *state = 1;
}

//判断是否是关键字
int is_keyword(string word){//如果是关键字，返回关键字的种别码否则返回0

    for(int i=0; i<29; ++i){
        if(word == keywords[i]){
            return i+1;
        }
    }
    return 30;//是标识符
}

//判断是否是字符
int is_symbol(string word){//如果是运算符/界符，返回他们的种别码，否则返回0
    for(int i=0; i<40; ++i){
        if(word == symbol[i]){
            return i+32;
        }
    }
    return 0;
}

//返回对应的种别码
int state_to_code(string s){
    int code = 0;
    if(is_letter(s[0]) || s[0]=='_'){
        code = is_keyword(s);
    }
    else if(is_number(s[0])){
        for(int i=1;i<s.size();++i){
            if(s[i]=='.')   return 71;//简单判断,如果有小数点就作为浮点数型常量
        }
        code = 72;//如果没有就作为整形常量
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

//自动机，进行状态转换，输入当前的状态以及读到的字符，返回转变后的字符
int state_change(int state, char c){
    if(state == 1){     //起点为1
        if(c == ' ' || c == '\n'){
            return 1;   //继续读取
        }
        else if(is_letter(c) || c == '_'){//字符串
            return 2;
        }
        else if(is_number(c)){            //数字
            return 3;
        }
        else if(c=='?' || c=='%' || c=='#' || c=='.' || c==',' || c==':' || c==';' || c=='^'){
            return 35;                    //35是一个默认的状态，这个状态之后一定是0状态
        }
        else if(c=='(' || c==')' || c=='{' || c=='}' || c=='[' || c==']'){//界符
            return 35;
        }
        else{
            switch (c)//可以由状态转换图看到
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
    else{//其他状态的转换
        switch(state){
            case 2://生成字符串
                if(is_letter(c) || is_number(c) || c == '_'){
                    return 2;      
                }
                else{
                    return 0;
                }
            case 3://生成数字
                if(is_number(c)){
                    return 3;
                }
                else if(c == '.'){
                    return 4;
                }
                else{
                    return 0;
                }
            case 4://生成小数
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
                else return -1;//单引号之后只能读取一个字符，如果在状态6下仍没有右单引号，出错
            case 7:
                if(c == '\"')   return 8;
                else if(c == '\\') return 37;//读到一个转义字符，将其后的字符依旧读入，防止转义符后的双引号造成混淆
                else if(c == '\n') return -1;//一行结束，仍没有右引号，词法错误
                else  return 7;              //继续读取引号内的字符串型常量
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

bool scan(ifstream *code_file, ofstream *token_file){//扫描文件
    int state = 1;//初始状态
    int before_state = 0;
    char c;
    string token;
    int code;     //种别码
    bool remain = false;//用来判断上一次读到字符是否还可用，即本次状态转换时是否还需要读取一个字符

    while(remain || (c=(*code_file).get())!=EOF){//如果上一次重启状态时读取到的字符可用，就不再读取数据了
        remain = false;//下一次一定是需要读取字符

        before_state = state;
        state = state_change(state, c);//改变状态

        //根据现有状态做判断
        if(state > 1){//如果是1说明需要继续读取后续字符来转换状态

            //读取引号内容的特殊情况
            if(state == 5 || (state == 7 && before_state==1)){//收到左双引号/单引号的情况，将引号单独作为一词输出，将引号内的内容作为常量
                token = token + c;
                code = state_to_code(token);
                //将得到的token序列写入文件
                *token_file << "< " << token << " , " << code << " >" << endl;
                my_token.push_back({token,code});
                token = "";//接下来读取引号里的内容，因此清空字符串
            }
            else if(state == 9 || state == 8){//如果到了状态9/8，说明收到了右单双引号，将收到的字符常量先输出，再将右单引号作为一个token
                code = 31;//字符型常量
                //写入文件
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
        else if(state == 0){ //状态为0,需要重新读取下一个token序列了
            code = state_to_code(token);//判断本次字符的种别码
            //解决注释的问题(不读入)
            if(code == 68){//遇到//的情况
                string temp;
                getline(*code_file,temp);//跳过这注释了的一行
            }
            else if(code == 69){//左注释符出现
                //不断读取字符,直到扫描到有右注释符*/出现
                char last_char = (*code_file).get();
                char now_char = last_char;
                while(now_char!=EOF){
                    last_char = now_char;
                    now_char = (*code_file).get();
                    if(now_char == '/' && last_char == '*') break;
                }
                //如果直到文件结束都没有右注释符出现,说明出错
                if(now_char == EOF){
                    cout<<"ERROR! missing right comment"<<endl;
                    return false;
                }
            }
            else{
                //如果收到的字符不是注释符,就将得到的token序列写入文件
                *token_file << "< " << token << " , " << code << " >" << endl;
                my_token.push_back({token,code});
                //判断此次"多"读取的字符是否是空格/换行,如果不是(说明可能有意义),下一个状态转换时就使用这个字符,不再重新读取
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
    // 词法分析
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
