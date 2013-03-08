// bdemf_if.h                -*-C++-*-
#ifndef INCLUDED_BDEMF_IF
#define INCLUDED_BDEMF_IF

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide a compile-time 'if/else' (conditional) meta-function.
//
//@DEPRECATED: Use 'bslmf_if' instead.
//
//@CLASSES:
//  bdemf_If: meta-function for compile-time selection of one of two types
//
//@AUTHOR: Oleg Grunin (ogrunin)
//
//@SEE_ALSO: bdemf_assert, bdemf_nil
//
//@DESCRIPTION: This component contains the template class meta-function
// 'bdemf_If' that is parameterized on three arguments.  The first argument is
// a (compile-time constant) integral expression that is interpreted by the
// meta-function as a boolean conditional; the other two parameters accept type
// arguments.  If the value of the first argument is non-zero (true), the
// meta-function "returns" its second argument (i.e., corresponding to the
// first type parameter); otherwise it returns its third argument (the second
// type parameter).  If the selected type argument is not explicitly specified,
// the meta-function returns the default 'bdemf_Nil' type.
//
///Meta-Function Return Types and Values
///-------------------------------------
// A meta-function is a class template that evaluates, at compile-time, to one
// or more types and values.  An example of a simple meta-function that adds
// two (compile-type constant) integer values is the following 'bdemf_Plus'
// class template:
//..
//  template <int A, int B>
//  struct bdemf_Plus {
//      enum { VALUE = A + B };  // 'VALUE' is meta-function result
//  };
//..
// The initializer of the 'VALUE' enumerator is the compile-time summation of
// the constant values 'A' and 'B'.  The result "returned" by 'bdemf_Plus' is
// provided by the 'VALUE' enumerator.
//
// An example where a type is returned from a meta-function rather than a value
// is illustrated by 'bdemf_SelectLarger' below.  The 'bdemf_SelectLarger'
// meta-function selects the larger of two types.  The result "returned" by
// 'bdemf_SelectLarger' is provided by the 'bdemf_SelectLarger<...>::Type'
// 'typedef':
//..
//  template <class T1, class T2>
//  struct bdemf_SelectLarger {
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
// 'bdemf' to denote the types and values returned by meta-functions.  In
// particular, enumerators or 'static const' integral variables named 'VALUE'
// provide the results of value-returning meta-functions, and nested types
// named 'Type' provide the results of type-returning meta-functions.
//
///Usage
///-----
// The following snippets of code illustrate basic use of the 'bdemf_If'
// meta-function.  The examples make use of the following declarations to
// identify the type that is selected by a given constant integral expression:
//..
//  enum TypeCode { T_UNKNOWN = 0, T_CHAR = 1, T_INT = 2, T_NIL = 3 };
//
//  TypeCode whatType(char)      { return T_CHAR; }
//  TypeCode whatType(int)       { return T_INT; }
//  TypeCode whatType(bdemf_Nil) { return T_NIL; }
//  TypeCode whatType(...)       { return T_UNKNOWN; }
//..
// In the following example, the meta-function condition (the first argument to
// 'bdemf_If') evaluates to true (non-zero).  Thus, 'bdemf_If<...>::Type' is a
// synonym for 'int'; i.e., it "evaluates" (at compile time) to 'int':
//..
//   typedef int  T1;  assert(1 <  sizeof(T1));
//   typedef char T2;  assert(1 == sizeof(T2));
//
//   typedef bdemf_If<(sizeof(T1) > sizeof(T2)), T1, T2>::Type LargerType;
//   assert(T_INT == whatType(LargerType()));
//..
// In the next example, the condition argument evaluates to false (zero).  In
// this case, 'bdemf_If<...>::Type' evaluates to 'bdemf_Nil' since the third
// template argument (the "else" type) is not explicitly specified:
//..
//   typedef bdemf_If<(sizeof(T2) > 1), int>::Type Type2;
//   assert(T_NIL == whatType(Type2()));
//..
//
///DEPRECATED
///----------
// 'bdemf_if' (lowercase "i") has been replaced with 'bdemf_If' (uppercase "I")
// for consistency with other meta-functions in 'bdemf'.

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

#ifndef INCLUDED_BSLMF_IF
#include <bslmf_if.h>
#endif

#ifndef bdemf_If
#define bdemf_If   bslmf::If
    // This meta-function selects 'IF_TRUE_TYPE' if 'CONDITION' is non-zero.
#endif

#ifndef bdemf_if
#define bdemf_if   bslmf::If
    // This meta-function selects 'IF_TRUE_TYPE' if 'CONDITION' is non-zero.
    //
    // DEPRECATED: Replaced by 'bslmf::If' (note the uppercase "I").
#endif

namespace BloombergLP {

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2004
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
