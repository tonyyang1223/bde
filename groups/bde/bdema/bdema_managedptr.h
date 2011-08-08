// bdema_managedptr.h                                                 -*-C++-*-
#ifndef INCLUDED_BDEMA_MANAGEDPTR
#define INCLUDED_BDEMA_MANAGEDPTR

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide a managed pointer class.
//
//@CLASSES:
//                bdema_ManagedPtr: proctor for automatic memory management
//      bdema_ManagedPtrNilDeleter: used for creating managed pointers to
//                                  stack-allocated objects (DEPRECATED)
//     bdema_ManagedPtrNoOpDeleter: used for creating managed pointers to
//                                  stack-allocated objects
//
//@AUTHOR: Ilougino Rocha (irocha), Pablo Halpern (phalpern)
//
//@DESCRIPTION: This component provides a proctor, similar to 'bsl::auto_ptr',
// that supports user-specified deleters.  The proctor (OED: "a person employed
// to manage the affairs of another") is responsible for the automatic
// destruction of the object referenced by the managed pointer.  (See
// "Aliasing" section below for the detailed description of this behavior.)
//
///Deleters
///--------
// When a managed pointer is destroyed, the managed object is destroyed
// using the user supplied "deleter".  A deleter is simply a function
// that is invoked with a pointer to the object to be deleted, and an optional
// pointer to a 'factory'.  The meaning of 'factory' depends on the deleter.
// The following are examples of deleter functions:
//..
// void nilDeleter(void *, void*)
// {
//    // Do nothing
// }
//
// void MyFactoryDeleter(MyObject *object, MyFactory *factory)
// {
//    factory->deleteObject(object);
// }
//
// void MyReleaseOnlyDeleter(MyObject *object, MyFactory *factory)
// {
//    factory->releaseObject(object);
// }
//..
// 'bdema_ManagedPtr' provides default deleters for factory objects that
// provide a 'deleteObject' operation in the fashion of the second example
// above (e.g. 'bslma_Allocator').  For example, to construct a managed pointer
// to an object constructed from a 'bdema_Pool' memory pool, simply do the
// following:
//..
// bdema_ManagedPtr<MyObject> createObject(bdema_Pool *pool)
// {
//    return bdema_ManagedPtr<MyObject>(new(*pool) MyObject(), pool);
// }
//..
// In the above example, when the returned managed pointer is destroyed,
// 'deleteObject' will be invoked on 'pool' with a pointer to the 'MyObject'
// instance, returning all memory allocated by 'MyObject' to the 'pool'.
//
// Note that the instance pointer that is passed to the deleter is always the
// object that was used to initialize the managed pointer.  That is, if the
// managed pointer is converted to a new pointer value through either casting
// or aliasing, then the original object pointer is remembered, and will be
// passed to the deleter when destroying the managed instance.  This ensures
// that the proper destructor logic is called regardless of how the managed
// pointer is transformed.
//
///Exception safety
///----------------
// The principal usage of a managed pointer is to guarantee that a local object
// will be deallocated properly should an operation throw after its allocation.
// In this, it is very similar to 'bsl::auto_ptr'.  It is required for the
// proper functioning of this component that a deleter does not throw at
// invocation (upon destruction or re-assignment of the managed pointer).
//
///Aliasing
///--------
// In a managed pointer, the pointer value (the value returned by the 'ptr'
// method) and the pointer to the managed object need not be the same value.
// the 'loadAlias' method allows a managed pointer to be constructed as an
// "alias" to another managed pointer (possibly of a different type), which
// we'll call the "original" managed pointer.  When 'ptr' is invoked on the
// alias, the aliased pointer value is returned, but when the managed pointer
// is destroyed, the original managed instance will be passed to the deleter.
// (See also the documentation of the 'alias' constructor or of the
// 'loadAlias' method.)
//
// For instance, suppose that we wish to give access to an item in a temporary
// array via a pointer which we'll call the "finger".  The entire array must
// remain valid until the finger is destroyed, but the finger is the only
// reference to the array or any part of the array.  The finger is the only
// pointer to the array or any part of the array, but the entire array must be
// valid until the finger is destroyed, at which time the entire array must be
// deleted.  We handle this situation by first creating a managed pointer to
// the entire array, then creating an alias of that pointer for the
// finger.  The finger takes ownership of the array instance, and when the
// finger is destroyed, it is the array's address, rather than the finger,
// that is passed to the deleter.
//
// What follows is a concrete example illustrating the alias concept.
// Let's say our array stores data acquired from a ticker
// plant accessible by a global 'getQuote()' function:
//..
//  double getQuote() // From ticker plant.  Simulated here
//  {
//      static const double QUOTES[] = {
//          7.25, 12.25, 11.40, 12.00, 15.50, 16.25, 18.75, 20.25, 19.25, 21.00
//      };
//      static const int NUM_QUOTES = sizeof(QUOTES) / sizeof(QUOTES[0]);
//      static int index = 0;
//
//      double ret = QUOTES[index];
//      index = (index + 1) % NUM_QUOTES;
//      return ret;
//  }
//..
// We now want to find the first quote larger than a specified threshold, but
// would also like to keep the earlier and later quotes for possible
// examination.  Our 'getFirstQuoteLargerThan' function must allocate memory
// for an array of quotes (the threshold and its neighbors).  It thus returns
// a managed pointer to the desired value:
//..
//  const double END_QUOTE = -1;
//
//  bdema_ManagedPtr<double>
//  getFirstQuoteLargerThan(double threshold, bslma_Allocator *allocator)
//  {
//      assert( END_QUOTE < 0 && 0 <= threshold );
//..
// We allocate our array with extra room to mark the beginning and end with a
// special 'END_QUOTE' value:
//..
//      const int MAX_QUOTES = 100;
//      int numBytes = (MAX_QUOTES + 2) * sizeof(double);
//      double *quotes = (double*) allocator->allocate(numBytes);
//      quotes[0] = quotes[MAX_QUOTES + 1] = END_QUOTE;
//..
// Then we read quotes until the array is full, keeping track of the first
// quote that exceeds the threshold.
//..
//      double *finger = 0;
//
//      for (int i = 1; i <= MAX_QUOTES; ++i) {
//          double quote = getQuote();
//          quotes[i] = quote;
//          if (! finger && quote > threshold) {
//              finger = &quotes[i];
//          }
//      }
//..
// Before we return, we create a managed pointer to the entire array:
//..
//      bdema_ManagedPtr<double> managedQuotes(quotes, allocator);
//..
// Then we use the alias constructor to create a managed pointer that points
// to the desired value (the finger) but manages the entire array:
//..
//      return bdema_ManagedPtr<double>(managedQuotes, finger);
//  }
//..
// Our main program calls 'getFirstQuoteLargerThan' like this:
//..
//  int main()
//  {
//      bslma_TestAllocator ta;
//      bdema_ManagedPtr<double> result = getFirstQuoteLargerThan(16.00, &ta);
//      assert(*result > 16.00);
//      assert(1 == ta.numBlocksInUse());
//      bsl::cout << "Found quote: " << *result << bsl::endl;
//..
// We also print the preceding 5 quotes in last-to-first order:
//..
//      int i;
//      bsl::cout << "Preceded by:";
//      for (i = -1; i >= -5; --i) {
//          double quote = result.ptr()[i];
//          if (END_QUOTE == quote) {
//              break;
//          }
//          assert(quote < *result);
//          bsl::cout << ' ' << quote;
//      }
//      bsl::cout << bsl::endl;
//..
// To move the finger, e.g. to the last position printed, one must be careful
// to retain the ownership of the entire array.  Using the statement
// 'result.load(result.ptr()-i)' would be an error, because it would first
// compute the pointer value 'result.ptr()-i' of the argument, then release the
// entire array before starting to manage what has now become an invalid
// pointer.  Instead, 'result' must retain its ownership to the entire array,
// which can be attained by:
//..
//      result.loadAlias(result, result.ptr()-i);
//..
// If we reset the result pointer, the entire array is deallocated:
//..
//      result.clear();
//      assert(0 == ta.numBlocksInUse());
//      assert(0 == ta.numBytesInUse());
//
//      return 0;
//  }
//..
//
///Type Casting
///------------
// 'bdema_ManagedPtr' objects can be implicitly and explicitly cast to
// different types in the same way as native pointers can.
//
///Implicit Casting
/// - - - - - - - -
// As with native pointers, a pointer of the type 'B' that is derived from the
// type 'A', can be directly assigned a 'bdema_ManagedPtr' of 'A'.
// In other words, consider the following code snippets:
//..
//  void implicitCastingExample()
//  {
//..
// If the statements:
//..
//      B *b_p = 0;
//      A *a_p = b_p; // conversion from 'B*' to 'A*'
//..
// are legal expressions, then the statements
//..
//      bdema_ManagedPtr<B> b_mp1;
//      bdema_ManagedPtr<A> a_mp1;
//      a_mp1 = b_mp1; // conversion assignment
//..
// and
//..
//      bdema_ManagedPtr<B> b_mp2;
//      bdema_ManagedPtr<A> a_mp2(b_mp2); // conversion construction
//  }
//..
// should also be valid.  Note that in all of the above cases, the proper
// destructor of 'B' will be invoked when the instance is destroyed even if
// 'A' does not provide a polymorphic destructor.
//
///Explicit Casting
/// - - - - - - - -
// Through "aliasing", a managed pointer of any type can be explicitly cast
// to a managed pointer of any other type using any legal cast expression.
// For example, to static-cast a managed pointer of type A to a managed pointer
// of type B, one can simply do the following:
//..
//  void explicitCastingExample()
//  {
//      bdema_ManagedPtr<A> a_mp1;
//      bdema_ManagedPtr<B> b_mp1(a_mp, static_cast<B*>(a_mp1.ptr()));
//..
// or even use the less safe "C"-style casts:
//..
//      bdema_ManagedPtr<A> a_mp2;
//      bdema_ManagedPtr<B> b_mp2(a_mp, (B*)(a_mp2.ptr()));
//
//  } // explicitCastingExample()
//..
// Note that when using dynamic cast, if the cast fails, the target managed
// pointer will be reset to an unset state, and the source will not be
// modified.  Consider for example the following snippet of code:
//..
//  void processPolymorphicObject(bdema_ManagedPtr<A> aPtr)
//  {
//      bdema_ManagedPtr<B> bPtr(aPtr, dynamic_cast<B*>(aPtr.ptr()));
//      if (bPtr) {
//          processBObject(bPtr);
//      }
//      else {
//          processAObject(aPtr);
//      }
//  }
//..
// If the value of 'aPtr' can be dynamically cast to 'B*' then ownership is
// transferred to 'bPtr', otherwise 'aPtr' is to be modified.  As previously
// stated, the managed instance will be destroyed correctly regardless of how
// it is cast.

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

#ifndef INCLUDED_BDEMA_MANAGEDPTRDELETER
#include <bdema_managedptrdeleter.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITS
#include <bslalg_typetraits.h>
#endif

#ifndef INCLUDED_BSLMF_ADDREFERENCE
#include <bslmf_addreference.h>
#endif

#ifndef INCLUDED_BSLMF_ASSERT
#include <bslmf_assert.h>
#endif

#ifndef INCLUDED_BSLMF_ENABLEIF
#include <bslmf_enableif.h>
#endif

#ifndef INCLUDED_BSLMF_IS_CONVERTIBLE
#include <bslmf_isconvertible.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BSLS_COMPILERFEATURES
#include <bsls_compilerfeatures.h>
#endif

#ifndef INCLUDED_BSLS_UNSPECIFIEDBOOL
#include <bsls_unspecifiedbool.h>
#endif

#ifndef INCLUDED_BSL_UTILITY
#include <bsl_utility.h>
#endif

namespace BloombergLP {

class bslma_Allocator;

template <class BDEMA_TYPE>
class bdema_ManagedPtr_Ref;

                     // =====================================
                     // private class bdema_ManagedPtrMembers
                     // =====================================

struct bdema_ManagedPtr_Nullptr {
    // This 'struct' provides a typedef for a type that can match an null
    // pointer constant, but is not a pointer itself.  This provides a limited
    // emulation of the C++11 'std::nullptr_t' type.

  private:
    struct Nullptr_ProxyType { int dummy; };

  public:
    typedef int Nullptr_ProxyType::* Type;
};

                     // =====================================
                     // private class bdema_ManagedPtrMembers
                     // =====================================

class bdema_ManagedPtr_Members {
    // Non-type-specific managed pointer member variables.  This type exists
    // so that a 'bdema_ManagedPtr_Ref' can point to the representation of a
    // 'bdema_ManagedPtr' even if the ref object is instantiated on a different
    // type than the managed pointer type (e.g. in the case of conversions).
    // 'bdema_ManagerPtr_Members' also "erases" the types of each member, so
    // that the same object code can be shared between multiple instantiations
    // of the 'bdema_ManagedPtr' class template, reducing template bloat.

public:
    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS(bdema_ManagedPtr_Members,
                                 bslalg_TypeTraitBitwiseMoveable);

private:
    // PRIVATE TYPES
    typedef bdema_ManagedPtrDeleter::Deleter DeleterFunc;

    void                    *d_obj_p;     // pointer to the managed instance.
                                          // A void pointer is used so that a
                                          // bdem_ManagedPtrRef of a different
                                          // type can point to this pointer.
    bdema_ManagedPtrDeleter  d_deleter;   // deleter-related information

    // NOT IMPLEMENTED
    bdema_ManagedPtr_Members& operator=(const bdema_ManagedPtr_Members&);

public:
    //CREATORS
    bdema_ManagedPtr_Members();
        // Default constructor.  Sets 'd_obj_p' to 0, and default constructs
        // 'd_deleter'.

    explicit bdema_ManagedPtr_Members(bdema_ManagedPtr_Members& other);
        // Create a 'bdema_ManagedPtr_Members' object having the 'ptr' and
        // 'rep' of the specified 'other', and then put 'other' into an unset
        // state.

    bdema_ManagedPtr_Members(void *object, void *factory, DeleterFunc deleter);
        // If 'object' is null, create a 'bdema_ManagedPtr_Members' object that
        // does not manage a pointer; otherwise create a
        // 'bdema_ManagedPtr_Members' object having the specified 'object',
        // 'factory' and 'deleter'.

    //! ~bdema_ManagedPtr_Members();
        // Destroy this 'bdema_ManagedPtr_Members' object.  Note that this
        // trivial destructor's definition is compiler generated.

    //MANIPULATORS
    void clear();
        // Reset this managed pointer to an unset state.  If this managed
        // pointer currently has a value, then the managed instance will not
        // be destroyed.

    void move(bdema_ManagedPtr_Members& other);
        // Re-initialize this
        // managed pointer to the specified 'ptr' pointer value, the specified
        // 'object' pointer value, and the specified 'deleter' function, using
        // the specified 'factory'.  The specified 'ptr' points to the object
        // that will be referred to by the '*' and '->' operators, while the
        // specified 'object' points to the object that will be destroyed and
        // deallocated when this ManagedPtr is destroyed.

    void set(void *object, void *factory, DeleterFunc deleter);
        // Re-initialize this managed pointer to the specified 'object' pointer
        // value, and the specified 'deleter' function, using the specified
        // 'factory'.


    void setAliasPtr(void *ptr);
        // The specified 'ptr' points to the object
        // that will be referred to by the '*' and '->' operators, while the
        // specified 'object' points to the object that will be destroyed and
        // deallocated when this ManagedPtr is destroyed.

    void swap(bdema_ManagedPtr_Members& other);
        // Efficiently exchange the value of this object with the value of the
        // specified 'other' object.  This method provides the no-throw
        // exception-safety guarantee.

    //ACCESSORS
    void runDeleter() const;
        // Destroy the current managed object(if any).  Note that calling this
        // method twice without assigning a new pointer to manage will produce
        // undefined behavior, unless this object's current deleter
        // specifically supports 

    void                          *pointer() const;
    const bdema_ManagedPtrDeleter& deleter() const;
};

                           // ======================
                           // class bdema_ManagedPtr
                           // ======================

template <class BDEMA_TYPE>
class bdema_ManagedPtr {
    // This class provides a "smart pointer" to an object of the parameterized
    // 'BDEMA_TYPE', which supports sole ownership of objects: a managed
    // pointer ensures that the object it manages is destroyed when the managed
    // pointer is destroyed, using the appropriate deletion method.  The object
    // (of the parameterized 'BDEMA_TYPE') pointed to by a shared pointer
    // instance may be accessed directly using the '->' operator, or the
    // dereference operator (operator '*') can be used to get a reference to
    // that object.  See the component-level documentation for a thorough
    // description and examples of its many usages.
    //
    // Note that the object managed by a managed pointer instance is usually
    // the same as the object pointed to by that instance (of the same
    // 'BDEMA_TYPE'), but this need not always be true in the presence of
    // conversions or "aliasing": the object pointed-to, of the parameterized
    // 'BDEMA_TYPE', may differ from the managed object of type 'OTHERTYPE'
    // (see the section "Aliasing" in the component-level documentation).
    // Nevertheless, both shall exist or else the managed pointer is *unset*
    // (i.e., manages no object, has no deleter, and points to 0).

    // PRIVATE TYPES
    typedef bdema_ManagedPtrDeleter::Deleter DeleterFunc;

    typedef typename bsls_UnspecifiedBool<bdema_ManagedPtr>::BoolType BoolType;
    // 'BoolType' is an alias for an unspecified type that is implicitly
    // convertible to 'bool', but will not promote to 'int'.  This (opaque)
    // type can be used as an "unspecified boolean type" for converting a
    // managed pointer to 'bool' in contexts such as 'if (mp) { ... }' without
    // actually having a conversion to 'bool' or being less-than comparable
    // (either of which would also enable undesirable implicit comparisons of
    // managed pointers to 'int' and less-than comparisons).

    struct Unspecified {}; // private type to guarantee bslmf_EnableIf
                           // disambiguates

    // COMPONENT-PRIVATE MACROS TO SIMPLIFY DECLARING FUNCTION TEMPLATES
#define BDEMA_COMPATIBLE_POINTERS_ONLY                                  \
    typename bslmf_EnableIf<bslmf_IsConvertible<BDEMA_TARGET_TYPE *,    \
                                                BDEMA_TYPE *>::VALUE,   \
                            Unspecified>::type *
    // This macro can be used to restrict pointer-types deduced in function
    // template signatures to only those that are convertible to 'BDEMA_TYPE *'
    // such as from pointers to derived types to pointer-to-unambiguous-base,
    // or from 'T *' to 'const T *' (but not the other way around).

#define BDEMA_VOID_FOR_ONLY_COMPATIPLE_POINTERS                              \
    typename bslmf_EnableIf<bslmf_IsConvertible<BDEMA_TARGET_TYPE *,         \
                                                BDEMA_TYPE *>::VALUE>::type
    // This macro can be used to restrict pointer-types deduced in function
    // template signatures to only those that are convertible to 'BDEMA_TYPE *'
    // such as from pointers to derived types to pointer-to-unambiguous-base,
    // or from 'T *' to 'const T *' (but not the other way around).

    // DATA
    bdema_ManagedPtr_Members d_members;

    // PRIVATE UTILITY FUNCTION
    static void *stripPointerType(BDEMA_TYPE *ptr);
        // Return the value of the specified 'ptr' as a 'void *', after 
        // stripping all 'const' and 'volatile' qualifiers from 'BDEMA_TYPE'.
        // To avoid accidental type-safety errors, the 'd_members.d_obj_p'
        // variable should never be set directly except by calling through this
        // function.

    // NOT IMPLEMENTED
    void operator==(const bdema_ManagedPtr&) const;
    void operator!=(const bdema_ManagedPtr&) const;
        // The two operator overloads are declared as 'private' but never
        // defined in order to eliminate accidental equality comparisons that
        // would occur through the implicit conversion to an unspecified
        // boolean type.  Note that the return type of 'void' is chosen as it
        // will often produce a clearer error message than relying on the
        // 'private' control failure.

    // FRIENDS
    template <class OTHER> friend class bdema_ManagedPtr;

  public:
    // CREATORS
    bdema_ManagedPtr();
        // Construct a managed pointer that is in an unset state.

    explicit bdema_ManagedPtr(bdema_ManagedPtr_Nullptr::Type);
        // Construct a managed pointer that is in an unset state.  Note that
        // this constructor is necessary to match a single null literal
        // argument, as the only other viable constructor is a template that
        // cannot deduce the pointed-to type from a null pointer literal.

    template<class BDEMA_TARGET_TYPE>
    explicit 
    bdema_ManagedPtr(BDEMA_TARGET_TYPE *ptr, BDEMA_COMPATIBLE_POINTERS_ONLY=0);
        // Construct a managed pointer that manages the specified 'ptr' using
        // the current default allocator to destroy 'ptr' when this managed
        // pointer is destroyed or re-assigned, unless it is released before
        // then.  Note that the object will be initialized to an unset state
        // if 'ptr' == 0.  The behavior is undefined unless the object referred
        // to by 'ptr' can by destroyed by the current default allocator.
        // The behavior is undefined if the lifetime of the object referred to
        // by 'ptr' is already managed by another object.
        // The behavior is undefined if the ...

    bdema_ManagedPtr(bdema_ManagedPtr_Ref<BDEMA_TYPE> ref);
        // Construct a 'bdema_ManagedPtr' and transfer the value and ownership
        // from the managed pointer referred-to by the specified 'ref' object
        // to this managed pointer.  Note that the managed pointer referred-to
        // by 'ref' will be re-initialized to an unset state.  This constructor
        // is used to construct from a managed pointer rvalue.

    bdema_ManagedPtr(bdema_ManagedPtr& other);
        // Construct a 'bdema_ManagedPtr' and transfer the value and ownership
        // from the specified 'other' managed pointer to this managed
        // pointer.  Note that 'other' will be re-initialized to an unset
        // state.

    template <class BDEMA_OTHER_TYPE>
    bdema_ManagedPtr(bdema_ManagedPtr<BDEMA_OTHER_TYPE>& other);
        // Construct a 'bdema_ManagedPtr' and transfer the value and ownership
        // from the specified 'other' managed pointer this managed pointer.
        // Note that 'other' will be re-initialized to an unset state.
        // Compilation will fail unless 'BDEMA_OTHER_TYPE *' is convertible to
        // 'BDEMA_TYPE*'.

    template <class BDEMA_OTHER_TYPE>
    bdema_ManagedPtr(bdema_ManagedPtr<BDEMA_OTHER_TYPE>& alias,
                     BDEMA_TYPE *ptr);
        // Construct a managed pointer that takes over management of the value
        // of the specified 'alias' managed pointer, but which has the
        // specified 'ptr' as an externally-visible value.  The value of 'ptr'
        // is treated as an alias to all or part of 'alias.ptr()'.  Unless it
        // is released first, destroying or re-assigning a managed pointer
        // created with this constructor will delete the object originally
        // managed by 'alias' -- the destructor for '*ptr' is not called
        // directly.  Note that 'alias' will be re-initialized to an unset
        // state, and that the object previously managed by 'alias' will not be
        // deleted until this managed pointer is destroyed or re-assigned.
        // Compilation will fail unless 'BDEMA_OTHER_TYPE *' is convertible to
        // 'BDEMA_TYPE*'.  If '0 == ptr', then this object will be
        // initialized to an unset state.  The behavior is undefined if 'ptr'
        // is already managed by a managed pointer other than 'alias'.

    template <class FACTORY>
    bdema_ManagedPtr(BDEMA_TYPE *ptr, FACTORY *factory);
        // Construct a managed pointer to manage the specified 'ptr' using the
        // specified 'factory' to destroy 'ptr' when this managed pointer is
        // destroyed or re-assigned, unless it is released before then.  The
        // 'FACTORY' class can be any class that has a 'deleteObject' method.
        // 'bslma_Allocator' or any class derived from 'bslma_Allocator' meets
        // the requirements for 'FACTORY'.  If '0 == ptr', then this
        // object will be initialized to an unset state.  The behavior is
        // undefined if 'ptr' is already managed by another managed pointer.

    bdema_ManagedPtr(BDEMA_TYPE *ptr, void *factory, DeleterFunc deleter);
        // Construct a managed pointer to manage the specified 'ptr' using the
        // specified 'deleter' and associated 'factory' to destroy 'ptr' when
        // this managed pointer is destroyed or re-assigned (unless it is
        // released before then).  If 0 == 'ptr', then this object
        // will be initialized to an unset state.  The behavior is undefined
        // if 'ptr' is already managed by another managed pointer.

    bdema_ManagedPtr(BDEMA_TYPE *ptr,
                     bdema_ManagedPtr_Nullptr::Type,
                     void      (*deleter)(BDEMA_TYPE *, void*));
        // Construct a managed pointer to manage the specified 'ptr' using the
        // specified 'deleter' to destroy 'ptr' when this managed pointer is
        // destroyed or re-assigned (unless it is released before then).  If
        // '0 == ptr', then this object will be initialized to an unset state.
        // The behavior is undefined if 'ptr' is already managed by another
        // managed pointer.  Note that behavior is undefined unless 'deleter'
        // accepts null pointers in the second argument.

    template <class FACTORY>
    bdema_ManagedPtr(BDEMA_TYPE *ptr,
                     FACTORY    *factory,
                     void      (*deleter)(BDEMA_TYPE *, FACTORY *));
        // Construct a managed pointer to manage the specified 'ptr' using the
        // specified 'deleter' and associated 'factory' to destroy 'ptr' when
        // this managed pointer is destroyed or re-assigned (unless it is
        // released before then).  If 0 == 'ptr', then this object will be
        // initialized to an unset state.  The behavior is undefined if 'ptr'
        // is already managed by another managed pointer.  Note that if
        // '0 == factory' then behavior is undefined unless 'deleter' accepts
        // null pointers in the second argument.

    ~bdema_ManagedPtr();
        // Destroy this managed pointer object and any managed instance by
        // invoking the user-supplied deleter.
        // AJM : note that this destructor does nothing if this object is unset
        //       i.e. it is managing a null pointer.
        // AJM : Note that even aliased null pointers do not execute a deleter.

    // MANIPULATORS
    bdema_ManagedPtr& operator=(bdema_ManagedPtr& rhs);
        // Assign to this managed pointer the value and ownership of the
        // specified 'rhs' managed pointer.  Note that 'rhs' will be
        // re-initialized to an unset state and that the previous contents of
        // this pointer (if any) are destroyed unless this pointer and 'rhs'
        // point to the same object.
        // TBD document 'return' and note no-op when self assigning.

    template <class BDEMA_OTHERTYPE>
    bdema_ManagedPtr& operator=(bdema_ManagedPtr<BDEMA_OTHERTYPE>& rhs);
        // Assign to this managed pointer the value and ownership of the
        // specified 'rhs' managed pointer.  Note that 'rhs' will be
        // re-initialized to an unset state and that the previous contents of
        // this pointer (if any) are destroyed unless this pointer and 'rhs'
        // point to the same object.  Note the deleter from rhs will
        // be copied, so at destruction, that, rather than ~BDEMA_TYPE(), will
        // be called.

    bdema_ManagedPtr& operator=(bdema_ManagedPtr_Ref<BDEMA_TYPE> ref);
        // Transfer the value and ownership from the specified 'ref' managed
        // pointer to this managed pointer.  Note that 'ref' will be
        // re-initialized to an unset state and that the previous contents of
        // this pointer (if any) are destroyed unless this pointer and 'ref'
        // point to the same object.

    template <class BDEMA_OTHER_TYPE>
    operator bdema_ManagedPtr_Ref<BDEMA_OTHER_TYPE>();
        // Implicit conversion to a managed pointer reference.  This
        // conversion operator is used to allow the construction of managed
        // pointer rvalues because temporaries cannot be passed by modifiable
        // reference.
        // TBD AJM notes that the test driver does not exercise case where
        // conversion is needed.

    void clear();
        // Destroy the current managed object (if any) and re-initialize this
        // managed pointer to an unset state.

    void load(bdema_ManagedPtr_Nullptr::Type =0);
        // Destroy the current managed object (if any) and re-initialize this
        // managed pointer to an unset state.

    template<class BDEMA_TARGET_TYPE>
    BDEMA_VOID_FOR_ONLY_COMPATIPLE_POINTERS 
    load(BDEMA_TARGET_TYPE *ptr);
        // Destroy the current managed object (if any) and re-initialize this
        // managed pointer to manage the specified 'ptr' using the default
        // allocator to destroy 'ptr' when this managed pointer is destroyed
        // or re-assigned, unless it is released before then.  Note that if 0
        // == 'ptr', then this object will be initialized to an unset state.
        // The behavior is undefined if 'ptr' is already managed by another
        // managed pointer.

    template <class FACTORY>
    void load(BDEMA_TYPE *ptr, FACTORY *factory);
        // Destroy the current managed object (if any) and re-initialize this
        // managed pointer to manage the specified 'ptr' using the specified
        // 'factory' deleter to destroy 'ptr' when this managed pointer is
        // destroyed or re-assigned, unless it is released before then.  Note
        // that if '0 == ptr', then this object will be initialized to an
        // unset state.  The behavior is undefined if 'ptr' is already managed
        // by another managed pointer.

    void load(BDEMA_TYPE *ptr, void *factory, DeleterFunc deleter);
        // Destroy the current managed object (if any) and re-initialize this
        // managed pointer to manage the specified 'ptr' using the specified
        // 'deleter' with arguments 'ptr' and the specified 'factory' to
        // destroy 'ptr' when this managed pointer is destroyed or
        // re-assigned, unless it is released before then.  Note that if
        // '0 == ptr', then this object will be initialized to an unset state.
        // The behavior is undefined if 'ptr' is already managed by another
        // managed pointer.

    void load(BDEMA_TYPE *ptr,
              bdema_ManagedPtr_Nullptr::Type,
              void      (*deleter)(BDEMA_TYPE *, void*));
        // Destroy the current managed object (if any) and re-initialize this
        // managed pointer to manage the specified 'ptr' using the specified
        // 'deleter' with argument 'ptr' and '0' when this managed pointer is
        // destroyed or re-assigned, unless it is released before then.  Note
        // that if '0 == ptr', then this object will be initialized to an unset
        // state.  The behavior is undefined if 'ptr' is already managed by
        // another managed pointer, or if 'deleter' does not support null
        // pointer values in its second argument.

    template <class FACTORY>
    void load(BDEMA_TYPE *ptr,
              FACTORY    *factory,
              void      (*deleter)(BDEMA_TYPE *, FACTORY *));
        // Destroy the current managed object (if any) and re-initialize this
        // managed pointer to manage the specified 'ptr' using the specified
        // 'deleter' with arguments 'ptr' and the specified 'factory' to
        // destroy 'ptr' when this managed pointer is destroyed or
        // re-assigned, unless it is released before then.  Note that if
        // '0 == ptr', then this object will be initialized to an unset state.
        // The behavior is undefined if 'ptr' is already managed by another
        // managed pointer.

    template <class BDEMA_OTHER_TYPE>
    void loadAlias(bdema_ManagedPtr<BDEMA_OTHER_TYPE>& alias, BDEMA_TYPE *ptr);
        // Destroy the current managed object (if any) and re-initialize this
        // managed pointer to take over management of the value of the
        // specified 'alias' managed pointer, but with the specified 'ptr' as
        // an externally-visible value.  The value of 'ptr' is treated as an
        // alias to all or part of 'alias.ptr()'.  Unless it is released first,
        // destroying or re-assigning this managed pointer will delete the
        // object originally managed by 'alias' -- the destructor for '*ptr' is
        // not called directly.  Note that 'alias' will be re-initialized to an
        // unset state, and that the object previously managed by 'alias' will
        // not be deleted until this managed pointer is destroyed or
        // re-assigned.  The behavior is undefined if 'ptr' is already managed
        // by a managed pointer other than 'alias'.

    bsl::pair<BDEMA_TYPE *, bdema_ManagedPtrDeleter> release();
        // Return a raw pointer to the current managed object (if any) and its
        // current deleter and factory through the 'bdema_ManagedPtrDeleter'
        // member of the return value, and re-initialize this managed pointer
        // to an unset state.  It is undefined behavior to run the deleter
        // unless the pointer to the returned managed object is not null.

    void swap(bdema_ManagedPtr& rhs);
        // Exchange the value and ownership of this managed pointer with the
        // specified 'rhs' managed pointer.

    // ACCESSORS
    operator BoolType() const;
        // Return a value of the "unspecified bool" that evaluates to 'false'
        // if this managed pointer is in an unset state, and 'true' otherwise.
        // Note that this conversion operator allows a managed pointer to be
        // used within a conditional context (e.g., within an 'if' or 'while'
        // statement), but does *not* allow managed pointers to be compared
        // (e.g., via '<' or '>').

    typename bslmf_AddReference<BDEMA_TYPE>::Type operator*() const;
        // Return a reference to the managed instance.  The behavior is
        // undefined if this managed pointer is in an unset state.
        // TBD document 'void' as a special case

    BDEMA_TYPE *operator->() const;
        // Return the address of the managed (modifiable) instance, or 0 if
        // this managed pointer is in an unset state.

    BDEMA_TYPE *ptr() const;
        // Return the address of the managed (modifiable) instance, or 0 if
        // this managed pointer is in an unset state.

    const bdema_ManagedPtrDeleter& deleter() const;
        // Return a non-modifiable reference to the deleter information
        // associated with this managed pointer.
};

template <class BDEMA_TYPE>
void swap(bdema_ManagedPtr<BDEMA_TYPE>& a, bdema_ManagedPtr<BDEMA_TYPE>& b);
    // Efficiently exchange the values of the specified 'a' and 'b' objects.
    // This function provides the no-throw exception-safety guarantee.

                    // ==================================
                    // struct bdema_ManagedPtrNoOpDeleter
                    // ==================================

struct bdema_ManagedPtrNoOpDeleter {
    // This utility class provides a general no-op deleter, which is useful
    // when creating managed pointers to stack-allocated objects.

    static void deleter(void *object, void *);
        // Deleter function that does nothing.
};

                     // =================================
                     // struct bdema_ManagedPtrNilDeleter
                     // =================================

template <class BDEMA_TYPE>
struct bdema_ManagedPtrNilDeleter {
    // [!DEPRECATED!] This utility class provides a general no-op deleter,
    // which is useful when creating managed pointers to stack-allocated
    // objects.  Note that the non-template class 'bdema_ManagedPtrNoOpDeleter'
    // should be used in preference to this deprecated class.

    static void deleter(void *object, void *);
        // Deleter function that does nothing.
};

                   // ======================================
                   // struct bdema_ManagedPtr_FactoryDeleter
                   // ======================================

template <class BDEMA_TYPE, typename FACTORY>
struct bdema_ManagedPtr_FactoryDeleter {
    // This utility provides a general deleter for objects that provide
    // a 'deleteObject' operation (e.g., 'bslma_Allocator', 'bdema_Pool').

    static void deleter(void *object, void *factory);
        // Deleter function that deletes the specified 'object' by invoking
        // 'deleteObject' on the specified 'factory', casting 'object' to
        // 'BDEMA_TYPE *' and 'factory' to 'FACTORY *'.  Note that the behavior
        // is undefined if 0 == 'object' or if 'factory' does not point to an
        // an object of type 'FACTORY'.
};

                    // ==================================
                    // private class bdema_ManagedPtr_Ref
                    // ==================================

template <class BDEMA_TYPE>
class bdema_ManagedPtr_Ref {
    // This struct holds a managed pointer reference, returned by the implicit
    // conversion operator in the class 'bdema_ManagedPtr'.  This struct is
    // used to allow the construction of temporary managed pointer objects
    // (since temporaries cannot be passed by reference to a modifiable).
    // Note that while no members or methods of this class template depend on
    // the specified 'BDEMA_TYPE', it is important to carry this type into
    // conversions to support passing ownership of 'bdema_ManagedPtr_Members'
    // pointers when assigning or constructing 'bdema_ManagedPtr' objects.
    //
    // TBD document class invariant, '0 != d_base_p'
    // TBD this class could be marked as bitwise-movable, although it should
    //     not show up where the trait is actually useful.

    bdema_ManagedPtr_Members *d_base_p;

public:
    explicit bdema_ManagedPtr_Ref(bdema_ManagedPtr_Members *base);
        // Create a 'bdema_ManagedPtr_Ref' object having the specified 'base'
        // value for its 'base' attribute.  Behavior is undefined unless
        // '0 != base'.


    //! bdema_ManagedPtr_Ref(const bdema_ManagedPtr_Ref& original);
        // Create a 'bdema_ManagedPtr_Ref' object having the same 'd_base_p'
        // value as the specified 'original'.  Note that this trivial
        // constructor's definition is compiler generated.

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
    // The following destructor is generated by the compiler, except in "SAFE"
    // build modes (e.g., to enable the checking of class invariants).

    ~bdema_ManagedPtr_Ref();
        // Destroy this object.
#endif

    //! bdema_ManagedPtr_Ref& operator=(const bdema_ManagedPtr_Ref& original);
        // Create a 'bdema_ManagedPtr_Ref' object having the same 'd_base_p'
        // as the specified 'original'.  Note that this trivial copy-assignment
        // operator's definition is compiler generated.

    // ACCESSORS
    bdema_ManagedPtr_Members *base() const;
        // Return a pointer to 
};

// ============================================================================
//                      INLINE FUNCTION DEFINITIONS
// ============================================================================

                           // ----------------------
                           // class bdema_ManagedPtr
                           // ----------------------
// CREATORS
template <class BDEMA_TYPE>
inline
bdema_ManagedPtr<BDEMA_TYPE>::bdema_ManagedPtr()
{
}

template <class BDEMA_TYPE>
inline
bdema_ManagedPtr<BDEMA_TYPE>::bdema_ManagedPtr(bdema_ManagedPtr_Nullptr::Type)
{
}

template<class BDEMA_TYPE>
template<class BDEMA_TARGET_TYPE>
bdema_ManagedPtr<BDEMA_TYPE>::bdema_ManagedPtr(BDEMA_TARGET_TYPE *ptr,
                                               BDEMA_COMPATIBLE_POINTERS_ONLY)
: d_members(stripPointerType(ptr),
            bslma_Default::allocator(),
            &bdema_ManagedPtr_FactoryDeleter<BDEMA_TARGET_TYPE,bslma_Allocator>
                                                                     ::deleter)
{
#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
    if(!ptr) {
        d_members.clear();
    }
#endif
}

template <class BDEMA_TYPE>
bdema_ManagedPtr<BDEMA_TYPE>::bdema_ManagedPtr(
                                          bdema_ManagedPtr_Ref<BDEMA_TYPE> ref)
: d_members(*ref.base())
{
}

template <class BDEMA_TYPE>
inline
bdema_ManagedPtr<BDEMA_TYPE>::bdema_ManagedPtr(bdema_ManagedPtr& other)
: d_members(other.d_members)
{
}

template <class BDEMA_TYPE>
template <class BDEMA_OTHER_TYPE>
inline
bdema_ManagedPtr<BDEMA_TYPE>::bdema_ManagedPtr(
                                     bdema_ManagedPtr<BDEMA_OTHER_TYPE>& other)
:   d_members(other.d_members)
{
}

template <class BDEMA_TYPE>
template <class BDEMA_OTHER_TYPE>
bdema_ManagedPtr<BDEMA_TYPE>::bdema_ManagedPtr(
                                     bdema_ManagedPtr<BDEMA_OTHER_TYPE>& alias,
                                     BDEMA_TYPE                         *ptr)
: d_members()
{
    if(0 != ptr) {
        d_members.move(alias.d_members);
        d_members.setAliasPtr(stripPointerType(ptr));
    }
}

template <class BDEMA_TYPE>
template <class FACTORY>
inline
bdema_ManagedPtr<BDEMA_TYPE>::bdema_ManagedPtr(BDEMA_TYPE *ptr,
                                                              FACTORY *factory)
: d_members(stripPointerType(ptr),
            factory,
            &bdema_ManagedPtr_FactoryDeleter<BDEMA_TYPE, FACTORY>::deleter)
{
    BSLS_ASSERT_SAFE(0 != factory || 0 == ptr);

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
    if(!ptr) {
        d_members.clear();
    }
#endif
}

template <class BDEMA_TYPE>
inline
bdema_ManagedPtr<BDEMA_TYPE>::bdema_ManagedPtr(BDEMA_TYPE *ptr,
                                               void       *factory,
                                               DeleterFunc deleter)
: d_members(stripPointerType(ptr),
            factory,
            deleter)
{
    BSLS_ASSERT_SAFE(!ptr || 0 != deleter);

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
    if(!ptr) {
        d_members.clear();
    }
#endif
}

template <class BDEMA_TYPE>
inline
bdema_ManagedPtr<BDEMA_TYPE>::bdema_ManagedPtr(BDEMA_TYPE *ptr,
                                               bdema_ManagedPtr_Nullptr::Type,
                                          void (*deleter)(BDEMA_TYPE *, void*))
: d_members(stripPointerType(ptr),
            stripPointerType(ptr),
            0,
            reinterpret_cast<DeleterFunc>(deleter))
{
    BSLS_ASSERT_SAFE(0 != deleter);
}

template <class BDEMA_TYPE>
template <class FACTORY>
inline
bdema_ManagedPtr<BDEMA_TYPE>::bdema_ManagedPtr(
                                 BDEMA_TYPE *ptr,
                                 FACTORY    *factory,
                                 void      (*deleter)(BDEMA_TYPE *, FACTORY*) )
: d_members(stripPointerType(ptr),
            factory,
            reinterpret_cast<DeleterFunc>(deleter))
{
    BSLS_ASSERT_SAFE(0 != factory || 0 == ptr);
    BSLS_ASSERT_SAFE(0 != deleter || 0 == ptr);
}

template <class BDEMA_TYPE>
inline
bdema_ManagedPtr<BDEMA_TYPE>::~bdema_ManagedPtr()
{
    d_members.runDeleter();
}

template <class BDEMA_TYPE>
inline
void *bdema_ManagedPtr<BDEMA_TYPE>::stripPointerType(BDEMA_TYPE *ptr)
{
    return const_cast<void*>(static_cast<const void*>(ptr));
}

template <class BDEMA_TYPE>
inline
void bdema_ManagedPtr<BDEMA_TYPE>::load(bdema_ManagedPtr_Nullptr::Type)
{
    d_members.runDeleter();
    d_members.clear();
}

template <class BDEMA_TYPE>
template <class BDEMA_TARGET_TYPE>
inline
BDEMA_VOID_FOR_ONLY_COMPATIPLE_POINTERS
bdema_ManagedPtr<BDEMA_TYPE>::load(BDEMA_TARGET_TYPE *ptr)
{
    typedef bdema_ManagedPtr_FactoryDeleter<BDEMA_TARGET_TYPE,bslma_Allocator>
                                                                DeleterFactory;
    d_members.runDeleter();
    d_members.set(stripPointerType(ptr), 
                  bslma_Default::allocator(),
                  &DeleterFactory::deleter);
}

template <class BDEMA_TYPE>
template <class FACTORY>
inline
void bdema_ManagedPtr<BDEMA_TYPE>::load(BDEMA_TYPE *ptr, FACTORY *factory)
{
    BSLS_ASSERT_SAFE(0 != factory || 0 == ptr);

    typedef bdema_ManagedPtr_FactoryDeleter<BDEMA_TYPE,FACTORY> DeleterFactory;

    d_members.runDeleter();
    d_members.set(stripPointerType(ptr), 
                  factory, 
                  &DeleterFactory::deleter);
}

template <class BDEMA_TYPE>
inline
void bdema_ManagedPtr<BDEMA_TYPE>::load(BDEMA_TYPE *ptr,
                                        void       *factory,
                                        DeleterFunc deleter)
{
    BSLS_ASSERT_SAFE(0 != deleter || 0 == ptr);

    d_members.runDeleter();
    d_members.set(stripPointerType(ptr),
                  factory,
                  deleter);
}

template <class BDEMA_TYPE>
inline
void bdema_ManagedPtr<BDEMA_TYPE>::load(BDEMA_TYPE *ptr,
                                        bdema_ManagedPtr_Nullptr::Type,
                                        void (*deleter)(BDEMA_TYPE *, void*))
{
    BSLS_ASSERT_SAFE(0 != deleter);

    d_members.runDeleter();
    d_members.set(stripPointerType(ptr),
                  0,
                  reinterpret_cast<DeleterFunc>(deleter));
}

template <class BDEMA_TYPE>
template <class FACTORY>
inline
void
bdema_ManagedPtr<BDEMA_TYPE>::load(BDEMA_TYPE *ptr,
                                   FACTORY    *factory,
                                   void (*deleter)(BDEMA_TYPE *, FACTORY *))
{
    BSLS_ASSERT_SAFE(0 != deleter || 0 == ptr);

    d_members.runDeleter();
    d_members.set(stripPointerType(ptr),
                  factory, 
                  reinterpret_cast<DeleterFunc>(deleter));
}


template <class BDEMA_TYPE>
template <class BDEMA_OTHER_TYPE>
void
bdema_ManagedPtr<BDEMA_TYPE>::loadAlias(
                                     bdema_ManagedPtr<BDEMA_OTHER_TYPE>& alias,
                                     BDEMA_TYPE                         *ptr)
{
    BSLS_ASSERT_SAFE(0 != ptr || 0 == alias.ptr());

    if (ptr && alias.d_members.pointer()) {
        if (&d_members != &alias.d_members) {
            d_members.runDeleter();
            d_members.move(alias.d_members);
        }
        d_members.setAliasPtr(stripPointerType(ptr));
    }
    else {
        d_members.runDeleter();
        d_members.clear();
    }
}

template <class BDEMA_TYPE>
inline
void bdema_ManagedPtr<BDEMA_TYPE>::clear()
{
    d_members.runDeleter();
    d_members.clear();
}

template<typename BDEMA_TYPE>
bsl::pair<BDEMA_TYPE*,bdema_ManagedPtrDeleter> 
bdema_ManagedPtr<BDEMA_TYPE>::release()
{
    BDEMA_TYPE *p = ptr();
    bsl::pair<BDEMA_TYPE*,bdema_ManagedPtrDeleter> ret(p, d_members.deleter());
    d_members.clear();
    return ret;
}

template <class BDEMA_TYPE>
inline
void bdema_ManagedPtr<BDEMA_TYPE>::swap(bdema_ManagedPtr& rhs)
{
    d_members.swap(rhs.d_members);
}

template <class BDEMA_TYPE>
bdema_ManagedPtr<BDEMA_TYPE>&
bdema_ManagedPtr<BDEMA_TYPE>::operator=(bdema_ManagedPtr& rhs)
{   // Must protect against self-assignment due to destructive move
    if (&d_members != &rhs.d_members) {
        d_members.runDeleter();
        d_members.move(rhs.d_members);
    }

    return *this;
}

template <class BDEMA_TYPE>
template <class BDEMA_OTHERTYPE>
bdema_ManagedPtr<BDEMA_TYPE>&
bdema_ManagedPtr<BDEMA_TYPE>::operator=(bdema_ManagedPtr<BDEMA_OTHERTYPE>& rhs)
{   // Must protect against self-assignment due to destructive move
    if (&d_members != &rhs.d_members) {
        d_members.runDeleter();
        d_members.move(rhs.d_members);
    }

    return *this;
}

template <class BDEMA_TYPE>
inline
bdema_ManagedPtr<BDEMA_TYPE>&
bdema_ManagedPtr<BDEMA_TYPE>::operator=(bdema_ManagedPtr_Ref<BDEMA_TYPE> ref)
{   // Must protect against self-assignment due to destructive move
    if (&d_members != ref.base()) {
        d_members.runDeleter();
        d_members.move(*ref.base());
    }

    return *this;
}

template <class BDEMA_TYPE>
template <class BDEMA_OTHER_TYPE>
inline
bdema_ManagedPtr<BDEMA_TYPE>::operator bdema_ManagedPtr_Ref<BDEMA_OTHER_TYPE>()
{
    return bdema_ManagedPtr_Ref<BDEMA_OTHER_TYPE>(&d_members);
}

// ACCESSORS
template <class BDEMA_TYPE>
inline
#if defined(BSLS_PLATFORM__CMP_IBM)
bdema_ManagedPtr<BDEMA_TYPE>::operator 
                                    typename bdema_ManagedPtr::BoolType() const
#else
bdema_ManagedPtr<BDEMA_TYPE>::operator BoolType() const
#endif
{
    return d_members.pointer()
         ? bsls_UnspecifiedBool<bdema_ManagedPtr>::trueValue()
         : bsls_UnspecifiedBool<bdema_ManagedPtr>::falseValue();
}

template <class BDEMA_TYPE>
inline
typename bslmf_AddReference<BDEMA_TYPE>::Type
bdema_ManagedPtr<BDEMA_TYPE>::operator*() const
{
    BSLS_ASSERT_SAFE(d_members.pointer());
    return *static_cast<BDEMA_TYPE*>(d_members.pointer());
}

template <class BDEMA_TYPE>
inline
BDEMA_TYPE *bdema_ManagedPtr<BDEMA_TYPE>::operator->() const
{
    return static_cast<BDEMA_TYPE*>(d_members.pointer());
}

template <class BDEMA_TYPE>
inline
const bdema_ManagedPtrDeleter& bdema_ManagedPtr<BDEMA_TYPE>::deleter() const
{
    return d_members.deleter();
}

template <class BDEMA_TYPE>
inline
BDEMA_TYPE *bdema_ManagedPtr<BDEMA_TYPE>::ptr() const
{
    return static_cast<BDEMA_TYPE*>(d_members.pointer());
}

template <class BDEMA_TYPE>
inline
void swap(bdema_ManagedPtr<BDEMA_TYPE>& a, bdema_ManagedPtr<BDEMA_TYPE>& b)
{
    a.swap(b);
}

                    // --------------------------------------
                    // private class bdema_ManagedPtr_Members
                    // --------------------------------------

// CREATORS
inline
bdema_ManagedPtr_Members::bdema_ManagedPtr_Members()
: d_obj_p(0)
{
}

inline
bdema_ManagedPtr_Members::bdema_ManagedPtr_Members(void *object, 
                                                   void *factory, 
                                                   DeleterFunc deleter)
: d_obj_p(object)
, d_deleter(object, factory, deleter)
{
    BSLS_ASSERT_SAFE(0 != deleter || 0 == object);

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
    if(!object) {
        d_deleter.clear();
    }
#endif
}

// MANIPULATORS
inline
void bdema_ManagedPtr_Members::setAliasPtr(void *ptr)
{
    BSLS_ASSERT_SAFE(!ptr == !d_obj_p); // both are null, or neither is null
    d_obj_p = ptr;
}

inline
void bdema_ManagedPtr_Members::runDeleter() const
{
    if (d_obj_p) {
        d_deleter.deleteManagedObject();
    }
}

inline
void *bdema_ManagedPtr_Members::pointer() const
{
    return d_obj_p;
}

inline
const bdema_ManagedPtrDeleter& bdema_ManagedPtr_Members::deleter() const 
{
    return d_deleter;
}

                      // ----------------------------------
                      // private class bdema_ManagedPtr_Ref
                      // ----------------------------------

// CREATOR
template <class BDEMA_TYPE>
inline
bdema_ManagedPtr_Ref<BDEMA_TYPE>::bdema_ManagedPtr_Ref(
                                                bdema_ManagedPtr_Members *base)
: d_base_p(base)
{
    BSLS_ASSERT_SAFE(0 != base);
}

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
template <class BDEMA_TYPE>
inline
bdema_ManagedPtr_Ref<BDEMA_TYPE>::~bdema_ManagedPtr_Ref()
{
    BSLS_ASSERT_SAFE(0 != d_base_p);
}
#endif

// ACCESSORS
template <class BDEMA_TYPE>
inline
bdema_ManagedPtr_Members *bdema_ManagedPtr_Ref<BDEMA_TYPE>::base() const
{
    return d_base_p;
}

                    // --------------------------------------
                    // struct bdema_ManagedPtr_FactoryDeleter
                    // --------------------------------------

template <class BDEMA_TYPE, typename FACTORY>
inline
void bdema_ManagedPtr_FactoryDeleter<BDEMA_TYPE,FACTORY>::deleter(void *object,
                                                                 void *factory)
{
    BSLS_ASSERT_SAFE(0 != object);
    BSLS_ASSERT_SAFE(0 != factory);
    reinterpret_cast<FACTORY *>(factory)->deleteObject(
                                       reinterpret_cast<BDEMA_TYPE *>(object));
}

                    // ---------------------------------
                    // struct bdema_ManagedPtrNilDeleter
                    // ---------------------------------

template <class BDEMA_TYPE>
inline
void bdema_ManagedPtrNilDeleter<BDEMA_TYPE>::deleter(void *, void*)
{
}

}  // close namespace BloombergLP

#undef BDEMA_COMPATIBLE_POINTERS_ONLY
#undef BDEMA_VOID_FOR_ONLY_COMPATIPLE_POINTERS

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2005
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
