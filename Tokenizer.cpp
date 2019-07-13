#include "Tokenizer.h"
#include "Exception.h"
#include <fstream>

namespace cultivateInput {
	Tokenizer::Tokenizer()
	{
		m_token = "";
		m_currentState = tokenizerState::Initial;
	}

	void Tokenizer::tokenize(std::ifstream& input_file)
	{
		char symbol;
		while (input_file.get(symbol)) {
			switch (m_currentState) {
			case tokenizerState::Initial:
			{
				cultivateInitialState(symbol);
				break;
			}
			case tokenizerState::Word:
			{
				cultivateWordState(symbol);
				break;
			}
			case tokenizerState::Number:
			{
				cultivateNumberState(symbol);
				break;
			}
			case tokenizerState::OpBracket:
			{
				cultivateOpBracketState(symbol);
				break;
			}
			case tokenizerState::ClosingBracket:
			{
				cultivateClosingBracketState(symbol);
				break;
			}
			case tokenizerState::Label:
			{
				cultivateLabelState(symbol);
				break;
			}
			}
		}
	}

	std::vector<TokenInfPtr>& Tokenizer::getTokenPool()
	{
		return m_tokensPool;
	}


	void Tokenizer::cultivateInitialState(char symbol) {
		switch (symbol) {
		case ' ':
		case '\n':
		{
			break;
		}
		case '(':
		{
			cultivateFromInitToOpBracket();
			break;
		}
		case ')':
		{
			cultivateFromInitToClosingBracket();
			break;
		}
		case ':':
		{
			cultivateFromInitToLabel();
			break;
		}
		case ',':
		{
			cultivateFromInitToComma();
			break;
		}
		default:
		{
			if (isalpha(symbol)) {
				cultivateFromInitToWord(symbol);
			}
			else if (isdigit(symbol)) {
				cultivateFromInitToNumber(symbol);
			}
			else {
				throw ExceptionError("Token can't be started with symbol " + symbol);
			}
			break;
		}
		}

	}

	void Tokenizer::cultivateWordState(char symbol)
	{
		switch (symbol) {
		case ' ':
		case '\n':
		{
			cultivateFromWordToInit();
			break;
		}
		case ';':
		{
			cultivateFromWordToFinal();
			break;
		}
		default:
		{
			if (isalpha(symbol) || isdigit(symbol)) {
				cultivateFromWordToWord(symbol);
			}
			else {
				throw ExceptionError("Indentifire can't contain symbol: " + symbol);
			}
			break;
		}
		}
	}

	void Tokenizer::cultivateNumberState(char symbol)
	{
		switch (symbol) {
		case ' ':
		case '\n':
		{
			cultivateFromNumberToInit();
			break;
		}
		default:
		{
			if (isdigit(symbol)) {
				cultivateFromNumberToNumber(symbol);
			}
			else {
				throw ExceptionError("Number can't contain symbol: " + symbol);
			}
			break;
		}
		}

	}

	void Tokenizer::cultivateOpBracketState(char symbol)
	{
		switch (symbol) {
		case ' ':
		{
			cultivateFromOpBracketToInit();
			break;
		}
		default:
		{
			throw ExceptionError("After Opening Bracket can't be symbol: " + symbol);
			break;
		}
		}
	}

	void Tokenizer::cultivateClosingBracketState(char symbol)
	{
		switch(symbol) {
		case ' ':
		case '\n':
		{	
			cultivateFromClosingBracketToInit();
			break;
		}
		case ';':
		{
			cultivateFromClosingBracketToFinal();
			break;
		}
		default:
		{
			throw ExceptionError("After Closing Bracket can't be symbol: " + symbol);
			break;
		}
		}
	}

	void Tokenizer::cultivateLabelState(char symbol)
	{
		switch (symbol) {
		case ' ':
		case '\n':
		{
			cultivateFromLabelToInit();
			break;
		}
		default:
		{
			throw ExceptionError("After ':' can't be symbol: " + symbol);
			break;
		}
		}
	}

	void Tokenizer::cultivateFromInitToWord(const char& symbol)
	{
		m_token += symbol;
		m_currentState = tokenizerState::Word;
	}

	void Tokenizer::cultivateFromInitToNumber(const char & symbol)
	{
		m_token += symbol;
		m_currentState = tokenizerState::Number;
	}

	void Tokenizer::cultivateFromInitToOpBracket()
	{
		m_currentState = tokenizerState::OpBracket;
	}

	void Tokenizer::cultivateFromInitToClosingBracket()
	{
		m_currentState = tokenizerState::ClosingBracket;
	}

	void Tokenizer::cultivateFromInitToLabel()
	{
		m_currentState = tokenizerState::Label;
	}

	void Tokenizer::cultivateFromInitToComma()
	{
		TokenInfPtr m_tok_inf = new TokenInf();
		m_tok_inf->e_tokType = comma;
		m_tok_inf->name = ',';
		m_tokensPool.push_back(m_tok_inf);
		m_token = "";
		m_currentState = tokenizerState::Initial;
		return;
	}

	void Tokenizer::cultivateFromWordToInit()
	{
		TokenInfPtr m_tok_inf = new TokenInf();
		m_tok_inf->e_tokType = word;
		m_tok_inf->name = m_token;
		m_tokensPool.push_back(m_tok_inf);
		m_token = "";
		m_currentState = tokenizerState::Initial;
		return;
	}

	void Tokenizer::cultivateFromWordToFinal()
	{
		TokenInfPtr m_tok_inf = new TokenInf();
		m_tok_inf->e_tokType = word;
		m_tok_inf->name = m_token;
		m_tokensPool.push_back(m_tok_inf);
		TokenInfPtr m_tok_inf2 = new TokenInf();
		m_tok_inf2->e_tokType = semicolon;
		m_tok_inf2->name = ';';
		m_tokensPool.push_back(m_tok_inf2);
		m_token = "";
		m_currentState = tokenizerState::Initial;
		return;
	}

	void Tokenizer::cultivateFromWordToWord(const char & symbol)
	{
		m_token += symbol;
	}

	void Tokenizer::cultivateFromNumberToInit()
	{
		TokenInfPtr m_tok_inf = new TokenInf();
		m_tok_inf->e_tokType = number;
		m_tok_inf->value = std::stoi(m_token);
		m_tokensPool.push_back(m_tok_inf);
		m_token = "";
		m_currentState = tokenizerState::Initial;
		return;
	}

	void Tokenizer::cultivateFromNumberToNumber(const char & symbol)
	{
		m_token += symbol;
	}

	void Tokenizer::cultivateFromOpBracketToInit()
	{
		TokenInfPtr m_tok_inf = new TokenInf();
		m_tok_inf->e_tokType = opening_bracket;
		m_tok_inf->name = '(';
		m_tokensPool.push_back(m_tok_inf);
		m_token = "";
		m_currentState = tokenizerState::Initial;
		return;
	}

	void Tokenizer::cultivateFromClosingBracketToInit()
	{
		TokenInfPtr m_tok_inf = new TokenInf();
		m_tok_inf->e_tokType = closing_bracket;
		m_tok_inf->name = ')';
		m_tokensPool.push_back(m_tok_inf);
		m_token = "";
		m_currentState = tokenizerState::Initial;
		return;
	}

	void Tokenizer::cultivateFromClosingBracketToFinal()
	{
		TokenInfPtr m_tok_inf = new TokenInf();
		m_tok_inf->e_tokType = closing_bracket;
		m_tok_inf->name = ')';
		m_tokensPool.push_back(m_tok_inf);
		TokenInfPtr m_tok_inf2 = new TokenInf();
		m_tok_inf2->e_tokType = semicolon;
		m_tok_inf2->name = ';';
		m_tokensPool.push_back(m_tok_inf2);
		m_token = "";
		m_currentState = tokenizerState::Initial;
		return;
	}

	void Tokenizer::cultivateFromLabelToInit()
	{
		TokenInfPtr m_tok_inf = new TokenInf();
		m_tok_inf->e_tokType = label;
		m_tok_inf->name = ':';
		m_tokensPool.push_back(m_tok_inf);
		m_token = "";
		m_currentState = tokenizerState::Initial;
		return;
	}




}


