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

#include <cmath>
#include <OffSideRule.hpp>

/**
 * An abstract method for getting the next token.
 * 
 * Whenever a `blockStartToken` is found, an INDENT token is generated. Whenever the next line
 * starts with less indentation that the previous line, at least 1 DEDENT token is generated.
 * 
 * INDENT and DEDENT are meant to handle blocks (which start with `:`) in Python.
 */
antlr::RefToken OffSideRule::nextToken() {
    // There are no backlogged tokens
    if (buffer.empty()) {
        const auto& token = input.nextToken();  // reads in a token

        // Detect if currently in/out of `()`, `{}`, or `[]`
        checkBracketToken(token);

        // Detect if the statement should have a block
        expectBlockCheck(token);

        // [INDENT] The token matches the token used to indicate the start of a block
        if (token->getType() == blockStartToken && expectBlock && numBrackets == 0) {
            token->setType(srcMLParser::INDENT);
            ++numIndents;
            expectBlock = false;
            recordToken = true;
            checkDocstring = true;

            handleBlocks(token);
        }

        // Place unhandled token in buffer to ensure it is not lost
        if (buffer.empty())
            buffer.emplace_back(token);
    }

    // Return the token that was most recently added to the buffer
    antlr::RefToken bufferToken = buffer.back();
    buffer.pop_back();
    return bufferToken;
}

/**
 * Checks for indentation/dedentation levels in a block. Handles nested blocks, too.
 * 
 * Indentation should stay consistent when inside the block for best results.
 * Indentation is determined based on the first non-EOL/WS/WS_EOL/Comment token in the block.
 * 
 * Operates under the assumption `token` is an INDENT token.
 */
void OffSideRule::handleBlocks(antlr::RefToken token) {
    while (true) {
        const auto& nextToken = input.nextToken();  // reads in a token
        int startingIndents = numIndents;
        bool addNextToken = true;  // Ensure nextToken was not already added to the indent buffer

        // Detect if currently in/out of `()`, `{}`, or `[]`
        checkBracketToken(nextToken);

        // Detect if the statement should have a block
        expectBlockCheck(nextToken);

        // [DEDENT] If the statement begins and ends on a single line, end it at EOL
        if (isOneLineStatement && nextToken->getType() == srcMLParser::EOL && numBrackets == 0) {
            indentBuffer.emplace_back(nextToken);

            auto dedentToken = srcMLToken::factory();
            dedentToken->setType(srcMLParser::DEDENT);
            indentBuffer.emplace_back(dedentToken);

            --numIndents;
            isOneLineStatement = false;
            addNextToken = false;
        }

        // Check if the statement begins and ends on a single line
        if (checkOneLineStatement) {
            isOneLineStatement = false;

            switch (tokenAfterIndent->getType()) {
                // Typical multi-line statement with a block
                case srcMLParser::EOL:
                case srcMLParser::WS_EOL:
                    break;

                // Whitespace alone is not enough to determine a one-line statement
                case srcMLParser::WS: {
                    // WS + non-Comment token indicates a one-line statement
                    if (!checkCommentToken(nextToken))
                        isOneLineStatement = true;

                    break;
                }
                // All other non-comment tokens indicate a one-line statement
                default:
                    if (!checkCommentToken(tokenAfterIndent))
                        isOneLineStatement = true;

                    break;
            }

            checkOneLineStatement = false;
        }

        // Record the token directly after INDENT
        if (recordToken) {
            tokenAfterIndent = nextToken;
            checkOneLineStatement = true;
            recordToken = false;
        }

        // [INDENT] The token matches the token used to indicate the start of a block
        if (nextToken->getType() == blockStartToken && expectBlock && numBrackets == 0) {
            nextToken->setType(srcMLParser::INDENT);
            ++numIndents;
            expectBlock = false;
            recordToken = true;
            checkDocstring = true;
        }

        // Detect if a one-line function or class block starts with a string
        if (isOneLineStatement && nextToken->getType() != srcMLParser::WS && checkDocstring)
            convertToDocstring(nextToken);

        // Record the indentation level at the start of a line (if in a block)
        // Do not record indentation level if the first statements are one-line statements
        if (numIndents > 0 && nextToken->getColumn() == 1 && nextToken->getType() == srcMLParser::WS && !isOneLineStatement) {
            const auto& postWSToken = input.nextToken();  // reads in a token

            if (checkCommentToken(postWSToken)) {
                indentBuffer.emplace_back(nextToken);

                // Detect if currently in/out of `()`, `{}`, or `[]`
                checkBracketToken(postWSToken);

                // Detect if the statement should have a block
                expectBlockCheck(postWSToken);

                indentBuffer.emplace_back(postWSToken);
                continue;
            }
            else {
                // Detect if a multi-line function or class block starts with a string
                if (checkDocstring)
                    convertToDocstring(postWSToken);

                tempPostWSToken = postWSToken;
            }

            // The number of spaces per indent was not initialized yet
            if (numSpacesPerIndent == -1)
                numSpacesPerIndent = ceil(nextToken->getText().length() / numIndents);

            int numExpectedIndents = ceil(nextToken->getText().length() / numSpacesPerIndent);

            // [DEDENT] There is less indentation on the current line than the previous line
            if (numExpectedIndents < numIndents) {
                indentBuffer.emplace_back(nextToken);

                // Add the extra token from earlier to the original buffer, if applicable
                if (tempPostWSToken->getType() != 0) {
                    indentBuffer.emplace_back(tempPostWSToken);

                    // Detect if currently in/out of `()`, `{}`, or `[]`
                    checkBracketToken(tempPostWSToken);

                    // Detect if the statement should have a block
                    expectBlockCheck(tempPostWSToken);

                    tempPostWSToken = srcMLToken::factory();
                }

                for (int i = 0; i < (numIndents - numExpectedIndents); ++i) {
                    auto dedentToken = srcMLToken::factory();
                    dedentToken->setType(srcMLParser::DEDENT);
                    indentBuffer.emplace_back(dedentToken);
                }

                numIndents = numExpectedIndents;
                addNextToken = false;
            }
        }

        // [DEDENT] Close any INDENT tokens if the start of the next non-EOL/WS/WS_EOL/Comment line is 1
        if (
            numIndents > 0
            && !isOneLineStatement
            && nextToken->getColumn() == 1
            && nextToken->getType() != srcMLParser::EOL
            && nextToken->getType() != srcMLParser::WS
            && nextToken->getType() != srcMLParser::WS_EOL
            && !checkCommentToken(nextToken)
        ) {
            indentBuffer.emplace_back(nextToken);

            // Add the extra token from earlier to the original buffer, if applicable
            if (tempPostWSToken->getType() != 0) {
                indentBuffer.emplace_back(tempPostWSToken);

                // Detect if currently in/out of `()`, `{}`, or `[]`
                checkBracketToken(tempPostWSToken);

                // Detect if the statement should have a block
                expectBlockCheck(tempPostWSToken);

                tempPostWSToken = srcMLToken::factory();
            }

            for (int i = 0; i < numIndents; ++i) {
                auto dedentToken = srcMLToken::factory();
                dedentToken->setType(srcMLParser::DEDENT);
                indentBuffer.emplace_back(dedentToken);
            }

            numIndents = 0;
        }

        // [DEDENT] Close remaining INDENT tokens if at the end of the file
        else if (numIndents > 0 && nextToken->getType() == srcMLParser::EOF_) {
            indentBuffer.emplace_back(nextToken);

            // Add the extra token from earlier to the original buffer, if applicable
            if (tempPostWSToken->getType() != 0) {
                indentBuffer.emplace_back(tempPostWSToken);

                // Detect if currently in/out of `()`, `{}`, or `[]`
                checkBracketToken(tempPostWSToken);

                // Detect if the statement should have a block
                expectBlockCheck(tempPostWSToken);

                tempPostWSToken = srcMLToken::factory();
            }

            for (int i = 0; i < numIndents; ++i) {
                auto dedentToken = srcMLToken::factory();
                dedentToken->setType(srcMLParser::DEDENT);
                indentBuffer.emplace_back(dedentToken);
            }

            numIndents = 0;
        }

        // Place unhandled token in buffer to ensure it is not lost
        else {
            // Is false if any DEDENT tokens were generated before this if-else block
            // In that case, nextToken was already added to the indent buffer
            if (addNextToken) {
                indentBuffer.emplace_back(nextToken);

                // Add the extra token from earlier to the original buffer, if applicable
                if (tempPostWSToken->getType() != 0) {
                    indentBuffer.emplace_back(tempPostWSToken);

                    // Detect if currently in/out of `()`, `{}`, or `[]`
                    checkBracketToken(tempPostWSToken);

                    // Detect if the statement should have a block
                    expectBlockCheck(tempPostWSToken);

                    tempPostWSToken = srcMLToken::factory();
                }
            }
        }

        // Block is finished once there are zero INDENTs remaining
        if (startingIndents > 0 && numIndents == 0)
            break;
    }

    arrangeBlockEnds();
    buffer.emplace_back(token);
}

/**
 * Approximates where a block "should" end, then moves DEDENT tokens to that location.
 * 
 * Should only be called after generating DEDENT tokens via the `handleBlocks` function.
 */
void OffSideRule::arrangeBlockEnds() {
    while (!indentBuffer.empty()) {
        // Found at least 1 DEDENT token, so re-arrange them to fit closer to where the block "ends"
        if (indentBuffer.back()->getType() == srcMLParser::DEDENT) {
            int numDedents = 0;

            // Record and remove all DEDENTs from the indent buffer
            while (indentBuffer.back()->getType() == srcMLParser::DEDENT) {
                ++numDedents;
                indentBuffer.pop_back();
            }

            // Top of indent buffer is the first token after the block ends
            // Remove it from the indent buffer and add it to the original buffer
            buffer.emplace_back(indentBuffer.back());
            indentBuffer.pop_back();

            // Remove tokens until we find a non-EOL/WS/WS_EOL/Comment token
            // Add any token removed this way to the temporary buffer
            while (!indentBuffer.empty()) {
                if (
                    indentBuffer.back()->getType() == srcMLParser::EOL
                    || indentBuffer.back()->getType() == srcMLParser::WS
                    || indentBuffer.back()->getType() == srcMLParser::WS_EOL
                    || checkCommentToken(indentBuffer.back())
                ) {
                    tempBuffer.emplace_front(indentBuffer.back());
                    indentBuffer.pop_back();
                }
                else
                    break;
            }

            // Handle invalid indentation or blocks with only comments as their content
            if (indentBuffer.empty()) {
                // Empty the rest of the temporary buffer and add its contents to the original buffer
                while (!tempBuffer.empty()) {
                    buffer.emplace_back(tempBuffer.back());
                    tempBuffer.pop_back();
                }

                // Add the DEDENT token(s) to the original buffer
                for (int i = 0; i < numDedents; ++i) {
                    auto dedentToken = srcMLToken::factory();
                    dedentToken->setType(srcMLParser::DEDENT);
                    buffer.emplace_back(dedentToken);
                }

                break;
            }

            // DEDENT tokens do not contain line/column info, so they are handled differently
            // Occurs if a one-line statement is the last/only content in a block
            // (e.g., a Python class with only a one-line function in the class)
            if (indentBuffer.back()->getType() == srcMLParser::DEDENT) {
                // Empty the rest of the temporary buffer and add its contents to the original buffer
                while (!tempBuffer.empty()) {
                    buffer.emplace_back(tempBuffer.back());
                    tempBuffer.pop_back();
                }

                // Add the DEDENT token(s) to the original buffer
                for (int i = 0; i < numDedents; ++i) {
                    auto dedentToken = srcMLToken::factory();
                    dedentToken->setType(srcMLParser::DEDENT);
                    buffer.emplace_back(dedentToken);
                }
            }
            else {
                int blockEndLine = indentBuffer.back()->getLine();  // DEDENT will go on the line after this line number (where the block "ends")

                // Empty the temporary buffer into the original buffer until reaching the block end line
                if (!tempBuffer.empty()) {
                    while (tempBuffer.back()->getLine() != blockEndLine) {
                        buffer.emplace_back(tempBuffer.back());
                        tempBuffer.pop_back();
                    }
                }

                // Add the DEDENT token(s) to the original buffer
                for (int i = 0; i < numDedents; ++i) {
                    auto dedentToken = srcMLToken::factory();
                    dedentToken->setType(srcMLParser::DEDENT);
                    buffer.emplace_back(dedentToken);
                }

                // Empty the rest of the temporary buffer and add its contents to the original buffer
                while (!tempBuffer.empty()) {
                    buffer.emplace_back(tempBuffer.back());
                    tempBuffer.pop_back();
                }
            }
        }
        // Add non-DEDENT tokens back to the original buffer so they are not lost
        else {
            buffer.emplace_back(indentBuffer.back());
            indentBuffer.pop_back();
        }
    }
}

/**
 * Detects opening and closing brackets (e.g., `()`, `{}`, and `[]`).
 * 
 * Ensures certain colon (`:`) tokens do not start blocks in Python.
 * Examples including array slicing, dictionairies, and type annotations.
 * 
 * Operates under the assumption the code contains balanced brackets.
 */
void OffSideRule::checkBracketToken(antlr::RefToken token) {
    switch (token->getType()) {
        case srcMLParser::LPAREN:
        case srcMLParser::PY_LCURLY:
        case srcMLParser::LBRACKET:
            ++numBrackets;
            bracketBuffer.emplace_front(token->getType());
            break;

        case srcMLParser::RPAREN:
        case srcMLParser::PY_RCURLY:
        case srcMLParser::RBRACKET:
            if (!bracketBuffer.empty()) {
                --numBrackets;
                bracketBuffer.pop_front();
            }
            break;

        default:
            break;
    }
}

/**
 * Checks if `token` is a starting or ending comment token in Python.
 * 
 * Returns `true` if `token` is a comment token, `false` otherwise.
 */
bool OffSideRule::checkCommentToken(antlr::RefToken token) {
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
 * Checks if `token` is a Python docstring (or an orindary string).
 * 
 * Converts a `STRING_START` token's type to `DQUOTE_DOCSTRING_START`
 * (or a `CHAR_START` token's type to `SQUOTE_DOCSTRING_START`) if
 * `token` starts a docstring.
 * 
 * Python docstrings are supported if they appear directly beneath a
 * class/function or if the it is the first non-EOL/WS/WS_EOL/Comment
 * token in the file.
 */
void OffSideRule::convertToDocstring(antlr::RefToken token) {
    if (isFunctionOrClass && token->getType() == srcMLParser::STRING_START)
        token->setType(srcMLParser::DQUOTE_DOCSTRING_START);

    if (isFunctionOrClass && token->getType() == srcMLParser::CHAR_START)
        token->setType(srcMLParser::SQUOTE_DOCSTRING_START);

    checkDocstring = false;
    isFunctionOrClass = false;
}

/**
 * Checks if a block is expected because a line starts with `token` in Python.
 * 
 * Such tokens include keywords (e.g., `if`), specifiers (e.g., `async`), and attributes (e.g., `@`).
 */
void OffSideRule::expectBlockCheck(antlr::RefToken token) {
    if ((token->getColumn() == 1 || delayExpectBlockCheck) && numBrackets == 0) {
        if (delayExpectBlockCheck)
            delayExpectBlockCheck = false;

        if (srcMLParser::expect_blocks_token_set.member(token->getType()))
            expectBlock = true;

        if (token->getType() == srcMLParser::PY_FUNCTION || token->getType() == srcMLParser::CLASS)
            isFunctionOrClass = true;

        // whitespace at column 1 is indentation
        if (token->getType() == srcMLParser::WS)
            delayExpectBlockCheck = true;
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
