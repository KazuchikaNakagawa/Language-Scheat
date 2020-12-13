//
//  Classes.h
//  ProjectScheat
//
//  Created by かずちか on 2020/10/02.
//

#ifndef Classes_h
#define Classes_h
#include "ScheatBasicStructures.h"
#include "ScheatObjects.h"

namespace scheat {
//using scheat::Token;
class Scheat;
class Token;

class ScheatLexer {
public:
    virtual void lex(Scheat *) {};
    virtual Token *getNextTok() { return nullptr; };
    virtual Token *eatThisTok() { return nullptr; };
    virtual ~ScheatLexer() {};
};

class ScheatAnalyzer {
public:
    virtual void analyze(Scheat *);
    ~ScheatAnalyzer() {};
};

class ScheatEncoder {
public:
    virtual void encode(Scheat *);
    virtual ~ScheatEncoder() {};
};


}

#endif /* Classes_h */
