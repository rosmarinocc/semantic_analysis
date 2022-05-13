﻿#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif
#include "Semantic.h"
#include <sstream>
#include<QDebug>

int str2num(string s)
{
    int num;
    stringstream ss(s);
    ss >> num;
    return num;
}
/* SemanticSymtable类*/
/*构造函数*/
SemanticSymtable::SemanticSymtable(const int t_type, const string& t_name) {
    this->type = t_type;
    this->name = t_name;
}

/*查询函数*/
int SemanticSymtable::findSym(const string& id_name) {
    for (int i = 0; i < Symtable.size(); ++i) {
        if (Symtable[i].name == id_name)
            return i;
    }
    return -1;//找不到!
}

/*插入函数*/
int SemanticSymtable::insertSym(const Identifier& id) {
    if (findSym(id.name) != -1)
        return -1;//无插入,已存在

    Symtable.push_back(id);
    return Symtable.size() - 1;//最后
}


/*SemanticAnalyzer类*/
/*构造函数*/
SemanticAnalyzer::SemanticAnalyzer() {
    nextQ_index = 1;//从1开始,0为(j,-,-,main_addr)
    main_index = -1;
    backpatching_level = 0;//无需回填
    cnt = 0;
    //全局符号表
    allTable.push_back(SemanticSymtable(GLOBAL, "全局表"));
    allTable.push_back(SemanticSymtable(TEMP, "临时变量表"));
    cur_tableStack.push_back(0);
}

/*插入语义分析过程中符号流符号*/
void SemanticAnalyzer::insertSymbol(const SemanticSymbol& sym) {
    symbolList.push_back(sym);
}

/*打印四元式表*/
void SemanticAnalyzer::printQuadruple(const string outpath) {
    ofstream fout(outpath, ios::out);
    fout << "*****Original Code*****\n";
    for (auto x : quadruple)
        if (x.index >= 0) {
            fout << x.index << " ("
                << x.op << ',' << x.arg1 << ',' << x.arg2 << ',' << x.res
                << ")" << endl;
        }
       /* else if (x.index == FRAG_START) {
            fout << "FRAG_START"<< endl;
        }
        else {
            fout << "FRAG_END" << endl;
        }*/
    fout << "\n\n\n*****Optimized Code*****\n";
    for (auto x : opt)
        if (x.index >= 0) {
            fout << x.index << " ("
                << x.op << ',' << x.arg1 << ',' << x.arg2 << ',' << x.res
                << ")" << endl;
        }
        /*else if (x.index == FRAG_START) {
            fout << "FRAG_START" << endl;
        }
        else {
            fout << "FRAG_END" << endl;
        }*/
}

void SemanticAnalyzer::optimize()
{
    for (auto a : quadruple)
        opt.push_back(a);
    opt_remove_public_exp_useless_code();
    opt_kk();
    for (int i = 0; i < opt.size(); i++)
    {
        opt[i].index = i;
    }
    opt.pop_back();
}

void SemanticAnalyzer::opt_kk()
{
    Quadruple cur_q, front_q, back_q;
    int cur_step = 0, front_search = 0, back_search = 0;
    int max = opt.size();
    while (cur_step < max)
    {
        cur_q = opt[cur_step];
        if (cur_q.res[0] != 'T'&&cur_q.op=="=")//cur_q是赋值临时变量的四元式，不严谨
        {
            front_search = cur_step - 1;
            while (front_search >= 0)
            {
                front_q = opt[front_search];
                //有别的临时变量和当前临时变量计算式一致
                if (front_q.res==cur_q.arg1)
                {
                    opt[front_search].res = cur_q.res;
                    //删除当前临时变量
                    opt.erase(opt.begin() + cur_step);
                    break;
                }
                front_search--;
            }
        }
        cur_step++;
    }
}

//消除块内公共子表达式和无效中间代码
void SemanticAnalyzer::opt_remove_public_exp_useless_code()
{
    int cur_step = 0, front_search = 0, back_search = 0;
    int max = opt.size();
    vector<string>used_temp;
    Quadruple cur_q,front_q,back_q;
    while (cur_step < max)
    {
        cur_q = opt[cur_step];
        if (cur_q.res[0]=='T')//cur_q是赋值临时变量的四元式，不严谨
        {
            front_search = cur_step - 1;
            while (front_search >= 0)
            {
                front_q = opt[front_search];
                //有别的临时变量和当前临时变量计算式一致
                if (front_q.op == cur_q.op && front_q.res[0] == 'T' && front_q.arg1 == cur_q.arg1 && front_q.arg2 == cur_q.arg2)
                {
                    //搜索块内所有用当前临时变量赋值的语句，替换
                    for (back_search = cur_step + 1, back_q = opt[back_search]; back_q.index != FRAG_END; )
                    {
                        if (back_q.arg1 == cur_q.res)
                        {
                            opt[back_search].arg1 = front_q.res;
                        }
                        if (back_q.arg2 == cur_q.res)
                        {
                            opt[back_search].arg2 = front_q.res;
                        }
                        back_search++;
                        if (back_search > opt.size())
                            break;
                        back_q = opt[back_search];
                        int count = (back_q.index == FRAG_START);
                        while(count)
                        {
                            back_search++;
                            if (back_search > opt.size())
                                break;
                            back_q = opt[back_search];
                            count += (back_q.index == FRAG_START);
                            count -= (back_q.index == FRAG_END);
                        }
                        if (back_search > opt.size())
                            break;
                    }
                    //删除当前临时变量
                    opt.erase(opt.begin() + cur_step);
                    break;
                }
                /*else if (front_q.res[0] == 'T')
                {
                    modified_temp.push_back(front_q.res);
                }*/
                front_search--;
            }
        }
        //记录实际用到的临时变量
        if (cur_q.op[0] == 'j' || (cur_q.op == "=" && cur_q.res[0] != 'T'))
        {
            if (cur_q.arg1[0] == 'T')
                used_temp.push_back(cur_q.arg1);
            if (cur_q.arg2[0] == 'T')
                used_temp.push_back(cur_q.arg2);
        }
        cur_step++;
    }
    cur_step = 0;
    max = opt.size();
    //删去无用变量
    while (cur_step < max)
    {
        if (cur_q.res[0] == 'T')
        {
            bool used = false;
            for (auto x : used_temp)
                if (x == cur_q.res)
                {
                    used = 1;
                    break;
                }
            if(!used)
                opt.erase(opt.begin() + cur_step);
        }
        cur_step++;
    }
}




/*分析主体函数*/
void SemanticAnalyzer::semanticANL(const Production& pro) {
    //<Program> -> <DeclareString>
    if (pro.Left.content == "<Program>")
        SemanProd_Program(pro);
    //<DeclareString-option> -> <ParameterDeclare> [ID] [SEMI] | <FunctionDeclare> <FunDec> <Block>
    else if (pro.Left.content == "<DeclareString-option>")
        SemanProd_DecOption(pro);
    /*<ParameterDeclare> ->[INT]*/
    else if (pro.Left.content == "<ParameterDeclare>")
        SemanProd_ParaDec(pro);
    /*<FunctionDeclare> ->[VOID] | [INT]*/
    else if (pro.Left.content == "<FunctionDeclare>")
        SemanProd_FunctionDeclare(pro);
    /*<FunDec> ->[ID] <CreateFunTable_m>[LPAREN] <VarList>[RPAREN]*/
    else if (pro.Left.content == "<FunDec>")
        SemanProd_FunDec(pro);
    /*<CreateFunTable_m> ->[z]*/
    else if (pro.Left.content == "<CreateFunTable_m>")
        SemanProd_CreateFunTable_m(pro);
    //<ParamDec> -> <ParameterDeclare>[ID]
    else if (pro.Left.content == "<ParamDec>")
        SemanProd_ParamDec(pro);
    /*<Block> ->[LBBRACKET] <DefList> <StmtList>[RBBRACKET]*/
    else if (pro.Left.content == "<Block>")
        SemanProd_Block(pro);
    /*<Def> -> <ParameterDeclare>[ID][SEMI]*/
    else if (pro.Left.content == "<Def>")
        SemanProd_Def(pro);
    /*<AssignStmt> ->[ID][ASSIGN] <Exp>*/
    else if (pro.Left.content == "<AssignStmt>")
        SemanProd_AssignStmt(pro);
    /*<Exp> -> <AddSubExp> | <Exp> <Relop> <AddSubExp>*/
    else if (pro.Left.content == "<Exp>")
        SemanProd_Exp(pro);
    /*<AddSubExp> -> <Item> | <Item>[PLUS] <Item> | <Item>[SUB] <Item>*/
    else if (pro.Left.content == "<AddSubExp>")
        SemanProd_AddSubExp(pro);
    /*<Item> -> <Factor> | <Factor>[MUL] <Factor> | <Factor>[DIV] <Factor>*/
    else if (pro.Left.content == "<Item>")
        SemanProd_Item(pro);
    /*<Factor> ->[NUM] | [LPAREN] <Exp>[RPAREN] | [ID] | <CallStmt>*/
    else if (pro.Left.content == "<Factor>")
        SemanProd_Factor(pro);
    /*<CallStmt> ->[ID][LPAREN] <CallFunCheck> <Args>[RPAREN]*/
    else if (pro.Left.content == "<CallStmt>")
        SemanProd_CallStmt(pro);
    /*<CallFunCheck> ->[z]*/
    else if (pro.Left.content == "<CallFunCheck>")
        SemanProd_CallFunCheck(pro);
    /*<Args> -> <Exp> [COMMA] <Args> | <Exp> | [z]*/
    else if (pro.Left.content == "<Args>")
        SemanProd_Args(pro);
    /*<ReturnStmt> ->[RETURN] <Exp> | [RETURN]*/
    else if (pro.Left.content == "<ReturnStmt>")
        SemanProd_ReturnStmt(pro);
    /*<Relop> ->[GT] | [LT] | [GE] | [LE] | [EQ] | [NQ]*/
    else if (pro.Left.content == "<Relop>")
        SemanProd_Relop(pro);
    /*<IfStmt> ->[IF] <IfStmt_m1>[LPAREN] <Exp>[RPAREN] <IfStmt_m2> <Block> <IfNext>*/
    else if (pro.Left.content == "<IfStmt>")
        SemanProd_IfStmt(pro);
    /*<IfStmt_m1> ->[z]*/
    else if (pro.Left.content == "<IfStmt_m1>")
        SemanProd_IfStmt_m1(pro);
    /*<IfStmt_m2> ->[z]*/
    else if (pro.Left.content == "<IfStmt_m2>")
        SemanProd_IfStmt_m2(pro);
    /*<IfNext> ->[z] | <IfStmt_next>[ELSE] <Block>*/
    else if (pro.Left.content == "<IfNext>")
        SemanProd_IfNext(pro);
    /*<IfStmt_next>->[z]*/
    else if (pro.Left.content == "<IfStmt_next>")
        SemanProd_IfStmt_next(pro);
    /*<WhileStmt> ->[WHILE] <WhileStmt_m1>[LPAREN] <Exp>[RPAREN] <WhileStmt_m2> <Block>*/
    else if (pro.Left.content == "<WhileStmt>")
        SemanProd_WhileStmt(pro);
    /*<WhileStmt_m1> ->[z]*/
    else if (pro.Left.content == "<WhileStmt_m1>")
        SemanProd_WhileStmt_m1(pro);
    /*<WhileStmt_m2> ->[z]*/
    else if (pro.Left.content == "<WhileStmt_m2>")
        SemanProd_WhileStmt_m2(pro);
    else {
        if (pro.Right[0] != symEps) {
            int count = pro.Right.size();
            while (count--)
                symbolList.pop_back();
        }
        symbolList.push_back({ {-1,"",pro.Left.content},-1, -1 });
    }
}

void SemanticAnalyzer::SemanProd_Program(const Production& pro)
{
    //如果没有定义main函数，则报错
    if (main_index == -1) {
        errorInfo.push_back("[ERROR-10000] : 未定义main函数");
    }
    int count = pro.Right.size();
    while (count--) {
        symbolList.pop_back();
    }
    //在最前面加入四元式
    quadruple.insert(quadruple.begin(), { 0, "j","-" , "-", to_string(main_index) });

    symbolList.push_back({ {  -1,"",pro.Left.content}, -1, -1 });
}

//<DeclareString-option> -> <ParameterDeclare> [ID] [SEMI] | <FunctionDeclare> <FunDec> <Block>
void SemanticAnalyzer::SemanProd_DecOption(const Production& pro)
{
    //如果是定义变量
    if (pro.Right.size() == 3) {
        // <ParameterDeclare> [ID] [SEMI]
        SemanticSymbol pardec = symbolList[0];//int
        SemanticSymbol ident = symbolList[1];//变量名

        //用于判断该变量是否在当前层已经定义
        bool existed = false;
        SemanticSymtable current_table = allTable[cur_tableStack.back()];
        if (current_table.findSym(ident.toke.content) != -1) {
            errorInfo.push_back("[ERROR-10001] : " + to_string(ident.toke.line) + "行，变量" + ident.toke.content +"重定义");
        }

        //将这一变量加入table
        Identifier variable;
        variable.name = ident.toke.content;//变量名
        variable.type = VAR;
        variable.specialType = pardec.toke.content;//int

        //加入table
        allTable[cur_tableStack.back()].insertSym(variable);

        //symbolList更新
        int count = pro.Right.size();
        while (count--)
            symbolList.pop_back();

        symbolList.push_back({{ident.toke.line,ident.toke.content,pro.Left.content }, cur_tableStack.back(), int(allTable[cur_tableStack.back()].Symtable.size() - 1)});
    }
    //如果是定义函数
    else {
        SemanticSymbol ident = symbolList[symbolList.size() - 2];

        //需要退出作用域
        cur_tableStack.pop_back();
        //更新symbolList
        int count = pro.Right.size();
        while (count--)
            symbolList.pop_back();
        symbolList.push_back({ {ident.toke.line,ident.toke.content,pro.Left.content },ident.table_index,ident.index });
    }
}
/**********************************************************************************/
void SemanticAnalyzer::SemanProd_ParaDec(const Production& pro)
{
    //symbol_list的最后一个是int
    SemanticSymbol specifier = symbolList.back();
    int count = pro.Right.size();
    while (count--)
        symbolList.pop_back();
    symbolList.push_back({{specifier.toke.line,specifier.toke.content ,pro.Left.content}, -1, -1,specifier.is_num});
}
void SemanticAnalyzer::SemanProd_FunctionDeclare(const Production& pro)
{
    //symbol_list的最后一个是int/void
    SemanticSymbol specifier = symbolList.back();
    int count = pro.Right.size();
    while (count--)
        symbolList.pop_back();
    symbolList.push_back({ {specifier.toke.line,specifier.toke.content ,pro.Left.content}, -1, -1 ,specifier.is_num});
}
/**********************************************************************************/

void SemanticAnalyzer::SemanProd_FunDec(const Production& pro)
{
    //symbol_list的CreateFunTable_m记录了table信息
    SemanticSymbol specifier = symbolList[symbolList.size() - 4];
    int count = pro.Right.size();
    while (count--)
        symbolList.pop_back();
    symbolList.push_back({ {specifier.toke.line,specifier.toke.content ,pro.Left.content} ,specifier.table_index,specifier.index,specifier.is_num});
}

void SemanticAnalyzer::SemanProd_CreateFunTable_m(const Production& pro)
{
    //创建函数表
//此时symbolList最后一个符号为函数名，倒数第二个为函数返回值
    SemanticSymbol ident = symbolList.back();
    SemanticSymbol specifier = symbolList[symbolList.size() - 2];

    //首先在全局的table判断函数名是否重定义
    if (allTable[0].findSym(ident.toke.content) != -1) {
        errorInfo.push_back("[ERROR-10002] :" + to_string(ident.toke.line) + "行，函数" + ident.toke.content +"重定义" );
    }

    //创建函数表
    allTable.push_back(SemanticSymtable(FUNC, ident.toke.content));
    //在全局符号表创建当前函数的符号项（这里参数个数和入口地址会进行回填）
    allTable[0].insertSym({ FUNC,specifier.toke.content,ident.toke.content,0,0,int(allTable.size() - 1) });

    //函数表压入栈
    cur_tableStack.push_back(allTable.size() - 1);

    //返回值
    Identifier return_value;
    return_value.type = RET;
    return_value.name = allTable.back().name + "_return_value";
    return_value.specialType = specifier.toke.content;
    //如果为main函数，则进行记录
    if (ident.toke.content == "main")
        main_index = nextQ_index;
    //加入四元式
    quadruple.push_back({ nextQ_index++ , ident.toke.content,"-","-" ,"-" });

    //函数块开始
    quadruple.push_back({ FRAG_START , "proc","-","-" ,"-" });

    //向函数表中加入返回变量
    allTable[cur_tableStack.back()].insertSym(return_value);
    //空串不需要pop
    //进行pushback
    symbolList.push_back({ {ident.toke.line,ident.toke.content,pro.Left.content},0,int(allTable[0].Symtable.size() - 1),ident.is_num });
}

void SemanticAnalyzer::SemanProd_ParamDec(const Production& pro)
{
    //symbolList最后一个为变量名，倒数第二个为类型
    SemanticSymbol ident = symbolList.back();
    SemanticSymbol specifier = symbolList[symbolList.size() - 2];
    //当前函数表
    SemanticSymtable& function_table = allTable[cur_tableStack.back()];


    //如果已经进行过定义
    if (function_table.findSym(ident.toke.content) != -1) {
        errorInfo.push_back("[ERROR-10003] : " + to_string(ident.toke.line) + "行，函数参数" + ident.toke.content + "重定义");
    }
    //函数表加入形参变量
    int new_position = function_table.insertSym({ VAR ,specifier.toke.content ,ident.toke.content,-1,-1,-1 });
    //当前函数在全局符号中的索引
    int table_position = allTable[0].findSym(function_table.name);
    //形参个数++
    allTable[0].Symtable[table_position].paraNum++;

    //加入四元式
    quadruple.push_back({ nextQ_index++, "defpar","-" , "-", ident.toke.content });

    //symbolList更新
    int count = pro.Right.size();
    while (count--)
        symbolList.pop_back();
    symbolList.push_back({{ ident.toke.line,ident.toke.content,pro.Left.content }, cur_tableStack.back(), new_position,ident.is_num });
}

void SemanticAnalyzer::SemanProd_Block(const Production& pro)
{
    //块结束
    quadruple.push_back({ FRAG_END , "-","-","-" ,"-" });
    //更新symbolList
    int count = pro.Right.size();
    while (count--)
        symbolList.pop_back();
    symbolList.push_back({ { -1, to_string(nextQ_index), pro.Left.content }, -1, -1 });
}

void SemanticAnalyzer::SemanProd_Def(const Production& pro)
{
    //symbolList的倒数第二个、倒数第三个是变量名和类型
    SemanticSymbol ident = symbolList[symbolList.size() - 2];
    SemanticSymbol specifier = symbolList[symbolList.size() - 3];
    SemanticSymtable& current_table = allTable[cur_tableStack.back()];

    //重定义则报错
    if (current_table.findSym(ident.toke.content) != -1)
    {
        errorInfo.push_back("[ERROR-10004] : " +to_string(ident.toke.line)+ "行，变量" + ident.toke.content + "重定义");
    }

    current_table.insertSym({ VAR,specifier.toke.content,ident.toke.content ,-1,-1,-1 });

    int count = pro.Right.size();
    while (count--)
        symbolList.pop_back();
    symbolList.push_back({ {ident.toke.line ,ident.toke.content,pro.Left.content},cur_tableStack.back(),int(allTable[cur_tableStack.back()].Symtable.size() - 1),ident.is_num });
}

void SemanticAnalyzer::SemanProd_AssignStmt(const Production& pro)
{
    //symbolList的倒数第一个、倒数第三个是Exp和变量名
    SemanticSymbol ident = symbolList[symbolList.size() - 3];
    SemanticSymbol exp = symbolList.back();

    //检查id是否存在，不存在则报错
    bool existed = false;
    int table_index = -1, index = -1;
    //从当前层开始向上遍历
    for (int scope_layer = cur_tableStack.size() - 1; scope_layer >= 0; scope_layer--) {
        auto current_table = allTable[cur_tableStack[scope_layer]];
        if ((index = current_table.findSym(ident.toke.content)) != -1) {
            existed = true;
            table_index = cur_tableStack[scope_layer];
            break;
        }
    }
    if (existed == false) {
        errorInfo.push_back("[ERROR-10005] : " +to_string( ident.toke.line)+ "行，变量" + ident.toke.content + "未定义" );
    }

    quadruple.push_back({ nextQ_index++, "=", exp.toke.content, "-", ident.toke.content });

    //更新symbolList
    int count = pro.Right.size();
    while (count--)
        symbolList.pop_back();
    symbolList.push_back({ { ident.toke.line,ident.toke.content ,pro.Left.content },table_index, index,ident.is_num});
}

void SemanticAnalyzer::SemanProd_Exp(const Production& pro)
{
    if (pro.Right.size() == 1) {
        SemanticSymbol exp = symbolList.back();
        //更新symbolList
        int count = pro.Right.size();
        while (count--)
            symbolList.pop_back();
        symbolList.push_back({ {exp.toke.line,exp.toke.content ,pro.Left.content },  exp.table_index, exp.index,exp.is_num});
    }
    else {
        SemanticSymbol sub_exp1 = symbolList[symbolList.size() - 3];
        SemanticSymbol op = symbolList[symbolList.size() - 2];
        SemanticSymbol sub_exp2 = symbolList[symbolList.size() - 1];
        int next_label_num = nextQ_index++;
        string new_tmp_var = "T" + to_string(cnt++);
        quadruple.push_back({ next_label_num, "j" + op.toke.content, sub_exp1.toke.content, sub_exp2.toke.content, to_string(next_label_num + 3) });
        quadruple.push_back({ nextQ_index++, "=", "0", "-", new_tmp_var });
        quadruple.push_back({ nextQ_index++, "j", "-", "-", to_string(next_label_num + 4) });
        quadruple.push_back({ nextQ_index++, "=", "1", "-", new_tmp_var });

        int count = pro.Right.size();
        while (count--)
            symbolList.pop_back();
        symbolList.push_back({ {-1, new_tmp_var ,pro.Left.content},-1,-1 });
    }
}

void SemanticAnalyzer::SemanProd_AddSubExp(const Production& pro)
{
    if (pro.Right.size() == 1) {
        SemanticSymbol exp = symbolList.back();
        //更新symbolList
        int count = pro.Right.size();
        while (count--)
            symbolList.pop_back();
        symbolList.push_back({ {exp.toke.line,exp.toke.content , pro.Left.content },exp.table_index,exp.index,exp.is_num });
    }
    else {
        SemanticSymbol sub_exp1 = symbolList[symbolList.size() - 3];
        SemanticSymbol op = symbolList[symbolList.size() - 2];
        SemanticSymbol sub_exp2 = symbolList[symbolList.size() - 1];
        if (sub_exp1.is_num && sub_exp2.is_num)
        {
            int exp1 = str2num(sub_exp1.toke.content), exp2 = str2num(sub_exp2.toke.content);
            qDebug() << "[NUMS]" << exp1 << " " << exp2 << endl;
            int result;
            if (op.toke.content == "+")
                result = exp1 + exp2;
            else if (op.toke.content == "-")
                result = exp1 - exp2;
            qDebug() << "result: " << result << " output: " << to_string(result).c_str() << endl;
            int count = pro.Right.size();
            while (count--)
                symbolList.pop_back();
            symbolList.push_back({ {-1,to_string(result) ,pro.Left.content }, -1, -1,true});
        }
        else {
            string new_tmp_var = "T" + to_string(cnt++);
            quadruple.push_back({ nextQ_index++, op.toke.content, sub_exp1.toke.content, sub_exp2.toke.content, new_tmp_var });

            int count = pro.Right.size();
            while (count--)
                symbolList.pop_back();
            symbolList.push_back({ {-1,new_tmp_var ,pro.Left.content }, -1, -1,false});
        }
    }
}

void SemanticAnalyzer::SemanProd_Item(const Production& pro)
{
    if (pro.Right.size() == 1) {
        SemanticSymbol exp = symbolList.back();
        //更新symbolList
        int count = pro.Right.size();
        while (count--)
            symbolList.pop_back();
        symbolList.push_back({{exp.toke.line,exp.toke.content,pro.Left.content },exp.table_index, exp.index,exp.is_num	});
    }
    else {
        SemanticSymbol sub_exp1 = symbolList[symbolList.size() - 3];
        SemanticSymbol op = symbolList[symbolList.size() - 2];
        SemanticSymbol sub_exp2 = symbolList[symbolList.size() - 1];
        if (sub_exp1.is_num && sub_exp2.is_num)
        {
            int exp1 = str2num(sub_exp1.toke.content), exp2 = str2num(sub_exp2.toke.content);
            int result;
            if (op.toke.content == "*")
                result = exp1 * exp2;
            else if (op.toke.content == "/")
                result = exp1 / exp2;
            int count = pro.Right.size();
            while (count--)
                symbolList.pop_back();
            symbolList.push_back({ {-1,to_string(result) ,pro.Left.content }, -1, -1,true });
        }
        else 
        {
            std::string new_tmp_var = "T" + to_string(cnt++);
            quadruple.push_back({ nextQ_index++, op.toke.content, sub_exp1.toke.content, sub_exp2.toke.content, new_tmp_var });

            int count = pro.Right.size();
            while (count--)
                symbolList.pop_back();
            symbolList.push_back({ {-1,new_tmp_var,pro.Left.content }, -1, -1,false });
        }
    }
}

void SemanticAnalyzer::SemanProd_Factor(const Production& pro)
{
    if (pro.Right.size() == 1) {
        SemanticSymbol exp = symbolList.back();
        //如果是ID检查其是否进行过定义
        if (pro.Right[0].content == "[ID]") {
            bool existed = false;
            for (int scope_layer = cur_tableStack.size() - 1; scope_layer >= 0; scope_layer--) {
                auto current_table = allTable[cur_tableStack[scope_layer]];
                if (current_table.findSym(exp.toke.content) != -1) {
                    existed = true;
                    break;
                }
            }
            if (existed == false) {
                errorInfo.push_back("[ERROR-10006] : " + to_string(exp.toke.line)+ "行，变量" +exp.toke.content + "未定义" );
            }
        }

        //更新symbolList
        int count = pro.Right.size();
        while (count--)
            symbolList.pop_back();
        if (exp.toke.type == "[NUM]")
        {
            symbolList.push_back({ {exp.toke.line,exp.toke.content ,pro.Left.content},exp.table_index,exp.index,true });
            qDebug() << "[GETNUM]" << exp.toke.content.c_str() << endl;
        }
        else
            symbolList.push_back({ {exp.toke.line,exp.toke.content ,pro.Left.content },exp.table_index,exp.index,exp.is_num });
    }
    else {
        SemanticSymbol exp = symbolList[symbolList.size() - 2];

        //更新symbolList
        int count = pro.Right.size();
        while (count--)
            symbolList.pop_back();
        symbolList.push_back({ { exp.toke.line, exp.toke.content,pro.Left.content},exp.table_index, exp.index,exp.is_num});
    }
}




void SemanticAnalyzer::SemanProd_CallStmt(const Production& pro)
{
    SemanticSymbol identifier = symbolList[symbolList.size() - 5];
    SemanticSymbol check = symbolList[symbolList.size() - 3];
    SemanticSymbol args = symbolList[symbolList.size() - 2];

    //检查函数是否定义（在CallFunCheck时已经检查）

    //检查参数个数
    int para_num = allTable[check.table_index].Symtable[check.index].paraNum;
    if (para_num > stoi(args.toke.content)) {
        errorInfo.push_back("[ERROR-10007] : " + to_string(identifier.toke.line) + "行，函数" + identifier.toke.content + "调用时参数过少");
    }
    else if (para_num < stoi(args.toke.content)) {
        errorInfo.push_back("[ERROR-10008] : " + to_string(identifier.toke.line) + "行，函数" + identifier.toke.content + "调用时参数过多");
    }

    // 生成函数调用四元式
    string new_tmp_var = "T" + to_string(cnt++);
    quadruple.push_back({ nextQ_index++, "call", identifier.toke.content, "-", new_tmp_var });

    int count = pro.Right.size();
    while (count--)
        symbolList.pop_back();
    // 新的exp的value为临时变量名
    symbolList.push_back({ {-1, new_tmp_var , pro.Left.content},-1,-1 });
}

void SemanticAnalyzer::SemanProd_CallFunCheck(const Production& pro)
{
    SemanticSymbol fun_id = symbolList[symbolList.size() - 2];

    int fun_id_pos = allTable[0].findSym(fun_id.toke.content);

    if (-1 == fun_id_pos) {
        errorInfo.push_back("[ERROR-10009] : " + to_string(fun_id.toke.line) + "行，函数" + fun_id.toke.content + "调用未定义");
    }
    if (allTable[0].Symtable[fun_id_pos].type != FUNC) {
        errorInfo.push_back("[ERROR-10010] : " + to_string(fun_id.toke.line) + "行，函数" + fun_id.toke.content + "调用未定义");
    }
    symbolList.push_back({ {fun_id.toke.line, fun_id.toke.content, pro.Left.content}, 0, fun_id_pos });
}

void SemanticAnalyzer::SemanProd_Args(const Production& pro)
{
    if (pro.Right.size() == 3) {
        SemanticSymbol exp = symbolList[symbolList.size() - 3];
        quadruple.push_back({ nextQ_index++, "param", exp.toke.content, "-", "-" });
        int aru_num = stoi(symbolList.back().toke.content) + 1;
        int count = pro.Right.size();
        while (count--)
            symbolList.pop_back();
        symbolList.push_back({ { -1,to_string(aru_num),pro.Left.content }, -1, -1 });
    }
    else if (pro.Right[0].content == "<Exp>") {
        SemanticSymbol exp = symbolList.back();
        quadruple.push_back({ nextQ_index++, "param", exp.toke.content, "-", "-" });
        int count = pro.Right.size();
        while (count--)
            symbolList.pop_back();
        symbolList.push_back({ {-1, "1",pro.Left.content}, -1,-1 });
    }
    else if (pro.Right[0].content == "[z]") {
        symbolList.push_back({ {1, "0",pro.Left.content}, -1,-1 });
    }
}

void SemanticAnalyzer::SemanProd_ReturnStmt(const Production& pro)
{
    if (pro.Right.size() == 2) {
        //返回值
        SemanticSymbol return_exp = symbolList.back();
        //函数表
        SemanticSymtable function_table = allTable[cur_tableStack.back()];

        //添加四元式
        quadruple.push_back({ nextQ_index++,"=",return_exp.toke.content,"-",function_table.Symtable[0].name });

        //添加四元式
        quadruple.push_back({ nextQ_index++ ,"return",function_table.Symtable[0].name,"-",function_table.name });

        //更新symbolList
        int count = pro.Right.size();
        while (count--)
            symbolList.pop_back();
        symbolList.push_back({ {-1, return_exp.toke.content,pro.Left.content }, -1, -1 });
    }
    else {
        //函数表
        SemanticSymtable function_table = allTable[cur_tableStack.back()];

        //检查函数的返回值是否为void
        if (allTable[0].Symtable[allTable[0].findSym(function_table.name)].specialType != "void") {
            errorInfo.push_back("[ERROR-10011] : " + to_string( symbolList.back().toke.line) +"行，函数" + function_table.name + "必须有返回值" );
        }

        //添加四元式
        quadruple.push_back({ nextQ_index++ ,"return","-","-",function_table.name });

        //更新symbolList
        int count = pro.Right.size();
        while (count--)
            symbolList.pop_back();
        symbolList.push_back({ { -1, "",pro.Left.content }, -1, -1 });
    }
}

void SemanticAnalyzer::SemanProd_Relop(const Production& pro)
{
    SemanticSymbol op = symbolList.back();

    int count = pro.Right.size();
    while (count--) {
        symbolList.pop_back();
    }
    symbolList.push_back({ {-1,op.toke.content ,pro.Left.content}, -1,-1 });
}

void SemanticAnalyzer::SemanProd_IfStmt(const Production& pro)
{
    SemanticSymbol ifstmt_m2 = symbolList[symbolList.size() - 3];
    SemanticSymbol ifnext = symbolList[symbolList.size() - 1];

    if (ifnext.toke.content.empty()) {
        //只有if没有else
        //真出口
        quadruple[backpatching_list.back()].res = ifstmt_m2.toke.content;
        backpatching_list.pop_back();

        //假出口
        quadruple[backpatching_list.back()].res = to_string(nextQ_index);
        backpatching_list.pop_back();
    }
    else {
        //if块出口
        quadruple[backpatching_list.back()].res = to_string(nextQ_index);
        backpatching_list.pop_back();
        //if真出口
        quadruple[backpatching_list.back()].res = ifstmt_m2.toke.content;
        backpatching_list.pop_back();
        //if假出口
        quadruple[backpatching_list.back()].res = ifnext.toke.content;
        backpatching_list.pop_back();
    }
    backpatching_level--;

    //popback
    int count = pro.Right.size();
    while (count--)
        symbolList.pop_back();
    symbolList.push_back({ {-1,"",pro.Left.content},-1,-1 });
}

void SemanticAnalyzer::SemanProd_IfStmt_m1(const Production& pro)
{
    backpatching_level++;
    symbolList.push_back({ {-1,to_string(nextQ_index),pro.Left.content},-1,-1 });
}

void SemanticAnalyzer::SemanProd_IfStmt_m2(const Production& pro)
{
    SemanticSymbol if_exp = symbolList[symbolList.size() - 2];

    //待回填四元式 : 假出口
    quadruple.push_back({ nextQ_index++,"j=",if_exp.toke.content,"0","" });
    backpatching_list.push_back(quadruple.size() - 1);

    //待回填四元式 : 真出口
    quadruple.push_back({ nextQ_index++,"j","-","-","" });
    backpatching_list.push_back(quadruple.size() - 1);

    //if块开始
    quadruple.push_back({ FRAG_START , "if","-","-" ,"-" });

    symbolList.push_back({ {-1,to_string(nextQ_index),pro.Left.content},-1,-1 });
}

void SemanticAnalyzer::SemanProd_IfNext(const Production& pro)
{
    SemanticSymbol if_stmt_next = symbolList[symbolList.size() - 3];

    int count = pro.Right.size();
    while (count--)
        symbolList.pop_back();

    symbolList.push_back({ { -1,if_stmt_next.toke.content,pro.Left.content }, -1, -1 });
}

void SemanticAnalyzer::SemanProd_IfStmt_next(const Production& pro)
{
    //if 的跳出语句(else 之前)(待回填)
    quadruple.push_back({ nextQ_index++,"j","-","-","" });

    //else块开始
    quadruple.push_back({ FRAG_START , "else","-","-" ,"-" });

    backpatching_list.push_back(quadruple.size() - 1);
    symbolList.push_back({ {-1,to_string(nextQ_index),pro.Left.content},-1,-1 });
}
void SemanticAnalyzer::SemanProd_WhileStmt(const Production& pro)
{
    SemanticSymbol whilestmt_m1 = symbolList[symbolList.size() - 6];
    SemanticSymbol whilestmt_m2 = symbolList[symbolList.size() - 2];

    // 无条件跳转到 while 的条件判断语句处
    quadruple.push_back({ nextQ_index++,"j","-","-" ,whilestmt_m1.toke.content });

    //回填真出口
    quadruple[backpatching_list.back()].res = whilestmt_m2.toke.content;
    backpatching_list.pop_back();

    //回填假出口
    quadruple[backpatching_list.back()].res = to_string(nextQ_index);
    backpatching_list.pop_back();

    backpatching_level--;
    int count = pro.Right.size();
    while (count--)
        symbolList.pop_back();
    symbolList.push_back({ {-1, "",pro.Left.content},-1,-1 });
}

void SemanticAnalyzer::SemanProd_WhileStmt_m1(const Production& pro)
{
    backpatching_level++;
    symbolList.push_back({ {-1,to_string(nextQ_index), pro.Left.content }, -1, -1 });
}

void SemanticAnalyzer::SemanProd_WhileStmt_m2(const Production& pro)
{
    SemanticSymbol while_exp = symbolList[symbolList.size() - 2];

    //假出口
    quadruple.push_back({ nextQ_index++,"j=",while_exp.toke.content,"0","" });
    backpatching_list.push_back(quadruple.size() - 1);
    //真出口
    quadruple.push_back({ nextQ_index++ ,"j","-","-" ,"" });
    backpatching_list.push_back(quadruple.size() - 1);

    //while块开始
    quadruple.push_back({ FRAG_START , "while","-","-" ,"-" });

    symbolList.push_back({ { -1,to_string(nextQ_index),pro.Left.content }, -1, -1 });
}
