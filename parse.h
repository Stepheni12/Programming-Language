/*
 * parse.h
 *
 *  Created on: Mar 26, 2018
 *      Author: Stephen
 */

#ifndef PARSE_H_
#define PARSE_H_

#include <iostream>
using namespace std;

#include "projlex.h"
#include "parsetree.h"

extern ParseTree *Prog(istream *in, int *line);
extern ParseTree *Slist(istream *in, int *line);
extern ParseTree *Stmt(istream *in, int *line);
extern ParseTree *VarStmt(istream *in, int *line);
extern ParseTree *SetStmt(istream *in, int *line);
extern ParseTree *PrintStmt(istream *in, int *line);
extern ParseTree *RepeatStmt(istream *in, int *line);
extern ParseTree *Expr(istream *in, int *line);
extern ParseTree *Term(istream *in, int *line);
extern ParseTree *Factor(istream *in, int *line);
extern ParseTree *Primary(istream *in, int *line);

#endif /* PARSE_H_ */
