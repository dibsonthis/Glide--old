#pragma once
#include <string>
#include <unordered_map>
#include <memory>
#include <iostream>
#include <vector>
#include <unordered_map>

enum class NodeType {
	INT,
	FLOAT,
	STRING,
	BOOL,
	ID,
	OP,
	ERROR,
	ANY,
	FUNC_T,
	EQ_EQ,
	NOT_EQUAL,
	LT_EQUAL,
	GT_EQUAL,
	PLUS_EQ,
	MINUS_EQ,
	PLUS_PLUS,
	MINUS_MINUS,
	RIGHT_ARROW,
	RIGHT_ARROW_SINGLE,
	DOUBLE_COLON,
	AND,
	OR,
	EQUAL,
	L_PAREN,
	R_PAREN,
	L_BRACE,
	R_BRACE,
	L_BRACKET,
	R_BRACKET,
	L_ANGLE,
	R_ANGLE,
	DOT,
	BACKSLASH,
	APOSTROPHE,
	EXCLAMATION,
	AT,
	HASH,
	DOLLAR,
	CARET,
	QUESTION,
	PERCENT,
	DOUBLE_QUOTE,
	MINUS,
	PLUS,
	SLASH,
	STAR,
	COMMA,
	PIPE,
	COLON,
	SEMICOLON,
	END_OF_FILE,
	START_OF_FILE,
	EMPTY,
	POS,
	NEG,
	BLOCK,
	LIST,
	KEYWORD,
	FUNC_CALL,
	DOUBLE_DOT,
	TRIPLE_DOT,
	DOUBLE_L_BRACE,
	DOUBLE_R_BRACE,
	OBJECT,
	RETURN,
	COMMA_LIST,
	FUNC,
	PARTIAL_OP,
	FUNC_CTX,
	SWITCH,
	BREAK,
	FOR_LOOP,
	BUILTIN_FOR_LOOP,
	WHILE_LOOP,
	IF_STATEMENT,
	LAMBDA,
	ITERATOR,
	IF_BLOCK,

	VARIABLE,
	TYPE,
	COPY,
};

struct Node;

struct Int_Node
{
	long long value = 0;
	Int_Node() = default;
	Int_Node(long value) : value(value) {}
};

struct Float_Node
{
	double value = 0;
	Float_Node() = default;
	Float_Node(double value) : value(value) {}
};

struct Bool_Node
{
	bool value = 0;
	Bool_Node() = default;
	Bool_Node(bool value) : value(value) {}
};

struct ID_Node
{
	std::string value = "";
	ID_Node() = default;
	ID_Node(std::string value) : value(value) {}
};

struct String_Node
{
	std::string value = "";
	String_Node() = default;
	String_Node(std::string value) : value(value) {}
};

struct Op_Node
{
	std::string value = "";
	NodeType op_type;
	Op_Node() = default;
	Op_Node(std::string value) : value(value) {}
};

struct Block_Node
{
	std::unordered_map<std::string, std::shared_ptr<Node>> symbol_table;
	std::vector<std::shared_ptr<Node>> nodes;
	NodeType context;
	Block_Node() = default;
};

struct List_Node
{
	std::vector<std::shared_ptr<Node>> nodes;
	List_Node() = default;
};

struct Object_Node
{
	std::unordered_map<std::string, std::shared_ptr<Node>> symbol_table;
	std::unordered_map<std::string, std::shared_ptr<Node>> properties;
	bool evaluated = false;
	bool is_import = false;
};

struct Func_T_Node
{
	std::vector<std::pair<std::string, std::shared_ptr<Node>>> params;
	std::shared_ptr<Node> return_type;
};

struct Func_Call_Node
{
	std::shared_ptr<Node> name;
	std::vector<std::shared_ptr<Node>> args;
};

struct Variable_Node
{
	std::shared_ptr<Node> name = nullptr;
    std::shared_ptr<Node> type = nullptr;
    std::shared_ptr<Node> value = nullptr;
	bool is_import = false;
};

struct Return_Node
{
	std::shared_ptr<Node> value = nullptr;
	NodeType returned_from;
};

struct Iterator_Node
{
	std::shared_ptr<Node> iterator;
	std::shared_ptr<Node> current_element;
	std::string index_name;
	std::string item_name;
	int current_index;
	int iter_count = 0;
};

struct Type_Node
{
	std::string name;
	std::unordered_map<std::string, std::shared_ptr<Node>> interface;
};

struct Inferred_Type_Node
{
	std::shared_ptr<Node> type;
};

struct List_Element_Node
{
	std::shared_ptr<Node> container;
	int index;
	std::shared_ptr<Node> value;
};

struct Func_Node
{
	std::string name;
	std::shared_ptr<Node> return_type;
	std::vector<std::shared_ptr<Node>> params;
	std::shared_ptr<Node> body;
	std::unordered_map<std::string, std::shared_ptr<Node>> closure;
};

struct Func_Ctx_Node
{
	std::string name;
	std::vector<std::shared_ptr<Node>> args;
	std::vector<std::shared_ptr<Node>> params;
	std::shared_ptr<Node> body;
	std::unordered_map<std::string, std::shared_ptr<Node>> closure;

};

struct Switch_Node
{
	std::unordered_map<std::string, std::shared_ptr<Node>> symbol_table;
	std::vector<std::shared_ptr<Node>> cases;
};


struct Error_Node
{
	std::vector<std::string> errors;
};

struct Node
{
	NodeType type;

	int column = 1;
	int line = 1;

	std::shared_ptr<Node> left = nullptr;
	std::shared_ptr<Node> right = nullptr;

	std::string name;

	Node() = default;
	Node(NodeType type) : type(type) {}
	Node(int line, int column) : line(line), column(column) {}
	Node(NodeType type, int line, int column) : type(type), line(line), column(column) {}

	Int_Node				INT;
	Float_Node				FLOAT;
	Bool_Node				BOOL;
	ID_Node					ID;
	String_Node				STRING;
	Op_Node					OP;
	Block_Node				BLOCK;
	Object_Node				OBJECT;
	List_Node				LIST;
	Func_Call_Node			FUNC_CALL;
	Variable_Node			VAR;
	Type_Node				TYPE;
	Error_Node				ERROR;
	Inferred_Type_Node		INFERRED_TYPE;
	Return_Node				RETURN;
	List_Node				COMMA_LIST;
	Func_Node				FUNC;
	Func_Ctx_Node			FUNC_CTX;
	Switch_Node				SWITCH;
	Iterator_Node			ITER;
	Func_T_Node				FUNC_T;

	void print();
	std::string repr();
};