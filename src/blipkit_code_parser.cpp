#include "blipkit_code_parser.hpp"

using namespace godot;
using namespace BlipKit;

static constexpr char32_t NO_BREAK_SPACE = 0xA0;

const BlipKitCodeParser::TokenType BlipKitCodeParser::token_table[TOKEN_TABLE_SIZE] = {
	['\t'] = BlipKitCodeParser::TOKEN_INDENTION,
	['\n'] = BlipKitCodeParser::TOKEN_LINE_BREAK,
	['\r'] = BlipKitCodeParser::TOKEN_SPACE,
	[' '] = BlipKitCodeParser::TOKEN_SPACE,
	['%'] = BlipKitCodeParser::TOKEN_COMMENT,
	[';'] = BlipKitCodeParser::TOKEN_SEMICOLON,
	[NO_BREAK_SPACE] = BlipKitCodeParser::TOKEN_SPACE,
	[TOKEN_TABLE_SIZE - 1] = BlipKitCodeParser::TOKEN_OTHER, // Explicite fallback.
};

BlipKitCodeParser::TokenType BlipKitCodeParser::get_token() {
	token.offset_start = offset;
	token.line = line;

	TokenType type = next_token();
	token.offset_end = offset;

	switch (type) {
		case TOKEN_INDENTION: {
			next_while(TOKEN_INDENTION);
			indention = offset - token.offset_start;
			token.offset_end = offset;
			break;
		}
		case TOKEN_SPACE:
		case TOKEN_SEMICOLON: {
			next_while(TOKEN_SPACE);
			token.offset_end = offset;
			break;
		}
		case TOKEN_LINE_BREAK: {
			advance_line();
			token.offset_end = offset;
			break;
		}
		case TOKEN_COMMENT: {
			token.offset_start = offset;
			do {
				type = next_token();
				if (type != TOKEN_LINE_BREAK) {
					token.offset_end = offset;
				} else {
					advance_line();
					break;
				}
			} while (type != TOKEN_END);
			type = TOKEN_COMMENT;
			token.offset_end = offset;
			break;
		}
		case TOKEN_OTHER: {
			next_while(TOKEN_OTHER);
			token.offset_end = offset;
			next_while(TOKEN_SPACE);
			break;
		}
		default: {
			break;
		}
	}

	return type;
}

BlipKitCodeParser::StateType BlipKitCodeParser::next_state(StateType p_state, TokenType p_token) {
	switch (p_state) {
		case STATE_ROOT: {
			switch (p_token) {
				case TOKEN_SPACE: {
					return STATE_IGNORE;
				}
				case TOKEN_LINE_BREAK:
				case TOKEN_COMMENT: {
					return STATE_ROOT;
				}
				case TOKEN_INDENTION: {
					return STATE_LINE;
				}
				case TOKEN_OTHER: {
					return next_state(STATE_COMMAND, p_token);
				}
				case TOKEN_END: {
					return STATE_END;
				}
				default: {
					return unexpected_token(p_token);
				}
			}
			break;
		}
		case STATE_LINE: {
			switch (p_token) {
				case TOKEN_SPACE: {
					return STATE_IGNORE;
				}
				case TOKEN_LINE_BREAK:
				case TOKEN_COMMENT: {
					return STATE_ROOT;
				}
				case TOKEN_OTHER: {
					return next_state(STATE_COMMAND, p_token);
				}
				case TOKEN_END: {
					return STATE_END;
				}
				default: {
					return unexpected_token(p_token);
				}
			}
			break;
		}
		case STATE_IGNORE: {
			switch (p_token) {
				case TOKEN_SPACE:
				case TOKEN_INDENTION: {
					return STATE_IGNORE;
				}
				case TOKEN_LINE_BREAK:
				case TOKEN_COMMENT: {
					return STATE_ROOT;
				}
				case TOKEN_END: {
					return STATE_END;
				}
				default: {
					return unexpected_token(p_token);
				}
			}
			break;
		}
		case STATE_COMMAND: {
			switch (p_token) {
				case TOKEN_OTHER: {
					return begin_command();
				}
				case TOKEN_LINE_BREAK:
				case TOKEN_COMMENT: {
					return STATE_ROOT;
				}
				case TOKEN_END: {
					return STATE_END;
				}
				default: {
					return unexpected_token(p_token);
				}
			}
			break;
		}
		case STATE_ARGUMENT: {
			switch (p_token) {
				case TOKEN_OTHER: {
					return add_argument();
				}
				case TOKEN_SEMICOLON: {
					return STATE_COMMAND;
				}
				case TOKEN_LINE_BREAK:
				case TOKEN_COMMENT: {
					return STATE_ROOT;
				}
				case TOKEN_END: {
					return STATE_END;
				}
				default: {
					return unexpected_token(p_token);
				}
			}
			break;
		}
		default: {
			break;
		}
	}

	return STATE_ERROR;
}

String BlipKitCodeParser::get_token_value() const {
	return code.substr(token.offset_start, token.offset_end - token.offset_start);
}

BlipKitCodeParser::StateType BlipKitCodeParser::begin_command() {
	if (indention > prev_indention + 1) {
		error = vformat("Wrong indention on line %d (got %d but expected %d)", line + 1, indention, prev_indention + 1);
		return STATE_ERROR;
	}

	// TODO: handle indention.

	prev_indention = indention;

	// TODO: Implement.

	const String value = code.substr(token.offset_start, token.offset_end - token.offset_start);
	printf("CMD %s (%d)\n", get_token_value().utf8().ptr(), indention);

	return STATE_ARGUMENT;
}

BlipKitCodeParser::StateType BlipKitCodeParser::add_argument() {
	// TODO: Implement.

	const String value = code.substr(token.offset_start, token.offset_end - token.offset_start);
	printf("ARG %s\n", get_token_value().utf8().ptr());

	return STATE_ARGUMENT;
}

BlipKitCodeParser::StateType BlipKitCodeParser::unexpected_token(TokenType p_token) {
	error = vformat("Unexpected token \"%s\" (%d) on line %d", get_token_value(), p_token, line + 1);
	return STATE_ERROR;
}

bool BlipKitCodeParser::parse(const String &p_code) {
	clear();

	code = p_code;
	code_ptr = code.ptr();
	code_length = code.length();
	StateType state = STATE_ROOT;

	while (state < STATE_END) {
		const TokenType type = get_token();
		state = next_state(state, type);
	}

	return state != STATE_ERROR;
}

String BlipKitCodeParser::get_error_message() const {
	return error;
}

void BlipKitCodeParser::clear() {
	code.resize(0);
	code_ptr = nullptr;
	code_length = 0;
	error.resize(0);
	offset = 0;
	line = 0;
	prev_indention = -1;
	indention = 0;
	token.line = 0;
	token.offset_start = 0;
	token.offset_end = 0;
}

void BlipKitCodeParser::_bind_methods() {
	ClassDB::bind_method(D_METHOD("parse", "code"), &BlipKitCodeParser::parse);
	ClassDB::bind_method(D_METHOD("get_error_message"), &BlipKitCodeParser::get_error_message);
	ClassDB::bind_method(D_METHOD("clear"), &BlipKitCodeParser::clear);
}

String BlipKitCodeParser::_to_string() const {
	return vformat("<BlipKitCodeParser#%d>", get_instance_id());
}
