#ifndef SYMBOL_H
#define SYMBOL_H
#include <QTextBrowser>
#include <QString>
#include <QDebug>
#include <iostream>
#include <string>

using namespace std;
/*文法符号类*/
#define VN   0
#define VT   1
#define EPSILON   2
#define END  3
#define symEps Symbol(EPSILON,"[z]")
#define symEnd Symbol(END,"[#]")
#define symS0 Symbol(VN,"<S0>")
#define strstart string("Program")
#define strend string("[#]")

#define QTmessage_clear 0
#define QTmessage_append 1

void Mess_to_control(QString mess,QTextBrowser*which,const int opt);

class Symbol {
public:
    int symbol_type;//符号类型
    string content;//存放内容
    friend bool operator==(const Symbol& s1, const Symbol& s2);
    friend bool operator!=(const Symbol& s1, const Symbol& s2);
    friend bool operator < (const Symbol& s1, const Symbol& s2);
    friend ostream& operator<<(ostream& out, const Symbol& s);
    Symbol();
    Symbol(const int& st, const string& c);
    Symbol(const Symbol& s);
};
#endif 

