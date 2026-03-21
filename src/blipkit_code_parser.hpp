#pragma once

#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/core/gdvirtual.gen.inc>
#include <godot_cpp/core/math.hpp>

using namespace godot;

namespace BlipKit {

class BlipKitCodeParser : public RefCounted {
	GDCLASS(BlipKitCodeParser, RefCounted)

private:
	enum TokenType {
		TOKEN_OTHER,
		TOKEN_WHITESPACE,
		TOKEN_INDENTION,
		TOKEN_LINE_BREAK,
		TOKEN_COMMENT,
		TOKEN_SEMICOLON,
		TOKEN_IDENTIFIER,
		// End states.
		TOKEN_END,
	};

	enum StateType {
		STATE_ROOT,
		STATE_LINE,
		STATE_IGNORE,
		STATE_COMMAND,
		STATE_ARGUMENT,
		// End states.
		STATE_END,
		STATE_ERROR,
	};

	struct TokenTable {
		static constexpr uint32_t SIZE = 256;
		static constexpr uint32_t CHAR_NO_BREAK_SPACE = 0xA0;

		TokenType table[SIZE] = {};

		constexpr TokenTable();

		constexpr _ALWAYS_INLINE_ TokenType get(uint32_t p_char) const {
			return table[Math::min(p_char, SIZE - 1)];
		}
	};

	static const TokenTable token_table;

	struct {
		String code;
		const char32_t *ptr = nullptr;
		uint32_t size = 0;
	} code;
	uint32_t offset = 0;
	uint32_t line = 0;
	uint32_t line_start = 0;
	int32_t indention = 0;
	int32_t indention_prev = -1;
	String error;
	struct {
		uint32_t start = 0;
		uint32_t end = 0;
		uint32_t line = 0;
	} token;

	_ALWAYS_INLINE_ uint32_t column() const {
		return offset - line_start;
	}

	_ALWAYS_INLINE_ bool has_token() const {
		return offset < code.size;
	}

	_ALWAYS_INLINE_ TokenType next_token() {
		return has_token() ? token_table.get(code.ptr[offset++]) : TOKEN_END;
	}

	_ALWAYS_INLINE_ void next_while(TokenType p_token) {
		while (has_token() && token_table.get(code.ptr[offset]) == p_token) {
			offset++;
		}
	}

	_ALWAYS_INLINE_ void advance_line() {
		line++;
		line_start = offset;
		indention = 0;
	}

	[[nodiscard]] TokenType get_token();
	[[nodiscard]] StateType next_state(StateType p_state, TokenType p_token);
	String get_token_value() const;

	[[nodiscard]] StateType begin_command();
	[[nodiscard]] StateType add_argument();
	[[nodiscard]] StateType unexpected_token(TokenType p_token);

public:
	bool parse(const String &p_code);
	String get_error_message() const;
	void clear();

	GDVIRTUAL2(_begin_command, String, int);
	GDVIRTUAL1(_add_argument, String);

protected:
	static void _bind_methods();
	String _to_string() const;
};

} //namespace BlipKit
