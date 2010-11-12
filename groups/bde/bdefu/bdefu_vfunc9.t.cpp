// bdefu_vfunc9.t.cpp              -*-C++-*-

#include <bdefu_vfunc9.h>

#include <bdema_testallocator.h>                // for testing only
#include <bdema_default.h>

#include <iostream>
#include <stdlib.h>     // atoi()
using namespace BloombergLP;
using namespace std;


//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
//
// Verify that the parameters are passed with the correct type and used
// properly to populate the functor.
//
// We use the following abbreviations:
//  'bA'      for 'bdema_Allcoator'
//  'A1-N'    for 'A1, A2, ... , AN'
//  'D1-N'    for 'D1, D2, ... , DN'
//  'c_A1'    for 'const A1'
//  'c_A1-N   for 'const A1, const A2, ... ,  const AN'
//  'c_A1-N&' for 'const A1&, const A2&, ... , const AN&'
//  'c_D1'    for 'const D1'
//  'c_D1-N'  for 'const D1, const D2, ... , const DN'
//  'c_D1-N&' for 'const D1&, const D2&, ... , const DN&'
//  'c_F'     for 'const F'
//  'c_C'     for 'const C'
//  'c_M'     for 'const M'
//  'c_O'     for 'const OBJ'
//
//-----------------------------------------------------------------------------
// [ 9] makeF(         bdef_Vfunc9<A1-9> *,       F);
// [ 9] makeC(         bdef_Vfunc9<A1-9> *, c_O&, C);
// [ 9] makeM(         bdef_Vfunc9<A1-9> *, OBJ*, M);
// [ 9] makeF(   bA *, bdef_Vfunc9<A1-9> *,       F);
// [ 9] makeC(   bA *, bdef_Vfunc9<A1-9> *, c_O&, C);
// [ 9] makeM(   bA *, bdef_Vfunc9<A1-9> *, OBJ*, M);
// [ 9] makeNull(      bdef_Vfunc9<A1-9> *);
// [ 9] makeNull(bA *, bdef_Vfunc9<A1-9> *);
//-----------------------------------------------------------------------------

//==========================================================================
//                  STANDARD BDE ASSERT TEST MACRO
//--------------------------------------------------------------------------
static int testStatus = 0;

static void aSsErT(int c, const char *s, int i) {
    if (c) {
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}

# define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }
//--------------------------------------------------------------------------
#define LOOP_ASSERT(I,X) { \
    if (!(X)) { cout << #I << ": " << I << "\n"; aSsErT(1, #X, __LINE__);}}

#define LOOP2_ASSERT(I,J,X) { \
    if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " \
        << J << "\n"; aSsErT(1, #X, __LINE__); } }


//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", "<< flush; // P(X) without '\n'
#define L_ __LINE__                           // current Line number


//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS/VARIABLES FOR TESTING
//-----------------------------------------------------------------------------
// This constant is used to verify that callback functions with zero arguments
// are being executed, as the callback functions will increment this variable.

    int a0_global;

// The following constants provide unique data used to verify the position
// correctness of functor arguments.  The constants are used in conjunction
// with the 'checkArgumentValues' family of helper functions.

    const int ARG0       = 256;
    const int ARG1       = 1;
    const int ARG2       = 2;
    const int ARG3       = 3;
    const int ARG4       = 4;
    const int ARG5       = 5;
    const int ARG6       = 6;
    const int ARG7       = 7;
    const int ARG8       = 8;
    const int ARG9       = 9;
    const int ARG1_PLUS1 = ARG1 + 1;
    const int ARG0_PLUS1 = ARG0 + 1;

// The following set of classes provide unique types used to verify that the
// template parameter types were implemented correctly.  These classes are
// used as argument types in the 'checkParameterTypes' family of helper
// functions.

    struct T1 { };
    struct T2 { };
    struct T3 { };
    struct T4 { };
    struct T5 { };
    struct T6 { };
    struct T7 { };
    struct T8 { };
    struct T9 { };

//=============================================================================
//                  CALLBACK CLASSES AND FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------
//
// We implement callback functions that check (a) the correct positional
// matching between the arguments and function parameters, i.e., that the
// first argument is passed as the first parameter, the second as the second,
// etc.  and (b) the type correctness of the parameters.  We implement
// multiple variations of each of these two functions as a free function and
// as a 'const'/non-'const' member function, with the number of parameters
// ranging from zero to nine.
//
// Here is the sample of each function:
//..
//   static void checkArgumentValues4(int *a1, int *a2, int *a3, int *a4)
//       // Invoke the global 'ASSERT' macro to verify that the specified
//       // arguments 'a1', 'a2', 'a3', and 'a4' are equal to the global values
//       // 'ARG1', 'ARG2', 'ARG3', and 'ARG4, respectively.  Then
//       // increment 'a1' by one.
//       //
//       // This function is used to test the correctness of the passed in
//       // arguments.  The arguments passed in for the specified 'a1', 'a2',
//       // 'a3', and 'a4' are initialized with the values of 'ARG1', 'ARG2',
//       // 'ARG3', and 'ARG4' respectively.  If the arguments are passed in
//       // the wrong order, the ASSERT statement will catch the bug.  The last
//       // statement incrementing '*a1' allows the caller to verify that the
//       // function was actually called and that the 'ASSERT' statements were
//       // in fact executed.
//   {
//       ASSERT(*a1 == ARG1);
//       ASSERT(*a2 == ARG2);
//       ASSERT(*a3 == ARG3);
//       ASSERT(*a4 == ARG4);
//       ++*a1;
//   }
//
//   static void checkParameterTypes4(T1, T2, T3, T4)
//       // Do nothing.
//       //
//       // This function is used to test the correctness of the argument types
//       // that 'bdefi_vfunc9X4' classes pass when calling the callback
//       // function.
//       //
//       // The specified types 'T1', 'T2', 'T3', and 'T4' are not convertible
//       // to each other or to or from any other type.  This uniqueness
//       // property allows the compiler to detect type mismatches between
//       // the types of the function parameters and the types of the arguments
//       // passed in.
//       //
//       // In addition, the uniqueness of the parameters is used to verify
//       // that the destructor of the object under test is properly executed.
//   {
//       return;
//   }
//..

//-----------------------------------------------------------------------------
//                           CALLBACK CLASSES
//-----------------------------------------------------------------------------

class CbClass {
  public:
    void checkVfunc9ExecutionC() const
    {
        ++a0_global;
    }

    void checkVfunc9ExecutionM() const
    {
        ++a0_global;
    }

    void checkArgumentValues1(int *a1) const
    {
        ASSERT(*a1 == ARG1);
        ++*a1;
    }

    void checkArgumentValues2(int *a1, int *a2) const
    {
        ASSERT(*a1 == ARG1);
        ASSERT(*a2 == ARG2);
        ++*a1;
    }

    void checkArgumentValues3(int *a1, int *a2, int *a3) const
    {
        ASSERT(*a1 == ARG1);
        ASSERT(*a2 == ARG2);
        ASSERT(*a3 == ARG3);
        ++*a1;
    }

    void checkArgumentValues4(int *a1, int *a2, int *a3, int *a4) const
    {
        ASSERT(*a1 == ARG1);
        ASSERT(*a2 == ARG2);
        ASSERT(*a3 == ARG3);
        ASSERT(*a4 == ARG4);
        ++*a1;
    }

    void checkArgumentValues5(int *a1, int *a2, int *a3, int *a4,
                              int *a5) const
    {
        ASSERT(*a1 == ARG1);
        ASSERT(*a2 == ARG2);
        ASSERT(*a3 == ARG3);
        ASSERT(*a4 == ARG4);
        ASSERT(*a5 == ARG5);
        ++*a1;
    }

    void checkArgumentValues6(int *a1, int *a2, int *a3, int *a4,
                              int *a5, int *a6) const
    {
        ASSERT(*a1 == ARG1);
        ASSERT(*a2 == ARG2);
        ASSERT(*a3 == ARG3);
        ASSERT(*a4 == ARG4);
        ASSERT(*a5 == ARG5);
        ASSERT(*a6 == ARG6);
        ++*a1;
    }

    void checkArgumentValues7(int *a1, int *a2, int *a3, int *a4,
                              int *a5, int *a6, int *a7) const
    {
        ASSERT(*a1 == ARG1);
        ASSERT(*a2 == ARG2);
        ASSERT(*a3 == ARG3);
        ASSERT(*a4 == ARG4);
        ASSERT(*a5 == ARG5);
        ASSERT(*a6 == ARG6);
        ASSERT(*a7 == ARG7);
        ++*a1;
    }

    void checkArgumentValues8(int *a1, int *a2, int *a3, int *a4,
                              int *a5, int *a6, int *a7, int *a8) const
    {
        ASSERT(*a1 == ARG1);
        ASSERT(*a2 == ARG2);
        ASSERT(*a3 == ARG3);
        ASSERT(*a4 == ARG4);
        ASSERT(*a5 == ARG5);
        ASSERT(*a6 == ARG6);
        ASSERT(*a7 == ARG7);
        ASSERT(*a8 == ARG8);
        ++*a1;
    }

    void checkArgumentValues9(int *a1, int *a2, int *a3, int *a4,
                              int *a5, int *a6, int *a7, int *a8,
                              int *a9) const
    {
        ASSERT(*a1 == ARG1);
        ASSERT(*a2 == ARG2);
        ASSERT(*a3 == ARG3);
        ASSERT(*a4 == ARG4);
        ASSERT(*a5 == ARG5);
        ASSERT(*a6 == ARG6);
        ASSERT(*a7 == ARG7);
        ASSERT(*a8 == ARG8);
        ASSERT(*a9 == ARG9);
        ++*a1;
    }

    void checkParameterTypesC1(T1) const { }
    void checkParameterTypesC2(T1, T2) const { }
    void checkParameterTypesC3(T1, T2, T3) const { }
    void checkParameterTypesC4(T1, T2, T3, T4) const { }
    void checkParameterTypesC5(T1, T2, T3, T4, T5) const { }
    void checkParameterTypesC6(T1, T2, T3, T4, T5, T6) const { }
    void checkParameterTypesC7(T1, T2, T3, T4, T5, T6, T7) const { }
    void checkParameterTypesC8(T1, T2, T3, T4, T5, T6, T7, T8) const { }
    void checkParameterTypesC9(T1, T2, T3, T4, T5, T6, T7, T8, T9) const { }

    void checkParameterTypesM1(T1) { }
    void checkParameterTypesM2(T1, T2) { }
    void checkParameterTypesM3(T1, T2, T3) { }
    void checkParameterTypesM4(T1, T2, T3, T4) { }
    void checkParameterTypesM5(T1, T2, T3, T4, T5) { }
    void checkParameterTypesM6(T1, T2, T3, T4, T5, T6) { }
    void checkParameterTypesM7(T1, T2, T3, T4, T5, T6, T7) { }
    void checkParameterTypesM8(T1, T2, T3, T4, T5, T6, T7, T8) { }
    void checkParameterTypesM9(T1, T2, T3, T4, T5, T6, T7, T8, T9) { }
};

//-----------------------------------------------------------------------------
//                           CALLBACK FREE FUNCTIONS
//-----------------------------------------------------------------------------

void checkVfunc9ExecutionF()
{
    ++a0_global;
}

void checkArgumentValues1(int *a1)
{
    ASSERT(*a1 == ARG1);
    ++*a1;
}

void checkArgumentValues2(int *a1, int *a2)
{
    ASSERT(*a1 == ARG1);
    ASSERT(*a2 == ARG2);
    ++*a1;
}

void checkArgumentValues3(int *a1, int *a2, int *a3)
{
    ASSERT(*a1 == ARG1);
    ASSERT(*a2 == ARG2);
    ASSERT(*a3 == ARG3);
    ++*a1;
}

void checkArgumentValues4(int *a1, int *a2, int *a3, int *a4)
{
    ASSERT(*a1 == ARG1);
    ASSERT(*a2 == ARG2);
    ASSERT(*a3 == ARG3);
    ASSERT(*a4 == ARG4);
    ++*a1;
}

void checkArgumentValues5(int *a1, int *a2, int *a3, int *a4, int *a5)
{
    ASSERT(*a1 == ARG1);
    ASSERT(*a2 == ARG2);
    ASSERT(*a3 == ARG3);
    ASSERT(*a4 == ARG4);
    ASSERT(*a5 == ARG5);
    ++*a1;
}

void checkArgumentValues6(int *a1, int *a2, int *a3, int *a4, int *a5,
                          int *a6)
{
    ASSERT(*a1 == ARG1);
    ASSERT(*a2 == ARG2);
    ASSERT(*a3 == ARG3);
    ASSERT(*a4 == ARG4);
    ASSERT(*a5 == ARG5);
    ASSERT(*a6 == ARG6);
    ++*a1;
}

void checkArgumentValues7(int *a1, int *a2, int *a3, int *a4, int *a5,
                          int *a6, int *a7)
{
    ASSERT(*a1 == ARG1);
    ASSERT(*a2 == ARG2);
    ASSERT(*a3 == ARG3);
    ASSERT(*a4 == ARG4);
    ASSERT(*a5 == ARG5);
    ASSERT(*a6 == ARG6);
    ASSERT(*a7 == ARG7);
    ++*a1;
}

void checkArgumentValues8(int *a1, int *a2, int *a3, int *a4, int *a5,
                          int *a6, int *a7, int *a8)
{
    ASSERT(*a1 == ARG1);
    ASSERT(*a2 == ARG2);
    ASSERT(*a3 == ARG3);
    ASSERT(*a4 == ARG4);
    ASSERT(*a5 == ARG5);
    ASSERT(*a6 == ARG6);
    ASSERT(*a7 == ARG7);
    ASSERT(*a8 == ARG8);
    ++*a1;
}

void checkArgumentValues9(int *a1, int *a2, int *a3, int *a4, int *a5,
                          int *a6, int *a7, int *a8, int *a9)
{
    ASSERT(*a1 == ARG1);
    ASSERT(*a2 == ARG2);
    ASSERT(*a3 == ARG3);
    ASSERT(*a4 == ARG4);
    ASSERT(*a5 == ARG5);
    ASSERT(*a6 == ARG6);
    ASSERT(*a7 == ARG7);
    ASSERT(*a8 == ARG8);
    ASSERT(*a9 == ARG9);
    ++*a1;
}

void checkParameterTypes1(T1) { }
void checkParameterTypes2(T1, T2) { }
void checkParameterTypes3(T1, T2, T3) { }
void checkParameterTypes4(T1, T2, T3, T4) { }
void checkParameterTypes5(T1, T2, T3, T4, T5) { }
void checkParameterTypes6(T1, T2, T3, T4, T5, T6) { }
void checkParameterTypes7(T1, T2, T3, T4, T5, T6, T7) { }
void checkParameterTypes8(T1, T2, T3, T4, T5, T6, T7, T8) { }
void checkParameterTypes9(T1, T2, T3, T4, T5, T6, T7, T8, T9) { }


//=============================================================================
//                  CLASSES FOR TESTING USAGE EXAMPLES
//-----------------------------------------------------------------------------


//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    // int veryVerbose = argc > 3;
    int veryVeryVerbose = argc > 4;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;;

    bdema_TestAllocator ta(veryVeryVerbose);
    int numDeallocations;

    switch (test) { case 0:  // Zero is always the leading case.
      case 9: {
        // --------------------------------------------------------------------
        // TESTING 9-ARGUMENT FUNCTOR CONFIGURATORS
        //
        //   We have the following specific concerns:
        //    1. Each function passes parameters correctly: i.e., we need to
        //       make sure that the each argument used at its place.
        //    2. The types of the parameters are passed correctly.
        //    3. Verify that the memory allocator is used when approproate.
        //
        // Plan:
        //   Use each function to initialize a separate 'bdef_Vfunc9' object
        //   with the 'checkArgumentValues' function, which verifies that the
        //   parameters are "wired" correctly.  Use each function to
        //   initialize 'bdef_Vfunc9' object with the 'checkParameterTypes'
        //   function, which verifies the correctness of the parameter types.
        //   Call methods that use user-defined memory allocator twice and
        //   make sure the memory is deallocated after the functor was
        //   reloaded.
        //
        // Testing:
        //    makeF(         bdef_Vfunc9<A1-9> *,        F);
        //    makeC(         bdef_Vfunc9<A1-9> *, c_O&,  C);
        //    makeM(         bdef_Vfunc9<A1-9> *, OBJ *, M);
        //    makeNull(      bdef_Vfunc9<A1-9> *);
        //    makeF(   bA *, bdef_Vfunc9<A1-9> *,        F);
        //    makeC(   bA *, bdef_Vfunc9<A1-9> *, c_O&,  C);
        //    makeM(   bA *, bdef_Vfunc9<A1-9> *, OBJ *, M);
        //    makeNull(bA *, bdef_Vfunc9<A1-9> *);
        // --------------------------------------------------------------------

        if (verbose)
            cout << endl
                 << "TESTING 9-ARGUMENT FUNCTOR CONFIGURATORS" << endl
                 << "========================================" << endl;

        if (verbose)
            cout << endl
                 << "Testing Configurators Without Memory Allocator" << endl
                 << "----------------------------------------------" << endl;

        if (verbose)
            cout << "\nTesting argument values." << endl;
        {
            int a1 = ARG1;
            int a2 = ARG2;
            int a3 = ARG3;
            int a4 = ARG4;
            int a5 = ARG5;
            int a6 = ARG6;
            int a7 = ARG7;
            int a8 = ARG8;
            int a9 = ARG9;

            CbClass cbObj;

            if (verbose) cout << "Testing 'makeF'" << endl;
            {
                bdef_Vfunc9<int *, int *, int *, int *, int *, int *,
                            int *, int *, int *> x1;
                bdefu_Vfunc9::makeF(&x1, checkArgumentValues9);
                x1(&a1, &a2, &a3, &a4, &a5, &a6, &a7, &a8, &a9);
                ASSERT(ARG1_PLUS1 == a1);
                a1 = ARG1;
            }

            if (verbose) cout << "Testing 'makeC'" << endl;
            {
                bdef_Vfunc9<int *, int *, int *, int *, int *, int *,
                            int *, int *, int *> x1;
                bdefu_Vfunc9::makeC(&x1, cbObj,
                                    &CbClass::checkArgumentValues9);
                x1(&a1, &a2, &a3, &a4, &a5, &a6, &a7, &a8, &a9);
                ASSERT(ARG1_PLUS1 == a1);
                a1 = ARG1;
            }

            if (verbose) cout << "Testing 'makeM'" << endl;
            {
                bdef_Vfunc9<int *, int *, int *, int *, int *, int *,
                            int *, int *, int *> x1;
                bdefu_Vfunc9::makeM(&x1, &cbObj,
                                    &CbClass::checkArgumentValues9);
                x1(&a1, &a2, &a3, &a4, &a5, &a6, &a7, &a8, &a9);
                ASSERT(ARG1_PLUS1 == a1);
                a1 = ARG1;
            }

            if (verbose) cout << "\nTesting Null functor." << endl;
            {
                bdef_Vfunc9<int *, int *, int *, int *, int *, int *,
                            int *, int *, int *> x1;
                bdefu_Vfunc9::makeNull(&x1);
                x1(&a1, &a2, &a3, &a4, &a5, &a6, &a7, &a8, &a9);
            }
        }
        if (verbose)
            cout << "\nTesting parameter types." << endl;
        {
            T1 a1;
            T2 a2;
            T3 a3;
            T4 a4;
            T5 a5;
            T6 a6;
            T7 a7;
            T8 a8;
            T9 a9;

            if (verbose) cout << "Testing 'makeF'" << endl;
            {
                bdef_Vfunc9<T1, T2, T3, T4, T5, T6, T7, T8, T9> x1;
                bdefu_Vfunc9::makeF(&x1, checkParameterTypes9);
                x1(a1, a2, a3, a4, a5, a6, a7, a8, a9);
            }

            if (verbose) cout << "Testing 'makeC'" << endl;
            {
                bdef_Vfunc9<T1, T2, T3, T4, T5, T6, T7, T8, T9> x1;
                CbClass cbObj;
                bdefu_Vfunc9::makeC(&x1, cbObj,
                                    &CbClass::checkParameterTypesC9);
                x1(a1, a2, a3, a4, a5, a6, a7, a8, a9);
            }

            if (verbose) cout << "Testing 'makeM'" << endl;
            {
                bdef_Vfunc9<T1, T2, T3, T4, T5, T6, T7, T8, T9> x1;
                CbClass cbObj;
                bdefu_Vfunc9::makeM(&x1, &cbObj,
                                     &CbClass::checkParameterTypesM9);
                x1(a1, a2, a3, a4, a5, a6, a7, a8, a9);
            }
        }


        if (verbose)
            cout << endl
                 << "Testing Configurators With Memory Allocator" << endl
                 << "-------------------------------------------" << endl;

        if (verbose)
            cout << "\nTesting argument values." << endl;
        {
            int a1 = ARG1;
            int a2 = ARG2;
            int a3 = ARG3;
            int a4 = ARG4;
            int a5 = ARG5;
            int a6 = ARG6;
            int a7 = ARG7;
            int a8 = ARG8;
            int a9 = ARG9;

            CbClass cbObj;

            if (verbose) cout << "Testing 'makeF'" << endl;
            {
                bdef_Vfunc9<int *, int *, int *, int *, int *, int *,
                            int *, int *, int *> x1;
                bdefu_Vfunc9::makeF(&ta, &x1, checkArgumentValues9);
                x1(&a1, &a2, &a3, &a4, &a5, &a6, &a7, &a8, &a9);
                ASSERT(ARG1_PLUS1 == a1);
                a1 = ARG1;

                numDeallocations = ta.numDeallocation();
                bdefu_Vfunc9::makeF(&ta, &x1, checkArgumentValues9);
                ASSERT(++numDeallocations == ta.numDeallocation());
            }

            if (verbose) cout << "Testing 'makeC'" << endl;
            {
                bdef_Vfunc9<int *, int *, int *, int *, int *, int *,
                            int *, int *, int *> x1;
                bdefu_Vfunc9::makeC(&ta, &x1, cbObj,
                                    &CbClass::checkArgumentValues9);
                x1(&a1, &a2, &a3, &a4, &a5, &a6, &a7, &a8, &a9);
                ASSERT(ARG1_PLUS1 == a1);
                a1 = ARG1;

                numDeallocations = ta.numDeallocation();
                bdefu_Vfunc9::makeC(&ta, &x1, cbObj,
                                    &CbClass::checkArgumentValues9);
                ASSERT(++numDeallocations == ta.numDeallocation());

            }

            if (verbose) cout << "Testing 'makeM'" << endl;
            {
                bdef_Vfunc9<int *, int *, int *, int *, int *, int *,
                            int *, int *, int *> x1;
                bdefu_Vfunc9::makeM(&ta, &x1, &cbObj,
                                    &CbClass::checkArgumentValues9);
                x1(&a1, &a2, &a3, &a4, &a5, &a6, &a7, &a8, &a9);
                ASSERT(ARG1_PLUS1 == a1);
                a1 = ARG1;

                numDeallocations = ta.numDeallocation();
                bdefu_Vfunc9::makeM(&ta, &x1, &cbObj,
                                    &CbClass::checkArgumentValues9);
                ASSERT(++numDeallocations == ta.numDeallocation());

            }

            if (verbose)
                cout << "\nTesting Null functor." << endl;
            {
                bdef_Vfunc9<int *, int *, int *, int *, int *, int *,
                            int *, int *, int *> x1;
                bdefu_Vfunc9::makeNull(&ta, &x1);
                x1(&a1, &a2, &a3, &a4, &a5, &a6, &a7, &a8, &a9);

                numDeallocations = ta.numDeallocation();
                bdefu_Vfunc9::makeNull(&ta, &x1);
                ASSERT(++numDeallocations == ta.numDeallocation());
            }
        }
        if (verbose)
            cout << "\nTesting parameter types." << endl;
        {
            T1 a1;
            T2 a2;
            T3 a3;
            T4 a4;
            T5 a5;
            T6 a6;
            T7 a7;
            T8 a8;
            T9 a9;

            if (verbose) cout << "Testing 'makeF'" << endl;
            {
                bdef_Vfunc9<T1, T2, T3, T4, T5, T6, T7, T8, T9> x1;
                bdefu_Vfunc9::makeF(&ta, &x1, checkParameterTypes9);
                x1(a1, a2, a3, a4, a5, a6, a7, a8, a9);
            }

            if (verbose) cout << "Testing 'makeC'" << endl;
            {
                bdef_Vfunc9<T1, T2, T3, T4, T5, T6, T7, T8, T9> x1;
                CbClass cbObj;
                bdefu_Vfunc9::makeC(&ta, &x1, cbObj,
                                    &CbClass::checkParameterTypesC9);
                x1(a1, a2, a3, a4, a5, a6, a7, a8, a9);
            }

            if (verbose) cout << "Testing 'makeM'" << endl;
            {
                bdef_Vfunc9<T1, T2, T3, T4, T5, T6, T7, T8, T9> x1;
                CbClass cbObj;
                bdefu_Vfunc9::makeM(&ta, &x1, &cbObj,
                                     &CbClass::checkParameterTypesM9);
                x1(a1, a2, a3, a4, a5, a6, a7, a8, a9);
            }
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

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2002
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
