// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file NewlineTerminatePython.hpp
 *
 * @copyright Copyright (C) 2024 srcML, LLC. (www.srcML.org)
 *
 * This file is part of the srcML Toolkit.
 *
 * Injects INDENT and DEDENT tokens to the token stream
 */

#include <NewlineTerminatePython.hpp>

// Inserts TERMINATE tokens at EOL for Python
antlr::RefToken NewlineTerminatePython::nextToken() {

    // place all input tokens in the buffer so we can insert a TERMINATE
    if (buffer.empty()) {
        auto token = input.nextToken();

        if (token->getType() == srcMLParser::LPAREN)
            ++parenthesesCount;
        else if (token->getType() == srcMLParser::RPAREN && parenthesesCount > 0)
            --parenthesesCount;

        // // For a newline, insert a TERMINATE, except on an INDENT line
        // if (parenthesesCount == 0 && !first && lastToken->getType() != srcMLParser::TERMINATE &&
        //     (!isEmptyLine && token->getType() == srcMLParser::EOL && lastToken->getType() != srcMLParser::INDENT) ||
        //     (token->getType() == 1 /* EOF */ && lastToken->getType() != srcMLParser::EOL)) {

        // For a newline, insert a TERMINATE in certain cases
        if ((token->getType() == srcMLParser::EOL &&

            // not an empty line
            !firstCharacter &&

            // not an existing TERMINATE
            lastToken->getType() != srcMLParser::TERMINATE &&

            // not in the middle of an expression with a previous operator
            // Python does not have any postfix operators, so an operator at the end means the expression is not complete
            lastNonWhitespaceToken->getType() != srcMLParser::OPERATORS &&
            lastNonWhitespaceToken->getType() != srcMLParser::TEMPOPE &&
            lastNonWhitespaceToken->getType() != srcMLParser::TEMPOPS &&

            // not a whitespace line
            !isWhitespaceLine &&

            // no inserted INDENT, which implies a block
            lastToken->getType() != srcMLParser::INDENT) ||

            // At EOF with no previous EOL
            (token->getType() == 1 /* EOF */ && lastToken->getType() != srcMLParser::EOL)) {

            // create new terminate token
            auto terminateToken = srcMLToken::factory();
            terminateToken->setType(srcMLParser::TERMINATE);
            terminateToken->setColumn(1);
            terminateToken->setLine(token->getLine());

            // insert terminal token
            buffer.emplace_back(terminateToken);
        }

        if (token->getType() == srcMLParser::EOL) {
            first = true;
            isEmptyLine = true;
        } else if (token->getType() != srcMLParser::WS) {
            first = false;
            isEmptyLine = false;
        }

        // record to check for previous INDENT
        lastToken = token;

        // insert read token
        buffer.emplace_back(token);
    }

    // next token
    auto token = buffer.front();
    buffer.pop_front();
    return token;
}
