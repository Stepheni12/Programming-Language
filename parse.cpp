/*
 * parse.cpp
 *
 *  Created on: Mar 8, 2018
 *      Author: gerardryan
 */

#include "parse.h"
#include "value.h"
#include <set>
using namespace std;
static set<string> vars;

map<string, Value> Sym;

namespace Parser {
bool pushed_back = false;
Token	pushed_token;

static Token GetNextToken(istream *in, int *line) {
	if( pushed_back ) {
		pushed_back = false;
		return pushed_token;
	}
	return getNextToken(in, line);
}

static void PushBackToken(Token& t) {
	if( pushed_back ) {
		abort();
	}
	pushed_back = true;
	pushed_token = t;
}

}

static int error_count = 0;

void
ParseError(int line, string msg)
{
	++error_count;
	cout << line << ": " << msg << endl;
}

ParseTree *Prog(istream *in, int *line)
{
	ParseTree *sl = Slist(in, line);

	if( sl == 0 )
		ParseError(*line, "No statements in program");

	if( error_count )
		return 0;

	return sl;
}

// Slist is a Statement followed by a Statement List
ParseTree *Slist(istream *in, int *line) {
	ParseTree *s = Stmt(in, line);
	if( s == 0 )
		return 0;

	if( Parser::GetNextToken(in, line) != SC ) {
		ParseError(*line, "Missing semicolon");
		delete s;
		return 0;
	}

	return new StmtList(s, Slist(in,line));
}

ParseTree *Stmt(istream *in, int *line) {
	ParseTree *s;

	Token t = Parser::GetNextToken(in, line);
	switch( t.GetTokenType() ) {
	case VAR:
		s = VarStmt(in, line);
		break;

	case SET:
		s = SetStmt(in,line);
		break;

	case PRINT:
		s = PrintStmt(in, line);
		break;

	case REPEAT:
		s = RepeatStmt(in, line);
		break;

	case DONE:
		return 0;

	default:
		ParseError(*line, "Invalid statement");
		return 0;
	}
	return s;
}

ParseTree *VarStmt(istream *in, int *line) {
	Token t = Parser::GetNextToken(in, line);

	if( t != IDENT ) {
		ParseError(*line, "Missing identifier after var");
		return 0;
	}

	ParseTree *ex = Expr(in, line);
	if( ex == 0 ) {
		ParseError(*line, "Missing expression after identifier");
		return 0;
	}

	return new VarDecl(t, ex);


	return 0;
}

ParseTree *SetStmt(istream *in, int *line) {
	Token t = Parser::GetNextToken(in, line);

	if( t != IDENT ) {
		ParseError(*line, "Missing identifier after set");
		return 0;
	}

	ParseTree *ex = Expr(in, line);
	if( ex == 0 ) {
		ParseError(*line, "Missing expression after identifier");
		return 0;
	}
		return new Assignment(t, ex);
}

ParseTree *PrintStmt(istream *in, int *line) {
	int l = *line;

	ParseTree *ex = Expr(in, line);
	if( ex == 0 ) {
		ParseError(*line, "Missing expression after print");
		return 0;
	}

	return new Print(l, ex);
}

ParseTree *RepeatStmt(istream *in, int *line) {
	int l = *line;

	ParseTree *ex = Expr(in, line);
	if( ex == 0 ) {
		ParseError(*line, "Missing expression after repeat");
		return 0;
	}

	ParseTree *st = Stmt(in, line);
	if( st == 0 ) {
		ParseError(*line, "Missing statement for repeat");
		return 0;
	}

	return new Repeat(l, ex, st);
}

ParseTree *Expr(istream *in, int *line) {
	ParseTree *t1 = Term(in, line);
	if( t1 == 0 ) {
		return 0;
	}

	while ( true ) {
		Token t = Parser::GetNextToken(in, line);

		if( t != PLUS && t != MINUS ) {
			Parser::PushBackToken(t);
			return t1;
		}

		ParseTree *t2 = Term(in, line);
		if( t2 == 0 ) {
			ParseError(*line, "Missing expression after operator");
			return 0;
		}

		if( t == PLUS )
			t1 = new PlusExpr(t.GetLinenum(), t1, t2);
		else
			t1 = new MinusExpr(t.GetLinenum(), t1, t2);
	}
}

ParseTree *Term(istream *in, int *line) {
	ParseTree *t1 = Factor(in, line);
	if( t1 == 0 ) {
		return 0;
	}

	while ( true ) {
		Token t = Parser::GetNextToken(in, line);

		if( t != STAR ) {
			Parser::PushBackToken(t);
			return t1;
		}

		ParseTree *t2 = Factor(in, line);
		if( t2 == 0 ) {
			ParseError(*line, "Missing expression after operator");
			return 0;
		}

		t1 = new TimesExpr(t.GetLinenum(), t1, t2);
	}
}

ParseTree *Factor(istream *in, int *line) {
	ParseTree *p1 = Primary(in, line);

	while ( true ) {
		Token t = Parser::GetNextToken(in, line);

		if( t != LSQ ) {
			Parser::PushBackToken(t);
			return p1;
		}

		ParseTree *e1 = Expr(in, line);
		if( e1 == 0 ) {
			ParseError(*line, "Missing expression after [");
			return 0;
		}

		if( Parser::GetNextToken(in,line) != COLON ) {
			ParseError(*line, "Missing colon after expression");
			return 0;
		}

		ParseTree *e2 = Expr(in, line);
		if( e2 == 0 ) {
			ParseError(*line, "Missing expression after :");
			return 0;
		}

		if( Parser::GetNextToken(in,line) != RSQ ) {
			ParseError(*line, "Missing ] after expression");
			return 0;
		}

		p1 = new SliceExpr( t.GetLinenum(), p1, new SliceOperand(*line,e1,e2));
	}
}

ParseTree *Primary(istream *in, int *line) {
	Token t = Parser::GetNextToken(in, line);

	if( t == IDENT ) {
		return new Ident(t);
	}
	else if( t == ICONST ) {
		return new IConst(t);
	}
	else if( t == SCONST ) {
		return new SConst(t);
	}
	else if( t == LPAREN ) {
		ParseTree *ex = Expr(in, line);
		if( ex == 0 ) {
			ParseError(*line, "Missing expression after repeat");
			return 0;
		}
		if( Parser::GetNextToken(in, line) == RPAREN )
			return ex;

		ParseError(*line, "Missing ) after expression");
		return 0;
	}

	ParseError(*line, "Primary expected");
	return 0;
}
