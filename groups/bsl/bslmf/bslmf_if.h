// bslmf_if.h                                                         -*-C++-*-
#ifndef INCLUDED_BSLMF_IF
#define INCLUDED_BSLMF_IF

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a compile-time 'if/else' (conditional) meta-function.
//
//@CLASSES:
//  bslmf::If: meta-function for compile-time selection of one of two types
//
//@SEE_ALSO: bslmf_assert, bslmf_nil
//
//@AUTHOR: Oleg Grunin (ogrunin)
//
//@DESCRIPTION: This component contains the template class meta-function
// 'bslmf::If' that is parameterized on three arguments.  The first argument is
// a (compile-time constant) integral expression that is interpreted by the
// meta-function as a boolean conditional; the other two parameters accept type
// arguments.  If the value of the first argument is non-zero (true), the
// meta-function "returns" its second argument (i.e., corresponding to the
// first type parameter); otherwise it returns its third argument (the second
// type parameter).  If the selected type argument is not explicitly specified,
// the meta-function returns the default 'bslmf::Nil' type.
//
///Meta-Function Return Types and Values
///-------------------------------------
// A meta-function is a class template that evaluates, at compile-time, to one
// or more types and values.  An example of a simple meta-function that adds
// two (compile-type constant) integer values is the following 'Plus' class
// template:
//..
//  template <int A, int B>
//  struct Plus {
//      enum { VALUE = A + B };  // 'VALUE' is meta-function result
//  };
//..
// The initializer of the 'VALUE' enumerator is the compile-time summation of
// the constant values 'A' and 'B'.  The result "returned" by 'Plus' is
// provided by the 'VALUE' enumerator.
//
// An example where a type is returned from a meta-function rather than a value
// is illustrated by 'SelectLarger' below.  The 'SelectLarger' meta-function
// selects the larger of two types.  The result "returned" by 'SelectLarger' is
// provided by the 'SelectLarger<...>::Type' 'typedef':
//..
//  template <class T1, class T2>
//  struct SelectLarger {
//      template <class U1, class U2, bool V = (sizeof(T1) > sizeof(T2))>
//      struct Impl {
//          typedef T1 Type;
//      };
//      template <class U1, class U2>
//      struct Impl <U1, U2, false> {
//          typedef T2 Type;
//      };
//
//      typedef typename Impl<T1, T2>::Type Type;  // 'Type' is meta-function
//                                                 // result
//  };
//..
// The preceding two examples illustrate the naming conventions used throughout
// 'bslmf' to denote the types and values returned by meta-functions.  In
// particular, enumerators or 'static const' integral variables named 'VALUE'
// provide the results of value-returning meta-functions, and nested types
// named 'Type' provide the results of type-returning meta-functions.
//
///Usage
///-----
// The following snippets of code illustrate basic use of the 'bslmf::If'
// meta-function.  The examples make use of the following declarations to
// identify the type that is selected by a given constant integral expression:
//..
//  enum TypeCode { T_UNKNOWN = 0, T_CHAR = 1, T_INT = 2, T_NIL = 3 };
//
//  TypeCode whatType(char)       { return T_CHAR; }
//  TypeCode whatType(int)        { return T_INT; }
//  TypeCode whatType(bslmf::Nil) { return T_NIL; }
//  TypeCode whatType(...)        { return T_UNKNOWN; }
//..
// In the following example, the meta-function condition (the first argument to
// 'bslmf::If') evaluates to true (non-zero).  Thus, 'bslmf::If<...>::Type' is
// a synonym for 'int'; i.e., it "evaluates" (at compile time) to 'int':
//..
//  typedef int  T1;  assert(1 <  sizeof(T1));
//  typedef char T2;  assert(1 == sizeof(T2));
//
//  typedef bslmf::If<(sizeof(T1) > sizeof(T2)), T1, T2>::Type LargerType;
//  assert(T_INT == whatType(LargerType()));
//..
// In the next example, the condition argument evaluates to false (zero).  In
// this case, 'bslmf::If<...>::Type' evaluates to 'bslmf::Nil' since the third
// template argument (the "else" type) is not explicitly specified:
//..
//  typedef bslmf::If<(sizeof(T2) > 1), int>::Type Type2;
//  assert(T_NIL == whatType(Type2()));
//..

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLMF_CONDITIONAL
#include <bslmf_conditional.h>
#endif

#ifndef INCLUDED_BSLMF_NIL
#include <bslmf_nil.h>
#endif

namespace BloombergLP {

namespace bslmf {

                         // =========
                         // struct If
                         // =========

template <int   CONDITION,
          class IF_TRUE_TYPE = Nil, class IF_FALSE_TYPE = Nil>
struct If {
    // This meta-function selects 'IF_TRUE_TYPE' if 'CONDITION' is non-zero.
    // and 'IF_FALSE_TYPE' otherwise.

    typedef typename bsl::conditional<CONDITION,
                                      IF_TRUE_TYPE,
                                      IF_FALSE_TYPE>::type
            Type;
};

}  // close package namespace

#ifndef BDE_OMIT_TRANSITIONAL  // BACKWARD_COMPATIBILITY
// ===========================================================================
//                           BACKWARD COMPATIBILITY
// ===========================================================================

#ifdef bslmf_If
#undef bslmf_If
#endif
#define bslmf_If bslmf::If
    // This alias is defined for backward compatibility.
#endif  // BDE_OMIT_TRANSITIONAL -- BACKWARD_COMPATIBILITY

}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright (C) 2013 Bloomberg L.P.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
// ----------------------------- END-OF-FILE ----------------------------------
