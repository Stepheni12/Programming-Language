/*
 * projlex.h
 *
 * CS280
 * Spring 2018
 */

#ifndef PROJLEX_H_
#define PROJLEX_H_

#include <string>
#include <iostream>
using std::string;
using std::istream;
using std::ostream;

enum TType {
		// keywords
	SET,
	PRINT,
	VAR,
	REPEAT,

		// an identifier
	IDENT,

		// an integer and string constant
	ICONST,
	SCONST,

		// the operators, parens and semicolon
	PLUS,
	MINUS,
	STAR,
	COLON,
	LSQ,
	RSQ,
	LPAREN,
	RPAREN,
	SC,

		// any error returns this token
	ERR,

		// when completed (EOF), return this token
	DONE
};

class Token {
	TType	tt;
	string		lexeme;
	int			lnum;

public:
	Token() {
		tt = ERR;
		lnum = -1;
	}
	Token(TType tt, string lexeme, int line) {
		this->tt = tt;
		this->lexeme = lexeme;
		this->lnum = line;
	}

	bool operator==(const TType tt) const { return this->tt == tt; }
	bool operator!=(const TType tt) const { return this->tt != tt; }

	TType		GetTokenType() const { return tt; }
	string		GetLexeme() const { return lexeme; }
	int			GetLinenum() const { return lnum; }
};

extern ostream& operator<<(ostream& out, const Token& tok);

extern Token getNextToken(istream *in, int *linenum);


#endif /* PROJLEX_H_ */
