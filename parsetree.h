/*
 * parsetree.h
 */

#ifndef PARSETREE_H_
#define PARSETREE_H_

#include <vector>
#include <map>
using std::vector;
#include <string>
#include <string.h>


// NodeType represents all possible types
enum NodeType { ERRTYPE, INTTYPE, STRTYPE };
// a "forward declaration" for a class to hold values
#include "value.h"

extern map<string, Value> Sym;

//Three variables for slicing
static string slice;
static int a = 0;
static int b = 0;

inline ostream& operator<<(ostream& out, const Value& val){
	if (val.GetType() == INTTYPE){
		out << val.GetIntValue();
	}
	else if(val.GetType() == STRTYPE){
		out << val.GetStrValue();
	}
	else{
		out << "ERROR." << endl;
	}
	return out;
}

inline Value operator+(const Value& val1, const Value& val2){
	if (val1.GetType() == INTTYPE && val2.GetType() == INTTYPE){
		return val1.GetIntValue() + val2.GetIntValue();
	}
	if (val1.GetType() == STRTYPE && val2.GetType() == STRTYPE){
		string str = val1.GetStrValue();
		return str.append(val2.GetStrValue());
	}
	cout << "ERROR." << endl;
	return 0;
}

inline Value operator-(const Value& val1, const Value& val2){
	if (val1.GetType() == INTTYPE && val2.GetType() == INTTYPE){
		return val1.GetIntValue() - val2.GetIntValue();
	}
	if (val1.GetType() == STRTYPE && val2.GetType() == STRTYPE){
		string str1 = val1.GetStrValue();
		string str2 = val2.GetStrValue();

		//Found this online for removing first occurrence of string from another
		std::string::size_type i = str1.find(str2);

		if (i != std::string::npos){
			str1.erase(i, str2.length());
		}
		return str1;
	}
	cout << "ERROR." << endl;
	return 0;
}

inline Value operator*(const Value& val1, const Value& val2){
	if (val1.GetType() == INTTYPE && val2.GetType() == INTTYPE){
		return val1.GetIntValue() * val2.GetIntValue();
	}
	if (val1.GetType() == STRTYPE && val2.GetType() == INTTYPE){
		int x = val2.GetIntValue();
		//if x is not greater than 0 then there's an error, where should that go?
		string str;
		for(int i = 0;i < x;i++){
			str.append(val1.GetStrValue());
		}
		return str;
	}
	else if (val1.GetType() == INTTYPE && val2.GetType() == STRTYPE){
		int x = val1.GetIntValue();
		string str;

		for(int i = 0;i < x;i++){
			str.append(val2.GetStrValue());
		}
		return str;
	}
	else{
		cout << "ERROR." << endl;
	}
	return 0;
}

class ParseTree {
	int			linenum;
	protected : ParseTree	*left;
	protected : ParseTree	*right;

public:
	ParseTree(int linenum, ParseTree *l = 0, ParseTree *r = 0)
		: linenum(linenum), left(l), right(r) {}

	virtual ~ParseTree() {
		delete left;
		delete right;
	}

	int GetLineNumber() const { return linenum; }

	virtual NodeType GetType() const { return ERRTYPE; }

	int LeafCount() const {
		int lc = 0;
		if( left ) lc += left->LeafCount();
		if( right ) lc += right->LeafCount();
		if( left == 0 && right == 0 )
			lc++;
		return lc;
	}

	virtual bool IsIdent() const { return false; }
	virtual bool IsVar() const { return false; }
	virtual string GetId() const { return ""; }

	int IdentCount() const {
		int cnt = 0;
		if( left ) cnt += left->IdentCount();
		if( right ) cnt += right->IdentCount();
		if( IsIdent() )
			cnt++;
		return cnt;
	}

	void GetVars(map<string,bool>& var) {
		if( left ) left->GetVars(var);
		if( right ) right->GetVars(var);
		if( IsVar() )
			var[ this->GetId() ] = true;
	}
	virtual Value Eval() = 0;
};

class StmtList : public ParseTree {

public:
	StmtList(ParseTree *l, ParseTree *r) : ParseTree(0, l, r) {}
	Value Eval(){
		if (left){
			left->Eval();
		}
		if (right){
			right->Eval();
		}
		return 0;
	}


};

class VarDecl : public ParseTree {
    string id;
    int line;

public:
    VarDecl(Token& t, ParseTree *ex) : ParseTree(t.GetLinenum(), ex), id(t.GetLexeme()), line(t.GetLinenum()) {}

    bool IsVar() const { return true; }
    string GetId() const { return id; }

    NodeType GetType() const { return left -> GetType(); }
    Value Eval() {
      auto expr = left->Eval();
      if (Sym.find(id) == Sym.end()){
    	  Sym[id] = expr;
      }
      else{
    	  cout << line << ": Variable already defined with that name." << endl;
      }

      return 0;
    }
};

class Assignment : public ParseTree {
	string id;
	int line;

public:
	Assignment(Token& t, ParseTree *e) : ParseTree(t.GetLinenum(), e), id(t.GetLexeme()),line(t.GetLinenum()) {}

	Value Eval(){
		if (left){
			auto expr = left->Eval();
			if (Sym[id].GetType() == expr.GetType()){
				Sym[id] = expr;
			}
			else{
				cout << line << ": Type mismatch." << endl;
			}

		}
		return 0;
	}
};

class Print : public ParseTree {
public:
	Print(int line, ParseTree *e) : ParseTree(line, e) {}

	Value Eval(){
		if (left){
			cout << left->Eval() << endl;
		}

		return 0;
	}
};

class Repeat : public ParseTree {

public:
	Repeat(int line, ParseTree *e, ParseTree *s) : ParseTree(line, e, s) {}

	int rep = 0;

	Value Eval(){
		if (left){
			rep = left->Eval().GetIntValue();
			if (rep <= 0){
				cout << "RUNTIME ERROR: Repititons must be greater than 0.";
			}
		}
		if (right){
			for (int i = 0;i < rep;i++){
				right->Eval();
			}
		}
		return 0;
	}
};


class PlusExpr : public ParseTree {
public:
	PlusExpr(int line, ParseTree *l, ParseTree *r) : ParseTree(line,l,r) {}

	NodeType GetType() const {
		NodeType lt = left->GetType();
		NodeType rt = right->GetType();

		if (lt == rt && lt != ERRTYPE){
			return lt;
		}

		return ERRTYPE;
	}
	Value Eval(){
		auto a1 = left->Eval();
		auto a2 = right->Eval();

		return a1 + a2;
	}
};

class MinusExpr : public ParseTree {
public:
	MinusExpr(int line, ParseTree *l, ParseTree *r) : ParseTree(line,l,r) {}


	NodeType GetType() const {
		NodeType lt = left->GetType();
		NodeType rt = right->GetType();

		if (lt == rt && lt != ERRTYPE){
			return lt;
		}

		return ERRTYPE;
	}
	Value Eval(){
		auto a1 = left->Eval();
		auto a2 = right->Eval();

		return a1 - a2;
	}
};

class TimesExpr : public ParseTree {
public:
	TimesExpr(int line, ParseTree *l, ParseTree *r) : ParseTree(line,l,r) {}

	NodeType GetType() {
		NodeType lt = left->GetType();
		NodeType rt = right->GetType();

		if (lt == rt && lt == INTTYPE){
			return INTTYPE;
		}

		if (lt != rt && (lt != ERRTYPE && rt != ERRTYPE)){
			return STRTYPE;
		}
		return ERRTYPE;
	}
	Value Eval(){
		auto a1 = left->Eval();
		auto a2 = right->Eval();

		return a1 * a2;
	}
};

class SliceExpr : public ParseTree {
	int line;
public:
	SliceExpr(int line, ParseTree *l, ParseTree *r) : ParseTree(line,l,r), line(line){}

	NodeType GetType() {
		NodeType lt = left->GetType();
		NodeType rt = right->GetType();

		if (lt == STRTYPE && rt == INTTYPE){
			return STRTYPE;
		}

		return ERRTYPE;
	}
	Value Eval(){
		if (left){
			if (left->Eval().GetType() != STRTYPE){
				cout << line << ": Can't slice an integer." << endl;
			}
			else{
				slice = left->Eval().GetStrValue();
			}

		}
		if (right){
			right->Eval();
		}
		if (a >= 0 && b >= a && b <= (long) slice.length()){
			return slice.substr(a, (b - a + 1));
		}
		else{
			string err = "RUNTIME ERROR: Invalid slice operands.";
			return err;
		}

		return 0;
	}
};

class SliceOperand : public ParseTree {
	int linenum;
public:
	SliceOperand(int line, ParseTree *l, ParseTree *r) : ParseTree(line,l,r),linenum(line) {}

	NodeType GetType() const {
		NodeType lt = left->GetType();
		NodeType rt = right->GetType();

		if (lt == rt && lt != ERRTYPE){
			return lt;
		}

		return ERRTYPE;
	}

	Value Eval(){
		if (left){
			if (left->GetType() == STRTYPE){
				cout << linenum << ": Slice operands must be integers." << endl;
			}else{
				a = left->Eval().GetIntValue();
			}

		}
		if (right){
			if (right->GetType() == STRTYPE){
				cout << linenum << ": Slice operands must be integers." << endl;
			}
			else{
				b = right->Eval().GetIntValue();
			}

		}
		return 0;
	}

};

class IConst : public ParseTree {
	int val;

public:
	IConst(Token& t) : ParseTree(t.GetLinenum()) {
		val = stoi(t.GetLexeme());
	}

	NodeType GetType() const { return INTTYPE; }

	Value Eval() {
		return Value(val);
	}
};

class SConst : public ParseTree {
	string val;

public:
	SConst(Token& t) : ParseTree(t.GetLinenum()) {
		val = t.GetLexeme();
	}

	NodeType GetType() const { return STRTYPE; }

	Value Eval(){
		return Value(val);
	}
};

class Ident : public ParseTree {
	string id;

public:
	Ident(Token& t) : ParseTree(t.GetLinenum()), id(t.GetLexeme()) {}

	Value Eval(){
		if (Sym[id].GetType() == INTTYPE){
			return Value(Sym[id].GetIntValue());         //Should this be value??? or just the int or string
		}
		return Value(Sym[id].GetStrValue());
	}

	bool IsIdent() const { return true; }
	string GetId() const { return id; }
};

#endif /* PARSETREE_H_ */
