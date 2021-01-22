#ifndef COMPILER_H
#define COMPILER_H

#include <string>
#include "bits.h"

// Declarations

class Expression {
public:
	virtual std::string code() = 0;
	virtual std::string access();
	virtual uint evaluate() = 0;
	virtual bool is_const_zero() { return false; }
};

typedef Expression* expression_t;

class Constant : public Expression {
	uint c;
public:
	Constant(uint);
	std::string code() override;
	std::string access() override;
	uint evaluate() override;
	bool is_const_zero() override;
};
Constant *constant(uint);

class Variable : public Expression {
	uint v;
public:
	Variable(uint);
	void assign(uint);
	std::string code();
	uint evaluate();
};
Variable *variable(uint);

typedef Variable* variable_t;

class Shr : public Expression {
	Expression *a, *b;
public:
	Shr(Expression *, Expression *);
	std::string code();
	uint evaluate();
};
Shr *shr(Expression *a, Expression *b);

class Sar : public Expression {
	Expression *a, *b;
public:
	Sar(Expression *, Expression *);
	std::string code();
	uint evaluate();
};
Sar *sar(Expression *a, Expression *b);

class Umulhi : public Expression {
	Expression *a, *b;
public:
	Umulhi(Expression *, Expression *);
	std::string code();
	uint evaluate();
};
Umulhi* umulhi(Expression*, Expression*);

class Imulhi : public Expression {
	Expression *a, *b;
public:
	Imulhi(Expression *, Expression *);
	std::string code();
	uint evaluate();
};
Imulhi* imulhi(Expression*, Expression*);

class Add : public Expression {
	Expression *a, *b;
public:
	Add(Expression *, Expression *);
	std::string code();
	uint evaluate();
};
Add* add(Expression *, Expression *);

class Sub : public Expression {
	Expression *a, *b;
public:
	Sub(Expression *, Expression *);
	std::string code();
	uint evaluate();
};
Sub* sub(Expression *, Expression *);

class Neg : public Expression {
	Expression *a;
public:
	Neg(Expression *);
	std::string code();
	uint evaluate();
};
Neg* neg(Expression *);

class Gte : public Expression {
	Expression *a, *b;
public:
	Gte(Expression *, Expression *);
	std::string code();
	uint evaluate();
};
Gte *gte(Expression *, Expression *);

class Sbb : public Expression {
	Expression *a, *b;
public:
	Sbb(Expression *, Expression *);
	std::string code();
	uint evaluate();
};
Sbb* sbb(Expression *, Expression *);


// Implementations

std::string Expression::access() { return "r0"; }

Constant::Constant(uint c) : c(c) { }
std::string Constant::code() { return ""; }
std::string Constant::access() { return std::to_string(c); }
uint Constant::evaluate() { return c; }
Constant* constant(uint c) { return new Constant(c); }
bool Constant::is_const_zero() { return c == 0; }

Variable::Variable(uint v) : v(v) { }
std::string Variable::code() { return ""; }
uint Variable::evaluate() { return v; }
void Variable::assign(uint v) { this->v = v; }
Variable* variable(uint v) { return new Variable(v); }

Shr::Shr(Expression* a, Expression* b) : a(a), b(b) { }
std::string Shr::code() {
	if (b->is_const_zero()) return a->code();
	return a->code() + b->code() + "\tshr r0, " + a->access() + ", " + b->access() + "\n";
}
uint Shr::evaluate() { return a->evaluate() >> b->evaluate(); }
Shr* shr(Expression* a, Expression* b) { return new Shr(a, b); }

Sar::Sar(Expression* a, Expression* b) : a(a), b(b) { }
std::string Sar::code() {
	if (b->is_const_zero()) return a->code();
	return a->code() + b->code() + "\tsar r0, " + a->access() + ", " + b->access() + "\n";
}
uint Sar::evaluate() { return ((sint)a->evaluate()) >> b->evaluate(); }
Sar* sar(Expression* a, Expression* b) { return new Sar(a, b); }

Umulhi::Umulhi(Expression* a, Expression* b) : a(a), b(b) { }
std::string Umulhi::code() { return a->code() + b->code() + "\tumulhi r0, " + a->access() + ", " + b->access() + "\n"; }
uint Umulhi::evaluate() { return ((big_uint)a->evaluate() * b->evaluate()) >> N; }
Umulhi* umulhi(Expression* a, Expression* b) { return new Umulhi(a, b); }

Imulhi::Imulhi(Expression* a, Expression* b) : a(a), b(b) { }
std::string Imulhi::code() { return a->code() + b->code() + "\timulhi r0, " + a->access() + ", " + b->access() + "\n"; }
uint Imulhi::evaluate() { return (((big_sint)((sint)a->evaluate())) * ((sint)b->evaluate())) >> N; }
Imulhi* imulhi(Expression* a, Expression* b) { return new Imulhi(a, b); }

bool carry = false;
Add::Add(Expression* a, Expression* b) : a(a), b(b) { }
std::string Add::code() { return a->code() + b->code() + "\tadd r0, " + a->access() + ", " + b->access() + "\n"; }
uint Add::evaluate() {
	uint left = a->evaluate();
	uint right = b->evaluate();
	carry = left > UINT_MAX - right;
	return left + right;
}
Add *add(Expression* a, Expression* b) { return new Add(a, b); }

// This should set the carry flag overflow but doesn't, because I didn't need it
Sub::Sub(Expression* a, Expression* b) : a(a), b(b) { }
std::string Sub::code() { return a->code() + b->code() + "\tsub r0, " + a->access() + ", " + b->access() + "\n"; }
uint Sub::evaluate() { return a->evaluate() - b->evaluate(); }
Sub *sub(Expression* a, Expression* b) { return new Sub(a, b); }

Neg::Neg(Expression* a) : a(a) { }
std::string Neg::code() { return a->code() + "\tneg r0, " + a->access() + "\n"; }
uint Neg::evaluate() { return -a->evaluate(); }
Neg *neg(Expression* a) { return new Neg(a); }

Gte::Gte(Expression* a, Expression* b) : a(a), b(b) { }
std::string Gte::code() { return a->code() + b->code() + "\tgte r0, " + a->access() + ", " + b->access() + "\n"; }
uint Gte::evaluate() { return a->evaluate() >= b->evaluate(); }
Gte* gte(Expression* a, Expression* b) { return new Gte(a, b); }

Sbb::Sbb(Expression* a, Expression* b) : a(a), b(b) { }
std::string Sbb::code() { return a->code() + b->code() + "\tsbb r0, " + a->access() + ", " + b->access() + "\n"; }
uint Sbb::evaluate() { return a->evaluate() - b->evaluate() - carry; }
Sbb* sbb(Expression* a, Expression* b) { return new Sbb(a, b); }

#endif
