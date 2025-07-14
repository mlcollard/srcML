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
const srcMLState::MODE_TYPE ModeStack::MODE_STATEMENT                 (bit(0));
const srcMLState::MODE_TYPE ModeStack::MODE_LIST                      (bit(1));
const srcMLState::MODE_TYPE ModeStack::MODE_EXPECT                    (bit(2));
const srcMLState::MODE_TYPE ModeStack::MODE_DETECT_COLON              (bit(3));
const srcMLState::MODE_TYPE ModeStack::MODE_TEMPLATE                  (bit(4));
const srcMLState::MODE_TYPE ModeStack::MODE_ARGUMENT                  (bit(5));
const srcMLState::MODE_TYPE ModeStack::MODE_NAMESPACE                 (bit(6));
const srcMLState::MODE_TYPE ModeStack::MODE_PARAMETER                 (bit(7));
const srcMLState::MODE_TYPE ModeStack::MODE_EXPRESSION                (bit(8));
const srcMLState::MODE_TYPE ModeStack::MODE_CALL                      (bit(9));
const srcMLState::MODE_TYPE ModeStack::MODE_CONDITION                 (bit(10));
const srcMLState::MODE_TYPE ModeStack::MODE_TOP                       (bit(11));
const srcMLState::MODE_TYPE ModeStack::MODE_BLOCK                     (bit(12));
const srcMLState::MODE_TYPE ModeStack::MODE_INIT                      (bit(13));
const srcMLState::MODE_TYPE ModeStack::MODE_FUNCTION_TAIL             (bit(14));
const srcMLState::MODE_TYPE ModeStack::MODE_PARSE_EOL                 (bit(15));
const srcMLState::MODE_TYPE ModeStack::MODE_LOCAL                     (bit(16));
const srcMLState::MODE_TYPE ModeStack::MODE_VARIABLE_NAME             (bit(17));
const srcMLState::MODE_TYPE ModeStack::MODE_IF                        (bit(18));
const srcMLState::MODE_TYPE ModeStack::MODE_TOP_SECTION               (bit(19));
const srcMLState::MODE_TYPE ModeStack::MODE_CONTROL                   (bit(20));
const srcMLState::MODE_TYPE ModeStack::MODE_CONTROL_INITIALIZATION    (bit(21));
const srcMLState::MODE_TYPE ModeStack::MODE_CONTROL_CONDITION         (bit(22));
const srcMLState::MODE_TYPE ModeStack::MODE_CONTROL_INCREMENT         (bit(23));
const srcMLState::MODE_TYPE ModeStack::MODE_PREPROC                   (bit(24));
const srcMLState::MODE_TYPE ModeStack::MODE_NEST                      (bit(25));
const srcMLState::MODE_TYPE ModeStack::MODE_EXPRESSION_BLOCK          (bit(26));
const srcMLState::MODE_TYPE ModeStack::MODE_INTERNAL_END_PAREN        (bit(27));
const srcMLState::MODE_TYPE ModeStack::MODE_ACCESS_REGION             (bit(28));
const srcMLState::MODE_TYPE ModeStack::MODE_DO_STATEMENT              (bit(29));
const srcMLState::MODE_TYPE ModeStack::MODE_IGNORE_TERMINATE          (bit(30));
const srcMLState::MODE_TYPE ModeStack::MODE_USING                     (bit(31));
const srcMLState::MODE_TYPE ModeStack::MODE_EXTERN                    (bit(32));
const srcMLState::MODE_TYPE ModeStack::MODE_TRAILING_RETURN           (bit(33));
const srcMLState::MODE_TYPE ModeStack::MODE_INTERNAL_END_CURLY        (bit(34));
const srcMLState::MODE_TYPE ModeStack::MODE_ISSUE_EMPTY_AT_POP        (bit(35));
const srcMLState::MODE_TYPE ModeStack::MODE_INITIALIZATION_LIST       (bit(36));
const srcMLState::MODE_TYPE ModeStack::MODE_CLASS                     (bit(37));
const srcMLState::MODE_TYPE ModeStack::MODE_END_AT_ENDIF              (bit(38));
const srcMLState::MODE_TYPE ModeStack::MODE_END_AT_BLOCK              (bit(39));
const srcMLState::MODE_TYPE ModeStack::MODE_END_ONLY_AT_RPAREN        (bit(40));
const srcMLState::MODE_TYPE ModeStack::MODE_END_AT_BLOCK_NO_TERMINATE (bit(41));
const srcMLState::MODE_TYPE ModeStack::MODE_ARGUMENT_LIST             (bit(42));
const srcMLState::MODE_TYPE ModeStack::MODE_ASSOCIATION_LIST          (bit(43));
const srcMLState::MODE_TYPE ModeStack::MODE_FUNCTION_NAME             (bit(44));
const srcMLState::MODE_TYPE ModeStack::MODE_ELSE                      (bit(45));
const srcMLState::MODE_TYPE ModeStack::MODE_TYPEDEF                   (bit(46));
const srcMLState::MODE_TYPE ModeStack::MODE_DECL                      (bit(47));
const srcMLState::MODE_TYPE ModeStack::MODE_EAT_TYPE                  (bit(48));
const srcMLState::MODE_TYPE ModeStack::MODE_FUNCTION_PARAMETER        (bit(49));
const srcMLState::MODE_TYPE ModeStack::MODE_TERNARY                   (bit(50));
const srcMLState::MODE_TYPE ModeStack::MODE_OBJECTIVE_C_CALL          (bit(51));
const srcMLState::MODE_TYPE ModeStack::MODE_INNER_DECL                (bit(52));
const srcMLState::MODE_TYPE ModeStack::MODE_SWITCH                    (bit(53));
const srcMLState::MODE_TYPE ModeStack::MODE_TERNARY_CONDITION         (bit(54));
const srcMLState::MODE_TYPE ModeStack::MODE_THEN                      (bit(55));
const srcMLState::MODE_TYPE ModeStack::MODE_IN_INIT                   (bit(56));
const srcMLState::MODE_TYPE ModeStack::MODE_TRY                       (bit(57));
const srcMLState::MODE_TYPE ModeStack::MODE_END_LIST_AT_BLOCK         (bit(58));
const srcMLState::MODE_TYPE ModeStack::MODE_TEMPLATE_PARAMETER_LIST   (bit(59));
const srcMLState::MODE_TYPE ModeStack::MODE_ONLY_END_TERMINATE        (bit(60));
const srcMLState::MODE_TYPE ModeStack::MODE_ENUM                      (bit(61));
const srcMLState::MODE_TYPE ModeStack::MODE_ANONYMOUS                 (bit(62));
const srcMLState::MODE_TYPE ModeStack::MODE_END_AT_COMMA              (bit(63));
const srcMLState::MODE_TYPE ModeStack::MODE_RANGED_FOR                (bit(64));
const srcMLState::MODE_TYPE ModeStack::MODE_ASSOCIATION_TYPE          (bit(65));
const srcMLState::MODE_TYPE ModeStack::MODE_FRIEND                    (bit(66));
const srcMLState::MODE_TYPE ModeStack::MODE_CLASS_NAME                (bit(67));
const srcMLState::MODE_TYPE ModeStack::MODE_FUNCTION_BODY             (bit(68));
const srcMLState::MODE_TYPE ModeStack::MODE_FUNCTION_TYPE             (bit(69));
const srcMLState::MODE_TYPE ModeStack::MODE_END_CONTROL               (bit(70));
const srcMLState::MODE_TYPE ModeStack::MODE_FOR_LIKE_LIST             (bit(71));
const srcMLState::MODE_TYPE ModeStack::MODE_FUNCTION_CALL             (bit(72));
const srcMLState::MODE_TYPE ModeStack::MODE_IF_STATEMENT              (bit(73));
const srcMLState::MODE_TYPE ModeStack::MODE_BLOCK_CONTENT             (bit(74));
const srcMLState::MODE_TYPE ModeStack::MODE_NO_BLOCK_CONTENT          (bit(75));
const srcMLState::MODE_TYPE ModeStack::MODE_INCLUDE_ATTRIBUTE         (bit(76));
