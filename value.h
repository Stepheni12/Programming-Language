/*
 * value.h
 *
 *  Created on: Apr 16, 2018
 *      Author: Stephen
 */

#ifndef VALUE_H_
#define VALUE_H_

#include "parsetree.h"
#include <map>
class Value {
	int	ival;
	string sval;
	NodeType type;

public:
	Value() : ival(0),type(ERRTYPE) {}
	Value(int i) : ival(i),type(INTTYPE) {}
	Value(string s) : ival(0),sval(s),type(STRTYPE) {}

	NodeType GetType() const { return type; }

	int GetIntValue() const {
		if( type != INTTYPE )
			throw std::runtime_error("using GetIntValue on a Value that is not an INT");
		return ival;
	}

	string GetStrValue() const {
		if( type != STRTYPE )
			throw std::runtime_error("using GetStrValue on a Value that is not a STRING");
		return sval;
	}
};
#endif /* VALUE_H_ */
