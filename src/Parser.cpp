#include "Parser.hpp"

void Parser::init(std::string _file_name, std::vector<std::shared_ptr<Node>> lexer_nodes)
{
    file_name = _file_name;
    nodes = lexer_nodes;
    if (nodes.size() == 0)
    {
        current_node = nullptr;
        return;
    }

    current_node = nodes[0];
}

void Parser::advance(int n)
{
    if ((index + n) >= nodes.size())
    {
        index = nodes.size() - 1;
        current_node = nodes[index];
        return;
    }

    index += n;
    current_node = nodes[index];
}

std::shared_ptr<Node> Parser::peek(int n)
{
    if ((index + n) >= nodes.size())
    {
        return nodes[nodes.size() - 1];
    }

    return nodes[index + n];
}

void Parser::error_and_continue(std::string message, int line, int column)
{
	std::string error_message = "[Parser] Parsing Error in '" + file_name + "' @ (" + std::to_string(line) + ", " + std::to_string(column) + "): " + message;
	errors.push_back(error_message);
}

void Parser::print_nodes()
{
	std::cout << "[ ";
	for (std::shared_ptr<Node> node : nodes)
	{
		node->print();
		std::cout << " ";
	}
	std::cout << "]" << std::flush;
}

void Parser::parse_arrow(std::vector<NodeType> end_node_types)
{
    int start_index = index;

    while (!is_type(current_node, end_node_types))
    {
        if (is_type(current_node, end_node_types))
        {
            break;
        }

        if (is_type(current_node, {NodeType::RIGHT_ARROW_DOUBLE}) && (!current_node->right && !current_node->left))
        {

            std::shared_ptr<Node> prev = peek(-1);
            std::shared_ptr<Node> next = peek(1);

            std::vector<NodeType> prev_types = {NodeType::L_BRACKET, NodeType::L_BRACE, NodeType::L_PAREN, NodeType::COLON, NodeType::START_OF_FILE};
            std::vector<NodeType> next_types = {NodeType::R_BRACKET, NodeType::R_BRACE, NodeType::R_PAREN, NodeType::COLON, NodeType::END_OF_FILE};

            if (is_type(prev, prev_types) || is_type(next, next_types))
            {
                current_node->OP.op_type = current_node->OP.op_type;
                current_node->OP.value = current_node->OP.value;
                current_node->type = NodeType::PARTIAL_OP;

                if (is_type(prev, prev_types))
                {
                    current_node->left = std::make_shared<Node>(NodeType::EMPTY);
                }
                else
                {
                    current_node->left = prev;
                    nodes.erase(nodes.begin() + (index - 1));
                    index--;
                }

                if (is_type(next, next_types))
                {
                    current_node->right = std::make_shared<Node>(NodeType::EMPTY);
                }
                else
                {
                    current_node->right = next;
                    nodes.erase(nodes.begin() + (index + 1));
                }
            }
            else
            {
                current_node->left = prev;
                nodes.erase(nodes.begin() + (index - 1));
                index--;

                current_node->right = next;
                nodes.erase(nodes.begin() + (index + 1));

                // if (is_type(current_node->left, {NodeType::PARTIAL_OP}) || is_type(current_node->right, {NodeType::PARTIAL_OP}))
                // {
                //     current_node->type = NodeType::PARTIAL_OP;
                // }
            }
        }

        advance();
    }

    index = start_index;
    current_node = nodes[index];
}

void Parser::parse_equal(std::vector<NodeType> end_node_types)
{
    int start_index = index;

    while (!is_type(current_node, end_node_types))
    {
        if (is_type(current_node, end_node_types))
        {
            break;
        }

        if (is_type(current_node, {NodeType::EQUAL}) && (!current_node->right && !current_node->left))
        {

            std::shared_ptr<Node> prev = peek(-1);
            std::shared_ptr<Node> next = peek(1);

            std::vector<NodeType> prev_types = {NodeType::RIGHT_ARROW_DOUBLE, NodeType::L_BRACKET, NodeType::L_BRACE, NodeType::L_PAREN, NodeType::COMMA, NodeType::COLON, NodeType::START_OF_FILE};
            std::vector<NodeType> next_types = {NodeType::R_BRACKET, NodeType::R_BRACE, NodeType::R_PAREN, NodeType::COMMA, NodeType::COLON, NodeType::END_OF_FILE};

            if (is_type(prev, prev_types) || is_type(next, next_types) || is_underscore(prev) || is_underscore(next))
            {
                current_node->OP.op_type = current_node->OP.op_type;
                current_node->OP.value = current_node->OP.value;
                current_node->type = NodeType::PARTIAL_OP;

                if (is_type(prev, prev_types))
                {
                    current_node->left = std::make_shared<Node>(NodeType::EMPTY);
                }
                else
                {
                    current_node->left = prev;
                    nodes.erase(nodes.begin() + (index - 1));
                    index--;

                    if (is_underscore(current_node->left))
                    {
                        current_node->left->type = NodeType::EMPTY;
                    }
                }

                if (is_type(next, next_types))
                {
                    current_node->right = std::make_shared<Node>(NodeType::EMPTY);
                }
                else
                {
                    current_node->right = next;
                    nodes.erase(nodes.begin() + (index + 1));

                    if (is_underscore(current_node->right))
                    {
                        current_node->right->type = NodeType::EMPTY;
                    }
                }
            }
            else
            {
                current_node->left = prev;
                nodes.erase(nodes.begin() + (index - 1));
                index--;

                current_node->right = next;
                nodes.erase(nodes.begin() + (index + 1));
            }
        }

        advance();
    }

    index = start_index;
    current_node = nodes[index];
}

void Parser::parse_colon(std::vector<NodeType> end_node_types)
{
    int start_index = index;

    while (!is_type(current_node, end_node_types))
    {
        if (is_type(current_node, end_node_types))
        {
            break;
        }

        if (is_type(current_node, {NodeType::COLON}) && (!current_node->right && !current_node->left))
        {
            std::shared_ptr<Node> prev = peek(-1);
            std::shared_ptr<Node> next = peek(1);

            std::vector<NodeType> prev_types = {NodeType::L_BRACKET, NodeType::L_BRACE, NodeType::L_PAREN, NodeType::COMMA, NodeType::COLON, NodeType::START_OF_FILE};
            std::vector<NodeType> next_types = {NodeType::R_BRACKET, NodeType::R_BRACE, NodeType::R_PAREN, NodeType::COMMA, NodeType::COLON, NodeType::END_OF_FILE};

            if (is_type(prev, prev_types) || is_type(next, next_types) || is_underscore(prev) || is_underscore(next))
            {
                current_node->OP.op_type = current_node->OP.op_type;
                current_node->OP.value = current_node->OP.value;
                current_node->type = NodeType::PARTIAL_OP;

                if (is_type(prev, prev_types))
                {
                    current_node->left = std::make_shared<Node>(NodeType::EMPTY);
                }
                else
                {
                    current_node->left = prev;
                    nodes.erase(nodes.begin() + (index - 1));
                    index--;

                    if (is_underscore(current_node->left))
                    {
                        current_node->left->type = NodeType::EMPTY;
                    }
                }

                if (is_type(next, next_types))
                {
                    current_node->right = std::make_shared<Node>(NodeType::EMPTY);
                }
                else
                {
                    current_node->right = next;
                    nodes.erase(nodes.begin() + (index + 1));

                    if (is_underscore(current_node->right))
                    {
                        current_node->right->type = NodeType::EMPTY;
                    }
                }
            }
            else
            {
                current_node->left = prev;
                nodes.erase(nodes.begin() + (index - 1));
                index--;

                current_node->right = next;
                nodes.erase(nodes.begin() + (index + 1));
            }
        }

        advance();
    }

    index = start_index;
    current_node = nodes[index];
}

void Parser::parse_comma(std::vector<NodeType> end_node_types)
{
    int start_index = index;

    while (!is_type(current_node, end_node_types))
    {
        if (is_type(current_node, end_node_types))
        {
            break;
        }

        if (is_type(current_node, {NodeType::COMMA}) && (!current_node->right && !current_node->left))
        {

            std::shared_ptr<Node> prev = peek(-1);
            std::shared_ptr<Node> next = peek(1);

            std::vector<NodeType> prev_types = {NodeType::L_BRACE, NodeType::L_PAREN, NodeType::L_BRACKET, NodeType::START_OF_FILE};
            std::vector<NodeType> next_types = {NodeType::COMMA, NodeType::R_BRACE, NodeType::R_PAREN, NodeType::R_BRACKET, NodeType::END_OF_FILE};

            if (is_type(next, next_types))
            {
                nodes.erase(nodes.begin() + index);
                index--;
                advance();

                continue;
            }

            if (is_type(prev, prev_types))
            {
                nodes.erase(nodes.begin() + index);
                index--;
                advance();

                continue;
            }

            current_node->left = prev;
            current_node->right = next;
            nodes.erase(nodes.begin() + (index + 1));
            nodes.erase(nodes.begin() + (index - 1));
            index--;

            advance();
            continue;
        }

        advance();
    }

    index = start_index;
    current_node = nodes[index];
}

void Parser::parse_bin_op(std::vector<NodeType> types, std::vector<NodeType> end_node_types)
{
    int start_index = index;

    while (!is_type(current_node, end_node_types))
    {
        if (is_type(current_node, end_node_types))
        {
            break;
        }

        if (is_type(current_node, types) && (!current_node->right && !current_node->left))
        {

            std::shared_ptr<Node> prev = peek(-1);
            std::shared_ptr<Node> next = peek(1);

            std::vector<NodeType> prev_types = {NodeType::RIGHT_ARROW_DOUBLE, 
            NodeType::L_BRACKET, NodeType::L_BRACE, NodeType::L_PAREN, 
            NodeType::COMMA, NodeType::COLON, NodeType::EQUAL, NodeType::START_OF_FILE};
            std::vector<NodeType> next_types = {NodeType::RIGHT_ARROW_DOUBLE, 
            NodeType::R_BRACKET, NodeType::R_BRACE, NodeType::R_PAREN, 
            NodeType::COMMA, NodeType::COLON, NodeType::SEMICOLON, NodeType::END_OF_FILE};

            bool prev_type_no_children = (is_type(prev, prev_types) && prev->left == nullptr && prev->right == nullptr);
            bool next_type_no_children = (is_type(next, next_types) && next->left == nullptr && next->right == nullptr);

            if (prev_type_no_children || next_type_no_children || is_underscore(prev) || is_underscore(next))
            {
                current_node->OP.op_type = current_node->OP.op_type;
                current_node->OP.value = current_node->OP.value;
                current_node->type = NodeType::PARTIAL_OP;

                if (is_type(prev, prev_types))
                {
                    current_node->left = std::make_shared<Node>(NodeType::EMPTY);
                }
                else
                {
                    current_node->left = prev;
                    nodes.erase(nodes.begin() + (index - 1));
                    index--;

                    if (is_underscore(current_node->left))
                    {
                        current_node->left->type = NodeType::EMPTY;
                    }
                }

                if (is_type(next, next_types))
                {
                    current_node->right = std::make_shared<Node>(NodeType::EMPTY);
                }
                else
                {
                    current_node->right = next;
                    nodes.erase(nodes.begin() + (index + 1));

                    if (is_underscore(current_node->right))
                    {
                        current_node->right->type = NodeType::EMPTY;
                    }
                }
            }
            else
            {
                current_node->left = prev;
                nodes.erase(nodes.begin() + (index - 1));
                index--;

                current_node->right = next;
                nodes.erase(nodes.begin() + (index + 1));

                if (is_type(current_node->left, {NodeType::PARTIAL_OP}) || is_type(current_node->right, {NodeType::PARTIAL_OP}))
                {
                    current_node->type = NodeType::PARTIAL_OP;
                }
            }
        }

        advance();
    }

    index = start_index;
    current_node = nodes[index];
}

void Parser::parse_right_arrow(std::vector<NodeType> end_node_types)
{
    int start_index = index;

    while (!is_type(current_node, end_node_types))
    {
        if (is_type(current_node, end_node_types))
        {
            break;
        }

        if (is_type(current_node, {NodeType::RIGHT_ARROW}) && (!current_node->right && !current_node->left))
        {
            std::shared_ptr<Node> prev = peek(-1);
            std::shared_ptr<Node> next = peek(1);

            std::vector<NodeType> prev_types = {NodeType::RIGHT_ARROW_DOUBLE, 
            NodeType::L_BRACKET, NodeType::L_BRACE, NodeType::L_PAREN, 
            NodeType::COMMA, NodeType::COLON, NodeType::START_OF_FILE};
            std::vector<NodeType> next_types = {NodeType::RIGHT_ARROW_DOUBLE, 
            NodeType::R_BRACKET, NodeType::R_BRACE, NodeType::R_PAREN, 
            NodeType::COMMA, NodeType::COLON, NodeType::END_OF_FILE};

            bool prev_type_no_children = (is_type(prev, prev_types) && prev->left == nullptr && prev->right == nullptr);
            bool next_type_no_children = (is_type(next, next_types) && next->left == nullptr && next->right == nullptr);

            if (prev_type_no_children || next_type_no_children || is_underscore(prev) || is_underscore(next))
            {
                current_node->OP.op_type = current_node->OP.op_type;
                current_node->OP.value = current_node->OP.value;
                current_node->type = NodeType::PARTIAL_OP;

                if (is_type(prev, prev_types))
                {
                    current_node->left = std::make_shared<Node>(NodeType::EMPTY);
                }
                else
                {
                    current_node->left = prev;
                    nodes.erase(nodes.begin() + (index - 1));
                    index--;

                    if (is_underscore(current_node->left))
                    {
                        current_node->left->type = NodeType::EMPTY;
                    }
                }

                if (is_type(next, next_types))
                {
                    current_node->right = std::make_shared<Node>(NodeType::EMPTY);
                }
                else
                {
                    current_node->right = next;
                    nodes.erase(nodes.begin() + (index + 1));

                    if (is_underscore(current_node->right))
                    {
                        current_node->right->type = NodeType::EMPTY;
                    }
                }
            }
            else
            {
                current_node->left = prev;
                nodes.erase(nodes.begin() + (index - 1));
                index--;

                current_node->right = next;
                nodes.erase(nodes.begin() + (index + 1));

                if (is_type(current_node->left, {NodeType::PARTIAL_OP}) || is_type(current_node->right, {NodeType::PARTIAL_OP}))
                {
                    current_node->type = NodeType::PARTIAL_OP;
                }
            }

            if (is_type(current_node->left, {NodeType::ID}) && current_node->left->ID.value == "if" 
                && is_type(current_node->right, {NodeType::BLOCK}))
                {
                    current_node->type = NodeType::IF_BLOCK;
                    current_node->right->BLOCK.context = current_node->type;
                }

            else if (is_type(current_node->left, {NodeType::FUNC_CALL}) 
                && current_node->left->FUNC_CALL.name->ID.value == "for" 
                && is_type(current_node->right, {NodeType::BLOCK}))
                {
                    current_node->type = NodeType::FOR_LOOP;
                    current_node->right->BLOCK.context = current_node->type;
                }

            else if (is_type(current_node->left, {NodeType::FUNC_CALL}) 
                && current_node->left->FUNC_CALL.name->ID.value == "_for" 
                && is_type(current_node->right, {NodeType::BLOCK}))
                {
                    current_node->type = NodeType::BUILTIN_FOR_LOOP;
                    current_node->right->BLOCK.context = current_node->type;
                }

            else if (is_type(current_node->left, {NodeType::FUNC_CALL}) 
                && current_node->left->FUNC_CALL.name->ID.value == "while" 
                && is_type(current_node->right, {NodeType::BLOCK}))
                {
                    current_node->type = NodeType::WHILE_LOOP;
                    current_node->right->BLOCK.context = current_node->type;
                }

            else if (is_type(current_node->left, {NodeType::FUNC_CALL}) 
                && current_node->left->FUNC_CALL.name->ID.value == "if" 
                && is_type(current_node->right, {NodeType::BLOCK}))
                {
                    current_node->type = NodeType::IF_STATEMENT;
                    current_node->right->BLOCK.context = current_node->type;
                }

            else if (is_type(current_node->left, {NodeType::LIST}) 
                && is_type(current_node->right, {NodeType::BLOCK}))
                {
                    current_node->type = NodeType::LAMBDA;
                    current_node->right->BLOCK.context = current_node->type;
                }
            else if (is_type(current_node->left, {NodeType::DOUBLE_COLON}) 
                && is_type(current_node->right, {NodeType::BLOCK}))
                {
                    current_node->type = NodeType::LAMBDA;
                    current_node->right->BLOCK.context = current_node->type;
                }
        }

        advance();
    }

    index = start_index;
    current_node = nodes[index];
}

void Parser::parse_not(std::vector<NodeType> end_node_types)
{
    int start_index = index;

    while (!is_type(current_node, end_node_types))
    {
        if (is_type(current_node, end_node_types))
        {
            break;
        }

        if (is_type(current_node, {NodeType::EXCLAMATION}) && (!current_node->right && !current_node->left))
        {

            std::shared_ptr<Node> prev = peek(-1);
            std::shared_ptr<Node> next = peek(1);

            std::vector<NodeType> next_types = { 
            NodeType::R_BRACKET, NodeType::R_BRACE, NodeType::R_PAREN, 
            NodeType::COMMA, NodeType::COLON, NodeType::END_OF_FILE};

            current_node->left = std::make_shared<Node>(NodeType::EMPTY);

            if (is_type(next, next_types) || is_underscore(next))
            {
                current_node->OP.op_type = current_node->OP.op_type;
                current_node->OP.value = current_node->OP.value;
                current_node->type = NodeType::PARTIAL_OP;

                if (is_type(next, next_types))
                {
                    current_node->right = std::make_shared<Node>(NodeType::EMPTY);
                }
                else
                {
                    current_node->right = next;
                    nodes.erase(nodes.begin() + (index + 1));

                    if (is_underscore(current_node->right))
                    {
                        current_node->right->type = NodeType::EMPTY;
                    }
                }
            }
            else
            {
                current_node->right = next;
                nodes.erase(nodes.begin() + (index + 1));

                if (is_type(current_node->left, {NodeType::PARTIAL_OP}) || is_type(current_node->right, {NodeType::PARTIAL_OP}))
                {
                    current_node->type = NodeType::PARTIAL_OP;
                }
            }
        }

        advance();
    }

    index = start_index;
    current_node = nodes[index];
}

void Parser::parse_at(std::vector<NodeType> end_node_types)
{
    int start_index = index;

    while (!is_type(current_node, end_node_types))
    {
        if (is_type(current_node, end_node_types))
        {
            break;
        }

        if (is_type(current_node, {NodeType::AT}))
        {
            if (!current_node->right)
            {
                std::shared_ptr<Node> next = peek(1);

                if (is_type(next, {NodeType::RIGHT_ARROW_DOUBLE, NodeType::R_BRACKET, NodeType::R_BRACE, NodeType::R_PAREN, NodeType::COMMA, NodeType::END_OF_FILE}))
                {
                    current_node = custom_error(current_node, "@ expects a '{'");
                    advance();
                    continue;
                }
                else 
                {
                    current_node->right = next;
                    nodes.erase(nodes.begin() + (index + 1));

                    if (is_type(current_node->right, {NodeType::BLOCK}))
                    {
                        current_node->type = NodeType::OBJECT;
                    }
                }

                current_node->left = std::make_shared<Node>(NodeType::EMPTY);
            }  
            
        }

        advance();
    }

    index = start_index;
    current_node = nodes[index];
}

void Parser::parse_hash(std::vector<NodeType> end_node_types)
{
    int start_index = index;

    while (!is_type(current_node, end_node_types))
    {
        if (is_type(current_node, end_node_types))
        {
            break;
        }

        if (is_type(current_node, {NodeType::HASH}))
        {
            std::shared_ptr<Node> prev = peek(-1);

            if (is_type(prev, {NodeType::START_OF_FILE}) || (prev->type == NodeType::OP && prev->left == nullptr && prev->right == nullptr))
            {
                if (!current_node->right)
                {
                    std::shared_ptr<Node> next = peek(1);

                    if (is_type(next, {NodeType::RIGHT_ARROW_DOUBLE, NodeType::R_BRACKET, NodeType::R_BRACE, NodeType::R_PAREN, NodeType::COMMA, NodeType::END_OF_FILE}))
                    {
                        current_node = custom_error(current_node, "# expects an identifier");
                        advance();
                        continue;
                    }
                    else 
                    {
                        current_node->right = next;
                        nodes.erase(nodes.begin() + (index + 1));

                        if (is_type(current_node->right, {NodeType::ID}))
                        {
                            current_node->type = NodeType::COPY;
                        }
                    }

                    current_node->left = std::make_shared<Node>(NodeType::EMPTY);
                }  
            }
        }

        advance();
    }

    index = start_index;
    current_node = nodes[index];
}

void Parser::parse_un_op(std::vector<NodeType> types, std::vector<NodeType> end_node_types)
{
    int start_index = index;

    while (!is_type(current_node, end_node_types))
    {
        if (is_type(current_node, end_node_types))
        {
            break;
        }

        if (is_type(current_node, types))
        {
            std::shared_ptr<Node> prev = peek(-1);

            if (is_type(prev, {NodeType::START_OF_FILE}) 
            || (is_type(prev, {NodeType::ID}) && prev->ID.value == "ret") 
            || (prev->type == NodeType::OP && prev->left == nullptr && prev->right == nullptr))
            {
                if (!current_node->right)
                {
                    std::shared_ptr<Node> next = peek(1);

                    if (!is_type(next, {NodeType::RIGHT_ARROW_DOUBLE, NodeType::R_BRACKET, NodeType::R_BRACE, NodeType::R_PAREN, NodeType::COMMA, NodeType::END_OF_FILE}))
                    {
                        current_node->right = next;
                        nodes.erase(nodes.begin() + (index + 1));

                        if (is_type(current_node, {NodeType::PLUS}))
                        {
                            current_node->OP.op_type = NodeType::POS;
                        }
                        if (is_type(current_node, {NodeType::MINUS}))
                        {
                            current_node->OP.op_type = NodeType::NEG;
                        }
                    }
                    else 
                    {
                        current_node->right = std::make_shared<Node>(NodeType::EMPTY);
                        current_node->type = NodeType::PARTIAL_OP;
                    }

                    current_node->left = std::make_shared<Node>(NodeType::EMPTY);

                    if (is_type(prev, {NodeType::RIGHT_ARROW_DOUBLE}))
                    {
                        current_node->type = NodeType::PARTIAL_OP;

                        if (is_type(current_node, {NodeType::POS}))
                        {
                            current_node->OP.op_type = NodeType::PLUS;
                        }
                        if (is_type(current_node, {NodeType::NEG}))
                        {
                            current_node->OP.op_type = NodeType::MINUS;
                        }
                    }
                }  
            }
        }

        advance();
    }

    index = start_index;
    current_node = nodes[index];
}

void Parser::parse_postfix_un_op(std::vector<NodeType> types, std::vector<NodeType> end_node_types)
{
    int start_index = index;

    while (!is_type(current_node, end_node_types))
    {
        if (is_type(current_node, end_node_types))
        {
            break;
        }

        if (is_type(current_node, types))
        {
            if (!current_node->left)
            {
                std::shared_ptr<Node> prev = peek(-1);

                if (!is_type(prev, {NodeType::RIGHT_ARROW_DOUBLE, NodeType::L_BRACKET, NodeType::L_BRACE, NodeType::L_PAREN, NodeType::COMMA, NodeType::START_OF_FILE}))
                {
                    current_node->left = prev;
                    nodes.erase(nodes.begin() + (index - 1));
                    index--;
                }
                else 
                {
                    current_node->left = std::make_shared<Node>(NodeType::EMPTY);
                }

                current_node->right = std::make_shared<Node>(NodeType::EMPTY);
            }   
        }

        advance();
    }

    index = start_index;
    current_node = nodes[index];
}

void Parser::parse_keywords(std::vector<NodeType> end_node_types)
{
    int start_index = index;

    while (!is_type(current_node, end_node_types))
    {
        if (is_type(current_node, end_node_types))
        {
            break;
        }

        std::vector<std::string> keywords = {"ret", "break", "continue"};
        std::vector<NodeType> next_types = {NodeType::R_BRACKET, NodeType::R_BRACE, NodeType::R_PAREN, NodeType::COMMA, NodeType::COLON, NodeType::END_OF_FILE};

        if (is_type(current_node, {NodeType::ID}) && (std::count(keywords.begin(), keywords.end(), current_node->ID.value)))
        {
            if (current_node->ID.value == "break" || current_node->ID.value == "continue")
            {
                current_node->right = std::make_shared<Node>(NodeType::EMPTY);
                current_node->type = NodeType::KEYWORD;
            }

            if (!current_node->right)
            {
                std::shared_ptr<Node> next = peek(1);
                if (is_type(next, next_types))
                {
                    current_node->right = std::make_shared<Node>(NodeType::EMPTY);
                }
                else
                {
                    current_node->right = next;
                    nodes.erase(nodes.begin() + (index + 1));
                }
                current_node->type = NodeType::KEYWORD;
            }   
        }

        advance();
    }

    index = start_index;
    current_node = nodes[index];
}

void Parser::parse_paren(std::vector<NodeType> end_node_types)
{
    int start_index = index;

    while (!is_type(current_node, end_node_types)) 
    {
        if (is_type(current_node, end_node_types))
        {
            break;
        }

        if (is_type(current_node, {NodeType::L_PAREN}))
        {
            int paren_start_index = index;
            advance(); // past '('

            parse({NodeType::R_PAREN, NodeType::END_OF_FILE});

            if (is_type(current_node, {NodeType::END_OF_FILE}))
            {
                break;
            }

            advance();
            nodes.erase(nodes.begin() + (index - 1)); // delete ')'
            index--;

            nodes.erase(nodes.begin() + paren_start_index); // delete '('
            index--;

            continue;
        }

        advance();
    }

    index = start_index;
    current_node = nodes[index];
}

void Parser::parse_block(std::vector<NodeType> end_node_types)
{
    int start_index = index;

    while (!is_type(current_node, end_node_types)) 
    {
        if (is_type(current_node, end_node_types))
        {
            break;
        }

        if (is_type(current_node, {NodeType::L_BRACE}))
        {
            nodes.erase(nodes.begin() + (index)); // delete '{'
            current_node = nodes[index];
            int start_block = index;
            parse({NodeType::R_BRACE, NodeType::END_OF_FILE});
            int end_block = index;

            if (is_type(current_node, {NodeType::END_OF_FILE}))
            {
                break;
            }

            auto begin = nodes.begin() + start_block;
            auto end = nodes.begin() + end_block;

            current_node->type = NodeType::BLOCK;
            current_node->BLOCK.nodes = std::vector<std::shared_ptr<Node>>(begin, end);

            advance(-(end_block-start_block));

            nodes.erase(begin, end);

            current_node = nodes[index];


            continue;
        }

        advance();
    }

    index = start_index;
    current_node = nodes[index];
}

void Parser::parse_list(std::vector<NodeType> end_node_types)
{
    int start_index = index;

    while (!is_type(current_node, end_node_types)) 
    {
        if (is_type(current_node, end_node_types))
        {
            break;
        }

        if (is_type(current_node, {NodeType::L_BRACKET}))
        {
            int start_block = index;
            advance();
            parse({NodeType::R_BRACKET, NodeType::END_OF_FILE});
            int end_block = index;

            if (is_type(current_node, {NodeType::END_OF_FILE}))
            {
                break;
            }

            auto begin = nodes.begin() + start_block;
            auto end = nodes.begin() + end_block;

            current_node->type = NodeType::LIST;
            current_node->LIST.nodes = std::vector<std::shared_ptr<Node>>(begin+1, end);

            advance(-(end_block-start_block));

            nodes.erase(begin, end);

            current_node = nodes[index];

            continue;
        }

        advance();
    }

    index = start_index;
    current_node = nodes[index];
}

void Parser::parse_function_call(std::vector<NodeType> end_node_types)
{
    int start_index = index;

    while (!is_type(current_node, end_node_types))
    {
        if (is_type(current_node, {NodeType::ID}) && is_type(peek(1), {NodeType::LIST}) && current_node->ID.value != "ret")
        {
            int current_node_index = index;

            current_node->FUNC_CALL.name = std::make_shared<Node>(*current_node);
            current_node->type = NodeType::FUNC_CALL;
            current_node->FUNC_CALL.args = peek(1)->LIST.nodes;

            nodes.erase(nodes.begin() + (index + 1)); // delete 'LIST'
        }

        if (is_type(current_node, end_node_types))
        {
            break;
        }

        advance();
    }

    index = start_index;
    current_node = nodes[index];
}

void Parser::flatten_comma_list(std::shared_ptr<Node>& node)
{
    if (is_type(node, {NodeType::COMMA}) 
        && (node->left != nullptr && node->right != nullptr))
    {
        node->type = NodeType::COMMA_LIST;
        if (is_type(node->left, {NodeType::COMMA}))
        {
            flatten_comma_list(node->left);
        }
        else
        {
            node->COMMA_LIST.nodes.push_back(node->left);
        }
        if (is_type(node->left, {NodeType::COMMA_LIST}))
        {
            for (auto elem : node->left->COMMA_LIST.nodes)
            {
                node->COMMA_LIST.nodes.push_back(elem);
            }

            node->left = std::make_shared<Node>(NodeType::EMPTY);
        }
        
        if (is_type(node->right, {NodeType::COMMA}))
        {
            flatten_comma_list(node->right);
        }
        else
        {
            node->COMMA_LIST.nodes.push_back(node->right);
        }
        if (is_type(node->right, {NodeType::COMMA_LIST}))
        {
            for (auto elem : node->right->COMMA_LIST.nodes)
            {
                node->COMMA_LIST.nodes.push_back(elem);
            }

            node->right = std::make_shared<Node>(NodeType::EMPTY);
        }
    }
}

void Parser::remove_node_type(std::vector<NodeType> types, std::vector<NodeType> end_node_types)
{
    int start_index = index;

    while (!is_type(current_node, end_node_types)) 
    {
        if (is_type(current_node, types))
        {
            std::shared_ptr<Node> node = nodes[index];
            nodes.erase(nodes.begin() + index);
            current_node = nodes[index];

            node.reset();
        }
        else
        {
            advance();
        }
    }

    index = start_index;
    current_node = nodes[index];
}

std::vector<std::shared_ptr<Node>> Parser::parse(std::vector<NodeType> end_node_types)
{  
    if (is_type(current_node, end_node_types))
    {
        return std::vector<std::shared_ptr<Node>>{};
    }

    parse_block(end_node_types);
    parse_list(end_node_types);
    parse_at(end_node_types);
    parse_hash(end_node_types);
    parse_function_call(end_node_types);
    parse_paren(end_node_types);
    parse_bin_op({NodeType::DOUBLE_DOT}, end_node_types);
    parse_bin_op({NodeType::DOT}, end_node_types);
    parse_postfix_un_op({NodeType::PLUS_PLUS, NodeType::MINUS_MINUS}, end_node_types);
    parse_un_op({NodeType::PLUS, NodeType::MINUS}, end_node_types);
    parse_bin_op({NodeType::PLUS_EQ, NodeType::MINUS_EQ}, end_node_types);
    parse_bin_op({NodeType::PERCENT}, end_node_types);
	parse_bin_op({NodeType::STAR, NodeType::SLASH}, end_node_types);
	parse_bin_op({NodeType::PLUS, NodeType::MINUS}, end_node_types);
	parse_bin_op({NodeType::L_ANGLE, NodeType::R_ANGLE}, end_node_types);
    parse_bin_op({NodeType::EQ_EQ, NodeType::NOT_EQUAL, 
    NodeType::GT_EQUAL, NodeType::LT_EQUAL}, end_node_types);
    parse_not(end_node_types);
    parse_bin_op({NodeType::AND, NodeType::OR}, end_node_types);
	parse_bin_op({NodeType::DOUBLE_COLON}, end_node_types);
    parse_right_arrow(end_node_types);
	parse_arrow(end_node_types);
	parse_colon(end_node_types);
    parse_comma(end_node_types);
    parse_keywords(end_node_types);
	parse_equal(end_node_types);
	parse_bin_op({NodeType::QUESTION}, end_node_types);
    remove_node_type({NodeType::SEMICOLON}, end_node_types);

    while (!is_type(current_node, end_node_types))
    {
        advance();
    }

    for (auto& node : nodes)
    {
        flatten_comma_list(node);
    }

    return nodes;
}