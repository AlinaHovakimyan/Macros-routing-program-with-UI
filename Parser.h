#ifndef PARSER_H
#define PARSER_H

#include <vector>
#include <unordered_map>

#include "TokenInf.h"
#include "db_cell.h"
#include "Net.h"

namespace cultivateInput {

	class Parser {
		const std::vector<std::string> labels = {
			"Macros",
			"Pins",
			"Nets",
			"layers"
		};
		enum label {
			CELL,
			LAYER
		};

		enum parserState {
			Initial,
			Identifier,
			Number,
			OpBracket,
			ClosingBracket,
			Label,
			Final,
		};
	private:
		std::vector<TokenInfPtr>& tokensPool;
		parserState m_currentState;
		std::string m_label;
		std::string m_word;
		int m_tempNumber;
		libs::db_cell* m_tempCell;
		libs::db_pin m_tempPin;
		libs::Net    m_tempNet;

	public:
		class Creator {
			Parser* m_parent;
		public:
			enum argIndex {
				first,
				second,
				third,
				fourth,
				fifth
			};
			Creator(Parser* parser) :m_parent(parser) {};
			void createCell(argIndex);
			void createPin(argIndex);
			void createNet(argIndex);
			void createLayer(argIndex);
			typedef void(cultivateInput::Parser::Creator::*execut)(argIndex);

			static const std::unordered_map<std::string, execut> createFuncs;
		};

		Creator* m_Creator;
		Creator::argIndex m_argIndex;
		bool m_firstNet;

	private:
		// cultivateState
		void cultivateInitialState(TokenInfPtr tokenInf);
		void cultivateIdentifierState(TokenInfPtr tokenInf);
		//-----------
		void cultivateOpeningBracketState(TokenInfPtr tokenInf);
		void cultivateClosingBracketState(TokenInfPtr tokenInf);
		void cultivateLabelState(TokenInfPtr tokenInf);
		void cultivateNumberState(TokenInfPtr tokenInf);
		void cultivateFinalState(TokenInfPtr tokenInf);

	public:
		Parser(std::vector<TokenInfPtr>& t_pool);
		void parse();
	};
}

#endif
