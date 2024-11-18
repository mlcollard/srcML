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
 * INDENT and DEDENT are meant to handle blocks in languages such as Python that use a colon to represent
 * the start of a block (as opposed to curly braces, etc.).
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
        if (token->getType() == blockStartToken) {
            const auto& nextToken = input.nextToken();  // reads in a token

            // Check if LA(1) == EOL or EOF_; valid indentation
            if ((nextToken->getType() == srcMLParser::EOL || nextToken->getType() == srcMLParser::EOF_) && nextToken->getColumn() > 1) {
                token->setType(srcMLParser::INDENT);
                buffer.emplace_back(nextToken);
                ++numIndents;

                newIndent = true;
                buffer.emplace_back(token);
            }

            // Check if LA(1) == WS; could be valid indentation
            if (buffer.empty() && nextToken->getType() == srcMLParser::WS) {
                const auto& extraToken = input.nextToken();  // reads in a token
                buffer.emplace_back(extraToken);

                // Check if LA(2) == EOL or EOF_; valid indentation
                if ((extraToken->getType() == srcMLParser::EOL || extraToken->getType() == srcMLParser::EOF_) && extraToken->getColumn() > 1) {
                    token->setType(srcMLParser::INDENT);
                    buffer.emplace_back(nextToken);
                    ++numIndents;

                    newIndent = true;
                    buffer.emplace_back(token);
                }
            }

            // blockStartToken did not indicate new indentation
            if (!newIndent) {
                buffer.emplace_back(nextToken);
                buffer.emplace_back(token);
            }
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

        // [DEDENT] A newline ('\n') token could indicate the end of a block
        if (buffer.empty() && token->getType() == srcMLParser::EOL && token->getColumn() > 1 && numIndents > 0) {
            while (true) {
                const auto& nextToken = input.nextToken();

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

        // Place the unhandled token into the buffer to ensure it is not lost
        if (buffer.empty())
            buffer.emplace_back(token);
    }

    // There is at least one backlogged token
    if (!buffer.empty()) {
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
