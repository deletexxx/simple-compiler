
#ifndef LEXICAL_H
#define LEXICAL_H

#include<iostream>
#include<fstream>
#include<string>
#include<vector>
using namespace std;

bool is_letter(char a);
bool is_number(char a);
void reset_state(string* s, int* state);
int is_keyword(string word);
int is_symbol(string word);
int state_to_code(string s);
int state_change(int state, char c);
bool scan(ifstream *code_file, ofstream *token_file);
void print_token();
bool lexical_analysis(string file);

#endif
