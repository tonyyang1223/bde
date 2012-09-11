// bsl_numeric.h                                                      -*-C++-*-
#ifndef INCLUDED_BSL_NUMERIC
#define INCLUDED_BSL_NUMERIC

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide functionality of the corresponding C++ Standard header.
//
//@SEE_ALSO: package bsl+stdhdrs
//
//@AUTHOR: Arthur Chiu (achiu21)
//
//@DESCRIPTION: Provide types, in the 'bsl' namespace, equivalent to those
// defined in the corresponding C++ standard header.  Include the native
// compiler-provided standard header, and also directly include Bloomberg's
// implementation of the C++ standard type (if one exists).  Finally, place the
// included symbols from the 'std' namespace (if any) into the 'bsl' namespace.

#ifndef INCLUDED_BSLS_NATIVESTD
#include <bsls_nativestd.h>
#endif

#include <numeric>

namespace bsl
{
    // Import selected symbols into bsl namespace
    using native_std::accumulate;
    using native_std::adjacent_difference;
    using native_std::bidirectional_iterator_tag;
    using native_std::forward_iterator_tag;
    using native_std::inner_product;
    using native_std::input_iterator_tag;
    using native_std::iterator;
    using native_std::output_iterator_tag;
    using native_std::partial_sum;
    using native_std::random_access_iterator_tag;
}

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2009
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
