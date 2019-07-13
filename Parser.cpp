#include "db_cell.h"
#include "Parser.h"
#include "Exception.h"
#include <vector>

namespace cultivateInput {

	typedef void(cultivateInput::Parser::Creator::*execut)(cultivateInput::Parser::Creator::argIndex);

	Parser::Parser(std::vector<TokenInfPtr>& t_pool)
		: tokensPool(t_pool)
	{
		m_Creator = new Creator(this);
		m_tempCell = new libs::db_cell();
		m_currentState = parserState::Initial;
		m_label = "";
		m_argIndex = Creator::argIndex::first;
		m_firstNet = true;
	}

	void Parser::parse()
	{
		std::vector<TokenInfPtr>::iterator i = tokensPool.begin();
		for (; i < tokensPool.end(); ++i)
		{
			//checkValidation(i);
			switch (m_currentState) {
			case parserState::Initial:
			{
				cultivateInitialState(*i);
				break;
			}
			case parserState::Identifier:
			{
				cultivateIdentifierState(*i);
				break;
			}
			case parserState::OpBracket:
			{
				cultivateOpeningBracketState(*i);
				break;
			}
			case parserState::ClosingBracket:
			{
				cultivateClosingBracketState(*i);
				break;
			}
			case parserState::Label:
			{
				cultivateLabelState(*i);
				break;
			}
			case parserState::Number:
			{
				cultivateNumberState(*i);
				break;
			}
			case parserState::Final:
			{
				cultivateFinalState(*i);
				break;
			}
			}
		}
	}
	void Parser::cultivateInitialState(TokenInfPtr tokenInf)
	{
		switch (tokenInf->e_tokType)
		{
		case tokenType::word:
		{
			m_word = tokenInf->name;
			m_currentState = parserState::Identifier;
			break;
		}
		default:
		{
			throw ExceptionError("Line can't start with:" + tokenInf->name);
		}
		}
	}
	void Parser::cultivateIdentifierState(TokenInfPtr tokenInf)
	{
		switch (tokenInf->e_tokType)
		{
		case tokenType::label:
		{
			if (std::find(labels.begin(), labels.end(), m_word) != labels.end()) {
				m_label = m_word;
				m_currentState = parserState::Label;
			}
			else {
				throw ExceptionError("Incorrect label name.");
			}
				break;
		}
		case tokenType::opening_bracket:
		{
			if (m_label == " ") {
				throw ExceptionError("Label was not declared.");
			}
			else {
				execut createFunc = (Creator::createFuncs.find(m_label))->second;
				(this->m_Creator->*createFunc)(m_argIndex);
				m_currentState = parserState::OpBracket;
			}
			break;
		}
		case tokenType::word:
		{
			execut createFunc = (Creator::createFuncs.find(m_label))->second;
			(this->m_Creator->*createFunc)(m_argIndex);
			m_currentState = parserState::Identifier;
			m_word = tokenInf->name;
			break;
		}
		case tokenType::comma:
		{
			execut createFunc = (Creator::createFuncs.find(m_label))->second;
			(this->m_Creator->*createFunc)(m_argIndex);
			m_currentState = parserState::Initial;
			break;
		}
		case tokenType::semicolon:
		{
			execut createFunc = (Creator::createFuncs.find(m_label))->second;
			(this->m_Creator->*createFunc)(m_argIndex);
			libs::nets.push_back(m_tempNet);
			m_tempNet.init();
			m_currentState = parserState::Initial;
			m_argIndex = Creator::argIndex::first;
			break;
		}
		default:
		{
			throw ExceptionError("Incorrect token after identifier.");
		}
		}
	}

	void Parser::cultivateOpeningBracketState(TokenInfPtr tokenInf)
	{
		switch (tokenInf->e_tokType)
		{
		case tokenType::number:
		{
			m_tempNumber = tokenInf->value;
			execut createFunc = (Creator::createFuncs.find(m_label))->second;
			(this->m_Creator->*createFunc)(m_argIndex);
			m_currentState = parserState::Number;
			break;
		}
		default:
		{
			throw ExceptionError("Incorrect token after opening bracket.");
		}
		}
	}

	void Parser::cultivateClosingBracketState(TokenInfPtr tokenInf)
	{
		switch (tokenInf->e_tokType)
		{
		case tokenType::opening_bracket:
		{
			m_currentState = parserState::OpBracket;
			break;
		}
		case tokenType::comma:
		{
			execut createFunc = (Creator::createFuncs.find(m_label))->second;
			(this->m_Creator->*createFunc)(m_argIndex);
			m_currentState = parserState::Initial;
			break;
		}
		case tokenType::semicolon:
		{
			execut createFunc = (Creator::createFuncs.find(m_label))->second;
			(this->m_Creator->*createFunc)(m_argIndex);
			libs::cells.push_back(*m_tempCell);
			m_tempCell = new libs::db_cell();
			m_tempCell->init();
			if (m_label == "Pins") {
				m_label = "Macros";
			}
			m_currentState = parserState::Final;
			break;
		}
		case tokenType::word:
		{
			m_word = tokenInf->name;
			m_currentState = parserState::Identifier;
			m_argIndex = Creator::argIndex::first;
			break;
		}
		default:
		{
			throw ExceptionError("Incorrect token after closing bracket.");
		}
		}
	}

	void Parser::cultivateLabelState(TokenInfPtr tokenInf)
	{

		switch (tokenInf->e_tokType)
		{
		case tokenType::word:
		{
			m_word = tokenInf->name;
			m_currentState = parserState::Identifier;
			break;
		}
		default:
		{
			throw ExceptionError("Incorrect token after number.");
		}
		}
		
	}

	void Parser::cultivateNumberState(TokenInfPtr tokenInf)
	{
		switch (tokenInf->e_tokType)
		{
		case tokenType::number:
		{
			m_tempNumber = tokenInf->value;
			execut createFunc = (Creator::createFuncs.find(m_label))->second;
			(this->m_Creator->*createFunc)(m_argIndex);
			m_currentState = parserState::Number;
			break;
		}
		case tokenType::closing_bracket:
		{
			m_currentState = parserState::ClosingBracket;
			break;
		}
		default:
		{
			throw ExceptionError("Incorrect token after number.");
		}
		}
	}

	void Parser::cultivateFinalState(TokenInfPtr tokenInf)
	{
		switch (tokenInf->e_tokType)
		{
		case tokenType::word:
		{
			m_word = tokenInf->name;
			m_currentState = parserState::Identifier;
			break;
		}
		default:
		{
			throw ExceptionError("Line can't start with:" + tokenInf->name);
		}
		}
		m_argIndex = Creator::argIndex::first;
	}

	const std::unordered_map<std::string, execut> Parser::Creator::createFuncs {
		{ "Macros", &Parser::Creator::createCell },
		{ "Pins", &Parser::Creator::createPin },
		{ "Nets", &Parser::Creator::createNet },
		{ "layers", &Parser::Creator::createLayer },
	};

	void Parser::Creator::createCell(argIndex index)
	{

		switch (index) {
		case first:
		{
			m_parent->m_tempCell->name = m_parent->m_word;
			m_parent->m_argIndex = second;
			break;
		}
		case second:
		{
			m_parent->m_tempCell->xl = m_parent->m_tempNumber;
			m_parent->m_argIndex = third;
			break;
		}
		case third:
		{
			m_parent->m_tempCell->yl = m_parent->m_tempNumber;
			m_parent->m_argIndex = fourth;
			break;
		}
		case fourth:
		{
			m_parent->m_tempCell->xh = m_parent->m_tempNumber;
			m_parent->m_argIndex = fifth;
			break;
		}
		case fifth:
		{
			m_parent->m_tempCell->yh = m_parent->m_tempNumber;
			break;
		}
		}
	}

	void Parser::Creator::createPin(argIndex index)
	{
		switch (index) {
		case first:
		{
			m_parent->m_tempPin.name = m_parent->m_word;
			m_parent->m_argIndex = second;
			break;
		}
		case second:
		{
			m_parent->m_tempPin.x = m_parent->m_tempNumber;
			m_parent->m_argIndex = third;
			break;
		}
		case third:
		{
			m_parent->m_tempPin.y = m_parent->m_tempNumber;
			m_parent->m_argIndex = fourth;
			break;
		}
		case fourth:
		{
			m_parent->m_tempCell->pins.push_back(m_parent->m_tempPin);
			m_parent->m_argIndex = first;
		}
		}
	}

	void Parser::Creator::createNet(argIndex index)
	{
		if (m_parent->m_firstNet) {
			std::vector<libs::db_cell>::iterator cellsI = libs::cells.begin();
			std::vector<libs::db_pin>::iterator pinsI;
			for (; cellsI != libs::cells.end(); ++cellsI) {
				pinsI = cellsI->pins.begin();
				for (; pinsI != cellsI->pins.end(); ++pinsI) {
					pinsI->parent = &*cellsI;
				}
			}
			m_parent->m_firstNet = false;
		}

		switch(index) {
		case first:
		{
			m_parent->m_tempNet.name = m_parent->m_word;
			m_parent->m_argIndex = second;
			m_parent->m_tempNet.cells.clear();
			break;
		}
		case second:
		{
			libs::db_cell tempCell;
			tempCell.name = m_parent->m_word;
			m_parent->m_tempCell = &*std::find(libs::cells.begin(), libs::cells.end(), tempCell);
			m_parent->m_tempNet.cells.push_back(*m_parent->m_tempCell);
			m_parent->m_argIndex = third;
			break;
		}
		case third:
		{
			libs::db_pin tempPin;
			tempPin.name = m_parent->m_word;
			m_parent->m_tempPin = *std::find(m_parent->m_tempCell->pins.begin(), m_parent->m_tempCell->pins.end(), tempPin);
			m_parent->m_tempNet.pins.push_back(m_parent->m_tempPin);
			m_parent->m_argIndex = second;
			break;
		}
		}
	}

	void Parser::Creator::createLayer(argIndex index)
	{
	}
}