//
//  YANodes.hpp
//  scheat
//
//  Created by かずちか on 2020/09/26.
//

#ifndef YANodes_hpp
#define YANodes_hpp

#include <stdio.h>
#include <string>
#include "ScheatBasicStructures.h"
#include "ScheatObjects.h"

namespace scheat {
using namespace std;
using namespace basics;

namespace nodes2 {

class Value {
public:
    TypeData type;
    string value;
    Value(string s, TypeData t) : value(s), type(t) {};
};

class Node {
public:
    SourceLocation location;
    TypeData type;
    virtual Value *codegen(IRStream &) {return nullptr;};
    virtual string userdump() { return "UNDEFINED"; };
    virtual ~Node() {};
};

class Expr;

//--------------------------------------------------------------//

// template class of Statements
class StatementNode : public Node{
public:
    Value * codegen(IRStream &) override{ return nullptr; };
    string userdump() override{ return "UNDEFINED"; };
    virtual ~StatementNode() {};
};

class TermNode : public Node {
public:
    Value * codegen(IRStream &) override{return nullptr;};
    string userdump() override{return "UNDEFINED";};
    virtual ~TermNode() {};
};

// -------------------------------------------------------------//
// specific classes
class IntTerm : public TermNode {
public:
    int value;
    IntTerm(Token *t) : value(t->value.intValue) { this->type = TypeData("Int", "i32");
        location = t->location;
    };
    Value * codegen(IRStream &) override;
    string userdump() override{ return to_string(value); };
};

class StringTerm : public TermNode {
public:
    // to do
    /*
     by using bitcast, (i8* @.str to %String*) we can generate String
     object directly.
     @.str = unnamed_addr inbound [7 x i8] "Hello"
     @s = bitcast (i8* getelementptr ()() to %String*)
     */
    string value;
    StringTerm(Token *t) : value(t->value.strValue) { this->type = TypeData("the Character", "i8*");
        location = t->location;
    };
    Value * codegen(IRStream &) override;
    string userdump() override{return value;};
};

class BoolTerm : public TermNode {
public:
    bool value;
    BoolTerm(Token *t) : value(t->value.boolValue) {
        type = TypeData("Bool", "i1");
        location = t->location;
    };
    Value * codegen(IRStream &) override;
    string userdump() override{
        if (value) {
            return "true";
        }else{
            return "false";
        }
    };
};

class FloatTerm : public TermNode {
public:
    double value;
    FloatTerm(Token *t) : value(t->value.doubleValue) {
        type = TypeData("Float", "double");
        location = t->location;
    };
    Value * codegen(IRStream &) override;
    string userdump() override{
        return to_string(value);
    };
};

// idterm : identifier
//        | identifier ( expr , expr, ... )
class IdentifierTerm : public TermNode {
public:
    string value;
    vector<unique_ptr<Expr>> args = {};
    IdentifierTerm(Token *t, TypeData ty, int ind = 0){
        type = ty;
        value = t->value.strValue;
        location = t->location;
        index = ind;
    }
    IdentifierTerm(Token *t, int c, TypeData ty);
    // when this means function, it is used for check
    int countOfArgs = 0;
    bool isFunc = false;
    // property index. used when generating getelementptr
    int index = 0;
    Function *funcptr = nullptr;
    void addArg(unique_ptr<Expr>);
    Value * codegen(IRStream &) override;
    string userdump() override;
};

// idexpr : idterm
//        | idexpr . idterm
class IdentifierExpr : public TermNode {
public:
    unique_ptr<IdentifierExpr> lhs = nullptr;
    Token *perTok = nullptr;
    unique_ptr<IdentifierTerm> rhs = nullptr;
    IdentifierExpr(unique_ptr<IdentifierTerm>,TypeData);
    IdentifierExpr(unique_ptr<IdentifierExpr>, Token *, unique_ptr<IdentifierTerm>,TypeData);
    static unique_ptr<IdentifierExpr> init(unique_ptr<IdentifierTerm>,
                                           TypeData);
    static
    unique_ptr<IdentifierExpr> initAsAccessExpr(
                                                unique_ptr<IdentifierExpr>,
                                                Token *,
                                                unique_ptr<IdentifierTerm>);
    Value * codegen(IRStream &) override;
    Value * codegenAsRef(IRStream &);
    string userdump() override{
        if (lhs != nullptr && perTok != nullptr) {
            return lhs->userdump() + perTok->value.strValue + rhs->userdump();
        }
        return rhs->userdump();
    };
};

// -------------------------------------------------------------//

// term : TermNode
//      | TermNode OP term
//      | OP term
//      | term OP
class Term : public Node{
public:
    unique_ptr<TermNode> lhs = nullptr;
    Operator *op = nullptr;
    unique_ptr<Term> rhs = nullptr;
    Value * codegen(IRStream &) override;
    string userdump() override;
    __deprecated_msg("this class is for unique_ptr")
    Term() {};
    static unique_ptr<Term> init(unique_ptr<TermNode>);
    static unique_ptr<Term> initAsInfixOperatorExpr(unique_ptr<TermNode>,
                                               Operator *,
                                               unique_ptr<Term>);
    static unique_ptr<Term> initAsPrefixOperatorExpr(Operator *,
                                                     unique_ptr<Term>);
    static unique_ptr<Term> initAsPostfixOperatorExpr(unique_ptr<Term>,
                                                      Operator *);
    
};

class PrimaryExprNode : public Node {
public:
    Value * codegen(IRStream &) override{ return nullptr; };
    string userdump() override{ return "UNDEFINED"; };
};

// primary : term
//         | term OP primary
//         | OP primary
//         | primary OP
class PrimaryExpr : public Node {
public:
    bool syntaxedExpr = false;
    unique_ptr<Term> lhs;
    Operator *op;
    unique_ptr<PrimaryExpr> rhs;
    unique_ptr<PrimaryExprNode> syntaxNode = nullptr;
    Value * codegen(IRStream &) override;
    string userdump() override;
    __deprecated_msg("this class is for unique_ptr")
    PrimaryExpr() {};
    static unique_ptr<PrimaryExpr> init(unique_ptr<Term>);
    static unique_ptr<PrimaryExpr> initAsOperatedExpr(unique_ptr<Term>,
                                               Operator *,
                                               unique_ptr<PrimaryExpr>);
    static unique_ptr<PrimaryExpr> initAsPrefixOperatorExpr(Operator *,
                                                     unique_ptr<PrimaryExpr>);
    static unique_ptr<PrimaryExpr> initAsPostfixOperatorExpr(unique_ptr<PrimaryExpr>,
                                                      Operator *);
    static unique_ptr<PrimaryExpr> initAsSyntaxExpr(unique_ptr<PrimaryExprNode>);
};

class ExprNode : public Node {
public:
    Value * codegen(IRStream &) override{ return nullptr; };
    string userdump() override {return "UNDEFINED";};
    
};

// expr : primary
//      | primary OP expr
//      | OP expr
//      | expr OP
//      | expr_syntax
class Expr : public Node {
public:
    bool syntaxedExpr = false;
    unique_ptr<PrimaryExpr> lhs;
    Operator *op;
    unique_ptr<Expr> rhs;
    unique_ptr<ExprNode> syntax = nullptr;
    Value * codegen(IRStream &) override;
    string userdump() override{return "UNDEFINED";};
    __deprecated_msg("this class is for unique_ptr")
    Expr() {};
    static unique_ptr<Expr> init(unique_ptr<PrimaryExpr>);
    static unique_ptr<Expr> initAsOperatedExpr(unique_ptr<PrimaryExpr>,
                                                      Operator *,
                                                      unique_ptr<Expr>);
    static unique_ptr<Expr> initAsPrefixOperatorExpr(Operator *,
                                                            unique_ptr<Expr>);
    static unique_ptr<Expr> initAsPostfixOperatorExpr(unique_ptr<Expr>,
                                                      Operator *);
    static unique_ptr<Expr> initAsSyntaxedExpr(unique_ptr<ExprNode>);
};

// statement : StatementNode .
//           | StatementNode , statement
class Statement : public Node {
public:
    unique_ptr<StatementNode> statement;
    Token *perTok;
    unique_ptr<Statement> stmt;
    Value * codegen(IRStream &) override;
    string userdump() override;
    Statement() {};
    Statement(unique_ptr<Statement> stn){
        stmt = move(stn);
    }
};

// statements : statement
//            | statements statement
class Statements : public Node {
public:
    unique_ptr<Statements> statements;
    unique_ptr<Statement> statement;
    Value * codegen(IRStream &) override;
    string userdump() override;
    Statements() {
        statement = nullptr;
        statements = nullptr;
    };
    
    Statements(unique_ptr<Statements> stmts, unique_ptr<Statement> stmt){
        statements = move(stmts);
        statement = move(stmt);
    };
};


// ----------------------------------------------------------//

// this ID is expr.
// expr : expr of IDExpr
// this statement is expected in global and local function
class DeclareVariableStatement : public StatementNode {
public:
    string name;
    
    bool isPublic;
    bool isConstant;
    bool isNullable;
    
    unique_ptr<Expr> value;
    static unique_ptr<DeclareVariableStatement>
    init(Token *, unique_ptr<Expr>,bool pub = true, bool con = false, bool nul = false);
    Value * codegen(IRStream &) override;
};

// ====================================
class DataHolder {
public:
    unique_ptr<Statements> statements = nullptr;
};

}

}

#endif /* YANodes_hpp */
