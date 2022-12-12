#pragma once
#include "../Node.hpp"
#include "../utils.hpp"
#include "StackObject.hpp"

struct StackObject;

enum class OpType 
{
    LOAD,
    LOAD_CONST,
    LOAD_BUILTIN,
    LOAD_TOS_OR_EMPTY,
    STORE,
    STORE_AT,
    ADD,
    SUB,
    MUL,
    DIV,
    DOT,
    LT,
    GT,
    LTE,
    GTE,
    EQ_EQ,
    NOT_EQ,
    PLUS_EQ,
    MINUS_EQ,
    AND,
    OR,
    NOT,
    POS,
    NEG,
    MOD,
    COLON,
    RANGE,
    BUILD_LIST,
    BUILD_COMMA_LIST,
    BUILD_OBJECT,
    BUILD_PARTIAL_OP,
    ARROW,
    FRAME_IN,
    FRAME_OUT,
    BUILD_ITER,
    BUILD_LOOP,
    REMOVE_ITER,
    BUILD_FUNCTION,
    LOOP_START,
    WHILE_START,
    JUMP_IF_FALSE,
    JUMP_TO,
    BREAK,
    RETURN,
    START,
    EXIT,
    CALL_FUNCTION,
    COPY
};

struct Bytecode
{
    OpType type;
    std::shared_ptr<Node> node;
    std::shared_ptr<StackObject> data;
    int index;

    int line;
    int column;

    void print();

    Bytecode() {}
    Bytecode(OpType type, int line, int column) : type(type), line(line), column(column) {}
    Bytecode(OpType type, std::shared_ptr<Node> node, int line, int column) : type(type), node(node), line(line), column(column) {}
    Bytecode(OpType type, std::shared_ptr<StackObject> data, int line, int column) : type(type), data(data), line(line), column(column) {}
    Bytecode(OpType type, int index, int line, int column) : type(type), index(index), line(line), column(column) {}
};

struct Bytecode_Generator
{
    std::string file_name;
    std::vector<std::string> errors;
    std::vector<Bytecode> instructions;
    std::vector<std::shared_ptr<Node>> nodes;
    int instruction_counter = 0;

    int line, column;

    Bytecode_Generator(std::string file_name, std::vector<std::shared_ptr<Node>> nodes) 
        : file_name(file_name), nodes(nodes) {}

    std::vector<Bytecode> gen_bytecode(std::shared_ptr<Node> node);
    std::vector<Bytecode> gen_op(std::shared_ptr<Node> node, OpType type);
    std::vector<Bytecode> gen_un_op(std::shared_ptr<Node> node, OpType type);
    std::vector<Bytecode> gen_op_equal(std::shared_ptr<Node> node, OpType type);
    std::vector<Bytecode> gen_equal(std::shared_ptr<Node> node);
    std::vector<Bytecode> gen_arrow(std::shared_ptr<Node> node);
    std::vector<Bytecode> gen_dot(std::shared_ptr<Node> node);
    std::vector<Bytecode> gen_copy(std::shared_ptr<Node> node);
    std::vector<Bytecode> gen_build_list(std::shared_ptr<Node> node);
    std::vector<Bytecode> gen_build_object(std::shared_ptr<Node> node);
    std::vector<Bytecode> gen_build_function(std::shared_ptr<Node> node);
    std::vector<Bytecode> gen_build_comma_list(std::shared_ptr<Node> node);
    std::vector<Bytecode> gen_if_statement(std::shared_ptr<Node> node);
    std::vector<Bytecode> gen_if_block(std::shared_ptr<Node> node);
    std::vector<Bytecode> gen_block(std::shared_ptr<Node> node);
    std::vector<Bytecode> gen_for_loop(std::shared_ptr<Node> node);
    std::vector<Bytecode> gen_builtin_for_loop(std::shared_ptr<Node> node);
    std::vector<Bytecode> gen_while_loop(std::shared_ptr<Node> node);
    std::vector<Bytecode> gen_function_call(std::shared_ptr<Node> node);
    std::vector<Bytecode> generate();
    
    void add_instruction(Bytecode op);
    void print_instructions();
};
