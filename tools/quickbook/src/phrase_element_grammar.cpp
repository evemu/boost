/*=============================================================================
    Copyright (c) 2002 2004 2006 Joel de Guzman
    Copyright (c) 2004 Eric Niebler
    http://spirit.sourceforge.net/

    Use, modification and distribution is subject to the Boost Software
    License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
    http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/

#include "grammar_impl.hpp"
#include "actions_class.hpp"
#include "utils.hpp"
#include "phrase_tags.hpp"
#include <boost/spirit/include/classic_core.hpp>
#include <boost/spirit/include/classic_assign_actor.hpp>
#include <boost/spirit/include/classic_clear_actor.hpp>
#include <boost/spirit/include/classic_if.hpp>
#include <boost/spirit/include/phoenix1_primitives.hpp>
#include <boost/spirit/include/phoenix1_casts.hpp>

namespace quickbook
{
    namespace cl = boost::spirit::classic;

    struct phrase_element_grammar_local
    {
        cl::rule<scanner>
                        image, anchor, link, empty, cond_phrase, inner_phrase
                        ;
    };

    void quickbook_grammar::impl::init_phrase_elements()
    {
        phrase_element_grammar_local& local = cleanup_.add(
            new phrase_element_grammar_local);

        elements.add
            ("?", element_info(element_info::phrase, &local.cond_phrase))
            ;

        local.cond_phrase =
                blank
            >>  macro_identifier                [actions.values.entry(ph::arg1, ph::arg2)]
            >>  actions.scoped_cond_phrase()
                [extended_phrase]
            ;

        elements.add
            ("$", element_info(element_info::phrase, &local.image, phrase_tags::image))
            ;

        local.image =
                blank
            >>  cl::if_p(qbk_since(105u)) [
                        (+(
                            *cl::space_p
                        >>  +(cl::anychar_p - (cl::space_p | phrase_end | '['))
                        ))                      [actions.values.entry(ph::arg1, ph::arg2)]
                    >>  hard_space
                    >>  *actions.values.list()
                        [   '['
                        >>  (*(cl::alnum_p | '_')) 
                                                [actions.values.entry(ph::arg1, ph::arg2)]
                        >>  space
                        >>  (*(cl::anychar_p - (phrase_end | '[')))
                                                [actions.values.entry(ph::arg1, ph::arg2)]
                        >>  ']'
                        >>  space
                        ]
                ].else_p [
                        (*(cl::anychar_p - phrase_end))
                                                [actions.values.entry(ph::arg1, ph::arg2)]
                ]
            >>  cl::eps_p(']')
            ;
            
        elements.add
            ("@", element_info(element_info::phrase, &local.link, phrase_tags::url))
            ("link", element_info(element_info::phrase, &local.link, phrase_tags::link))
            ("funcref", element_info(element_info::phrase, &local.link, phrase_tags::funcref))
            ("classref", element_info(element_info::phrase, &local.link, phrase_tags::classref))
            ("memberref", element_info(element_info::phrase, &local.link, phrase_tags::memberref))
            ("enumref", element_info(element_info::phrase, &local.link, phrase_tags::enumref))
            ("macroref", element_info(element_info::phrase, &local.link, phrase_tags::macroref))
            ("headerref", element_info(element_info::phrase, &local.link, phrase_tags::headerref))
            ("conceptref", element_info(element_info::phrase, &local.link, phrase_tags::conceptref))
            ("globalref", element_info(element_info::phrase, &local.link, phrase_tags::globalref))
            ;

        local.link =
                space
            >>  (*(cl::anychar_p - (']' | hard_space)))
                                                [actions.values.entry(ph::arg1, ph::arg2)]
            >>  hard_space
            >>  local.inner_phrase
            ;

        elements.add
            ("#", element_info(element_info::phrase, &local.anchor, phrase_tags::anchor))
            ;

        local.anchor =
                blank
            >>  (*(cl::anychar_p - phrase_end)) [actions.values.entry(ph::arg1, ph::arg2)]
            ;

        elements.add
            ("*", element_info(element_info::phrase, &local.inner_phrase, phrase_tags::bold))
            ("'", element_info(element_info::phrase, &local.inner_phrase, phrase_tags::italic))
            ("_", element_info(element_info::phrase, &local.inner_phrase, phrase_tags::underline))
            ("^", element_info(element_info::phrase, &local.inner_phrase, phrase_tags::teletype))
            ("-", element_info(element_info::phrase, &local.inner_phrase, phrase_tags::strikethrough))
            ("\"", element_info(element_info::phrase, &local.inner_phrase, phrase_tags::quote))
            ("~", element_info(element_info::phrase, &local.inner_phrase, phrase_tags::replaceable))
            ("footnote", element_info(element_info::phrase, &local.inner_phrase, phrase_tags::footnote))
            ;

        elements.add
            ("c++", element_info(element_info::phrase, &local.empty, source_mode_tags::cpp))
            ("python", element_info(element_info::phrase, &local.empty, source_mode_tags::python))
            ("teletype", element_info(element_info::phrase, &local.empty, source_mode_tags::teletype))
            ;
        
        local.empty = cl::eps_p;

        local.inner_phrase =
                blank
            >>  actions.scoped_output()
                [   phrase                          [actions.phrase_value]
                ]
            ;
    }
}
