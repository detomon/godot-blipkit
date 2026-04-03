// Copyright © 2026 Simon Schoenenberger and Contributors.

#include "blipkit_code_parser.hpp"
#include <godot_cpp/classes/time.hpp>

using namespace godot;
using namespace BlipKit;

constexpr BlipKitCodeParser::TokenTable::TokenTable() {
	constexpr uint32_t NO_BREAK_SPACE = 0xA0;

	set(TOKEN_END, '\0');

	set(TOKEN_WHITESPACE, { '\r', ' ', NO_BREAK_SPACE });

	set(TOKEN_INDENTION, '\t');
	set(TOKEN_LINE_BREAK, '\n');
	set(TOKEN_COMMENT, '%');
	set(TOKEN_SEMICOLON, ';');

	set(TOKEN_IDENTIFIER, { '#', '_' });
	range(TOKEN_IDENTIFIER, '0', '9');
	range(TOKEN_IDENTIFIER, 'A', 'Z');
	range(TOKEN_IDENTIFIER, 'a', 'Z');
}

const BlipKitCodeParser::TokenTable BlipKitCodeParser::token_table;

BlipKitCodeParser::TokenType BlipKitCodeParser::get_token() {
	token.start = offset;
	token.line = line;

	TokenType type = next_token();
	token.end = offset;

	switch (type) {
		case TOKEN_INDENTION: {
			next_while(TOKEN_INDENTION);
			indention = offset - token.start;
			token.end = offset;
			break;
		}
		case TOKEN_WHITESPACE:
		case TOKEN_SEMICOLON: {
			next_while(TOKEN_WHITESPACE);
			token.end = offset;
			break;
		}
		case TOKEN_LINE_BREAK: {
			advance_line();
			token.end = offset;
			break;
		}
		case TOKEN_COMMENT: {
			token.start = offset;
			do {
				type = next_token();
				if (type == TOKEN_LINE_BREAK) {
					advance_line();
					break;
				} else {
					token.end = offset;
				}
			} while (type != TOKEN_END);
			type = TOKEN_COMMENT;
			token.end = offset;
			break;
		}
		case TOKEN_IDENTIFIER: {
			next_while(TOKEN_IDENTIFIER);
			token.end = offset;
			next_while(TOKEN_WHITESPACE);
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
				case TOKEN_WHITESPACE: {
					return STATE_IGNORE;
				}
				case TOKEN_LINE_BREAK:
				case TOKEN_COMMENT: {
					return STATE_ROOT;
				}
				case TOKEN_INDENTION: {
					return STATE_LINE;
				}
				case TOKEN_IDENTIFIER: {
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
				case TOKEN_WHITESPACE: {
					return STATE_IGNORE;
				}
				case TOKEN_LINE_BREAK:
				case TOKEN_COMMENT: {
					return STATE_ROOT;
				}
				case TOKEN_IDENTIFIER: {
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
				case TOKEN_WHITESPACE:
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
				case TOKEN_IDENTIFIER: {
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
				case TOKEN_IDENTIFIER: {
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
			return STATE_ERROR;
		}
	}
}

String BlipKitCodeParser::get_token_value() const {
	return code.code.substr(token.start, token.end - token.start);
}

BlipKitCodeParser::StateType BlipKitCodeParser::begin_command() {
	const int32_t next_indention = indention_prev + 1;
	if (indention > next_indention) {
		error = vformat("Too much indention on line %d (got %d but expected %d)", line + 1, indention, next_indention);
		return STATE_ERROR;
	}

	indention_prev = indention;

	const String &command = get_token_value();
	GDVIRTUAL_CALL(_begin_command, command, indention);

	return STATE_ARGUMENT;
}

BlipKitCodeParser::StateType BlipKitCodeParser::add_argument() {
	const String &argument = get_token_value();
	GDVIRTUAL_CALL(_add_argument, argument);

	return STATE_ARGUMENT;
}

BlipKitCodeParser::StateType BlipKitCodeParser::unexpected_token(TokenType p_token) {
	const String &token_value = get_token_value();
	const uint32_t token_size = token_value.length();
	String escaped_value;

	for (uint32_t i = 0; i < token_size; i++) {
		const char32_t c = token_value[i];
		if (c >= 0x21 && c <= 0x7E) {
			escaped_value += c;
		} else {
			escaped_value += vformat("\\u%02X", c);
		}
	}

	error = vformat("Unexpected token \"%s\" on line %d:%d", escaped_value, line + 1, column());

	return STATE_ERROR;
}

bool BlipKitCodeParser::parse(const String &p_code) {
	const Time *time = Time::get_singleton();
	const uint64_t ticks = time->get_ticks_usec();

	clear();

	code.code = p_code;
	code.ptr = code.code.ptr();
	code.size = code.code.length();
	StateType state = STATE_ROOT;

	while (state < STATE_END) {
		const TokenType type = get_token();
		state = next_state(state, type);
	}

	const uint64_t ticks2 = time->get_ticks_usec();
	printf("*** parse %lfs\n", double(ticks2 - ticks) / 1000000.0);

	return state != STATE_ERROR;
}

String BlipKitCodeParser::get_error_message() const {
	return error;
}

void BlipKitCodeParser::clear() {
	code.code = String();
	code.ptr = nullptr;
	code.size = 0;
	error = String();
	offset = 0;
	line = 0;
	line_start = 0;
	indention = 0;
	indention_prev = -1;
	token.line = 0;
	token.start = 0;
	token.end = 0;
}

void BlipKitCodeParser::_bind_methods() {
	ClassDB::bind_method(D_METHOD("parse", "code"), &BlipKitCodeParser::parse);
	ClassDB::bind_method(D_METHOD("get_error_message"), &BlipKitCodeParser::get_error_message);
	ClassDB::bind_method(D_METHOD("clear"), &BlipKitCodeParser::clear);

	GDVIRTUAL_BIND(_begin_command, "command", "indention");
	GDVIRTUAL_BIND(_add_argument, "argument");
}

String BlipKitCodeParser::_to_string() const {
	return vformat("<BlipKitCodeParser#%d>", get_instance_id());
}
