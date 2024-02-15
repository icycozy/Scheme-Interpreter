#include "Def.hpp"
#include "syntax.hpp"
#include "expr.hpp"
#include "value.hpp"
#include "RE.hpp"
#include <sstream>
#include <iostream>
#include <map>


extern std :: map<std :: string, ExprType> primitives;
extern std :: map<std :: string, ExprType> reserved_words;

void REPL()
{
    // read - evaluation - print loop 
    while (1)         
    {
        Assoc global_env = empty();
        printf("scm> ");
        Syntax stx = readSyntax(std :: cin); // read
        try
        {
            Expr expr = stx -> parse(global_env); // parse
            // std::cout<<"parse done"<<std::endl;
            // stx -> show(std :: cout); // syntax print
            global_env = empty();
            Value val = expr -> eval(global_env);
            //std::cout<<"???"<<std::endl;
            if (val -> v_type == V_TERMINATE)
                break;
            val -> show(std :: cout); // value print
        }
        catch (const RuntimeError &RE)
        {
            // std :: cout << RE.message();
            std :: cout << "RuntimeError";
        }
        puts("");
    }
}


int main(int argc, char *argv[]) {
    initPrimitives();
    initReservedWords();
    REPL();
    return 0;
}