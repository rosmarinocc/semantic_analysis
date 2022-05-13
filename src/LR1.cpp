﻿#include "LR1.h"
#include <iomanip>
/******************LR1类******************/
LR1::LR1(QTextBrowser* control,QTextBrowser* lex1,QTextBrowser* lex2,QTextBrowser* out)
{
    this->control=control;
    this->lex1=lex1;
    this->lex2=lex2;
    this->out=out;
}


int LR1::read_generators(string dat_path)
{

    stringstream sstr;
    ifstream fin(dat_path, ios::in | ios::binary);
    if (!fin.is_open())
        return -1;
    char ch;
    while (1) {
        fin.get(ch);
        if (fin.eof())
            break;
        sstr << ch;
    }
    fin.close();    
    while (1) {
        char buf[1000] = { 0 };
        sstr.getline(buf, 1000, '\n');
        if (buf[0] == '\0')
            break;
        /*处理单行,每轮处理1行,生成1-n个Production*/

        string s0 = buf;
        int pos_t = s0.find('\r');
        if (pos_t != -1)
            s0.erase(pos_t, 1);
        while ((pos_t = s0.find(' ')) != -1)
            s0.erase(pos_t, 1);
        int pos_arrow = s0.find("->");
        //左部提取
        string left0 = s0.substr(0, pos_arrow);
        string left = left0.substr(left0.find('<'), left0.find('<') - left0.find('>') + 1);
        Symbol left_tag = Symbol(VN, left);
        vSet.insert(left_tag);
        vnSet.insert(left_tag);

        //右部提取
        string right0 = s0.substr(pos_arrow + 2);
        string str_t;
        while (!right0.empty()) {
            string right_t;//每轮处理右部'|'分隔的一项
            Production p1;
            int pos_t = right0.find('|');
            if (pos_t == -1)
                right_t = right0;
            else
                right_t = right0.substr(0, pos_t);
            for (int i = 0; i < right_t.length(); i++) {//每轮处理一条产生式的各个符号
                if (right_t[i] == '<') {//非终结符
                    int pos_rbracket = right_t.find_first_of('>', i);
                    string vn_s = right_t.substr(i, pos_rbracket - i + 1);
                    Symbol vn(VN, vn_s);
                    vSet.insert(vn);
                    vnSet.insert(vn);
                    p1.Right.push_back(vn);
                    i = pos_rbracket;
                }
                else if (right_t[i] == '[') {//终结符
                    int pos_rbracket = right_t.find_first_of(']', i);
                    string vt_s = right_t.substr(i, pos_rbracket - i + 1);
                    Symbol vt(VT, vt_s);
                    if (vt_s == "[z]")
                        vt.symbol_type = EPSILON;
                    vSet.insert(vt);
                    if (vt.symbol_type == VT)
                        vtSet.insert(vt);
                    p1.Right.push_back(vt);
                    i = pos_rbracket;
                }
            }
            p1.Left = left_tag;
            generator.push_back(p1);
            if (pos_t != -1)
                right0.erase(0, pos_t + 1);
            else
                break;
        }
    }

    qDebug()<<"93in"<<QString::fromStdString(dat_path)<<endl;
    return 1;
}
/******************************************************************************/
//标准merge
bool mergeSet(set<Symbol>& dst, const set<Symbol>& src)
{
    //合并src到dst
    set<Symbol> tmp(src);
    int before = dst.size();
    set_union(begin(tmp), end(tmp), begin(dst), end(dst), inserter(dst, begin(dst)));
    //判断前后数量,判断合并成功与否
    return before < dst.size();
}
//求非空集的并集-{ε}
bool mergeSetNoEpsilon(set<Symbol>& dst, const set<Symbol>& src)
{
    //合并src-{ε}到dst
    Symbol z{ EPSILON,"[z]" };
    set<Symbol> tmp(src);
    int before = dst.size();
    if (tmp.find(z) != tmp.end())//删除ε
        tmp.erase(z);
    set_union(begin(tmp), end(tmp), begin(dst), end(dst), inserter(dst, begin(dst)));
    //判断前后数量,判断合并成功与否
    return before < dst.size();
}
/******************************************************************************/
//初始化first集,包括VT和VN
void init_firstset(LR1& lr1)
{
    //初始化firstset的map
    for (auto sym = lr1.vSet.begin(); sym != lr1.vSet.end(); ++sym) {
        set<Symbol>tmp{};
        if (sym->symbol_type == VT)
            tmp.insert(*sym);
        lr1.first_set.insert(make_pair(*sym, tmp));
    }
    return;
}


//计算所有vn的first集合
void LR1::get_firstset_of_vn()
{
    init_firstset(*this);
    bool changed;
    while (1) {
        changed = false;
        //遍历所有非终结符
        for (auto vn = vnSet.begin(); vn != vnSet.end(); ++vn) {
            //遍历所有产生式
            for (auto prod = generator.begin(); prod != generator.end(); ++prod) {
                if (prod->Left != *vn)
                    continue;
                //找到对应产生式,开始遍历右部符号vector
                auto prod_r = prod->Right.begin();

                //是vt or ε直接加入first集合退出
                if (prod_r->symbol_type == VT || prod_r->symbol_type == EPSILON) {
                    changed = first_set[*vn].insert(*prod_r).second || changed;
                    continue;
                }

                //右部以vn开始
                bool eps = true;//可推导空串
                for (; prod_r != prod->Right.end(); ++prod_r) {
                    //遇vt,停止
                    if (prod_r->symbol_type == VT) {
                        changed = mergeSetNoEpsilon(first_set[*vn], first_set[*prod_r]) || changed;
                        eps = false;
                        break;
                    }

                    changed = mergeSetNoEpsilon(first_set[*vn], first_set[*prod_r]) || changed;
                    //可以推导出空串
                    eps = eps && first_set[*prod_r].count(symEps);
                    //否则退出当前产生式的处理
                    if (!eps)
                        break;
                }
                //所有右部均可推出空串
                if (eps && prod_r == prod->Right.end())
                    changed = first_set[*vn].insert(symEps).second || changed;
            }
        }
        if (!changed)
            break;
    }
}

//计算给定符号串的first集,返回一个set
set<Symbol> LR1::get_firstset_of_string(const vector<Symbol>& str)
{
    //set<Symbol>newSet;
    //if (str.empty())//空串
    //	return newSet;
    //auto s = str.begin();
    //for (; s != str.end(); ++s) {
    //	//终结符
    //	if (s->symbol_type == VT) {
    //		mergeSetNoEpsilon(newSet, first_set[*s]);
    //		break;
    //	}
    //	//空串
    //	if (s->symbol_type == EPSILON) {
    //		//newSet.insert(*s);
    //		continue;
    //	}
    //	//非终结符
    //	mergeSetNoEpsilon(newSet, first_set[*s]);
    //	//如果可以推导出空串,则继续循环
    //	if (first_set[*s].count(symEps) == 0)
    //		break;
    //}
    ////均能产生空串,则加入ε
    //if (s == str.end())
    //	newSet.insert(symEps);
    //return newSet;
    set<Symbol> newSet;
    //str为空直接返回
    if (str.empty())
        return newSet;
    //epsilonIn用于判断空串是否需要加入
    bool EpsIn = true;

    for (auto it = str.begin(); it != str.end(); it++) {
        //如果是终结符
        if (it->symbol_type == VT)
        {
            mergeSetNoEpsilon(newSet, first_set[*it]);
            EpsIn = false;
            break;
        }
        //是空串
        if (it->symbol_type == EPSILON)
        {
            newSet.insert(*it);
            EpsIn = false;
            break;
        }
        //非终结符，合并first集合
        mergeSetNoEpsilon(newSet, first_set[*it]);
        //如果当前非终结符可以推导出空串，则继续循环
        EpsIn = EpsIn && first_set[*it].count(symEps);
        if (!EpsIn)
            break;
    }
    //如果所有的都可以产生空串，first集加入空串
    if (EpsIn)
        newSet.insert(symEps);
    return newSet;

}
/******************************************************************************/
bool deriveEps(LR1& lr1, const Symbol& s)
{
    if (s.symbol_type == EPSILON)
        return true;
    if (s.symbol_type == VT)
        return false;
    //非终结符
    bool dEps = false, one_dEps;
    for (auto prod = lr1.generator.begin(); prod != lr1.generator.end(); ++prod) {
        if (prod->Left == s) {//找到A->
            if (prod->Right[0] == symEps) {//A->ε
                return true;
            }
            else {
                one_dEps = true;
                for (int i = 0; i < int(prod->Right.size()); ++i) {//遍历该产生式所有右符号
                    if (prod->Right[i].symbol_type == VT) {//右侧有终结符,换下一条产生式
                        one_dEps = false;
                        break;
                    }
                    else if (prod->Right[i] != s && !deriveEps(lr1, prod->Right[i])) {
                        one_dEps = false;
                    }
                    if (!one_dEps)
                        break;
                }
                dEps = dEps || one_dEps;
            }
            if (dEps)
                break;
        }
    }
    return dEps;
}

////初始化follow集,仅包括VN
//void init_followset(LR1& lr1)
//{
//	//初始化followset的map
//	for (auto sym = lr1.vnSet.begin(); sym != lr1.vnSet.end(); ++sym) {
//		set<Symbol>tmp{};
//		if (sym->content == "<Program>")
//			tmp.insert(Symbol(VT, "[#]"));//文法开始符号加入[#]
//		lr1.follow_set.insert(make_pair(*sym, tmp));
//	}
//	return;
//}
////计算所有vt的follow集合
//void LR1::get_followset_of_vn()
//{
//	bool changed;
//	while (1) {
//		changed = false;
//		//遍历所有非终结符
//		for (auto vn = vnSet.begin(); vn != vnSet.end(); ++vn) {//A
//			for (auto prod = generator.begin(); prod != generator.end(); ++prod) {
//				for (auto prod_r = prod->Right.begin(); prod_r != prod->Right.end(); ++prod_r) {
//					if (*prod_r == *vn) {//找到了产生式 .->.A.
//						if (prod_r == prod->Right.end() - 1) //产生式 .->.A
//							changed = mergeSet(follow_set[*vn], follow_set[prod->Left]) || changed;
//						else {//产生式 .->.A.
//							changed = mergeSetNoEpsilon(follow_set[*vn], first_set[*(prod_r + 1)]) || changed;
//							if (deriveEps(*this,*(prod_r + 1)))
//								changed = mergeSet(follow_set[*vn], follow_set[prod->Left]) || changed;
//						}
//					}
//				}
//			}
//		}
//		if (!changed)
//			break;
//	}
//}
/******************************************************************************/
//生成拓广文法,符号集中加入<S0>,产生式集合加入<S0>-><Program>
void get_extension(LR1& slr)
{
    //S0符号
    Symbol s0(VN, "<S0>");
    slr.vSet.insert(s0);
    slr.vnSet.insert(s0);
    slr.vSet.insert(symEnd);
    slr.vtSet.insert(symEnd);
    slr.first_set.insert(make_pair(symEnd, set<Symbol>{ symEnd }));
    //<S0>-><Program>
    Production s0_s;
    s0_s.Left = s0;
    s0_s.Right.push_back(Symbol(VN, "<Program>"));
    slr.generator.insert(slr.generator.begin(), s0_s);
    Event extS(s0_s, 0, symEnd); //包含S’的项目集
    EventClosure inieveclo;//包含展望符的项目闭包集合，默认空参构造函数
    inieveclo.Es.push_back(extS);
    slr.eventclo.push_back(slr.GetEventClo(inieveclo));
}



void LR1::init_items()//根据generator生成拓广文法的items集合
{
    get_extension(*this);

    qDebug()<<this->vSet.size()<<endl;
    qDebug()<<this->vtSet.size()<<endl;
    qDebug()<<this->vnSet.size()<<endl;
    qDebug()<<this->lex.LexResult.size()<<endl;
    qDebug()<<"lexresultsize"<<this->lex.Result.size()<<endl;

    for (int i = 0; i < int(eventclo.size()); i++)
    {
        for (set<Symbol>::iterator s = vSet.begin(); s != vSet.end(); s++)
        {
            //必须为终结符或非终结符
            if (!((*s).symbol_type == VT || (*s).symbol_type == VN))
                continue;
            //得到的是VT/VN
            EventClosure toclosure = GetEventTO(eventclo[i], (*s));
            //当为空时直接continue
            if (toclosure.Es.empty())
                continue;
            int whereclo = -1;//下标号不为0
            for (int j = 0; j < int(eventclo.size()); j++)
            {
                if (toclosure == eventclo[j])
                {
                    whereclo = j;
                    break;
                }
            }
            if (whereclo != -1)//说明已经存在，直接建立号goto关系
            {
                GotoInfo[{i, (*s)}] = whereclo;
            }
            else //不存在则插入
            {
                eventclo.push_back(toclosure);//插入到了最后的位置
                GotoInfo[{i, (*s)}] = eventclo.size() - 1;
//                qDebug()<<"insert"<<i<<endl;
//                qDebug()<<(*s).content.c_str()<<endl;
            }
        }
//        qDebug()<<"clouse"<<i<<endl;
    }
      qDebug()<<"init_item()"<<endl;
}


EventClosure& LR1::GetEventClo(EventClosure& CloJ)
{
    // TODO: 在此处插入 return 语句
    //注意这里不能使用迭代器，否则在CloJ.Es插值改变后会出错
    for (int it = 0; it < int(CloJ.Es.size()); it++)
    {
        Event nowit = CloJ.Es[it];
        //对于在最后位置的·没有终结符,不存在A->D·
        if (nowit.dotPos >= static_cast<int>(nowit.prod.Right.size()))
            continue;
        Symbol next_symbol = nowit.prod.Right[nowit.dotPos];//点之后的符号是什么
        if (next_symbol.symbol_type == VT) //如果是终结符
            continue;
        if (next_symbol.symbol_type == EPSILON)//如果是epsilon，执行·epsilon->epsilon·
        {
            CloJ.Es[it].dotPos++;
            continue;
        }
        //这里可以通过产生式A->alpha B beta , a，求beta a的first集合
        //当dotPos=size时，迭代器不会遍历到，因此不会越界

        vector<Symbol> ba((nowit.prod.Right.begin() + nowit.dotPos+1 ), nowit.prod.Right.end());
        ba.push_back(nowit.ahead);//在扩展区域插入展望符号


        set<Symbol> firstba = get_firstset_of_string(ba); //beta a的first集合
        //for(auto i=firstba.begin();)


        //找到nextsymbol开始的产生式
        for (auto pro = generator.begin(); pro != generator.end(); pro++)
        {
            if (pro->Left != next_symbol)//如果不是，那么就接着遍历
                continue;
            //这里存在有产生式生成空的形式A->epsilon·
            //如果找到，加入CloJ
            for (auto beta = firstba.begin(); beta != firstba.end(); beta++)
            {
                //假如产生式右侧是epslon产生式,那么不再考虑epsilon的转移，直接加入epsilon·即可
                //[A->alpha B beta , a] ;b in first(beta a) ; B-> E; [B->E, b ]对于前缀依然有效
                //对于有效的[B->E, b ]，全部加入
                if ((*pro).Right.front().symbol_type == EPSILON)
                {
                    Event tmp(*pro, 1, *beta);//临时构造的是epsilon·，所以dotpos直接取1
                    vector<Event>::iterator over;
                    for (over = CloJ.Es.begin(); over != CloJ.Es.end(); over++)
                    {
                        if (*over == tmp)
                            break;
                    }
                    if (over == CloJ.Es.end())
                    {
                        CloJ.Es.push_back(tmp);
                        //cout << "Episi" << *beta << endl;
                    }
                }
                else //产生式不产生空串
                {
                    Event tmp(*pro, 0, *beta);//临时构造的是epsilon·，所以dotpos直接取1
                    vector<Event>::iterator over;
                    for (over = CloJ.Es.begin(); over != CloJ.Es.end(); over++)
                    {
                        if (*over == tmp)
                            break;
                    }
                    if (over == CloJ.Es.end())
                    {
                        CloJ.Es.push_back(tmp);
                        //cout<< *beta << endl;

                    }
                }
            }
        }

    }

    return CloJ;
}

EventClosure LR1::GetEventTO(EventClosure& CloI, const Symbol sym)
{
    // TODO: 在此处插入 return 语句
    EventClosure toclo;
    //到这里的已经是VT或VN,不符合要求返回空
    if (!(sym.symbol_type == VT || sym.symbol_type == VN))
        return toclo;

    for (auto it = CloI.Es.begin(); it != CloI.Es.end(); it++)
    {
        //判断it的最后一个字符是“·”，直接跳过
        if ((*it).dotPos >= (*it).prod.Right.size())
            continue;
        //最后一个字符不是present_symbol，接着判断
        if ((*it).prod.Right[(*it).dotPos] != sym)
            continue;

        toclo.Es.push_back({ (*it).prod,(*it).dotPos + 1,(*it).ahead });
    }
    //qDebug()<<sym.content.c_str()<<endl;

    return GetEventClo(toclo);
}


static int findindex(const Production& prod,const LR1 &lr1)
{
    for (int i = 0; i < lr1.generator.size(); ++i) {
        if (prod == lr1.generator[i])
            return i;
    }

    return -1;//不可能找不到,但是还是写在这里
}

/****************待合并********************/
//生成Table
void LR1::getTable()
{
    for (int clo_index = 0; clo_index < eventclo.size(); ++clo_index) {
        for (int it_index = 0; it_index != eventclo[clo_index].Es.size(); ++it_index) {
            Event current = eventclo[clo_index].Es[it_index];

            //·在末尾
            if (current.dotPos >= current.prod.Right.size()) {
                //是否为拓展开始符号?否,进行规约
                if (current.prod.Left != symS0)
                    ActionTable[{clo_index, current.ahead}] = { REDUCE, findindex(current.prod,*this) };
                else
                    ActionTable[{clo_index, current.ahead}] = { ACCEPT,-1 };
            }
            //dot不在末尾
            else {
                Symbol SymAfterDot = current.prod.Right[current.dotPos];
                //非终结符
                if (SymAfterDot.symbol_type == VN) {
                    auto it = GotoInfo.find({ clo_index, SymAfterDot });
                    if (it != GotoInfo.end()) //找到
                        GotoTable[{clo_index, SymAfterDot}] = { SHIFTIN, it->second};
                }
                else if (SymAfterDot.symbol_type == VT) {
                    auto it = GotoInfo.find({ clo_index, SymAfterDot });
                    if (it != GotoInfo.end()) //找到
                        ActionTable[{clo_index, SymAfterDot}] = { SHIFTIN, it->second };
                }
            }
        }
    }
}


//打印Table
void LR1::printTable(const string file_path)
{
    ofstream fout(file_path, ios::out);

    fout << " ,";
    for (auto vt : vtSet)
        fout << vt << ",";
    fout << "||,";
    for (auto vn : vnSet)
        if (vn != symS0)
            fout << vn << ",";

    fout << endl;
    for (int i = 0; i < eventclo.size(); ++i) {
        fout << i << ",";
        for (auto vt : vtSet) {
            auto current = ActionTable.find({ i, vt });
            if (current == ActionTable.end())
                fout << " ,";
            else {
                if (current->second.action == ACCEPT)
                    fout << "acc,";
                else if (current->second.action == SHIFTIN)
                    fout << "s" << current->second.index << ",";
                else if (current->second.action == REDUCE)
                    fout << "r" << current->second.index << ",";
                else
                    fout << ",";
            }

        }
        fout << "||,";
        for (auto vn : vnSet) {
            if (vn == symS0)
                continue;

            auto current = GotoTable.find({ i, vn });
            if (current == GotoTable.end())
                fout << " ,";
            else
                fout << current->second.index << ",";
        }
        fout << endl;
    }
    fout.close();
}
int LR1::grammartree(const string tree_path, const string process_path, queue<SymToken>& Code)
{
    int ret=0;
    /*********************************************************************/

    //初始化 semantic 类
    SemanticAnalysis = SemanticAnalyzer();
    /*********************************************************************/
    fstream fout2(process_path, ios::out);
    if(!fout2.is_open())
    {
        Mess_to_control("[Draw-Tree] process File open Failed!\n",this->control,QTmessage_append);
    }
    vector<Symbol> Symstack; //符号栈
    vector<int>Statestack;  //状态栈
    vector<int> printsymstac; //记录打印符号栈
    int prt = 0;// 打印时的编号
    stringstream sstr;
    //存入文件打开，.dot
    fstream fout;
    fout.open(tree_path, ios::out);
    if (fout.is_open() == false)
    {
        Mess_to_control("[Draw-Tree] Dot File open Failed!\n",this->control,QTmessage_append);
    }
    sstr << "digraph tree { " << endl;
    Statestack.push_back(0);
    Symstack.push_back(symEnd);

    SymToken edsy = {{-1,strend,LexComponent::End},symEnd};//
    Code.push(edsy);//

    /*********************************************************************/

    //将 Start 加入 semantic 的语义符号列表
    SemanticAnalysis.insertSymbol( {{-1,"",strstart},-1,- 1 } );

    /*********************************************************************/


    //int pos;
    while (true && !Code.empty())
    {
        Symbol first = Code.front().sym;
        Token firtok = Code.front().tok;
        auto current = ActionTable.find({ Statestack.back(),first });
        //情况1：ERROR
        if (current == ActionTable.end()) {
            Mess_to_control("[Draw-Tree] Dot ACTIONERROR\n",this->control,QTmessage_append);
            ret=1;
            break;
        }
        //情况2：ACCEPT
        else if (current->second.action == ACCEPT) {
            Mess_to_control("[Draw-Tree] ACCEPT! Draw-Tree successfully!\n",this->control,QTmessage_append);
            sstr << "}" << endl;
            ret=0;
            break;
        }
        //情况3：SHIFTIN
        else if (current->second.action == SHIFTIN) {

            fout2 << first << endl;

            Symstack.push_back(first);
            Statestack.push_back(current->second.index);
            Code.pop();
            printsymstac.push_back(prt);
            sstr << "v" << prt++ << "[label=\"" << first << "\",fillcolor=paleturquoise,shape=doublecircle,style=filled];" << endl;
            /*********************************************************************/
            string tmp_in;
            if (firtok.type == LexComponent::ID)
                tmp_in = "[ID]";
            else if (firtok.type == LexComponent::Digit)
            {
                tmp_in = "[NUM]";
                qDebug() << "num:" << firtok.content.c_str()<<endl;
            }
            else
                tmp_in = firtok.content;
            //将 token 加入 semantic 的语义符号列表
            SemanticAnalysis.insertSymbol({{ firtok.line,firtok.content,tmp_in } , -1,- 1});

            /*********************************************************************/
        }
        //情况4：规约
        else if (current->second.action == REDUCE) {
            //GotoTable
            //找到generator中的规约状态
            Production now = generator[current->second.index];

            fout2 << "--------";
            for (int i = now.Right.size() - 1; i >= 0; --i)
                fout2 << now.Right[i];
            fout2 << " => " << now.Left << endl;

            int Rightnum = int(now.Right.size());
            vector<int>printsym;//为打印做准备
            if (now.Right.front().symbol_type != EPSILON)
            {
                while (Rightnum--)
                {
                    Symstack.pop_back();
                    Statestack.pop_back();
                    printsym.push_back(printsymstac.back());
                    printsymstac.pop_back();//自己打印的栈pop
                }
            }
            Symbol syleft = now.Left;
            //状态站查Goto表
            Symstack.push_back(syleft);

            int state = Statestack.back();
            Symbol codestart = Code.front().sym;
            auto goto_id = GotoTable.find({ state, syleft });
            if (goto_id == GotoTable.end())
            {
                Mess_to_control("[Draw-Tree] GOTOERROR!!!\n",this->control,QTmessage_append);
                ret=2;
                break;
            }
            Statestack.push_back(goto_id->second.index);

            printsymstac.push_back(prt);
            sstr << "v" << prt++ << "[label=\"" << syleft << "\"" << ",fillcolor=mistyrose,shape=doubleoctagon,style=filled" << "];" << endl;
            if (printsym.size() != 0)
            {
                for (auto t = printsym.begin(); t != printsym.end(); t++)
                    sstr << "v" << prt - 1 << " -> " << "v" << *t << ";" << endl;
            }
            else //表示是空串
            {
                sstr << "e" << prt << "[label=\"[Z]\"];" << endl;
                sstr << "v" << prt - 1 << "-> e" << prt << ";" << endl;
            }

            /*********************************************************************/

            //进行语义分析
            SemanticAnalysis.semanticANL(now);

            /*********************************************************************/

        }
    }
    SemanticAnalysis.optimize();
    fout << sstr.str();
    fout.close();
    fout2.close();
    return ret;
}

