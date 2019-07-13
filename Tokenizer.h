#ifndef TOKENIZER_H
#define TOKENIZER_H

#include "TokenInf.h"

#include <vector>
#include <string>

namespace cultivateInput {

	class Tokenizer {
	public:
		Tokenizer();
		void tokenize(std::ifstream& input_file);
		std::vector<TokenInfPtr>& getTokenPool();
	public:
		// cultivateFunctions
		void cultivateInitialState(char symbol);
		void cultivateWordState(char symbol);
		void cultivateNumberState(char symbol);
		void cultivateOpBracketState(char symbol);
		void cultivateClosingBracketState(char symbol);
		void cultivateLabelState(char symbol);

		// cultivateFromInit
		void cultivateFromInitToWord(const char& symbol);
		void cultivateFromInitToNumber(const char& symbol);
		void cultivateFromInitToOpBracket();
		void cultivateFromInitToClosingBracket();
		void cultivateFromInitToLabel();
		void cultivateFromInitToComma();

		//cultivateFromWord
		void cultivateFromWordToInit();
		void cultivateFromWordToWord(const char& symbol);
		void cultivateFromWordToFinal();

		//cultivateFromNumber
		void cultivateFromNumberToInit();
		void cultivateFromNumberToNumber(const char& symbol);
		void cultivateFromOpBracketToInit();

		void cultivateFromClosingBracketToInit();
		void cultivateFromClosingBracketToFinal();
		
		void cultivateFromLabelToInit();
	private:
		enum tokenizerState {
			Initial,
			Word,
			Number,
			OpBracket,
			ClosingBracket,
			Label,
			WordFinal,
			CELL,
			LAYER
		};
	private:
		std::vector<TokenInfPtr> m_tokensPool;
		tokenizerState m_currentState;
		std::string m_token;
	};
}

#endif
