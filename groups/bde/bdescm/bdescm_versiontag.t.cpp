// bdescm_versiontag.t.cpp    -*-C++-*-

#include <bdescm_versiontag.h>

#include <bsl_cstdlib.h>     // atoi()
#include <bsl_cstring.h>
#include <bsl_cstdio.h>


using namespace BloombergLP;

//==========================================================================
//                  STANDARD BDE ASSERT TEST MACRO
//--------------------------------------------------------------------------
// NOTE: THIS IS A LOW-LEVEL COMPONENT AND MAY NOT USE ANY C++ LIBRARY
// FUNCTIONS, INCLUDING IOSTREAMS.
static int testStatus = 0;
static bool verbose = false;
static bool veryVerbose = false;

static void aSsErT(int c, const char *s, int i) {
    if (c) {
        bsl::fprintf(stderr, "Error " __FILE__ "(%d): %s    (failed)\n", i, s);
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}

#define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }

#define LOOP_ASSERT(I,X) { \
    if (!(X)) { bsl::printf("%s: %d\n", #I, I); \
                aSsErT(1, #X, __LINE__); } }

//--------------------------------------------------------------------------

//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
// #define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) bsl::printf("<| " #X " |>\n");  // Quote identifier literally.
//#define P_(X) cout << #X " = " << (X) << ", " << flush; // P(X) without '\n'
#define L_ __LINE__                           // current Line number
#define T_ bsl::printf("\t");             // Print a tab (w/o newline)

//=============================================================================
//                  USAGE EXAMPLE HELPER FUNCTIONS
//-----------------------------------------------------------------------------

int newFunction()
    // Return 1
{
    return 1;
}

// int OldFunction()
// Not defined and never called due to conditional compilation

//=============================================================================
//                  MAIN PROGRAM
//-----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
    int test = argc > 1 ? bsl::atoi(argv[1]) : 0;

    verbose = (argc > 2);
    veryVerbose = (argc > 3);

    bsl::printf("TEST %s CASE %d\n", __FILE__, test);

    switch (test) { case 0:
      case 3: {
        //--------------------------------------------------------------------
        // TEST USAGE EXAMPLE
        //
        // Concern:
        //   That the usage example in the user documentation compiles and
        //   runs as expected.
        //
        // Plan:
        //   Use the exact text of the usage example from the user
        //   documentation, but change uses of 'assert' to 'ASSERT'.
        //
        // Testing:
        //   USAGE EXAMPLE
        //--------------------------------------------------------------------

        if (verbose) bsl::printf("\nTEST USAGE EXAMPLE"
                                 "\n==================\n");

// At compile time, the version of BDE can be used to select an older or newer
// way to accomplish a task, to enable new functionality, or to accommodate an
// interface change.  For example, if a function changed names (a rare
// occurrence, but disruptive when it does happen), disruption can be minimized
// by conditionally calling the old or new function name using conditional
// compilation.  The '#if' directive compares 'BDE_VERSION' to a specified
// major, minor, and patch version 4 composed using 'BDE_MAKE_VERSION':
//..
    #if BDE_VERSION > BDE_MAKE_VERSION(1, 2)
        // Call 'newFunction' for BDE version 1.2 and later:
        int result = newFunction();
    #else
        // Call 'oldFunction' for BDE older than version 1.2:
        int result = oldFunction();
    #endif

        ASSERT(result);
//..
      } break;

      case 2: {
        //--------------------------------------------------------------------
        // TEST BDE_MAKE_VERSION MACRO
        //
        // Concerns:
        //   That BDE_MAKE_VERSION create a compile-time constant if its
        //   arguments are all compile-time constants.
        //   That BDE_MAKE_VERSION correctly composes a major, minor, and
        //   patch version number into a single integer.  Each component can
        //   be in the range 0-99.
        //
        // Plan:
        //   Use the result of BDE_MAKE_VERSION as an array dimension to
        //   prove that it is a compile-time constant.
        //   Using ad-hoc data selection, create a number of version values
        //   using the 'BDE_MAKE_VERSION' macro and verify that the expected
        //   value matches the actual value.
        //
        // Testing:
        //   BDE_MAKE_VERSION(major, minor)
        //--------------------------------------------------------------------

        if (verbose) bsl::printf("\nTEST BDE_MAKE_VERSION MACRO"
                                 "\n===========================\n");
#if 0

        // Test that 'BDE_MAKE_VERSION(0,1,2)' is a compile-time constant.
        static const char COMPILE_ASSERT[BDE_MAKE_VERSION(0,1,2)] = { 0 };
        ASSERT(sizeof(COMPILE_ASSERT) == 102);

        static struct {
            int d_line;
            int d_major;
            int d_minor;
            int d_patch;
            int d_version;
        } const DATA[] = {
            //line major minor patch  version
            //---- ----- ----- -----  -------
            {  L_,    0,    0,    0,        0 },
            {  L_,    0,    0,    1,        1 },
            {  L_,    0,    1,    0,      100 },
            {  L_,    0,    1,    1,      101 },
            {  L_,    1,    0,    0,    10000 },
            {  L_,    1,    0,    1,    10001 },
            {  L_,    1,    1,    0,    10100 },
            {  L_,   12,   34,   56,   123456 },
            {  L_,   99,   99,   99,   999999 },
            {  L_,    9,    9,    9,    90909 },
            {  L_,   10,   20,   30,   102030 }
        };

        static const int NUM_DATA = sizeof(DATA) / sizeof(*DATA);

        for (int i = 0; i < NUM_DATA; ++i) {
            const int LINE  = DATA[i].d_line;
            const int MAJOR = DATA[i].d_major;
            const int MINOR = DATA[i].d_minor;
            const int PATCH = DATA[i].d_patch;
            const int EXP   = DATA[i].d_version;

            LOOP_ASSERT(LINE, EXP == BDE_MAKE_VERSION(MAJOR, MINOR, PATCH));
        }
#endif
        static const char COMPILE_ASSERT[BDE_MAKE_VERSION(0,1)] = { 0 };
        ASSERT(sizeof(COMPILE_ASSERT) == 100);

        static struct {
            int d_line;
            int d_major;
            int d_minor;
            int d_version;
        } const DATA[] = {
            //line major minor version
            //---- ----- ----- -------
            {  L_,    0,    0,       0 },
            {  L_,    0,    1,     100 },
            {  L_,    1,    0,   10000 },
            {  L_,    1,    1,   10100 },
            {  L_,   12,   34,  123400 },
            {  L_,   99,   99,  999900 },
            {  L_,    9,    9,   90900 },
            {  L_,   10,   20,  102000 }
        };

        static const int NUM_DATA = sizeof(DATA) / sizeof(*DATA);

        for (int i = 0; i < NUM_DATA; ++i) {
            const int LINE  = DATA[i].d_line;
            const int MAJOR = DATA[i].d_major;
            const int MINOR = DATA[i].d_minor;
            const int EXP   = DATA[i].d_version;

            LOOP_ASSERT(LINE, EXP == BDE_MAKE_VERSION(MAJOR, MINOR));
        }

      } break;

      case 1: {
        //--------------------------------------------------------------------
        // TEST VERSION CONSISTENCY
        //
        // Concerns:
        //   That BDE_VERSION corresponds to the two components
        //   BDE_VERSION_MAJOR and BDE_VERSION_MINOR
        //
        // Plan:
        //   Decompose BDE_VERSION into its three components and verify
        //   that they correspond to the defined macros.
        //
        // Testing:
        //   BDE_VERSION
        //   BDE_VERSION_MAJOR
        //   BDE_VERSION_MINOR
        //--------------------------------------------------------------------

        if (verbose) bsl::printf("\nTEST VERSION CONSISTENCY"
                                 "\n========================\n");

        int major = (BDE_VERSION / 10000) % 100;
        int minor = (BDE_VERSION / 100) % 100;

        ASSERT(BDE_VERSION_MAJOR == major);
        ASSERT(BDE_VERSION_MINOR == minor);

      } break;

      default: {
        bsl::fprintf(stderr, "WARNING: CASE `%d' NOT FOUND.\n", test);
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        bsl::fprintf(stderr, "Error, non-zero test status = %d.\n",
                     testStatus);
    }
    return testStatus;
}

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2002
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
