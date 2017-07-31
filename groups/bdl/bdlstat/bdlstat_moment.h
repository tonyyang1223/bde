// bdlstat_moment.h                                                   -*-C++-*-
#ifndef INCLUDED_BDLSTAT_MOMENT
#define INCLUDED_BDLSTAT_MOMENT

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Functions to calculate online mean, variance, skew, and kurtosis.
//
//@CLASSES:
//  bdlstat::Moment: calculate online mean, variance, skew, and kurtosis
//
//@AUTHOR: Ofer Imanuel (oimanuel@bloomberg.net)
//
//@DESCRIPTION: This component provides a mechanism, 'bdlstat::Moment', that
// provides online calculation of basic statistics: mean, variance, skew, and
// kurtosis while maintaining accuracy.  Online algorithms process the data in
// one pass, while keeping good accuracy.  The online algorithms used are
// Welford for variance, and the stable skew and kurtosis algoritms taken from:
// https://en.wikipedia.org/wiki/Algorithms_for_calculating_variance#Higher-order_statistics
//
// The implementation uses template specialization so the user can choose the
// statistics necessary, and not calculate or allocate memory for those
// statistics that are not needed.
//
// The template parameter is a value from the provided enum and having the
// following interpretation:
//..
//  M1 - mean
//  M2 - variance+mean
//  M3 - skew+variance+mean
//  M4 - kurtosis+skew+variance+mean
//..
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Calculating skew, variance, and mean
///- - - - - - - - - - - - - - - -
// This example shows how to accumulate a set of values, and calculate the
// skew, variance, and kurtosis.
//
// First, we create example input and instantiate the appropriate mechanism:
//..
//  double input[] = {1.0, 2.0, 4.0, 5.0};
//  bdlstat::Moment<bdlstat::M3> m3;
//..
// Then, we invoke the 'add' routine to accumulate the data:
//..
//  for(int i = 0; i < 4; ++i) {
//      m3.add(input[i]);
//  }
//..
// Finally, we assert that the mean, variance, and skew are what we expect:
//..
//  ASSERT(4   == m3.getCount());
//  ASSERT(3.0 == m3.getMean());
//  ASSERT(1e-5 > fabs(3.33333 - m3.getVariance()));
//  ASSERT(1e-5 > fabs(0.0     - m3.getSkew()));
//..

#ifndef INCLUDED_BDLSCM_VERSION
#include <bdlscm_version.h>
#endif

#ifndef INCLUDED_BSL_CMATH
#include <bsl_cmath.h>
#endif

#ifndef INCLUDED_LIMITS
#include <limits>           // 'std::numeric_limits'
#endif

namespace BloombergLP {
namespace bdlstat {

// BDE_VERIFY pragma: -TR17 // Avoid users having to specify template params
// BDE_VERIFY pragma: -CP01 // Avoid users having to specify template params
// BDE_VERIFY pragma: -AQa01 // Really, constexpr
const double k_DBL_NAN  = std::numeric_limits<double>::quiet_NaN();
    // Nan value to signify an illegal value returned.
// BDE_VERIFY pragma: +AQa01

// BDE_VERIFY pragma: -UC01
enum         MomentLevel {M1, M2, M3, M4};
    // Level of data desired: M1- mean; M2 - variance+mean; M3 - skew+variance+
    // mean; M4 - kurtosis+skew+variance+mean.
// BDE_VERIFY pragma: +TR17
// BDE_VERIFY pragma: +CP01
// BDE_VERIFY pragma: +UC01

                      // ==========================
                      // private struct Moment_Data
                      // ==========================

template <MomentLevel ML>
struct Moment_Data;

template<>
struct Moment_Data<M1> {
    // Data members for Mean only.

    // PUBLIC DATA
    int    d_count; // Number of entries.
    double d_sum;   // Sum of entries.

    // CREATORS
    Moment_Data();
        // Constructor initializes all members to zero.
};

template<>
struct Moment_Data<M2> {
    // Data members for Variance and below.

    // PUBLIC DATA
    int    d_count; // Number of entries.
    double d_sum;   // Sum of entries.
    double d_mean;  // Mean of entries.
    double d_M2;    // 2nd moment, for variance.

    // CREATORS
    Moment_Data();
        // Constructor initializes all members to zero.
};

template<>
struct Moment_Data<M3> {
    // Data members for Skew and below

    // PUBLIC DATA
    int    d_count; // Number of entries.
    double d_sum;   // Sum of entries.
    double d_mean;  // Mean of entries.
    double d_M2;    // 2nd moment, for variance.
    double d_M3;    // 3rd moment, for skew

    // CREATORS
    Moment_Data();
        // Constructor initializes all members to zero.
};

template<>
struct Moment_Data<M4> {
    // Data members for Kurtosis and below

    // PUBLIC DATA
    int    d_count; // Number of entries.
    double d_sum;   // Sum of entries.
    double d_mean;  // Mean of entries.
    double d_M2;    // 2nd moment, for variance.
    double d_M3;    // 3rd moment, for skew
    double d_M4;    // 4th moment, for kurtosis

    // CREATORS
    Moment_Data();
        // Constructor initializes all members to zero.
};

                            // ============
                            // class Moment
                            // ============

template <MomentLevel ML>
class Moment {
    // This class provides efficient and accurate online algorithms for
    // calculating mean, variance, skew, and kurtosis.  The class provides
    // template specializations, so that no unnecessary data members will be
    // kept or unnecessary calculations done.  The online algorithms used are
    // Welford for variance, and the stable M3 and M4 are taken from:
    // https://en.wikipedia.org/wiki/Algorithms_for_calculating_variance#Higher-order_statistics
    //
    // The formula for sample skewness is taken from:
    // http://www.macroption.com/skewness-formula/
    //
    // The formula for sample excess kurtosis is taken from:
    // http://www.macroption.com/kurtosis-formula/
  private:
    // DATA
    struct Moment_Data<ML> d_data;

  public:
    // MANIPULATORS
    void add(double value);
        // Add the specified 'value' to the data set.

    // ACCESSORS
    int getCount() const;
        // Returns the number of elements in the data set.

    double getKurtosis() const;
        // Return kurtosis of the data set.  The result is 'Nan' unless
        // '4 <= count' and the variance is not zero.

    double getKurtosisRaw() const;
        // Return kurtosis of the data set.  The behavior is undefined unless
        // '4 <= count'.

    double getMean() const;
        // Return mean of the data set.  The result is 'Nan' unless
        // '1 <= count'.

    double getMeanRaw() const;
        // Return mean of the data set.  The behavior is undefined unless
        // '1 <= count'.

    double getSkew() const;
        // Return kurtosis of the data set.  The result is 'Nan' unless
        // '3 <= count' and the variance is not zero..

    double getSkewRaw() const;
        // Return same excess kurtosis of the data set.  The behavior is
        // undefined unless '3 <= count'.

    double getVariance() const;
        // Return variance of the data set.  The result is 'Nan' unless
        // '2 <= count'.

    double getVarianceRaw() const;
        // Return variance of the data set.  The behavior is undefined unless
        // '2 <= count'.
};

// ============================================================================
//                               INLINE DEFINITIONS
// ============================================================================

                        // ---------------------------
                        // struct bdlstat::Moment_Data
                        // ---------------------------

// CREATORS
Moment_Data<M1>::Moment_Data()
: d_count(0)
, d_sum(0.0)
{
}

Moment_Data<M2>::Moment_Data()
: d_count(0)
, d_sum(0.0)
, d_mean(0.0)
, d_M2(0.0)
{
}

Moment_Data<M3>::Moment_Data()
: d_count(0)
, d_sum(0.0)
, d_mean(0.0)
, d_M2(0.0)
, d_M3(0.0)
{
}

Moment_Data<M4>::Moment_Data()
: d_count(0)
, d_sum(0.0)
, d_mean(0.0)
, d_M2(0.0)
, d_M3(0.0)
, d_M4(0.0)
{
}

                        // ---------------------
                        // class bdlstat::Moment
                        // ---------------------

// MANIPULATORS
template<>
inline
void Moment<M1>::add(double value)
{
    ++d_data.d_count;
    d_data.d_sum += value;
}

template<>
inline
void Moment<M2>::add(double value)
{
    // Modified Welford algorithm for variance
    const double delta = value - d_data.d_mean;
    d_data.d_sum += value;
    ++d_data.d_count;
    d_data.d_mean = d_data.d_sum / static_cast<double>(d_data.d_count);
    const double delta2 = value - d_data.d_mean;
    d_data.d_M2 += delta * delta2;
}

template<>
inline
void Moment<M3>::add(double value)
{
    // Modified Welford algorithm for variance, and similar algorithm for skew.
    const double delta = value - d_data.d_mean;
    const double nm1 = d_data.d_count;
    d_data.d_sum += value;
    ++d_data.d_count;
    const double n = d_data.d_count;
    const double deltaN = delta / n;
    d_data.d_mean = d_data.d_sum / n;
    const double term1 = delta * deltaN * nm1;
    d_data.d_M3 += term1 * deltaN * (n - 2.0) - 3.0 * deltaN * d_data.d_M2;
    d_data.d_M2 += term1;
}

template<>
inline
void Moment<M4>::add(double value)
{
    // Modified Welford algorithm for variance, and similar algorithms for skew
    // and kurtosis.
    const double delta = value - d_data.d_mean;
    const double nm1 = d_data.d_count;
    d_data.d_sum += value;
    ++d_data.d_count;
    const double n = d_data.d_count;
    const double n2 = n * n;
    const double deltaN = delta / n;
    d_data.d_mean = d_data.d_sum / n;
    const double term1 = delta * deltaN * nm1;
    const double deltaN2 = deltaN * deltaN;
    d_data.d_M4 += term1 * deltaN2 * (n2 - 3.0 * n + 3.0) +
                   6 * deltaN2 * d_data.d_M2 - 4.0 * deltaN * d_data.d_M3;
    d_data.d_M3 += term1 * deltaN * (n - 2.0) - 3.0 * deltaN * d_data.d_M2;
    d_data.d_M2 += term1;
}

// ACCESSORS
template <MomentLevel ML>
inline
int Moment<ML>::getCount() const
{
    return d_data.d_count;
}

// BDE_VERIFY pragma: -FABC01 // getKurtosisRaw needed before getKurtosis
template<>
inline
double Moment<M4>::getKurtosisRaw() const
{
    const double n = static_cast<double>(d_data.d_count);
    const double n1   = (n - 1.0);
    const double n2n3 = (n - 2.0) * (n - 3.0);
    return n * (n + 1.0) * n1 / n2n3 * d_data.d_M4 / d_data.d_M2 / d_data.d_M2
           - 3.0 * n1 * n1 / n2n3;
}
// BDE_VERIFY pragma: +FABC01

template<>
inline
double Moment<M4>::getKurtosis() const
{
    if (4 > d_data.d_count || 0.0 == d_data.d_M2) {
        return k_DBL_NAN;                                             // RETURN
    }
    return getKurtosisRaw();
}

template <MomentLevel ML>
inline
double Moment<ML>::getMean() const
{
    if (1 > d_data.d_count) {
        return k_DBL_NAN;                                             // RETURN
    }
    return getMeanRaw();
}

template <MomentLevel ML>
inline
double Moment<ML>::getMeanRaw() const
{
    return d_data.d_sum / static_cast<double>(d_data.d_count);
}

template <MomentLevel ML>
inline double Moment<ML>::getSkew() const
{
    if (3 > d_data.d_count || 0.0 == d_data.d_M2) {
        return k_DBL_NAN;                                             // RETURN
    }
    return getSkewRaw();
}

template <MomentLevel ML>
inline
double Moment<ML>::getSkewRaw() const
{
    const double n = static_cast<double>(d_data.d_count);
    return bsl::sqrt(n - 1.0) * n / (n- 2.0) * d_data.d_M3
                                                  / bsl::pow(d_data.d_M2, 1.5);
}

template <MomentLevel ML>
inline
double Moment<ML>::getVariance() const
{
    if (2 > d_data.d_count) {
        return k_DBL_NAN;                                             // RETURN
    }
    return getVarianceRaw();
}

template <MomentLevel ML>
inline
double Moment<ML>::getVarianceRaw() const
{
    return d_data.d_M2 / (d_data.d_count - 1);
}

}  // close package namespace
}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright 2017 Bloomberg Finance L.P.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// ----------------------------- END-OF-FILE ----------------------------------
