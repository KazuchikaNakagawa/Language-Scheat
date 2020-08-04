//
//  scheat.hpp
//  scheat
//
//  Created by かずちか on 2020/07/30.
//

#ifndef scheat_
#define scheat_

/* The classes below are exported */
#pragma GCC visibility push(default)
#include <string>
#include "Lexer.hpp"

namespace scheat{

/// Scheat needs this object to move.
class Scheat
{
    /// version.section.part
    /// 1.0.0
    int version;
    
    int section;
    
    int part;
    
    const char *target;
    
    bool debug;
    
public:
    Scheat(int version = 1, int section = 0, int part = 0, const char *target = "", bool debugOpt = false);
    void HelloWorld(const char *);
    __deprecated void old_FatalError(const char *, unsigned int);
    void FatalError(unsigned int, const char *, ...);
    __deprecated void old_Debug(const char *, unsigned int);
    void Debug(unsigned int, const char *, ...);
    void flagDebug() { debug = !debug; };
};

class IRBuilder{
    
public:
    IRBuilder(Scheat*, std::string);
};

}

#pragma GCC visibility pop
#endif
