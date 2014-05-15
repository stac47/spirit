/*=============================================================================
    Copyright (c) 2001-2013 Joel de Guzman

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/
#if !defined(BOOST_SPIRIT_X3_RULE_JAN_08_2012_0326PM)
#define BOOST_SPIRIT_X3_RULE_JAN_08_2012_0326PM

#if defined(_MSC_VER)
#pragma once
#endif

#include <boost/spirit/home/x3/nonterminal/detail/rule.hpp>
#include <boost/type_traits/is_same.hpp>
#include <boost/spirit/home/x3/support/context.hpp>
#include <boost/preprocessor/variadic/to_seq.hpp>
#include <boost/preprocessor/variadic/elem.hpp>
#include <boost/preprocessor/seq/for_each.hpp>

#if !defined(BOOST_SPIRIT_X3_NO_RTTI)
#include <typeinfo>
#endif

namespace boost { namespace spirit { namespace x3
{
    template <typename ID>
    struct identity {};
    
    template <typename ID, typename Attribute = unused_type>
    struct rule;

    template <typename ID, typename RHS, typename Attribute, bool explicit_attribute_propagation_>
    struct rule_definition : parser<rule_definition<ID, RHS, Attribute, explicit_attribute_propagation_>>
    {
        typedef rule_definition<ID, RHS, Attribute, explicit_attribute_propagation_> this_type;
        typedef ID id;
        typedef RHS rhs_type;
        typedef rule<ID, Attribute> lhs_type;
        typedef Attribute attribute_type;

        static bool const has_attribute =
            !is_same<Attribute, unused_type>::value;
        static bool const handles_container =
            traits::is_container<Attribute>::value;
        static bool const explicit_attribute_propagation =
            explicit_attribute_propagation_;

        rule_definition(RHS rhs, char const* name)
          : rhs(rhs), name(name) {}

        template <typename Iterator, typename Context, typename Attribute_>
        bool parse(Iterator& first, Iterator const& last
          , Context const& context, unused_type, Attribute_& attr) const
        {
            return detail::parse_rule<attribute_type, ID>
                ::call_rule_definition(
                    rhs, name, first, last
                  , context
                  , attr
                  , mpl::bool_<explicit_attribute_propagation>());
        }

        RHS rhs;
        char const* name;
    };

    template <typename ID, typename Attribute>
    struct rule : parser<rule<ID, Attribute>>
    {
        typedef ID id;
        typedef Attribute attribute_type;
        static bool const has_attribute =
            !is_same<Attribute, unused_type>::value;
        static bool const handles_container =
            traits::is_container<Attribute>::value;

#if !defined(BOOST_SPIRIT_X3_NO_RTTI)
        rule(char const* name = typeid(rule).name()) : name(name) {}
#else
        rule(char const* name = "unnamed") : name(name) {}
#endif

        template <typename RHS>
        rule_definition<
            ID, typename extension::as_parser<RHS>::value_type, Attribute, false>
        operator=(RHS const& rhs) const
        {
            return {as_parser(rhs), name};
        }

        template <typename RHS>
        rule_definition<
            ID, typename extension::as_parser<RHS>::value_type, Attribute, true>
        operator%=(RHS const& rhs) const
        {
            return {as_parser(rhs), name};
        }


        template <typename Iterator, typename Context, typename Attribute_>
        bool parse(Iterator& first, Iterator const& last
          , Context const& context, unused_type, Attribute_& attr) const
        {
            return parse_rule(*this, first, last, context, attr);
        }

        char const* name;
    };
    
    namespace traits
    {
        template <typename T, typename Enable = void>
        struct is_rule : mpl::false_ {};
    
        template <typename ID, typename Attribute>
        struct is_rule<rule<ID, Attribute>> : mpl::true_ {};
        
        template <typename ID, typename Attribute, typename RHS, bool explicit_attribute_propagation>
        struct is_rule<rule_definition<ID, RHS, Attribute, explicit_attribute_propagation>> : mpl::true_ {};
    }

    template <typename T>
    struct get_info<T, typename traits::is_rule<T>::type>
    {
        typedef std::string result_type;
        std::string operator()(T const& r) const
        {
            return r.name;
        }
    };
    
#define BOOST_SPIRIT_DEFINE(id, def)                                            \
    template <typename Iterator, typename Context                               \
      , typename Attribute, typename ActualAttribute>                           \
    inline bool parse_rule(                                                     \
        boost::spirit::x3::rule<class id, Attribute> const& rule_               \
      , Iterator& first, Iterator const& last                                   \
      , Context const& context, ActualAttribute& attr)                          \
    {                                                                           \
        using boost::spirit::x3::unused;                                        \
        auto const& def_ = (def);                                               \
        return boost::spirit::x3::detail::parse_rule<Attribute, class id>       \
            ::call_rule_definition(                                             \
                def_, #id, first, last                                          \
              , context                                                         \
              , attr                                                            \
              , boost::mpl::bool_<false>());                                    \
    }                                                                           \
    /***/

    
//#define BOOST_SPIRIT_DEFINE_(r, data, def)                                      \
//    template <typename Iterator, typename Context, typename Attribute>          \
//    inline bool parse_rule(                                                     \
//        decltype(def)::lhs_type const& rule_                                    \
//      , Iterator& first, Iterator const& last                                   \
//      , Context const& context, Attribute& attr)                                \
//    {                                                                           \
//        using boost::spirit::x3::unused;                                        \
//        auto const& def_ = (def);                                               \
//        return def_.parse(first, last, context, unused, attr);                  \
//    }                                                                           \
//    /***/
//
//#define BOOST_SPIRIT_DEFINE(...) BOOST_PP_SEQ_FOR_EACH(                         \
//    BOOST_SPIRIT_DEFINE_, _, BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__))             \
//    /***/
    
}}}

#endif
