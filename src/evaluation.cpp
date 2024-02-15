#include "Def.hpp"
#include "value.hpp"
#include "expr.hpp"
#include "RE.hpp"
#include "syntax.hpp"
#include <cstring>
#include <vector>
#include <map>

extern std :: map<std :: string, ExprType> primitives;
extern std :: map<std :: string, ExprType> reserved_words;

Value Let::eval(Assoc &env) {
    // std::cout<<"let"<<std::endl;
    Assoc e = empty();      
    for(auto i=env;i.get()!=nullptr;i=i->next){
        e = extend(i->x,i->v,e);
    }

    // std::cout<<"e"<<std::endl;
    // for(auto i=e; i.get()!=nullptr; i=i->next){
    //     std::cout<<i->x<<" ";
    //     i->v->show(std::cout);std::cout<<(i->v.get())<<std::endl;
    // }

    for(int i=0;i<bind.size();i++) {
        bool flag=0;
        Value t=bind[i].second->eval(env);
        for (auto j = e; j.get() != nullptr; j = j -> next) {
            if (j -> x == bind[i].first){
                flag = 1;
                modify(bind[i].first, t, e);
                break;
            }    
        }
        if(!flag) e = extend(bind[i].first, t, e);
    }

    // std::cout<<"e  after"<<std::endl;
    // for(auto i=e; i.get()!=nullptr; i=i->next){
    //     std::cout<<i->x<<" ";
    //     i->v->show(std::cout);std::cout<<(i->v.get())<<std::endl;
    // }

    return body->eval(e);
} // let expression 

Value Lambda::eval(Assoc &env) {return ClosureV(x,e,env);} // lambda expression

Value Apply::eval(Assoc &e) {
    // std::cout<<"apply"<<std::endl;

    Value val = rator->eval(e); 
    if(val->v_type != V_PROC) throw RuntimeError("RE");
    Closure *f = dynamic_cast<Closure*>(val.get());
    if((f->parameters.size())!=rand.size()) throw RuntimeError("RE");
 
    Assoc env = empty();
    for(auto i = e; i.get() != nullptr; i = i->next)
        env = extend(i->x, i->v, env);

    // std::cout<<"e"<<std::endl;
    // for(auto i=e; i.get()!=nullptr; i=i->next){
    //     std::cout<<i->x<<" ";
    //     if(i->v.get()!=nullptr) i->v->show(std::cout);
    //     std::cout<<(i->v.get())<<std::endl;
    // }
    // std::cout<<"f->env"<<std::endl;
    // for(auto i=f->env; i.get()!=nullptr; i=i->next){
    //     std::cout<<i->x<<" ";
    //     if(i->v.get()!=nullptr) i->v->show(std::cout);
    //     std::cout<<(i->v.get())<<std::endl;
    // }   

    for(auto i=f->env; i.get()!=nullptr; i=i->next){
        bool flag=0;
        for (auto j = env; j.get() != nullptr; j = j -> next) {
            if (j -> x == i -> x){
                flag = 1;
                modify(i->x, i->v, env);
                break;
            }    
        }
        if(!flag) env = extend(i->x, i->v, env);
    }
    
    for(int i=0; i<rand.size(); i++) {
        Value v=rand[i]->eval(e);
        bool flag=0;
        for (auto j = env; j.get() != nullptr; j = j -> next) {
            if (j -> x == f->parameters[i]){
                modify(f->parameters[i], v, env);
                flag = 1;
                break;
            }    
        }
        if(!flag){
            env = extend(f->parameters[i], v, env);
        }
    }

    // std::cout<<std::endl;
    // for(auto i=env;i.get()!=nullptr;i=i->next){
    //     std::cout<<i->x<<" ";
    //     i->v->show(std::cout);std::cout<<(i->v.get())<<std::endl;
    // }

    return f->e->eval(env);
    //return func->e->eval(e);
} // for function calling


void changenv(Value &v, Assoc &e, Assoc &en) {
    if(v->v_type == V_PROC) {
        Closure *p = dynamic_cast<Closure*>(v.get());
        for(auto i = e; i.get()!=nullptr; i = i->next) {
            bool f = 0;
            for(auto j = p->env; j.get()!=nullptr; j = j->next) {
                if(i->x == j->x) {
                    if(find(i->x, en).get() == j->v.get())
                        modify(i->x, i->v, p->env);
                    f=1; break;
                }
            }
            if(!f) p->env = extend(i->x, i->v, p->env);
        }
        return;
    }
    if(v->v_type == V_PAIR) {
        Pair *p = dynamic_cast<Pair*>(v.get());
        changenv(p->car, e, en); changenv(p->cdr, e, en);
        return;
    }
}

Value Letrec::eval(Assoc &env) {
    // std::cout<<"letrec"<<std::endl;
    Assoc e = empty();     
    for(auto i=env;i.get()!=nullptr;i=i->next){
        e = extend(i->x, i->v, e);
    }  

    for(int i=0;i<bind.size();i++) {
        bool flag=0;
        for (auto j = e; j.get() != nullptr; j = j -> next) {
            if (j -> x == bind[i].first){
                flag = 1;
                modify(bind[i].first, Value(nullptr), e);
                break;
            }    
        }
        if(!flag) e = extend(bind[i].first, Value(nullptr), e);
    }

    Assoc en = empty();      
    for(auto i=e;i.get()!=nullptr;i=i->next){
        en = extend(i->x,i->v,en);
    }  
    for(int i=0;i<bind.size();i++) {
        Value v=bind[i].second->eval(en);
        modify(bind[i].first,v,e);
    }

    // std::cout<<"rec"<<std::endl;
    // for(auto i=e;i.get()!=nullptr;i=i->next){
    //     std::cout<<i->x<<" ";
    //     if(i->v.get()!=nullptr) i->v->show(std::cout);
    //     std::cout<<i->v.get()<<std::endl;
    // }

    // for(auto i=e;i.get()!=nullptr;i=i->next) {
    //     changenv(i->v, e, en);
    // }
    for(int i=0;i<bind.size();i++) {
        for(auto j=e;j.get()!=nullptr;j=j->next)
            if(j->x == bind[i].first) changenv(j->v, e, en);
    }

    return body->eval(e);
} // letrec expression

Value Var::eval(Assoc &e) {
    Value v = find(x, e);
    if(v.get()==nullptr) throw RuntimeError("");
    return v;
} // evaluation of variable

Value Fixnum::eval(Assoc &e) {return IntegerV(n);} // evaluation of a fixnum

Value If::eval(Assoc &e) {
    // std::cout<<"if"<<std::endl;
    Value tmp=cond->eval(e); 
    // tmp->show(std::cout);std::cout<<std::endl;
    Boolean *p = dynamic_cast<Boolean*>(tmp.get());
    if(p == nullptr) return conseq->eval(e);
    if(p->b) return conseq->eval(e);
    return alter->eval(e);
} // if expression

Value True::eval(Assoc &e) {return BooleanV(true);} // evaluation of #t

Value False::eval(Assoc &e) {return BooleanV(false);} // evaluation of #f

Value Begin::eval(Assoc &e) {
    if(!es.size()) return NullV();
    return es[es.size()-1]->eval(e);
} // begin expression

Value Quote::eval(Assoc &e) {
    if(s->gettype()==E_FIXNUM){
        Number *p = dynamic_cast<Number*>(s.get());
        return IntegerV(p->n);
    }
    if(s->gettype()==E_TRUE){
        TrueSyntax *p = dynamic_cast<TrueSyntax*>(s.get());
        return BooleanV(true);
    }
    if(s->gettype()==E_FALSE){
        FalseSyntax *p = dynamic_cast<FalseSyntax*>(s.get());
        return BooleanV(false);
    }
    if(s->gettype()==E_LIST){
        List *p = dynamic_cast<List*>(s.get());
        if(!(p->stxs.size())) return NullV();
        Value v = PairV(Quote(p->stxs[p->stxs.size()-1]).eval(e), NullV());
        Value val = PairV(Quote(p->stxs[p->stxs.size()-1]).eval(e), NullV());
        for(int i=p->stxs.size()-2;i>=0;i--){
            val = PairV(Quote(p->stxs[i]).eval(e), v);
            v = val;
        }
        return val;
    }
    Identifier *p = dynamic_cast<Identifier*>(s.get());
    return SymbolV(p->s);
} // quote expression

Value MakeVoid::eval(Assoc &e) {return VoidV();} // (void)

Value Exit::eval(Assoc &e) {return TerminateV();} // (exit)

Value Binary::eval(Assoc &e) {
    //std::cout<<"+"<<std::endl;
    Value a=rand1->eval(e), b=rand2->eval(e);
    // a->show(std::cout);std::cout<<" ";b->show(std::cout);std::cout<<std::endl;
    if(e_type == E_MUL) {
        Mult *p = dynamic_cast<Mult*>(this); 
        return p->evalRator(a, b);
    }
    if(e_type == E_PLUS) {
        //std::cout<<1<<std::endl;
        Plus *p = dynamic_cast<Plus*>(this);
        return p->evalRator(a, b);
    }
    if(e_type == E_MINUS) {
        Minus *p = dynamic_cast<Minus*>(this);
        return p->evalRator(a, b);
    }
    if(e_type == E_LT) {
        Less *p = dynamic_cast<Less*>(this);
        return p->evalRator(a, b);
    }
    if(e_type == E_LE) {
        LessEq *p = dynamic_cast<LessEq*>(this);
        return p->evalRator(a, b);
    }
    if(e_type == E_EQ) {
        Equal *p = dynamic_cast<Equal*>(this);
        return p->evalRator(a, b);
    }
    if(e_type == E_GE) {
        GreaterEq *p = dynamic_cast<GreaterEq*>(this);
        return p->evalRator(a, b);
    }
    if(e_type == E_GT) {
        Greater *p = dynamic_cast<Greater*>(this);
        return p->evalRator(a, b);
    }
    if(e_type == E_CONS) {
        Cons *p = dynamic_cast<Cons*>(this);
        return p->evalRator(a, b);
    }
    if(e_type == E_EQQ) {
        IsEq *p = dynamic_cast<IsEq*>(this);
        return p->evalRator(a, b);
    }
    return NullV();
} // evaluation of two-operators primitive

Value Unary::eval(Assoc &e) {
    Value v=rand->eval(e);
    if(e_type == E_BOOLQ) {
        IsBoolean *p = dynamic_cast<IsBoolean*>(this);
        return p->evalRator(v);
    }
    if(e_type == E_INTQ) {
        IsFixnum *p = dynamic_cast<IsFixnum*>(this);
        return p->evalRator(v); 
    }
    if(e_type == E_NULLQ) {
        IsNull *p = dynamic_cast<IsNull*>(this);
        return p->evalRator(v);
    }
    if(e_type == E_PAIRQ) {
        IsPair *p = dynamic_cast<IsPair*>(this);
        return p->evalRator(v);
    }
    if(e_type == E_PROCQ) {
        IsProcedure *p = dynamic_cast<IsProcedure*>(this);
        return p->evalRator(v);
    }
    if(e_type == E_SYMBOLQ) {
        IsSymbol *p = dynamic_cast<IsSymbol*>(this);
        return p->evalRator(v);
    }
    if(e_type == E_CAR) {
        Car *p = dynamic_cast<Car*>(this);
        return p->evalRator(v);
    }
    if(e_type == E_CDR) {
        Cdr *p = dynamic_cast<Cdr*>(this);
        return p->evalRator(v);
    }
    if(e_type == E_NOT) {
        Not *p = dynamic_cast<Not*>(this);
        return p->evalRator(v);
    }
    return NullV();
} // evaluation of single-operator primitive

Value Mult::evalRator(const Value &rand1, const Value &rand2) {
    if(rand1->v_type!=V_INT || rand2->v_type!=V_INT) throw RuntimeError("RE");
    Integer *a = dynamic_cast<Integer*>(rand1.get()), *b = dynamic_cast<Integer*>(rand2.get());
    //std::cout<<a->n<<" "<<b->n<<std::endl;
    return IntegerV((a->n)*(b->n));
} // *

Value Plus::evalRator(const Value &rand1, const Value &rand2) {
    //std::cout<<rand1->v_type<<" "<<rand2->v_type<<std::endl;
    if(rand1->v_type!=V_INT || rand2->v_type!=V_INT) throw RuntimeError("RE");
    Integer *a = dynamic_cast<Integer*>(rand1.get()), *b = dynamic_cast<Integer*>(rand2.get());
    //std::cout<<a->n<<" "<<b->n<<std::endl;
    return IntegerV((a->n)+(b->n));
} // +

Value Minus::evalRator(const Value &rand1, const Value &rand2) {
    if(rand1->v_type!=V_INT || rand2->v_type!=V_INT) throw RuntimeError("RE");
    Integer *a = dynamic_cast<Integer*>(rand1.get()), *b = dynamic_cast<Integer*>(rand2.get());
    return IntegerV((a->n)-(b->n));
} // -

Value Less::evalRator(const Value &rand1, const Value &rand2) {
    if(rand1->v_type!=V_INT || rand2->v_type!=V_INT) throw RuntimeError("RE");
    Integer *a = dynamic_cast<Integer*>(rand1.get()), *b = dynamic_cast<Integer*>(rand2.get());
    return BooleanV((a->n)<(b->n));
} // <

Value LessEq::evalRator(const Value &rand1, const Value &rand2) {
    if(rand1->v_type!=V_INT || rand2->v_type!=V_INT) throw RuntimeError("RE");
    Integer *a = dynamic_cast<Integer*>(rand1.get()), *b = dynamic_cast<Integer*>(rand2.get());
    return BooleanV((a->n)<=(b->n));
} // <=

Value Equal::evalRator(const Value &rand1, const Value &rand2) {
    if(rand1->v_type!=V_INT || rand2->v_type!=V_INT) throw RuntimeError("RE");
    Integer *a = dynamic_cast<Integer*>(rand1.get()), *b = dynamic_cast<Integer*>(rand2.get());
    return BooleanV((a->n)==(b->n));
} // =

Value GreaterEq::evalRator(const Value &rand1, const Value &rand2) {
    if(rand1->v_type!=V_INT || rand2->v_type!=V_INT) throw RuntimeError("RE");
    Integer *a = dynamic_cast<Integer*>(rand1.get()), *b = dynamic_cast<Integer*>(rand2.get());
    return BooleanV((a->n)>=(b->n));
} // >=

Value Greater::evalRator(const Value &rand1, const Value &rand2) {
    if(rand1->v_type!=V_INT || rand2->v_type!=V_INT) throw RuntimeError("RE");
    Integer *a = dynamic_cast<Integer*>(rand1.get()), *b = dynamic_cast<Integer*>(rand2.get());
    return BooleanV((a->n)>(b->n));
} // >

Value IsEq::evalRator(const Value &rand1, const Value &rand2) {
    if(rand1->v_type != rand2->v_type) return BooleanV(false);
    if(rand1->v_type == V_INT) {
        Integer *a = dynamic_cast<Integer*>(rand1.get()), *b = dynamic_cast<Integer*>(rand2.get());
        return BooleanV((a->n)==(b->n));
    }
    if(rand1->v_type == V_BOOL) {
        Boolean *a = dynamic_cast<Boolean*>(rand1.get()), *b = dynamic_cast<Boolean*>(rand2.get());
        return BooleanV((a->b)==(b->b));
    }
    if(rand1->v_type == V_SYM) {
        Symbol *a = dynamic_cast<Symbol*>(rand1.get()), *b = dynamic_cast<Symbol*>(rand2.get());
        return BooleanV((a->s)==(b->s));
    }
    return BooleanV(rand1.get() == rand2.get());
} // eq?

Value Cons::evalRator(const Value &rand1, const Value &rand2) {
    return PairV(rand1, rand2);
} // cons

Value IsBoolean::evalRator(const Value &rand) {
    return BooleanV(rand->v_type == V_BOOL);
} // boolean?

Value IsFixnum::evalRator(const Value &rand) {
    return BooleanV(rand->v_type == V_INT);
} // fixnum?

Value IsNull::evalRator(const Value &rand) {
    return BooleanV(rand->v_type == V_NULL);
} // null?

Value IsPair::evalRator(const Value &rand) {
    if(rand->v_type != V_PAIR) return BooleanV(false);
    Pair *p = dynamic_cast<Pair*>(rand.get());
    if(p->cdr.get()==nullptr || p->car.get()==nullptr) return BooleanV(false);
    return BooleanV(true);
} // pair?

Value IsProcedure::evalRator(const Value &rand) {
    return BooleanV(rand->v_type == V_PROC);
} // procedure?

Value IsSymbol::evalRator(const Value &rand) {
    return BooleanV(rand->v_type == V_SYM);
} // symbol?

Value Not::evalRator(const Value &rand) {
    if(rand->v_type == V_BOOL) {
        Boolean *p = dynamic_cast<Boolean*>(rand.get());
        if(p->b == false) return BooleanV(true);
    }
    return BooleanV(false);
} // not

Value Car::evalRator(const Value &rand) {
    if(rand->v_type!=V_PAIR) throw RuntimeError("RE");
    Pair *p = dynamic_cast<Pair*>(rand.get());
    return p->car;
} // car

Value Cdr::evalRator(const Value &rand) {
    if(rand->v_type!=V_PAIR) throw RuntimeError("RE");
    Pair *p = dynamic_cast<Pair*>(rand.get());
    return p->cdr;
} // cdr