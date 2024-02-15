#ifndef PARSER 
#define PARSER

// parser of myscheme 

#include "RE.hpp"
#include "Def.hpp"
#include "syntax.hpp"
#include "expr.hpp"
#include "value.hpp"

#include <map>
#include <cstring>
#include <iostream>
#define mp make_pair
using std :: string;
using std :: vector;
using std :: pair;

extern std :: map<std :: string, ExprType> primitives;
extern std :: map<std :: string, ExprType> reserved_words;

Expr Number :: parse(Assoc &env) {   
    return Expr(new Fixnum(n));
}
ExprType Number :: gettype() {return E_FIXNUM;}

Expr TrueSyntax :: parse(Assoc &env) {
    return Expr(new True());
}
ExprType TrueSyntax :: gettype() {return E_TRUE;}

Expr FalseSyntax :: parse(Assoc &env) {
    return Expr(new False());
}
ExprType FalseSyntax :: gettype() {return E_FALSE;}

Expr Identifier :: parse(Assoc &env) {
    return Expr(new Var(s));
}
ExprType Identifier :: gettype() {
    //std::cout<<s<<std::endl;
    if(primitives[s]) return primitives[s];
    if(reserved_words[s]) return reserved_words[s];
    if(s=="let") return E_LET; 
    return E_VAR;
}

ExprType List :: gettype() {return E_LIST;}
Expr List :: parse(Assoc &env) {
    if(!stxs.size()) throw RuntimeError("RE");
    ExprType type = stxs[0]->gettype();   
    Identifier *id = dynamic_cast<Identifier*>(stxs[0].get());
    if(id!=nullptr){
        bool f=0;
        for(auto i=env;i.get()!=nullptr;i=i->next)
            if(i->x == id->s) {f=1;break;}
        if(f){
            // std::cout<<"used"<<std::endl;
            vector<Expr> vec;
            for(int i=1;i<stxs.size();i++)
                vec.push_back(stxs[i]->parse(env));
            return Expr(new Apply(id->parse(env), vec));
        }
    }
    //std::cout<<type<<std::endl;
    // primitives   
    if(type == E_EXIT) {
        if(stxs.size()!=1) throw RuntimeError("RE");
        return Expr(new Exit());
    }
    if(type == E_VOID) {
        if(stxs.size()!=1) throw RuntimeError("RE");
        return Expr(new MakeVoid());
    }
    if(type == E_MUL) {
        if(stxs.size()!=3) throw RuntimeError("RE");
        return Expr(new Mult(stxs[1]->parse(env), stxs[2]->parse(env)));
    }
    if(type == E_MINUS) {
        if(stxs.size()!=3) throw RuntimeError("RE");
        return Expr(new Minus(stxs[1]->parse(env), stxs[2]->parse(env)));
    }
    if(type == E_PLUS) {
        if(stxs.size()!=3) throw RuntimeError("RE");
        return Expr(new Plus(stxs[1]->parse(env), stxs[2]->parse(env)));
    }
    if(type == E_LT) {
        if(stxs.size()!=3) throw RuntimeError("RE");
        return Expr(new Less(stxs[1]->parse(env), stxs[2]->parse(env)));
    }
    if(type == E_LE) {
        if(stxs.size()!=3) throw RuntimeError("RE");
        return Expr(new LessEq(stxs[1]->parse(env), stxs[2]->parse(env)));
    }
    if(type == E_EQ) {
        if(stxs.size()!=3) throw RuntimeError("RE");
        return Expr(new Equal(stxs[1]->parse(env), stxs[2]->parse(env)));
    }
    if(type == E_GE) {
        if(stxs.size()!=3) throw RuntimeError("RE");
        return Expr(new GreaterEq(stxs[1]->parse(env), stxs[2]->parse(env)));
    }
    if(type == E_GT) {
        if(stxs.size()!=3) throw RuntimeError("RE");
        return Expr(new Greater(stxs[1]->parse(env), stxs[2]->parse(env)));
    }
    if(type == E_EQQ) {
        if(stxs.size()!=3) throw RuntimeError("RE");
        return Expr(new IsEq(stxs[1]->parse(env), stxs[2]->parse(env)));
    }
    if(type == E_BOOLQ) {
        if(stxs.size()!=2) throw RuntimeError("RE");
        return Expr(new IsBoolean(stxs[1]->parse(env)));
    }
    if(type == E_INTQ) {
        if(stxs.size()!=2) throw RuntimeError("RE");
        return Expr(new IsFixnum(stxs[1]->parse(env)));
    }
    if(type == E_NULLQ) {
        if(stxs.size()!=2) throw RuntimeError("RE");
        return Expr(new IsNull(stxs[1]->parse(env)));
    }
    if(type == E_PAIRQ) {
        if(stxs.size()!=2) throw RuntimeError("RE");
        return Expr(new IsPair(stxs[1]->parse(env)));
    }
    if(type == E_PROCQ) {
        if(stxs.size()!=2) throw RuntimeError("RE");
        return Expr(new IsProcedure(stxs[1]->parse(env)));
    }
    if(type == E_SYMBOLQ) {
        if(stxs.size()!=2) throw RuntimeError("RE");
        return Expr(new IsSymbol(stxs[1]->parse(env)));
    }
    if(type == E_CONS) {
        if(stxs.size()!=3) throw RuntimeError("RE");
        return Expr(new Cons(stxs[1]->parse(env), stxs[2]->parse(env)));
    }
    if(type == E_NOT) {
        if(stxs.size()!=2) throw RuntimeError("RE");
        return Expr(new Not(stxs[1]->parse(env)));
    }
    if(type == E_CAR) {
        if(stxs.size()!=2) throw RuntimeError("RE");
        return Expr(new Car(stxs[1]->parse(env)));
    }
    if(type == E_CDR) {
        if(stxs.size()!=2) throw RuntimeError("RE");
        return Expr(new Cdr(stxs[1]->parse(env)));
    }

    if(type == E_IF) {
        if(stxs.size()!=4) throw RuntimeError("RE");
        return Expr(new If(stxs[1]->parse(env), stxs[2]->parse(env), stxs[3]->parse(env)));
    }
    if(type == E_BEGIN) {
        vector<Expr> tmp;
        for(int i=1;i<stxs.size();i++)
            tmp.push_back(stxs[i]->parse(env));
        return Expr(new Begin(tmp));
    }
    if(type == E_QUOTE) {
        if(stxs.size()!=2) throw RuntimeError("RE");
        return Expr(new Quote(stxs[1]));
    }
    if(type == E_LAMBDA) {
        if(stxs.size()!=3) throw RuntimeError("RE");
        //std::cout<<"LAMBDA"<<std::endl;
        if(stxs[1]->gettype()!=E_LIST) throw RuntimeError("RE");
        //std::cout<<1<<std::endl;

        Assoc e = empty();
        for(auto i=env;i.get()!=nullptr;i=i->next)
            e = extend(i->x, i->v, e);

        vector<string> vec;
        List *vars = dynamic_cast <List*> (stxs[1].get());
        for(int i=0;i<vars->stxs.size();i++){
            Identifier *tmp = dynamic_cast<Identifier*>(vars->stxs[i].get());
            if(tmp==nullptr) throw RuntimeError("RE");
            vec.push_back(tmp->s);            
            e = extend(tmp->s, NullV(), e); 
        }
        // for(int i=0;i<vec.size();i++) std::cout<<vec[i]<<" ";
        // std::cout<<std::endl;
        return Expr(new Lambda(vec, stxs[2]->parse(e)));
    }
    if(type == E_LET || type == E_LETREC) {
        if(stxs.size()!=3) throw RuntimeError("RE");
        if(stxs[1]->gettype()!=E_LIST) throw RuntimeError("RE");
        vector<pair<string, Expr>> vec;
        List *vars = dynamic_cast <List*> (stxs[1].get());
        //std::cout<<vars->stxs.size()<<std::endl;

        Assoc e = empty();
        for(auto i=env;i.get()!=nullptr;i=i->next)
            e = extend(i->x, i->v, e);

        for(int i=0;i<vars->stxs.size();i++){
            if(vars->stxs[i]->gettype()!=E_LIST) throw RuntimeError("RE");
            List *par = dynamic_cast<List*>(vars->stxs[i].get());
            //std::cout<<par->stxs.size()<<std::endl;
            if(par->stxs.size()!=2) throw RuntimeError("RE");
            Identifier *tmp = dynamic_cast<Identifier*>(par->stxs[0].get());
            if(tmp==nullptr) throw RuntimeError("RE");
            
            e = extend(tmp->s, NullV(), e);

            Expr exp=par->stxs[1]->parse(env);
            vec.push_back(mp(tmp->s, exp));
            //std::cout<<vec[i].first<<std::endl;
        }
        //std::cout<<std::endl;
        Expr t=stxs[2]->parse(e);
        //std::cout<<1<<std::endl;
        if(type == E_LET) return Expr(new Let(vec, t));
        else return Expr(new Letrec(vec, t));
    }
    vector<Expr> exprs;
    for(int i=1;i<stxs.size();i++){
        exprs.push_back(stxs[i]->parse(env));
    }
    return Expr(new Apply(stxs[0]->parse(env), exprs));
}

#endif