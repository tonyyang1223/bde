// bdlma_defaultdeleter.t.cpp                                         -*-C++-*-
#include <bdlma_defaultdeleter.h>
#include <bslma_testallocator.h>
#include <bsl_memory.h>

#include <bslma_default.h>
#include <bslma_newdeleteallocator.h>

#include <bsl_cstdlib.h>     // atoi()
#include <bsl_cstring.h>     // memcpy()
#include <bsl_iostream.h>
#include <bsl_memory.h>

using namespace BloombergLP;
using namespace bsl;  // automatically added by script

//=============================================================================
//                              TEST PLAN
//-----------------------------------------------------------------------------
//                              OVERVIEW
// [TBD]
//-----------------------------------------------------------------------------
//=============================================================================

//=============================================================================
//                    STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------

static int testStatus = 0;
static void aSsErT(int c, const char *s, int i)
{
    if (c) {
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}
#define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }

//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", "<< flush; // P(X) without '\n'
#define L_ __LINE__                           // current Line number
#define PS(X) cout << #X " = \n" << (X) << endl; // Print identifier and value.
#define T_()  cout << "\t" << flush;          // Print a tab (w/o newline)

//=============================================================================
//                      CONCRETE DERIVED TYPES
//-----------------------------------------------------------------------------
class my_Obj {
    int d_flag;
    int *d_destructorFlag_p;
  public:
    my_Obj() : d_destructorFlag_p(&d_flag) {}

    my_Obj(int *destructorFlag) : d_destructorFlag_p(destructorFlag) {
        *d_destructorFlag_p = 0;
    }
    ~my_Obj() { *d_destructorFlag_p = 1; }
};

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[]) {

    int  test        = argc > 1 ? atoi(argv[1]) : 0;
    bool verbose     = argc > 2;
    bool veryVerbose = argc > 3;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:
      case 2: {
        // --------------------------------------------------------------------
        // TEST USAGE
        // --------------------------------------------------------------------
///Usage
///-----
// Suppose that we would like to transfer ownership of an object between
// threads using 'bsl::shared_ptr'.  For the sake of discussion, the type of
// this object is 'my_Obj' and we will suppose that it is created using a given
// 'basicAllocator'.  Note that we assume that 'my_Obj' does not require an
// allocator for any of its members:
//..
    bslma::NewDeleteAllocator basicAllocator;
    my_Obj *object = new(basicAllocator) my_Obj;
//..
// Next, create a concrete deleter for 'object' using the same allocator as was
// used to allocate its footprint:
//..
    bdlma::DefaultDeleter<my_Obj> deleter(&basicAllocator);
//..
// Finally, create a shared pointer passing to it 'object' and the address of
// 'deleter':
//..
    bsl::shared_ptr<my_Obj> handle(object, &deleter, &basicAllocator);
//..
// Now the 'handle' can be passed to another thread or enqueued efficiently.
// When the reference count of 'handle' goes to 0, 'object' is automatically
// deleted via the 'deleteObject' method of 'deleter', which in turn will
// invoke the destructor of 'object'.  Note that since the type of the deleter
// used to instantiate 'handle' is 'bdlma::Deleter<my_Obj>', any kind of
// deleter that implements this protocol can be passed.  Also note, on the
// downside, that the lifetime of 'deleter' must be longer than the lifetime of
// all associated instances.
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // TESTING bdlma::DefaultDeleter
        //
        // Concerns:
        //   o bdlma::DefaultDeleter works with global 'new' and 'delete'
        //   o bdlma::DefaultDeleter works with user-installed allocator
        // Plan:
        //   Create 'my_Obj' using different allocators
        // Testing:
        //   bdlma::DefaultDeleter(bcem::Allocator *);
        //   deleteObject(my_Obj *);
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "TESTING bdlma::DefaultDeleter" << endl
                                  << "============================" << endl;

        if (verbose) cout << "\tUsing global operators 'new' and 'delete"
                          << endl;
        {
            int destructorFlag;
            my_Obj *object = new (*bslma::Default::defaultAllocator())
                                                       my_Obj(&destructorFlag);
            bdlma::DefaultDeleter<my_Obj> deleter;
            bdlma::Deleter<my_Obj> *base = &deleter;
            base->deleteObject(object);
            ASSERT(1 == destructorFlag);
        }
        if (verbose) cout << "\tUsing user-installed allocator" << endl;
        {
            bslma::TestAllocator testAllocator(veryVerbose);
            bslma::Allocator *allocator = &testAllocator;
            int destructorFlag;
            my_Obj *object =
                new (testAllocator) my_Obj(&destructorFlag);
            bdlma::DefaultDeleter<my_Obj> deleter(allocator);
            deleter.deleteObject(object);
            ASSERT(1 == destructorFlag);
        }
      } break;
      default: {
        cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        cerr << "Error, non-zero test status = " << testStatus << "." << endl;
    }
    return testStatus;
}

// ----------------------------------------------------------------------------
// Copyright 2015 Bloomberg Finance L.P.
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
