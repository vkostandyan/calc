#include "../includes/Compiler.hpp"

void printNode(const Token& node)
{
	switch (node.type)
	{
		case NodeType::Num:
			std::cout << "Type: Number, Value: " << node.value << "\n";
			break;
		case NodeType::Var:
			std::cout << "Type: Variable, Name: " << node.value << "\n";
			break;
		case NodeType::Op:
			std::cout << "Type: Operator: " << node.value << "\n";
			break;
		case NodeType::OpBr:
			std::cout << "Type: Open Bracket, Symbol: " << node.value << "\n";
			break;
		case NodeType::ClBr:
			std::cout << "Type: Close Bracket, Symbol: " << node.value << "\n";
			break;
		case NodeType::EofEx:
			std::cout << "Type: End of Expression\n";
			break;
		case NodeType::If:
			std::cout << "Type: If\n";
			break;
		case NodeType::While:
			std::cout << "Type: While\n";
			break;
		case NodeType::Comp:
			std::cout << "Type: Comparison, Symbol: " << node.value << "\n";
			break;
		case NodeType::Assign:
			std::cout << "Type: Assignment, Symbol: " << node.value << "\n";
			break;
		case NodeType::Not:
			std::cout << "Type: Not, Symbol: " << node.value << "\n";
			break;
		case NodeType::Semi:
			std::cout << "Type: Semicolon, Symbol: " << node.value << "\n";
			break;
		case NodeType::OpBody:
			std::cout << "Type: Open Curly Bracket, Symbol: " << node.value << "\n";
			break;
		case NodeType::ClBody:
			std::cout << "Type: Close Curly Bracket, Symbol: " << node.value << "\n";
			break;
		case NodeType::Type:
			std::cout << "Type: Type, Symbol: " << node.value << "\n";
			break;
		case NodeType::Ret:
			std::cout << "Type: Return, Symbol: " << node.value << "\n";
			break;
		default:
			std::cout << "Type: Unknown\n";
			break;
	}
}

void printAST(Node* node)
{
	if (!node)
		return;


	switch (node->type)
	{
		case NodeType::Num:
			std::cout << "Num(" << node->value << ")\n";
			return;
		case NodeType::Var:
			std::cout << "Var(addr=" << node->symAddr << ")\n";
			return;
		case NodeType::Assign:
			std::cout << "Assign\n";
			break;
		case NodeType::Op:
		{
			switch (node->op)
			{
			case Operator::Add:
				std::cout << "Add\n";
				break;
			case Operator::Sub:
				std::cout << "Sub\n";
				break;
			case Operator::Mult:
				std::cout << "Mult\n";
				break;
			case Operator::Div:
				std::cout << "Div\n";
				break;
			default:
				break;
			}
			break;
		}
		case NodeType::Block:
		{
			std::cout << "Block\n";
			BlockNode* n = dynamic_cast<BlockNode*>(node);
			for (size_t i = 0; i < n->statements.size(); i++)
			{
				printAST(n->statements[i].get());
			}
			return;
		}
		case NodeType::Func:
		{
			FuncNode* n = dynamic_cast<FuncNode*>(node);

			std::cout<<"Func: "<<n->name<<" with params\n";
			for (size_t i = 0; i < n->params.size(); i++)
			{
				std::cout<<n->params[i].type<<"  "<<n->params[i].name<<"\n";
			}
			printAST(n->body.get());
			break;
		}
		case NodeType::Ret:
		{
			ReturnNode* n = dynamic_cast<ReturnNode*>(node);
			std::cout << "Return\n";
			printAST(n->expr.get());
			break;
		}
		case NodeType::Call:
		{
			CallNode* n = dynamic_cast<CallNode*>(node);
			std::cout << "Call " << n->name << " with params\n";
			for (size_t i = 0; i < n->args.size(); i++)
			{
				std::cout<<n->args[i].get()->name<<"\n";
			}
			break;
		}
		default:
			std::cout << "NodeType(" << (int)node->type << ")\n";
			break;
	}

	if (node->left)
	{
		std::cout << "L:";
		printAST(node->left.get());
	}

	if (node->right)
	{
		std::cout << "R:";
		printAST(node->right.get());
	}
}
