/*
 * lex.cpp
 *
 * CS280 - Fall 2017
 */

#include <cctype>
#include <map>
using std::map;

#include "projlex.h"

static map<TType,string> tokenPrint = {
	{ SET, "SET" },
	{ PRINT, "PRINT" },
	{ VAR, "VAR" },
	{ REPEAT, "REPEAT" },

	{ IDENT, "IDENT" },

	{ ICONST, "ICONST" },
	{ SCONST, "SCONST" },

	{ PLUS, "PLUS" },
	{ MINUS, "MINUS" },
	{ STAR, "STAR" },
	{ COLON, "COLON" },
	{ LSQ, "LSQ" },
	{ RSQ, "RSQ" },
	{ LPAREN, "LPAREN" },
	{ RPAREN, "RPAREN" },
	{ SC, "SC" },

	{ ERR, "ERR" },

	{ DONE, "DONE" }
};


ostream& operator<<(ostream& out, const Token& tok) {
	TType tt = tok.GetTokenType();
	out << tokenPrint[ tt ];
	if( tt == IDENT || tt == ICONST || tt == SCONST || tt == ERR ) {
		out << "(" << tok.GetLexeme() << ")";
	}
	return out;
}


static map<string,TType> kwmap = {
		{ "var", VAR },
		{ "set", SET },
		{ "print", PRINT },
		{ "repeat", REPEAT },
};

Token
id_or_kw(const string& lexeme, int linenum)
{
	TType tt = IDENT;

	auto kIt = kwmap.find(lexeme);
	if( kIt != kwmap.end() )
		tt = kIt->second;

	return Token(tt, lexeme, linenum);
}


Token
getNextToken(istream *in, int *linenum)
{
	enum LexState { BEGIN, INID, INSTRING, SAWMINUS, ININT, INCOMMENT } lexstate = BEGIN;
	string lexeme;
	char ch;

	while(in->get(ch)) {

		if( ch == '\n' ) {
			(*linenum)++;
		}

		switch( lexstate ) {
		case BEGIN:
			if( isspace(ch) )
				continue;

			lexeme = ch;

			if( isalpha(ch) ) {
				lexstate = INID;
			}
			else if( ch == '"' ) {
				lexstate = INSTRING;
			}
			else if( ch == '-' ) {
				lexstate = SAWMINUS;
			}
			else if( isdigit(ch) ) {
				lexstate = ININT;
			}
			else if( ch == '#' ) {
				lexstate = INCOMMENT;
			}
			else {
				TType tt = ERR;
				switch( ch ) {
				case '+':
					tt = PLUS;
					break;
				case '*':
					tt = STAR;
					break;
				case ':':
					tt = COLON;
					break;
				case '[':
					tt = LSQ;
					break;
				case ']':
					tt = RSQ;
					break;
				case '(':
					tt = LPAREN;
					break;
				case ')':
					tt = RPAREN;
					break;
				case ';':
					tt = SC;
					break;
				}

				return Token(tt, lexeme, *linenum);
			}
			break;

		case INID:
			if( isalpha(ch) || isdigit(ch) ) {
				lexeme += ch;
			}
			else {
				if( ch == '\n' )
					(*linenum)--;
				in->putback(ch);
				return id_or_kw(lexeme, *linenum);
			}
			break;

		case INSTRING:
			lexeme += ch;
			if( ch == '\n' ) {
				return Token(ERR, lexeme, *linenum );
			}
			if( ch == '"' ) {
				lexeme = lexeme.substr(1, lexeme.length()-2);
				return Token(SCONST, lexeme, *linenum );
			}
			break;

		case SAWMINUS:
			if( !isdigit(ch) ) {
				if( ch == '\n' )
					(*linenum)--;
				in->putback(ch);
				return Token(MINUS, lexeme, *linenum);
			}
			else {
				lexeme += ch;
				lexstate = ININT;
			}
			break;

		case ININT:
			if( isdigit(ch) ) {
				lexeme += ch;
			}
			else if( isalpha(ch) ) {
				lexeme += ch;
				return Token(ERR, lexeme, *linenum);
			}
			else {
				if( ch == '\n' )
					(*linenum)--;
				in->putback(ch);
				return Token(ICONST, lexeme, *linenum);
			}
			break;

		case INCOMMENT:
			if( ch == '\n' ) {
				lexstate = BEGIN;
			}
			break;
		}

	}

	if( in->eof() )
		return Token(DONE, "", *linenum);
	return Token(ERR, "some strange I/O error", *linenum);
}
