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

/**
 * An abstract method for getting the next token.
 * 
 * Whenever a `blockStartToken` is found, an INDENT token is generated. Whenever the next line starts with
 * less indentation that the previous line, a DEDENT token is generated (ignores blank lines).
 * 
 * INDENT and DEDENT are meant to handle blocks in languages such as Python that use a colon to represent
 * the start of a block (as opposed to curly braces, etc.).
 */
antlr::RefToken NewlineTerminatePython::nextToken() {

    // place all input tokens in the buffer so we can insert a TERMINATE
    if (buffer.empty()) {
        auto token = input.nextToken();

        // For a newline, insert a TERMINATE, except on an INDENT line
        if ((!isEmptyLine && token->getType() == srcMLParser::EOL && lastToken->getType() != srcMLParser::INDENT) ||
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
            isEmptyLine = true;
        } else if (token->getType() != srcMLParser::WS) {
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
