// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file OffSideRule.hpp
 *
 * @copyright Copyright (C) 2024 srcML, LLC. (www.srcML.org)
 *
 * This file is part of the srcML Toolkit.
 *
 * Injects INDENT and DEDENT tokens to the token stream
 */

#include <OffSideRule.hpp>

/**
 * An abstract method for getting the next token.
 * 
 * Whenever a `blockStartToken` is found, an INDENT token is generated. Whenever the next line starts with
 * less indentation that the previous line, a DEDENT token is generated (ignores blank lines).
 * 
 * INDENT and DEDENT are meant to handle blocks (which start with `:`) in Python.
 */
antlr::RefToken OffSideRule::nextToken() {
    // There are no backlogged tokens
    if (buffer.empty()) {
        const auto& token = input.nextToken();  // reads in a token

        // Update the current indentation level at the start of each new line
        if (token->getColumn() == 1 && token->getType() != srcMLParser::EOL) {
            prevColStart = currentColStart;
            currentColStart = token->getColumn();

            if (token->getType() == srcMLParser::WS) {
                currentColStart = token->getText().length() + 1;  // e.g., 4 spaces starting at column 1, text starting at column 5
            }
        }

        // [INDENT] The token matches the token used to indicate the start of a block
        // Colons always start a block unless in a comment or in group tokens (e.g., `()`, `{}`, and `[]`).
        if (token->getType() == blockStartToken && numParen == 0 && numBraces == 0 && numBrackets == 0) {
            const auto& nextToken = input.nextToken();  // reads in a token

            switch (nextToken->getType()) {
                // Typical multi-line statement with a block
                case srcMLParser::EOL:
                    break;

                // Could have EOL after whitespace
                case srcMLParser::WS: {
                    const auto& extraToken = input.nextToken();  // reads in a token
                    buffer.emplace_back(extraToken);

                    // no EOL after whitespace indicates a one-line statement
                    if (extraToken->getType() != srcMLParser::EOL)
                        oneLineStatement = true;

                    break;
                }
                // All other non-EOL tokens indicate a one-line statement
                default:
                    oneLineStatement = true;
                    break;
            }

            token->setType(srcMLParser::INDENT);
            buffer.emplace_back(nextToken);
            ++numIndents;

            if (!oneLineStatement)
                newIndent = true;

            buffer.emplace_back(token);
        }

        // [DEDENT] One-line statements with a block must end on their respective line
        if (buffer.empty() && oneLineStatement && (token->getType() == srcMLParser::EOL || token->getType() == srcMLParser::EOF_) && numIndents > 0) {
            oneLineStatement = false;
            int prevNumIndents = numIndents;

            // End of a one-line statement that is not nested; generate a DEDENT token
            auto dedentToken = srcMLToken::factory();
            dedentToken->setType(srcMLParser::DEDENT);
            dedentToken->setColumn(1);
            dedentToken->setLine(token->getLine());
            --numIndents;

            // End of a nested one-line statement; may need to generate multiple DEDENT tokens
            if (numIndents > 0)
                generateMultipleDedents(token);

            // Only add token to buffer if initial numIndents was 1 (it was already added if >1)
            if (prevNumIndents == 1)
                buffer.emplace_back(token);

            buffer.emplace_back(dedentToken);
        }

        // [DEDENT] Blocks with improper indentation need to be handled differently
        // Content after a block with no indentation will end the block before the new content begins
        if (buffer.empty() && newIndent) {
            newIndent = false;

            if (prevColStart == currentColStart && token->getType() != srcMLParser::EOL) {
                auto dedentToken = srcMLToken::factory();
                dedentToken->setType(srcMLParser::DEDENT);
                dedentToken->setColumn(1);
                dedentToken->setLine(token->getLine());

                buffer.emplace_back(token);
                --numIndents;

                buffer.emplace_back(dedentToken);
            }
        }

        // [DEDENT] Files that do not end with a newline need to be handled differently
        if (buffer.empty() && token->getType() == srcMLParser::EOF_ && token->getText().empty() && numIndents > 0) {
            buffer.emplace_back(token);

            for (int i = 0; i < numIndents; ++i) {
                auto dedentToken = srcMLToken::factory();
                dedentToken->setType(srcMLParser::DEDENT);
                dedentToken->setColumn(token->getColumn());
                dedentToken->setLine(token->getLine());
                buffer.emplace_back(dedentToken);
            }

            numIndents = 0;
        }

        // [DEDENT] A newline ('\n') token could indicate the end of a (nested) block
        if (buffer.empty() && token->getType() == srcMLParser::EOL && token->getColumn() > 1 && numIndents > 0)
            generateMultipleDedents(token);

        // Place the unhandled token into the buffer to ensure it is not lost
        if (buffer.empty())
            buffer.emplace_back(token);
    }

    // There is at least one backlogged token
    if (!buffer.empty()) {
        // Detect if currently in/out of `()`, `{}`, or `[]`.
        checkGroupSymbol();

        // Place any unhandled blank lines in the buffer before ending the file
        if (buffer.back()->getType() == srcMLParser::EOF_ && !blankLineBuffer.empty()) {
            while (!blankLineBuffer.empty()) {
                buffer.emplace_back(blankLineBuffer.back());
                blankLineBuffer.pop_back();
            }
        }

        // Update the current indentation level at the start of each new line
        if (buffer.back()->getColumn() == 1 && buffer.back()->getType() != srcMLParser::EOL) {
            prevColStart = currentColStart;
            currentColStart = buffer.back()->getColumn();

            if (buffer.back()->getType() == srcMLParser::WS) {
                currentColStart = buffer.back()->getText().length() + 1;  // e.g., 4 spaces starting at column 1, text starting at column 5
            }
        }
    }

    // Return the token that was most recently added to the buffer
    antlr::RefToken bufferToken = buffer.back();
    buffer.pop_back();
    return bufferToken;
}

/**
 * Generates multiple DEDENT tokens for nested Python blocks.
 * 
 * `token` should be the same token used at the top level of `nextToken()`.
 */
void OffSideRule::generateMultipleDedents(antlr::RefToken token) {
    while (true) {
        const auto& nextToken = input.nextToken();  // reads in a token

        // The next line starts with indentation, so record where the next token would start
        if (nextToken->getType() == srcMLParser::WS) {
            buffer.emplace_back(nextToken);
            prevColStart = currentColStart;
            currentColStart = nextToken->getText().length() + 1;  // e.g., 4 spaces starting at column 1, text starting at column 5

            while (!blankLineBuffer.empty()) {
                buffer.emplace_back(blankLineBuffer.back());
                blankLineBuffer.pop_back();
            }

            if (currentColStart < prevColStart) {
                auto dedentToken = srcMLToken::factory();
                dedentToken->setType(srcMLParser::DEDENT);
                dedentToken->setColumn(1);
                dedentToken->setLine(token->getLine() + 1);

                buffer.emplace_back(dedentToken);
                --numIndents;
            }

            buffer.emplace_back(token);
            break;
        }

        // The next line is a blank line, so look for the next non-blank-line token
        else if (nextToken->getType() == srcMLParser::EOL && nextToken->getColumn() == 1) {
            blankLineBuffer.emplace_back(nextToken);
            continue;
        }

        // The next token is the end of the file, so generate DEDENT tokens equal to the remaining INDENT tokens
        else if (nextToken->getType() == srcMLParser::EOF_) {
            buffer.emplace_back(nextToken);

            for (int i = 0; i < numIndents; ++i) {
                auto dedentToken = srcMLToken::factory();
                dedentToken->setType(srcMLParser::DEDENT);
                dedentToken->setLine(nextToken->getLine());
                dedentToken->setColumn(numIndents - i);

                buffer.emplace_back(dedentToken);
            }

            numIndents = 0;
            buffer.emplace_back(token);
            break;
        }

        // The next token starts at column 1
        else {
            buffer.emplace_back(nextToken);
            prevColStart = currentColStart;
            currentColStart = 1;

            while (!blankLineBuffer.empty()) {
                buffer.emplace_back(blankLineBuffer.back());
                blankLineBuffer.pop_back();
            }

            if (currentColStart < prevColStart) {
                for (int i = 0; i < numIndents; ++i) {
                    auto dedentToken = srcMLToken::factory();
                    dedentToken->setType(srcMLParser::DEDENT);
                    dedentToken->setLine(nextToken->getLine());
                    dedentToken->setColumn(numIndents - i);

                    buffer.emplace_back(dedentToken);
                }

                numIndents = 0;
            }

            buffer.emplace_back(token);
            break;
        }
    }
}

/**
 * Detects tokens that group elements (e.g., `()`, `{}`, and `[]`).
 * 
 * Ensures certain colon (`:`) tokens do not start blocks in Python.
 * Examples including array slicing, dictionairies, and type annotations.
 */
void OffSideRule::checkGroupSymbol() {
    switch (buffer.back()->getType()) {
        case srcMLParser::LPAREN:
            ++numParen;
            break;

        case srcMLParser::PY_LCURLY:
            ++numBraces;
            break;

        case srcMLParser::LBRACKET:
            ++numBrackets;
            break;

        case srcMLParser::RPAREN:
            --numParen;
            break;

        case srcMLParser::PY_RCURLY:
            --numBraces;
            break;

        case srcMLParser::RBRACKET:
            --numBrackets;
            break;

        default:
            break;
    }
}

/**
 * Assigns the value of `token` to `blockStartToken`.
 * 
 * Existing logic was built around the colon (`:`) in Python.
 * Other values may not work as expected, especially in niche situations.
 * 
 * Refer to `srcMLParserTokenTypes.hpp` for all supported values.
 */
void OffSideRule::setBlockStartToken(int token) {
    blockStartToken = token;
}

/**
 * Returns `blockStartToken`.
 */
int OffSideRule::getBlockStartToken() const {
    return blockStartToken;
}
