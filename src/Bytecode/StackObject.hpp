#pragma once
#include "../Node.hpp"
#include "generator.hpp"

struct StackObject;
struct Bytecode;

enum class Operation_Type
{
    ADD,
    SUB,
    MUL,
    DIV,
    POS,
    NEG,
    NOT,
    DOT,
    LT,
    LTE,
    GT,
    GTE,
    EQ_EQ,
    NOT_EQ,
    RANGE,
    EQUAL,
    ARROW,
    AND,
    OR
};

enum class SO_Type
{
    ID,
    BOOL,
    INT,
    FLOAT,
    STRING,
    OP,
    LIST,
    COMMA_LIST,
    OBJECT,
    FUNCTION,
    ITERATOR,
    BLOCK,
    EMPTY,
};

struct Int_Object
{
    long long value;
};

struct Float_Object
{
    double value;
};

struct Bool_Object
{
    bool value;
};

struct String_Object
{
    std::string value;
};

struct List_Object
{
    std::vector<std::shared_ptr<StackObject>> objects;
    int length;
};

struct Comma_List_Object
{
    std::vector<std::shared_ptr<StackObject>> objects;
    int length;
};

struct Object_Object
{
    std::unordered_map<std::string, std::shared_ptr<StackObject>> properties;
    std::string name;
};


struct Function_Object
{
    std::string name;
    std::string file_name;
    std::vector<std::string> parameters;
    std::vector<std::shared_ptr<StackObject>> arguments;
    std::unordered_map<std::string, std::shared_ptr<StackObject>> closure;
    std::vector<Bytecode> instructions;
    bool builtin = false;
};

struct Op_Object
{
    std::string repr;
    Operation_Type type;
    std::shared_ptr<StackObject> left;
    std::shared_ptr<StackObject> right;
    bool is_unary = false;
    bool is_post_fix = false;
};

struct Iterator_Object
{
    std::shared_ptr<StackObject> iterator;
	std::shared_ptr<StackObject> current_element;
	std::string index_name;
	std::string item_name;
	int current_index;
	int iter_count = 0;
};

struct Block_Object
{
    std::vector<Bytecode> instructions;
};

struct StackObject
{
    SO_Type type;
    int line;
    int column;

    StackObject() {}
    StackObject(SO_Type type) : type(type) {}
    StackObject(int line, int column, SO_Type type) : line(line), column(column), type(type) {}

    std::string var_name;

    Int_Object INT;
    Float_Object FLOAT;
    Bool_Object BOOL;
    String_Object STRING;
    List_Object LIST;
    Comma_List_Object COMMA_LIST;
    Function_Object FUNCTION;
    Object_Object OBJECT;
    Op_Object OP;
    Iterator_Object ITER;
    Block_Object BLOCK;

    std::string repr();
};

Operation_Type node_op_type_to_so_op_type(NodeType type);
std::shared_ptr<StackObject> node_to_stack_object(std::shared_ptr<Node> node);
std::shared_ptr<StackObject> so_make_bool(bool value);
std::shared_ptr<StackObject> so_make_int(long value);
std::shared_ptr<StackObject> so_make_float(double value);
std::shared_ptr<StackObject> so_make_string(std::string value);
std::shared_ptr<StackObject> so_make_list();
std::shared_ptr<StackObject> so_make_comma_list();
std::shared_ptr<StackObject> so_make_object();
std::shared_ptr<StackObject> so_make_function();
std::shared_ptr<StackObject> so_make_block();
std::shared_ptr<StackObject> so_make_empty();