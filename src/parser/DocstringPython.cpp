// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file DocstringPython.hpp
 *
 * @copyright Copyright (C) 2024 srcML, LLC. (www.srcML.org)
 *
 * This file is part of the srcML Toolkit.
 *
 * Alters string literals that should be docstrings in the token stream
 */

#include <DocstringPython.hpp>

// Converts STRING_START/CHAR_START to Python docstrings
antlr::RefToken DocstringPython::nextToken() {
    // place all input tokens in the buffer so we can check for docstrings
    if (buffer.empty()) {
        auto token = input.nextToken();

        // check if at the start of a function or class
        if (token->getType() == srcMLParser::PY_FUNCTION || token->getType() == srcMLParser::CLASS)
            isFunctionOrClass = true;

        // check if at the start of a block
        if (token->getType() == blockStartToken)
            isBlockStart = true;

        // determine if the first non-WS/blockStartToken token in a function or class
        // is a string (and thus a docstring); otherwise, stop looking for a docstring
        if (
            isFunctionOrClass
            && isBlockStart
            && token->getType() != blockStartToken
            && token->getType() != srcMLParser::EOL
            && !srcMLParser::whitespace_token_set.member(token->getType())
        ) {
            if (token->getType() == srcMLParser::STRING_START)
                token->setType(srcMLParser::DQUOTE_DOCSTRING_START);

            if (token->getType() == srcMLParser::CHAR_START)
                token->setType(srcMLParser::SQUOTE_DOCSTRING_START);

            isFunctionOrClass = false;
            isBlockStart = false;
        }

        // increment the line number in comments
        if (checkCommentToken(token))
            countWSNewlineTokens(token, false);

        // increment the line number in string literals
        if (token->getType() == srcMLParser::DQUOTE_DOCSTRING_END || token->getType() == srcMLParser::SQUOTE_DOCSTRING_END)
            countWSNewlineTokens(token, true);

        // increment the line number at the end of a line
        if (
            token->getType() == srcMLParser::EOL
            || token->getType() == srcMLParser::WS_EOL
            || token->getType() == srcMLParser::EOL_BACKSLASH
        )
            ++lineNumber;

        // insert read token
        buffer.emplace_back(token);
    }

    // next token
    auto token = buffer.front();
    buffer.pop_front();
    return token;
}

/**
 * Record end-of-line tokens in a whitespace `token`'s text.
 * 
 * @param token the whitespace token (e.g., comment, docstring, etc.) to analyze.
 * @param isDocstringEnd use `true` if `token` is `DQUOTE_DOCSTRING_END` or `SQUOTE_DOCSTRING_END`.
 */
void DocstringPython::countWSNewlineTokens(antlr::RefToken token, bool isDocstringEnd) {
    std::string text = token->getText();
    int newlines = std::count(text.begin(), text.end(), '\n');

    for (int i = 0; i < newlines; ++i)
        ++lineNumber;

    // ensure docstring end tokens have accurate line numbers
    if (isDocstringEnd)
        token->setLine(lineNumber);
}

/**
 * Checks if `token` is a starting or ending comment token in Python.
 * 
 * Returns `true` if `token` is a comment token, `false` otherwise.
 */
bool DocstringPython::checkCommentToken(antlr::RefToken token) {
    bool isComment = false;

    switch (token->getType()) {
        case srcMLParser::HASHTAG_COMMENT_START:
        case srcMLParser::HASHTAG_COMMENT_END:
        case srcMLParser::HASHBANG_COMMENT_START:
        case srcMLParser::HASHBANG_COMMENT_END:
            isComment = true;
            break;

        default:
            break;
    }

    return isComment;
}

/**
 * Assigns the value of `token` to `blockStartToken`.
 * 
 * Existing logic was built around the colon (`:`) in Python.
 * Other values may not work as expected, especially in niche situations.
 * 
 * Refer to `srcMLParserTokenTypes.hpp` for all supported values.
 */
void DocstringPython::setBlockStartToken(int token) {
    blockStartToken = token;
}

/**
 * Returns `blockStartToken`.
 */
int DocstringPython::getBlockStartToken() const {
    return blockStartToken;
}
