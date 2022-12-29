#include"../head/lexical.h"
#include"../head/LL1.h"
#include"../head/symbol.h"
#include"../head/semantic.h"
#include"../head/generate_code.h"

extern vector<pair<string,int>> my_token;

int main(){
    cout << "Lexical analysis:" << endl << "\t";
    if( lexical_analysis("../assert/g_test.txt") ){
        cout<<"Word analysis successfully completed!"<<endl;
    }
    else{
        cout<<"Word analysis failed..."<<endl;
    }


    cout << endl << "Gramma analysis:" << endl << "\t";
    if(grammar_analyze("../assert/LL1.txt")){
        cout<<"\tgrammar is true"<<endl;
    }
    else{
        cout<<"\tgrammar is false"<<endl;
    }

    cout << endl << "generate symbol table:" << endl << "\t";
    if(GET_SYMTABLE()){
        cout << "Symbol table built successfully" << endl;
    }
    else{
        cout<<"\tSymbol table build failed..."<<endl;
    }

    cout << endl << "Semantic Analysis:" << endl << "\t";
    if(generate_quat()){
        cout << "Semantic analysis completed" << endl;
    }
    else{
        cout << "Semantic analysis failed" << endl;
    }

    cout << endl << "start to generate code:" << endl << "\t";
    if(generate_object_code()){
        cout << "Object code has been successfully generated, in folder \"../process_result/object_code.asm\""<<endl;
    }
    else{
        cout << "Object code generation failed." << endl;
    }
    
    // 生成四元式，获取活跃信息后打印符号表
    PRINT_SYM_TABLE("../process_result/symbol_table.txt");
}