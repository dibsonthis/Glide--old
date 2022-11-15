#include "generator.hpp"

void Bytecode::print()
{
    switch(type)
    {
        case OpType::ADD: std::cout << "ADD"; break;
        case OpType::SUB: std::cout << "SUB"; break;
        case OpType::MUL: std::cout << "MUL"; break;
        case OpType::DIV: std::cout << "DIV"; break;
        case OpType::DOT: std::cout << "DOT"; break;
        case OpType::POS: std::cout << "POS"; break;
        case OpType::NEG: std::cout << "NEG"; break;
        case OpType::MOD: std::cout << "MOD"; break;
        case OpType::LT: std::cout << "LT"; break;
        case OpType::GT: std::cout << "GT"; break;
        case OpType::LTE: std::cout << "LTE"; break;
        case OpType::GTE: std::cout << "GTE"; break;
        case OpType::EQ_EQ: std::cout << "EQ_EQ"; break;
        case OpType::NOT_EQ: std::cout << "NOT_EQ"; break;
        case OpType::AND: std::cout << "AND"; break;
        case OpType::OR: std::cout << "OR"; break;
        case OpType::NOT: std::cout << "NOT"; break;
        case OpType::COLON: std::cout << "COLON"; break;
        case OpType::RANGE: std::cout << "RANGE"; break;
        case OpType::STORE_AT: std::cout << "STORE_AT"; break;
        case OpType::ARROW: std::cout << "ARROW"; break;
        case OpType::FRAME_IN: std::cout << "FRAME_IN"; break;
        case OpType::FRAME_OUT: std::cout << "FRAME_OUT"; break;
        case OpType::START: std::cout << "START"; break;
        case OpType::WHILE_START: std::cout << "WHILE_START"; break;
        case OpType::EXIT: std::cout << "EXIT"; break;
        case OpType::RETURN: std::cout << "RETURN"; break;
        case OpType::BUILD_PARTIAL_OP: std::cout << "BUILD_PARTIAL_OP"; break;
        case OpType::REMOVE_ITER: std::cout << "REMOVE_ITER"; break;
        case OpType::COPY: std::cout << "COPY"; break;
        case OpType::LOAD_TOS_OR_EMPTY: std::cout << "LOAD_TOS_OR_EMPTY"; break;

        case OpType::LOAD:
        {
            std::cout << "LOAD ";
            std::cout << data->repr();

            if (data->type == SO_Type::OP)
            {
                std::cout << " ( ";
                std::cout << data->OP.left->repr();
                std::cout << " ";
                std::cout << data->OP.right->repr();
                std::cout << " )";
            }
        } break;

        case OpType::LOAD_CONST:
        {
            std::cout << "LOAD_CONST ";
            std::cout << data->repr();
        } break;

        case OpType::LOAD_BUILTIN:
        {
            std::cout << "LOAD_BUILTIN ";
            std::cout << data->repr();
        } break;

        case OpType::STORE:
        {
            std::cout << "STORE ";
            std::cout << data->repr();
        } break;

        case OpType::BUILD_LIST:
        {
            std::cout << "BUILD_LIST ";
            std::cout << index;
        } break;

        case OpType::BUILD_COMMA_LIST:
        {
            std::cout << "BUILD_COMMA_LIST ";
            std::cout << index;
        } break;
        
        case OpType::BUILD_OBJECT:
        {
            std::cout << "BUILD_OBJECT ";
            std::cout << index;
        } break;

        case OpType::LOOP_START:
        {
            std::cout << "LOOP_START ";
            std::cout << index;
        } break;

        case OpType::BUILD_ITER:
        {
            std::cout << "BUILD_ITER ";
            std::cout << index;
        } break;

        case OpType::JUMP_IF_FALSE:
        {
            std::cout << "JUMP_IF_FALSE ";
            std::cout << index;
        } break;

        case OpType::JUMP_TO:
        {
            std::cout << "JUMP_TO ";
            std::cout << index;
        } break;

        case OpType::BREAK:
        {
            std::cout << "BREAK ";
            std::cout << index;
        } break;

        case OpType::BUILD_FUNCTION:
        {
            std::cout << "BUILD_FUNCTION ";
            std::cout << data->FUNCTION.name;
        } break;

        case OpType::CALL_FUNCTION:
        {
            std::cout << "CALL_FUNCTION ";
            std::cout << index;
        } break;
    }
}

std::vector<Bytecode> Bytecode_Generator::gen_equal(std::shared_ptr<Node> node)
{
    instructions = gen_bytecode(node->right);
    if (is_type(node->left, {NodeType::DOT}))
    {
        auto left = node->left->left;
        auto right = node->left->right;
        instructions = gen_bytecode(left);

        // x.[4], x.[y]
        if (is_type(right, {NodeType::LIST}) && right->LIST.nodes.size() == 1)
        {
            right = right->LIST.nodes[0];
        }
        // x.length
        else if (is_type(right, {NodeType::ID}))
        {
            right->type = NodeType::STRING;
            right->STRING.value = right->ID.value;
        }

        instructions = gen_bytecode(right);
        auto op = Bytecode(OpType::STORE_AT);
        op.line = node->left->line;
        op.column = node->left->column;
        add_instruction(op);
        return instructions;
    }
    auto data = node_to_stack_object(node->left);
    Bytecode store = Bytecode(OpType::STORE, data);
    add_instruction(Bytecode(store));
    return instructions;
}

std::vector<Bytecode> Bytecode_Generator::gen_op_equal(std::shared_ptr<Node> node, OpType type)
{
    if (is_type(node->left, {NodeType::DOT}))
    {
        instructions = gen_bytecode(node->left);
        instructions = gen_bytecode(node->right);
        auto op = Bytecode(type);
        add_instruction(op);

        auto left = node->left->left;
        auto right = node->left->right;
        instructions = gen_bytecode(left);

        // x.[4], x.[y]
        if (is_type(right, {NodeType::LIST}) && right->LIST.nodes.size() == 1)
        {
            right = right->LIST.nodes[0];
        }
        // x.length
        else if (is_type(right, {NodeType::ID}))
        {
            right->type = NodeType::STRING;
            right->STRING.value = right->ID.value;
        }

        instructions = gen_bytecode(right);

        auto store = Bytecode(OpType::STORE_AT);
        store.line = node->left->line;
        store.column = node->left->column;
        add_instruction(store);
        return instructions;
    }

    instructions = gen_bytecode(node->left);
    instructions = gen_bytecode(node->right);
    auto op = Bytecode(type);
    add_instruction(op);

    auto data = node_to_stack_object(node->left);
    Bytecode store = Bytecode(OpType::STORE, data);
    add_instruction(store);
    return instructions;
}

std::vector<Bytecode> Bytecode_Generator::gen_arrow(std::shared_ptr<Node> node)
{
    if (is_type(node->right, {NodeType::EQUAL}))
    {
        instructions = gen_bytecode(node->left);
        auto data = node_to_stack_object(node->right->left);
        Bytecode store = Bytecode(OpType::STORE, data);
        add_instruction(Bytecode(store));
        add_instruction(Bytecode(OpType::LOAD, data));
        return instructions;
    }

    instructions = gen_bytecode(node->left);
    instructions = gen_bytecode(node->right);
    auto arrow = Bytecode(OpType::ARROW);
    arrow.line = node->line;
    arrow.column = node->column;
    add_instruction(arrow);
    return instructions;
}

std::vector<Bytecode> Bytecode_Generator::gen_dot(std::shared_ptr<Node> node)
{
    auto dot = std::make_shared<Node>(*node);

    // x.[4], x.[y]
    if (is_type(dot->right, {NodeType::LIST}) && dot->right->LIST.nodes.size() == 1)
    {
        instructions = gen_bytecode(dot->left);
        dot->right = dot->right->LIST.nodes[0];
    }
    // x.length
    else if (is_type(dot->right, {NodeType::ID}))
    {
        instructions = gen_bytecode(dot->left);
        dot->right->type = NodeType::STRING;
        dot->right->STRING.value = dot->right->ID.value;
    }
    else if (is_type(dot->right, {NodeType::FUNC_CALL}))
    {
        for (auto arg : dot->right->FUNC_CALL.args)
        {
            instructions = gen_bytecode(arg);
        }

        instructions = gen_bytecode(dot->left);
        auto name = so_make_string(dot->right->FUNC_CALL.name->ID.value);
        add_instruction(Bytecode(OpType::LOAD_CONST, name));
        add_instruction(Bytecode(OpType::DOT));
        add_instruction(Bytecode(OpType::CALL_FUNCTION, dot->right->FUNC_CALL.args.size()));
        return instructions;
    }
    else
    {
        auto error = error_message(dot, file_name, "Invalid right operand of operator '.'");
        errors.push_back(error);
        return instructions;
    }

    instructions = gen_bytecode(dot->right);
    auto dot_op = Bytecode(OpType::DOT);
    dot_op.line = node->line;
    dot_op.column = node->column;
    add_instruction(dot_op);
    return instructions;
}

std::vector<Bytecode> Bytecode_Generator::gen_op(std::shared_ptr<Node> node, OpType type)
{
    instructions = gen_bytecode(node->left);
    instructions = gen_bytecode(node->right);
    auto op = Bytecode(type);
    op.line = node->line;
    op.column = node->column;
    add_instruction(op);
    return instructions;
}

std::vector<Bytecode> Bytecode_Generator::gen_un_op(std::shared_ptr<Node> node, OpType type)
{
    instructions = gen_bytecode(node->right);
    auto op = Bytecode(type);
    op.line = node->line;
    op.column = node->column;
    add_instruction(op);
    return instructions;
}

std::vector<Bytecode> Bytecode_Generator::gen_copy(std::shared_ptr<Node> node)
{
    instructions = gen_bytecode(node->right);
    auto op = Bytecode(OpType::COPY);
    op.line = node->line;
    op.column = node->column;
    add_instruction(op);
    return instructions;
}

std::vector<Bytecode> Bytecode_Generator::gen_build_list(std::shared_ptr<Node> node)
{
    for (auto element_node : node->LIST.nodes)
    {
        instructions = gen_bytecode(element_node);
    }

    auto list_length_node = node->LIST.nodes.size();
    auto build_list = Bytecode(OpType::BUILD_LIST, list_length_node);
    build_list.line = node->line;
    build_list.column = node->column;
    add_instruction(build_list);
    return instructions;
}

std::vector<Bytecode> Bytecode_Generator::gen_build_comma_list(std::shared_ptr<Node> node)
{
    for (auto element_node : node->COMMA_LIST.nodes)
    {
        instructions = gen_bytecode(element_node);
    }

    auto list_length_node = node->COMMA_LIST.nodes.size();
    auto build_list = Bytecode(OpType::BUILD_COMMA_LIST, list_length_node);
    build_list.line = node->line;
    build_list.column = node->column;
    add_instruction(build_list);
    return instructions;
}

std::vector<Bytecode> Bytecode_Generator::gen_build_object(std::shared_ptr<Node> node)
{
    add_instruction(Bytecode(OpType::FRAME_IN));
    std::reverse(node->right->BLOCK.nodes.begin(), node->right->BLOCK.nodes.end());
    for (auto property : node->right->BLOCK.nodes)
    {
        // To avoid a LOAD id, and looking up the ID in the scopes, we convert it directly
        // to a string node

        if (is_type(property->left, {NodeType::ID}))
        {
            property->left->type = NodeType::STRING;
            property->left->STRING.value = property->left->ID.value;
        }

        instructions = gen_bytecode(property->left);
        instructions = gen_bytecode(property->right);
    }

    auto obj_length_node = node->right->BLOCK.nodes.size();
    auto build_object = Bytecode(OpType::BUILD_OBJECT, obj_length_node);
    build_object.line = node->line;
    build_object.column = node->column;
    add_instruction(build_object);
    add_instruction(Bytecode(OpType::FRAME_OUT));
    return instructions;
}

std::vector<Bytecode> Bytecode_Generator::gen_block(std::shared_ptr<Node> node)
{
    add_instruction(Bytecode(OpType::FRAME_IN));
    for (auto element_node : node->BLOCK.nodes)
    {
        instructions = gen_bytecode(element_node);
    }
    add_instruction(Bytecode(OpType::FRAME_OUT));
    return instructions;
}

std::vector<Bytecode> Bytecode_Generator::gen_build_function(std::shared_ptr<Node> node)
{
    auto data = so_make_function();
    for (auto param : node->left->LIST.nodes)
    {
        data->FUNCTION.parameters.push_back(param->ID.value);
    }

    auto function_statements = node->right->BLOCK.nodes;

    function_statements.insert(function_statements.begin(), std::make_shared<Node>(NodeType::START_OF_FILE));
    // auto empty_return = std::make_shared<Node>(NodeType::KEYWORD);
    // empty_return->ID.value = "ret";
    // empty_return->right = std::make_shared<Node>(NodeType::EMPTY);
    // function_statements.push_back(empty_return);
    function_statements.push_back(std::make_shared<Node>(NodeType::END_OF_FILE));
    auto generator = Bytecode_Generator(file_name, function_statements);
    data->FUNCTION.instructions = generator.generate();

    generator.instructions.insert(generator.instructions.end() -1, Bytecode(OpType::LOAD_TOS_OR_EMPTY));
    generator.instructions.insert(generator.instructions.end() -1, Bytecode(OpType::RETURN));

    data->FUNCTION.instructions = generator.instructions;

    // std::cout << "\nStart Function Bytecode:\n\n";
    // generator.print_instructions();
    // std::cout << "\nEnd Function Bytecode:\n\n";

    auto build_function = Bytecode(OpType::BUILD_FUNCTION, data);
    build_function.column = node->column;
    build_function.line = node->line;

    if (is_type(node, {NodeType::LAMBDA}))
    {
        data->FUNCTION.name = "lambda";
    }
    else
    {
        data->FUNCTION.name = node->right->left->FUNC_CALL.name->ID.value;
    }

    add_instruction(build_function);
    return instructions;
}

std::vector<Bytecode> Bytecode_Generator::gen_function_call(std::shared_ptr<Node> node)
{
    for (auto element_node : node->FUNC_CALL.args)
    {
        instructions = gen_bytecode(element_node);
    }
    auto function_name = node_to_stack_object(node->FUNC_CALL.name);

    // check if built-in
    if (function_name->STRING.value == "print" 
        || function_name->STRING.value == "type"
        || function_name->STRING.value == "import"
        || function_name->STRING.value == "use"
        || function_name->STRING.value == "address"
        || function_name->STRING.value == "shape"
        || function_name->STRING.value == "var"
        || function_name->STRING.value == "var_out"
        || function_name->STRING.value == "delete"
        || function_name->STRING.value == "delete_prop"
        || function_name->STRING.value == "out"
        || function_name->STRING.value == "to_string"
        || function_name->STRING.value == "to_int"
        || function_name->STRING.value == "to_float"
        || function_name->STRING.value == "to_list"
        || function_name->STRING.value == "time"
        || function_name->STRING.value == "read"
        || function_name->STRING.value == "write"
        || function_name->STRING.value == "append"
        || function_name->STRING.value == "frame"
        || function_name->STRING.value == "clear_args"
        )
    {
        add_instruction(Bytecode(OpType::LOAD_BUILTIN, function_name));
    }
    else
    {
        add_instruction(Bytecode(OpType::LOAD, function_name));
    }

    int args_length = node->FUNC_CALL.args.size();
    add_instruction(Bytecode(OpType::CALL_FUNCTION, args_length));
    return instructions;
}

std::vector<Bytecode> Bytecode_Generator::gen_for_loop(std::shared_ptr<Node> node)
{
    auto left = node->left->FUNC_CALL;
    int args_length = left.args.size();

    if (args_length < 1)
    {
        auto error = error_message(node, file_name, "For loop must have an iterator");
        errors.push_back(error);
        return instructions;
    }

    auto iterator = left.args[0];
    instructions = gen_bytecode(iterator);

    if (args_length > 1)
    {
        if (!is_type(left.args[1], {NodeType::ID}))
        {
            auto error = error_message(node, file_name, "For loop index must be an id");
            errors.push_back(error);
            return instructions;
        }

        auto string_node = std::make_shared<Node>(*left.args[1]);
        string_node->type = NodeType::STRING;
        string_node->STRING.value = left.args[1]->ID.value;
        instructions = gen_bytecode(string_node);
    }

    if (args_length > 2)
    {
        if (!is_type(left.args[2], {NodeType::ID}))
        {
            auto error = error_message(node, file_name, "For loop iteration element must be an id");
            errors.push_back(error);
            return instructions;
        }

        auto string_node = std::make_shared<Node>(*left.args[2]);
        string_node->type = NodeType::STRING;
        string_node->STRING.value = left.args[2]->ID.value;
        instructions = gen_bytecode(string_node);
    }

    auto args_length_node = std::make_shared<Node>(*node->left);

    add_instruction(Bytecode(OpType::BUILD_ITER, args_length));
    add_instruction(Bytecode(OpType::LOOP_START));

    int loop_start_index = instruction_counter - 1;

    for (auto element_node : node->right->BLOCK.nodes)
    {
        instructions = gen_bytecode(element_node);
    }

    add_instruction(Bytecode(OpType::JUMP_TO, loop_start_index));

    int loop_end_index = instruction_counter - 1;
    instructions[loop_start_index].index = loop_end_index;

    add_instruction(Bytecode(OpType::REMOVE_ITER));

    // We iterate through the instructions between start and end indices, and if we encounter
    // a break that doesn't have an exit index, we update it with the current
    // loop's exit index

    for (int i = loop_start_index; i < loop_end_index; i++)
    {
        if (instructions[i].type == OpType::BREAK 
            && instructions[i].data->type == SO_Type::EMPTY)
        {
            instructions[i].index = loop_end_index;
            instructions[i].data->type = SO_Type::INT;
        }
    }

    return instructions;
}

std::vector<Bytecode> Bytecode_Generator::gen_while_loop(std::shared_ptr<Node> node)
{
    auto left = node->left->FUNC_CALL;
    int args_length = left.args.size();

    if (args_length != 1)
    {
        auto error = error_message(node, file_name, "While loop must have a conditional statement");
        errors.push_back(error);
        return instructions;
    }

    Bytecode while_start_op = Bytecode(OpType::WHILE_START);
    add_instruction(while_start_op);

    int while_start_index = instruction_counter;

    auto condition = left.args[0];
    instructions = gen_bytecode(condition);

    Bytecode jump_if_false = Bytecode(OpType::JUMP_IF_FALSE);
    int jump_if_false_current_index = instruction_counter;
    add_instruction(jump_if_false);

    for (auto element_node : node->right->BLOCK.nodes)
    {
        instructions = gen_bytecode(element_node);
    }

    add_instruction(Bytecode(OpType::JUMP_TO, while_start_index));

    int loop_end_index = instruction_counter;

    instructions[jump_if_false_current_index].index = loop_end_index;

    // We iterate through the instructions between start and end indices, and if we encounter
    // a break that doesn't have an exit index, we update it with the current
    // loop's exit index

    for (int i = while_start_index; i < loop_end_index; i++)
    {
        if (instructions[i].type == OpType::BREAK 
            && instructions[i].data->type == SO_Type::EMPTY)
        {
            instructions[i].index = loop_end_index - 1;
            instructions[i].data->type = SO_Type::INT;
        }
    }

    return instructions;
}

std::vector<Bytecode> Bytecode_Generator::gen_if_statement(std::shared_ptr<Node> node)
{
    if (node->left->FUNC_CALL.args.size() != 1)
    {
        auto error = error_message(node, file_name, "If statement must have a conditional statement");
        errors.push_back(error);
        return instructions;
    }

    auto condition = node->left->FUNC_CALL.args[0];

    instructions = gen_bytecode(condition);
    auto jump_if_false = Bytecode(OpType::JUMP_IF_FALSE);
    add_instruction(jump_if_false);

    int jump_if_false_current_index = instruction_counter - 1;

    for (auto element_node : node->right->BLOCK.nodes)
    {
        instructions = gen_bytecode(element_node);
    }

    auto jump_to = instruction_counter;

    instructions[jump_if_false_current_index].index = jump_to;

    return instructions;
}

std::vector<Bytecode> Bytecode_Generator::gen_if_block(std::shared_ptr<Node> node)
{
    auto cases = node->right->BLOCK.nodes;
    std::vector<int> jump_if_false_indices;
    std::vector<int> jump_to_indices;
    std::vector<int> case_indices;

    for (int i = 0; i < cases.size(); i++)
    {
        auto conditional = cases[i]->left;
        auto block = cases[i]->right;

        if (is_type(conditional, {NodeType::ID}) && conditional->ID.value == "default")
        {
            case_indices.push_back(instruction_counter);
            for (auto statement : block->BLOCK.nodes)
            {
                instructions = gen_bytecode(statement);
            }
        }
        else
        {
            if (i != 0)
            {
                case_indices.push_back(instruction_counter);
            }

            instructions = gen_bytecode(conditional);
            add_instruction(Bytecode(OpType::JUMP_IF_FALSE));
            jump_if_false_indices.push_back(instruction_counter - 1);

            for (auto statement : block->BLOCK.nodes)
            {
                instructions = gen_bytecode(statement);
            }

            add_instruction(Bytecode(OpType::JUMP_TO));
            jump_to_indices.push_back(instruction_counter - 1);

            if (i == cases.size() - 1)
            {
                case_indices.push_back(instruction_counter);
            }
        }
    }

    // update jump_to
    for (auto index : jump_to_indices)
    {
        instructions[index].index = instruction_counter;
    }

    // update jump_if_false
    for (int i = 0; i < jump_if_false_indices.size(); i++)
    {
        int index = jump_if_false_indices[i];
        instructions[index].index = case_indices[i];
    }

    return instructions;
}

std::vector<Bytecode> Bytecode_Generator::gen_bytecode(std::shared_ptr<Node> node)
{
    if (is_type(node, {NodeType::ID}))
    {
        auto data = node_to_stack_object(node);
        Bytecode load;

        // Load built-ins
        // TODO: change to if contains...

        if (node->ID.value == "print" 
            || node->ID.value == "type"
            || node->ID.value == "import"
            || node->ID.value == "use"
            || node->ID.value == "address"
            || node->ID.value == "shape"
            || node->ID.value == "var"
            || node->ID.value == "var_out"
            || node->ID.value == "delete"
            || node->ID.value == "delete_prop"
            || node->ID.value == "out"
            || node->ID.value == "to_string"
            || node->ID.value == "to_int"
            || node->ID.value == "to_float"
            || node->ID.value == "to_list"
            || node->ID.value == "time"
            || node->ID.value == "read"
            || node->ID.value == "write"
            || node->ID.value == "append"
            || node->ID.value == "frame"
            || node->ID.value == "clear_args"
            )
        {
            load = Bytecode(OpType::LOAD_BUILTIN, data);
        }
        else if (node->ID.value == "empty")
        {
            load = Bytecode(OpType::LOAD_CONST, so_make_empty());
        }
        else
        {
            load = Bytecode(OpType::LOAD, data);
        }

        load.column = node->column;
        load.line = node->line;

        add_instruction(load);
        return instructions;
    }

    if (is_type(node, {NodeType::INT, NodeType::FLOAT, NodeType::BOOL, 
    NodeType::STRING, NodeType::EMPTY}))
    {
        auto data = node_to_stack_object(node);
        Bytecode load = Bytecode(OpType::LOAD_CONST, data);
        load.column = node->column;
        load.line = node->line;

        add_instruction(load);
        return instructions;
    }
    if (is_type(node, {NodeType::PARTIAL_OP}))
    {
        instructions = gen_bytecode(node->left);
        instructions = gen_bytecode(node->right);
        Bytecode build_partial_op = Bytecode(OpType::BUILD_PARTIAL_OP);
        build_partial_op.column = node->column;
        build_partial_op.line = node->line;

        auto op = node_to_stack_object(node);

        add_instruction(Bytecode(OpType::LOAD_CONST, op));
        add_instruction(build_partial_op);
        return instructions;
    }
    if (is_type(node, {NodeType::KEYWORD}) && node->ID.value == "break")
    {
        auto _break = Bytecode(OpType::BREAK, so_make_empty());
        _break.column = node->column;
        _break.line = node->line;

        add_instruction(_break);
        return instructions;
    }
    if (is_type(node, {NodeType::KEYWORD}) && node->ID.value == "ret")
    {
        instructions = gen_bytecode(node->right);
        auto _return = Bytecode(OpType::RETURN);
        _return.column = node->column;
        _return.line = node->line;

        add_instruction(_return);
        return instructions;
    }
    if (is_type(node, {NodeType::LAMBDA}))
    {
        return gen_build_function(node);
    }
    if (is_type(node, {NodeType::POS}))
    {
        return gen_un_op(node, OpType::POS);
    }
    if (is_type(node, {NodeType::NEG}))
    {
        return gen_un_op(node, OpType::NEG);
    }
    if (is_type(node, {NodeType::PLUS}))
    {
        return gen_op(node, OpType::ADD);
    }
    if (is_type(node, {NodeType::MINUS}))
    {
        return gen_op(node, OpType::SUB);
    }
    if (is_type(node, {NodeType::STAR}))
    {
        return gen_op(node, OpType::MUL);
    }
    if (is_type(node, {NodeType::SLASH}))
    {
        return gen_op(node, OpType::DIV);
    }
    if (is_type(node, {NodeType::PERCENT}))
    {
        return gen_op(node, OpType::MOD);
    }
    if (is_type(node, {NodeType::DOT}))
    {
        return gen_dot(node);
    }
    if (is_type(node, {NodeType::L_ANGLE}))
    {
        return gen_op(node, OpType::LT);
    }
    if (is_type(node, {NodeType::R_ANGLE}))
    {
        return gen_op(node, OpType::GT);
    }
    if (is_type(node, {NodeType::LT_EQUAL}))
    {
        return gen_op(node, OpType::LTE);
    }
    if (is_type(node, {NodeType::GT_EQUAL}))
    {
        return gen_op(node, OpType::GTE);
    }
    if (is_type(node, {NodeType::EQ_EQ}))
    {
        return gen_op(node, OpType::EQ_EQ);
    }
    if (is_type(node, {NodeType::NOT_EQUAL}))
    {
        return gen_op(node, OpType::NOT_EQ);
    }
    if (is_type(node, {NodeType::PLUS_EQ}))
    {   
        return gen_op_equal(node, OpType::ADD);
    }
    if (is_type(node, {NodeType::MINUS_EQ}))
    {   
        return gen_op_equal(node, OpType::SUB);
    }
    if (is_type(node, {NodeType::AND}))
    {
        return gen_op(node, OpType::AND);
    }
    if (is_type(node, {NodeType::OR}))
    {
        return gen_op(node, OpType::OR);
    }
    if (is_type(node, {NodeType::EXCLAMATION}))
    {
        return gen_un_op(node, OpType::NOT);
    }
    if (is_type(node, {NodeType::COLON}))
    {
        return gen_op(node, OpType::COLON);
    }
    if (is_type(node, {NodeType::DOUBLE_DOT}))
    {
        return gen_op(node, OpType::RANGE);
    }
    if (is_type(node, {NodeType::EQUAL}))
    {
        return gen_equal(node);
    }
    if (is_type(node, {NodeType::LIST}))
    {
        return gen_build_list(node);
    }
    if (is_type(node, {NodeType::COMMA_LIST}))
    {
        return gen_build_comma_list(node);
    }
    if (is_type(node, {NodeType::OBJECT}))
    {
        return gen_build_object(node);
    }
    if (is_type(node, {NodeType::BLOCK}))
    {
        return gen_block(node);
    }
    if (is_type(node, {NodeType::FOR_LOOP}))
    {
        return gen_for_loop(node);
    }
    if (is_type(node, {NodeType::WHILE_LOOP}))
    {
        return gen_while_loop(node);
    }
    if (is_type(node, {NodeType::IF_STATEMENT}))
    {
        return gen_if_statement(node);
    }
    if (is_type(node, {NodeType::IF_BLOCK}))
    {
        return gen_if_block(node);
    }
    if (is_type(node, {NodeType::RIGHT_ARROW_SINGLE}))
    {
        return gen_arrow(node);
    }
    if (is_type(node, {NodeType::FUNC_CALL}))
    {
        return gen_function_call(node);
    }
    if (is_type(node, {NodeType::COPY}))
    {
        return gen_copy(node);
    }

    auto error = error_message(node, file_name, "Invalid syntax, failed to compile");
    errors.push_back(error);

    return instructions;
}

std::vector<Bytecode> Bytecode_Generator::generate()
{
    add_instruction(Bytecode(OpType::START));

    for (int i = 1; i < nodes.size() - 1; i++)
    {
        instructions = gen_bytecode(nodes[i]);
        if (errors.size() > 0)
        {
            for (auto error : errors)
            {
                std::cout << error << std::flush;
            }

            instructions.clear();
            return instructions;
        }
    }

    add_instruction(Bytecode(OpType::EXIT));
    return instructions;
}

void Bytecode_Generator::add_instruction(Bytecode op)
{
    instructions.push_back(op);
    instruction_counter += 1;
}

void Bytecode_Generator::print_instructions()
{
    std::cout << "\n[\n\n";
    for (int i = 0; i < instructions.size(); i++)
    {
        std::cout << "  ";
        std::cout << i << ": ";
        instructions[i].print();
        std::cout << "\n";
    }
    std::cout << "\n]\n";
    std::cout << std::flush;
}