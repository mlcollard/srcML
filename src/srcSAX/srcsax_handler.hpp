/**
 * @file srcsax_handler.hpp
 *
 * @copyright Copyright (C) 2013-2014 srcML, LLC. (www.srcML.org)
 *
 * srcSAX is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * srcSAX is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with the srcML Toolkit; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef INCLUDED_SRCSAX_HANDLER_HPP
#define INCLUDED_SRCSAX_HANDLER_HPP

#include <libxml/parser.h>

struct srcsax_context;

/**
 * srcsax_namespace
 *
 * Data structure for a srcML/xml namespace
 */
struct srcsax_namespace {

    /** a namespace prefix */
    const char* prefix;

    /** a namespace uri */
    const char* uri;
};

/**
 * srcsax_attribute
 *
 * Data structure for a srcML/xml attribute
 */
struct srcsax_attribute {

    /** attribute name */
    const char* localname;

    /** attribute namespace prefix */
    const char* prefix;

    /** attribute namespace uri */
    const char* uri;

    /** attribute value */
    const char* value;
};

/**
 * srcsax_handler
 *
 * Struct of srcSAX callback functions i.e. srcSAX handler.
 */
struct srcsax_handler {

    /**
     * start_document
     * @param context a srcSAX context
     *
     * Signature for srcSAX handler function for start of document.
     */
    void (*start_document)(srcsax_context* context);

    /**
     * end_document
     * @param context a srcSAX context
     *
     * Signature for srcSAX handler function for end of document.
     */
    void (*end_document)(srcsax_context* context);

    /**
     * start_root
     * @param context a srcSAX context
     * @param localname the name of the element tag
     * @param prefix the tag prefix
     * @param URI the namespace of tag
     * @param num_namespaces number of namespaces definitions
     * @param namespaces the defined namespaces
     * @param num_attributes the number of attributes on the tag
     * @param attributes list of attributes
     *
     * Signature for srcSAX handler function for start of the root element.
     */
    void (*start_root)(srcsax_context* context, const char* localname, const char* prefix, const char* URI,
                           int num_namespaces, const xmlChar** namespaces, int num_attributes,
                           const xmlChar** attributes);

    /**
     * start_unit
     * @param context a srcSAX context
     * @param localname the name of the element tag
     * @param prefix the tag prefix
     * @param URI the namespace of tag
     * @param num_namespaces number of namespaces definitions
     * @param namespaces the defined namespaces
     * @param num_attributes the number of attributes on the tag
     * @param attributes list of attributes
     *
     * Signature srcSAX handler function for start of an unit.
     */
    void (*start_unit)(srcsax_context* context, const char* localname, const char* prefix, const char* URI,
                           int num_namespaces, const xmlChar** namespaces, int num_attributes,
                           const xmlChar** attributes);

    /**
     * start_element
     * @param context a srcSAX context
     * @param localname the name of the element tag
     * @param prefix the tag prefix
     * @param URI the namespace of tag
     * @param num_namespaces number of namespaces definitions
     * @param namespaces the defined namespaces
     * @param num_attributes the number of attributes on the tag
     * @param attributes list of attributes
     *
     * Signature for srcSAX handler function for start of an element.
     */
    void (*start_element)(srcsax_context* context, const char* localname, const char* prefix, const char* URI,
                                int num_namespaces, const xmlChar** namespaces, int num_attributes,
                                const xmlChar** attributes);

    /**
     * end_root
     * @param context a srcSAX context
     * @param localname the name of the element tag
     * @param prefix the tag prefix
     * @param URI the namespace of tag
     *
     * Signature for srcSAX handler function for end of the root element.
     */
    void (*end_root)(srcsax_context* context, const char* localname, const char* prefix, const char* URI);

    /**
     * end_unit
     * @param context a srcSAX context
     * @param localname the name of the element tag
     * @param prefix the tag prefix
     * @param URI the namespace of tag
     *
     * Signature for srcSAX handler function for end of an unit.
     */
    void (*end_unit)(srcsax_context* context, const char* localname, const char* prefix, const char* URI);

    /**
     * end_element
     * @param context a srcSAX context
     * @param localname the name of the element tag
     * @param prefix the tag prefix
     * @param URI the namespace of tag
     *
     * Signature for srcSAX handler function for end of an element.
     */
    void (*end_element)(srcsax_context* context, const char* localname, const char* prefix, const char* URI);

    /**
     * characters_root
     * @param context a srcSAX context
     * @param ch the characers
     * @param len number of characters
     *
     * Signature for srcSAX handler function for character handling at the root level.
     */
    void (*characters_root)(srcsax_context* context, const char* ch, int len);

    /**
     * characters_unit
     * @param ch the characers
     * @param len number of characters
     *
     * Signature for srcSAX handler function for character handling within a unit.
     */
    void (*characters_unit)(srcsax_context* context, const char* ch, int len);

    /**
     * meta_tag
     * @param context a srcSAX context
     * @param localname the name of the element tag
     * @param prefix the tag prefix
     * @param URI the namespace of tag
     * @param num_namespaces number of namespaces definitions
     * @param namespaces the defined namespaces
     * @param num_attributes the number of attributes on the tag
     * @param attributes list of attributes
     *
     * Signature for srcSAX handler function for meta tags.
     */
    void (*meta_tag)(srcsax_context* context, const char* localname, const char* prefix, const char* URI,
                           int num_namespaces, const xmlChar** namespaces, int num_attributes,
                           const xmlChar** attributes);

    /**
     * comment
     * @param context a srcSAX context
     * @param value the comment content
     *
     * Signature for srcSAX handler function for a XML comment.
     */
    void (*comment)(srcsax_context* context, const char* value);

    /**
     * cdata_block
     * @param context a srcSAX context
     * @param value the pcdata content
     * @param len the block length
     *
     * Signature for srcSAX handler function for pcdata block.
     */
    void (*cdata_block)(srcsax_context* context, const char* value, int len);

    /**
     * processing_instruction
     * @param context a srcSAX context
     * @param target the processing instruction target.
     * @param data the processing instruction data.
     *
     * Signature for srcSAX handler function for processing instruction.
     */
    void (*processing_instruction)(srcsax_context* context, const char* target, const char* data);

};

#endif
