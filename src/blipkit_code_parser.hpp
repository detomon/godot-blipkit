#pragma once

#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/core/math.hpp>

using namespace godot;

namespace BlipKit {

class BlipKitCodeParser : public RefCounted {
	GDCLASS(BlipKitCodeParser, RefCounted)

private:
	enum TokenType {
		TOKEN_OTHER,
		TOKEN_SPACE,
		TOKEN_INDENTION,
		TOKEN_LINE_BREAK,
		TOKEN_COMMENT,
		TOKEN_SEMICOLON,
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

	static constexpr uint32_t TOKEN_TABLE_SIZE = 256;

	static const TokenType token_table[TOKEN_TABLE_SIZE];

	String code;
	const char32_t *code_ptr = nullptr;
	uint32_t code_length = 0;
	uint32_t offset = 0;
	uint32_t line = 0;
	int32_t prev_indention = -1;
	int32_t indention = 0;
	String error;
	struct {
		uint32_t offset_start = 0;
		uint32_t offset_end = 0;
		uint32_t line = 0;
	} token;

	_ALWAYS_INLINE_ bool has_token() const {
		return offset < code_length;
	}

	_ALWAYS_INLINE_ TokenType token_from_char(uint32_t p_char) const {
		return token_table[Math::min(p_char, TOKEN_TABLE_SIZE - 1)];
	}

	_ALWAYS_INLINE_ TokenType next_token() {
		return has_token() ? token_from_char(code_ptr[offset++]) : TOKEN_END;
	}

	_ALWAYS_INLINE_ void next_while(TokenType p_token) {
		while (has_token() && token_from_char(code_ptr[offset]) == p_token) {
			offset++;
		}
	}

	_ALWAYS_INLINE_ void advance_line() {
		line++;
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

protected:
	static void _bind_methods();
	String _to_string() const;
};

} //namespace BlipKit
