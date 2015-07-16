// bdlmtt_condition.t.cpp                                              -*-C++-*-

#include <bdlmtt_condition.h>

#include <bdlmtt_mutex.h>
#include <bdlmtt_xxxatomictypes.h>

#include <bdlimpxxx_fuzzy.h>
#include <bdlt_currenttime.h>

#include <bsls_stopwatch.h>
#include <bsls_systemtime.h>

#include <bsl_cerrno.h>
#include <bsl_cstdlib.h>
#include <bsl_iostream.h>

using namespace BloombergLP;
using namespace bsl;

//-----------------------------------------------------------------------------
//                  STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------
static int testStatus = 0;

static void aSsErT(int c, const char *s, int i) {
    if (c) {
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}
#define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }
//-----------------------------------------------------------------------------
#define LOOP_ASSERT(I,X) {                                                    \
    if (!(X)) { cout << #I << ": " << I << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP2_ASSERT(I,J,X) {                                                 \
    if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\n";   \
                aSsErT(1, #X, __LINE__); } }
#define LOOP3_ASSERT(I,J,K,X) {                                               \
    if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J <<         \
                        "\t" << #K << ": " << K << "\n";                      \
                aSsErT(1, #X, __LINE__); } }
//-----------------------------------------------------------------------------
#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", " << flush; // P(X) without '\n'
#define L_ __LINE__                           // current Line number
#define T_() cout << '\t' << flush;           // Print tab w/o line feed.

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

int verbose;
int veryVerbose;

typedef bdlmtt::Condition Obj;

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    verbose = argc > 2;
    veryVerbose = argc > 3;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:  // Zero is always the leading case.
        // --------------------------------------------------------------------
        // TEST THAT THIS IS A CONDITION
        //
        // Without relying on anything in, e.g., bdlmtt_threadutil, test that
        // this object likely forwards to an appropriate implementation.  We'll
        // test that timedWait on a default-constructed Condition object
        // returns in roughly the right amount of time.
        // --------------------------------------------------------------------
      case 1: {
          if (verbose) cout << "Basic forwarding test" << endl
                            << "=====================" << endl;

          {
              Obj x;
              bdlmtt::Mutex lock;

              lock.lock();
              bsls::Stopwatch timer;
              timer.start();
              x.timedWait(&lock, bdlt::CurrentTime::now() + 2);
              ASSERT(bdlimpxxx::Fuzzy::eq(timer.elapsedTime(), 2.0, -1, 0.1));
              lock.unlock();
          }

          if (verbose) cout << "Test condition with realtime clock" << endl
                            << "==================================" << endl;

          {
              Obj x(bsls::SystemClockType::e_REALTIME);
              bdlmtt::Mutex lock;

              lock.lock();
              bsls::Stopwatch timer;
              timer.start();
              x.timedWait(&lock, bsls::SystemTime::nowRealtimeClock() + 2);
              ASSERT(bdlimpxxx::Fuzzy::eq(timer.elapsedTime(), 2.0, -1, 0.1));
              lock.unlock();
          }


          if (verbose) cout << "Test condition with monotonic clock" << endl
                            << "===================================" << endl;

          {
              Obj x(bsls::SystemClockType::e_MONOTONIC);
              bdlmtt::Mutex lock;

              lock.lock();
              bsls::Stopwatch timer;
              timer.start();
              x.timedWait(&lock, bsls::SystemTime::nowMonotonicClock() + 2);
              ASSERT(bdlimpxxx::Fuzzy::eq(timer.elapsedTime(), 2.0, -1, 0.1));
              lock.unlock();
          }
      } break;
      default: {
        cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        cerr << "Error, non-zero test status = "
             << testStatus << "." << endl;
    }
    return testStatus;
}

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2014
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
