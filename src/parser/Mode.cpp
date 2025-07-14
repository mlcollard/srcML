// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file Mode.cpp
 *
 * @copyright Copyright (C) 2014-2024 srcML, LLC. (www.srcML.org)
 *
 * This file is part of the srcML Toolkit.
 */

#include <ModeStack.hpp>

// Constant MODE_TYPE with bit set
static constexpr srcMLState::MODE_TYPE bit(size_t pos) {
    return srcMLState::MODE_TYPE{}.set(pos);
}

/* Set of mode flags */
const srcMLState::MODE_TYPE ModeStack::MODE_STATEMENT                 (bit(__COUNTER__));
const srcMLState::MODE_TYPE ModeStack::MODE_LIST                      (bit(__COUNTER__));
const srcMLState::MODE_TYPE ModeStack::MODE_EXPECT                    (bit(__COUNTER__));
const srcMLState::MODE_TYPE ModeStack::MODE_DETECT_COLON              (bit(__COUNTER__));
const srcMLState::MODE_TYPE ModeStack::MODE_TEMPLATE                  (bit(__COUNTER__));
const srcMLState::MODE_TYPE ModeStack::MODE_ARGUMENT                  (bit(__COUNTER__));
const srcMLState::MODE_TYPE ModeStack::MODE_NAMESPACE                 (bit(__COUNTER__));
const srcMLState::MODE_TYPE ModeStack::MODE_PARAMETER                 (bit(__COUNTER__));
const srcMLState::MODE_TYPE ModeStack::MODE_EXPRESSION                (bit(__COUNTER__));
const srcMLState::MODE_TYPE ModeStack::MODE_CALL                      (bit(__COUNTER__));
const srcMLState::MODE_TYPE ModeStack::MODE_CONDITION                 (bit(__COUNTER__));
const srcMLState::MODE_TYPE ModeStack::MODE_TOP                       (bit(__COUNTER__));
const srcMLState::MODE_TYPE ModeStack::MODE_BLOCK                     (bit(__COUNTER__));
const srcMLState::MODE_TYPE ModeStack::MODE_INIT                      (bit(__COUNTER__));
const srcMLState::MODE_TYPE ModeStack::MODE_FUNCTION_TAIL             (bit(__COUNTER__));
const srcMLState::MODE_TYPE ModeStack::MODE_PARSE_EOL                 (bit(__COUNTER__));
const srcMLState::MODE_TYPE ModeStack::MODE_LOCAL                     (bit(__COUNTER__));
const srcMLState::MODE_TYPE ModeStack::MODE_VARIABLE_NAME             (bit(__COUNTER__));
const srcMLState::MODE_TYPE ModeStack::MODE_IF                        (bit(__COUNTER__));
const srcMLState::MODE_TYPE ModeStack::MODE_TOP_SECTION               (bit(__COUNTER__));
const srcMLState::MODE_TYPE ModeStack::MODE_CONTROL                   (bit(__COUNTER__));
const srcMLState::MODE_TYPE ModeStack::MODE_CONTROL_INITIALIZATION    (bit(__COUNTER__));
const srcMLState::MODE_TYPE ModeStack::MODE_CONTROL_CONDITION         (bit(__COUNTER__));
const srcMLState::MODE_TYPE ModeStack::MODE_CONTROL_INCREMENT         (bit(__COUNTER__));
const srcMLState::MODE_TYPE ModeStack::MODE_PREPROC                   (bit(__COUNTER__));
const srcMLState::MODE_TYPE ModeStack::MODE_NEST                      (bit(__COUNTER__));
const srcMLState::MODE_TYPE ModeStack::MODE_EXPRESSION_BLOCK          (bit(__COUNTER__));
const srcMLState::MODE_TYPE ModeStack::MODE_INTERNAL_END_PAREN        (bit(__COUNTER__));
const srcMLState::MODE_TYPE ModeStack::MODE_ACCESS_REGION             (bit(__COUNTER__));
const srcMLState::MODE_TYPE ModeStack::MODE_DO_STATEMENT              (bit(__COUNTER__));
const srcMLState::MODE_TYPE ModeStack::MODE_IGNORE_TERMINATE          (bit(__COUNTER__));
const srcMLState::MODE_TYPE ModeStack::MODE_USING                     (bit(__COUNTER__));
const srcMLState::MODE_TYPE ModeStack::MODE_EXTERN                    (bit(__COUNTER__));
const srcMLState::MODE_TYPE ModeStack::MODE_TRAILING_RETURN           (bit(__COUNTER__));
const srcMLState::MODE_TYPE ModeStack::MODE_INTERNAL_END_CURLY        (bit(__COUNTER__));
const srcMLState::MODE_TYPE ModeStack::MODE_ISSUE_EMPTY_AT_POP        (bit(__COUNTER__));
const srcMLState::MODE_TYPE ModeStack::MODE_INITIALIZATION_LIST       (bit(__COUNTER__));
const srcMLState::MODE_TYPE ModeStack::MODE_CLASS                     (bit(__COUNTER__));
const srcMLState::MODE_TYPE ModeStack::MODE_END_AT_ENDIF              (bit(__COUNTER__));
const srcMLState::MODE_TYPE ModeStack::MODE_END_AT_BLOCK              (bit(__COUNTER__));
const srcMLState::MODE_TYPE ModeStack::MODE_END_ONLY_AT_RPAREN        (bit(__COUNTER__));
const srcMLState::MODE_TYPE ModeStack::MODE_END_AT_BLOCK_NO_TERMINATE (bit(__COUNTER__));
const srcMLState::MODE_TYPE ModeStack::MODE_ARGUMENT_LIST             (bit(__COUNTER__));
const srcMLState::MODE_TYPE ModeStack::MODE_ASSOCIATION_LIST          (bit(__COUNTER__));
const srcMLState::MODE_TYPE ModeStack::MODE_FUNCTION_NAME             (bit(__COUNTER__));
const srcMLState::MODE_TYPE ModeStack::MODE_ELSE                      (bit(__COUNTER__));
const srcMLState::MODE_TYPE ModeStack::MODE_TYPEDEF                   (bit(__COUNTER__));
const srcMLState::MODE_TYPE ModeStack::MODE_DECL                      (bit(__COUNTER__));
const srcMLState::MODE_TYPE ModeStack::MODE_EAT_TYPE                  (bit(__COUNTER__));
const srcMLState::MODE_TYPE ModeStack::MODE_FUNCTION_PARAMETER        (bit(__COUNTER__));
const srcMLState::MODE_TYPE ModeStack::MODE_TERNARY                   (bit(__COUNTER__));
const srcMLState::MODE_TYPE ModeStack::MODE_OBJECTIVE_C_CALL          (bit(__COUNTER__));
const srcMLState::MODE_TYPE ModeStack::MODE_INNER_DECL                (bit(__COUNTER__));
const srcMLState::MODE_TYPE ModeStack::MODE_SWITCH                    (bit(__COUNTER__));
const srcMLState::MODE_TYPE ModeStack::MODE_TERNARY_CONDITION         (bit(__COUNTER__));
const srcMLState::MODE_TYPE ModeStack::MODE_THEN                      (bit(__COUNTER__));
const srcMLState::MODE_TYPE ModeStack::MODE_IN_INIT                   (bit(__COUNTER__));
const srcMLState::MODE_TYPE ModeStack::MODE_TRY                       (bit(__COUNTER__));
const srcMLState::MODE_TYPE ModeStack::MODE_END_LIST_AT_BLOCK         (bit(__COUNTER__));
const srcMLState::MODE_TYPE ModeStack::MODE_TEMPLATE_PARAMETER_LIST   (bit(__COUNTER__));
const srcMLState::MODE_TYPE ModeStack::MODE_ONLY_END_TERMINATE        (bit(__COUNTER__));
const srcMLState::MODE_TYPE ModeStack::MODE_ENUM                      (bit(__COUNTER__));
const srcMLState::MODE_TYPE ModeStack::MODE_ANONYMOUS                 (bit(__COUNTER__));
const srcMLState::MODE_TYPE ModeStack::MODE_END_AT_COMMA              (bit(__COUNTER__));
const srcMLState::MODE_TYPE ModeStack::MODE_RANGED_FOR                (bit(__COUNTER__));
const srcMLState::MODE_TYPE ModeStack::MODE_ASSOCIATION_TYPE          (bit(__COUNTER__));
const srcMLState::MODE_TYPE ModeStack::MODE_FRIEND                    (bit(__COUNTER__));
const srcMLState::MODE_TYPE ModeStack::MODE_CLASS_NAME                (bit(__COUNTER__));
const srcMLState::MODE_TYPE ModeStack::MODE_FUNCTION_BODY             (bit(__COUNTER__));
const srcMLState::MODE_TYPE ModeStack::MODE_FUNCTION_TYPE             (bit(__COUNTER__));
const srcMLState::MODE_TYPE ModeStack::MODE_END_CONTROL               (bit(__COUNTER__));
const srcMLState::MODE_TYPE ModeStack::MODE_FOR_LIKE_LIST             (bit(__COUNTER__));
const srcMLState::MODE_TYPE ModeStack::MODE_FUNCTION_CALL             (bit(__COUNTER__));
const srcMLState::MODE_TYPE ModeStack::MODE_IF_STATEMENT              (bit(__COUNTER__));
const srcMLState::MODE_TYPE ModeStack::MODE_BLOCK_CONTENT             (bit(__COUNTER__));
const srcMLState::MODE_TYPE ModeStack::MODE_NO_BLOCK_CONTENT          (bit(__COUNTER__));
const srcMLState::MODE_TYPE ModeStack::MODE_INCLUDE_ATTRIBUTE         (bit(__COUNTER__));
