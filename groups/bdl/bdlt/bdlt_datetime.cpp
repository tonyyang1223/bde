// bdlt_datetime.cpp                                                  -*-C++-*-
#include <bdlt_datetime.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdlt_datetime_cpp,"$Id$ $CSID$")

#include <bslim_printer.h>
#include <bslmf_assert.h>
#include <bsls_assert.h>

#include <bsl_c_stdio.h>          // 'snprintf'
#include <bsl_ostream.h>
#include <bsl_sstream.h>

namespace BloombergLP {
namespace bdlt {

// 'Datetime' is trivially copyable only if 'Date' and 'Time' are also
// trivially copyable.  In the header we have stated unconditionally that
// 'Datetime' is trivially copyable, so we assert our assumption about 'Date'
// and 'Time', as a sanity check.

BSLMF_ASSERT(bsl::is_trivially_copyable<Date>::value);
BSLMF_ASSERT(bsl::is_trivially_copyable<Time>::value);

                              // --------------
                              // class Datetime
                              // --------------

// CLASS DATA
#ifdef BDE_USE_PROLEPTIC_DATES
const bsls::Types::Uint64 Datetime::k_MAX_US_FROM_EPOCH =
                                    3652059ULL * TimeUnitRatio::k_US_PER_D - 1;
               // 3652059 is 9999/12/31 - 0001/01/01 + 1 in Proleptic Gregorian
#else
const bsls::Types::Uint64 Datetime::k_MAX_US_FROM_EPOCH =
                                    3652061ULL * TimeUnitRatio::k_US_PER_D - 1;
                             // 3652061 is 9999/12/31 - 0001/01/01 + 1 in POSIX
#endif

bsls::AtomicInt64 Datetime::s_invalidRepresentationCount(0);

// ACCESSORS
bsl::ostream& Datetime::print(bsl::ostream& stream,
                              int           level,
                              int           spacesPerLevel) const
{
    // Format the output to a buffer first instead of inserting into 'stream'
    // directly to improve performance and in case the caller has done
    // something like:
    //..
    //  os << bsl::setw(20) << myDatetime;
    //..
    // The user-specified width will be effective when 'buffer' is written to
    // the 'stream' (below).

    const int k_BUFFER_SIZE = 128;   // Buffer sized to hold a *bad* date.
    char      buffer[k_BUFFER_SIZE];

    int rc = printToBuffer(buffer,
                           k_BUFFER_SIZE,
                           k_DEFAULT_FRACTIONAL_SECOND_PRECISION);

    (void)rc;
    BSLS_ASSERT(25 == rc);  // The datetime format contains 25 characters.

    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start(true);    // 'true' -> suppress '['
    stream << buffer;
    printer.end(true);      // 'true' -> suppress ']'

    return stream;
}

int Datetime::printToBuffer(char *result,
                            int   numBytes,
                            int   fractionalSecondPrecision) const
{
    BSLS_ASSERT(result);
    BSLS_ASSERT(0 <= numBytes);
    BSLS_ASSERT(0 <= fractionalSecondPrecision);
    BSLS_ASSERT(fractionalSecondPrecision <= 6);

    int year;
    int month;
    int day;

    date().getYearMonthDay(&year, &month, &day);

    static const char *const k_MONTHS[] = {
        0,
        "JAN", "FEB", "MAR", "APR",
        "MAY", "JUN", "JUL", "AUG",
        "SEP", "OCT", "NOV", "DEC"
    };

    const char *const asciiMonth  = k_MONTHS[month];

    int hour;
    int minute;
    int second;
    int millisecond;
    int microsecond;

    getTime(&hour, &minute, &second, &millisecond, &microsecond);

    int value = millisecond * 1000 + microsecond;

    char spec[] = "%02d%s%04d_%02d:%02d:%02d.%0Xd";

    const int PRECISION_INDEX = sizeof spec - 3;

    spec[PRECISION_INDEX] = static_cast<char>('0' + fractionalSecondPrecision);

#if defined(BSLS_PLATFORM_CMP_MSVC)
    // Windows uses a different variant of snprintf that does not necessarily
    // null-terminate and returns -1 on overflow.

    const int rc = _snprintf(result,
                             numBytes,
                             spec,
                             day,
                             asciiMonth,
                             year,
                             hour,
                             minute,
                             second,
                             value);

    if ((0 > rc || rc == numBytes) && numBytes > 0) {
        result[numBytes - 1] = '\0';  // Make sure to null-terminate on
                                      // overflow.
    }
    return 25;  // Format of 'bdlt::Datetime' always has 25 characters.
#else
    return snprintf(result,
                    numBytes,
                    spec,
                    day,
                    asciiMonth,
                    year,
                    hour,
                    minute,
                    second,
                    value);
#endif
}

}  // close package namespace

// FREE OPERATORS
bsl::ostream& bdlt::operator<<(bsl::ostream& stream, const Datetime& object)
{
    return object.print(stream, 0, -1);
}

}  // close enterprise namespace

// ----------------------------------------------------------------------------
// Copyright 2016 Bloomberg Finance L.P.
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
