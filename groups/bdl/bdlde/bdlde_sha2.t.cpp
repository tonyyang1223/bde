// bdlde_sha2.t.cpp                                                   -*-C++-*-
#include <bdlde_sha2.h>

#include <bslim_testutil.h>

#include <bsls_assert.h>
#include <bsls_asserttest.h>
#include <bsls_types.h>

#include <bsl_algorithm.h>
#include <bsl_cstdlib.h>
#include <bsl_cstring.h>
#include <bsl_iostream.h>
#include <bsl_limits.h>
#include <bsl_vector.h>

using namespace BloombergLP;
using namespace bsl;

//=============================================================================
//                                  TEST PLAN
//-----------------------------------------------------------------------------
//                                  Overview
//                                  --------
// The various SHA hash types are mechanisms that are used to provide
// cryptographic hashes.  The primary concern is to ensure that the correct
// hashes are calculated.
//-----------------------------------------------------------------------------
// [ 2] Sha224::Sha224();
// [ 2] void Sha224::update(const void *message, bsl::size_t length);
// [ 6] void Sha224::loadDigest(unsigned char* digest);
// [ 3] Sha256::Sha256();
// [ 3] void Sha256::update(const void *message, bsl::size_t length);;
// [ 7] void Sha256::loadDigest(unsigned char* digest);
// [ 4] Sha384::Sha384();
// [ 4] void Sha384::update(const void *message, bsl::size_t length);;
// [ 8] void Sha384::loadDigest(unsigned char* digest);
// [ 5] Sha512::Sha512();
// [ 5] void Sha512::update(const void *message, bsl::size_t length);;
// [ 9] void Sha512::loadDigest(unsigned char* digest);
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST

// ============================================================================
//                     STANDARD BDE ASSERT TEST FUNCTION
// ----------------------------------------------------------------------------

namespace {

int testStatus = 0;

void aSsErT(bool condition, const char *message, int line)
{
    if (condition) {
        cout << "Error " __FILE__ "(" << line << "): " << message
             << "    (failed)" << endl;

        if (0 <= testStatus && testStatus <= 100) {
            ++testStatus;
        }
    }
}

}  // close unnamed namespace

// ============================================================================
//               STANDARD BDE TEST DRIVER MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT       BSLIM_TESTUTIL_ASSERT
#define ASSERTV      BSLIM_TESTUTIL_ASSERTV

#define LOOP_ASSERT  BSLIM_TESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BSLIM_TESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BSLIM_TESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BSLIM_TESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLIM_TESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLIM_TESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLIM_TESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLIM_TESTUTIL_LOOP6_ASSERT

#define Q            BSLIM_TESTUTIL_Q   // Quote identifier literally.
#define P            BSLIM_TESTUTIL_P   // Print identifier and value.
#define P_           BSLIM_TESTUTIL_P_  // P(X) without '\n'.
#define T_           BSLIM_TESTUTIL_T_  // Print a tab (w/o newline).
#define L_           BSLIM_TESTUTIL_L_  // current Line number

// ============================================================================
//                  NEGATIVE-TEST MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT_SAFE_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(EXPR)
#define ASSERT_SAFE_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(EXPR)
#define ASSERT_PASS(EXPR)      BSLS_ASSERTTEST_ASSERT_PASS(EXPR)
#define ASSERT_FAIL(EXPR)      BSLS_ASSERTTEST_ASSERT_FAIL(EXPR)
#define ASSERT_OPT_PASS(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_PASS(EXPR)
#define ASSERT_OPT_FAIL(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_FAIL(EXPR)

//=============================================================================
//                               GLOBAL TYPEDEF
//-----------------------------------------------------------------------------

//=============================================================================
//                   HELPER CLASSES AND FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

namespace {

template<bsl::size_t SIZE>
void toHex(bsl::string *output, const unsigned char (&input)[SIZE])
    // Store into the specified 'output' the hex representation of the bytes in
    // the specified 'input'.
{
    const char *hexTable = "0123456789abcdef";
    output->clear();
    for (bsl::size_t index = 0; index != SIZE; ++index) {
        const unsigned char byte = input[index];
        output->push_back(hexTable[byte / 16]);
        output->push_back(hexTable[byte % 16]);
    }
}

template<class VALUE, bsl::size_t SIZE>
bsl::size_t arraySize(const VALUE (&)[SIZE])
    // Return the size of the array parameter.
{
    return SIZE;
}

template<class HASHER>
void testKnownHashes(const char *const (&expected)[6])
    // Hash certain known messages using an instance of 'HASHER', and verify
    // that the results match the results in the specified 'expected'.
{
    const bsl::string repeat_a(1000000, 'a');
    bsl::string       all_characters;
    for (int c = -128; c != 128; ++c)
    {
        all_characters.push_back(static_cast<char>(c));
    }

    const bsl::string messages[6] =
    {
        "",
        "abc",
        "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq",
        "abcdefghbcdefghicdefghijdefghijkefghijklfghijklmghijklmnhijklmnoijklm"
            "nopjklmnopqklmnopqrlmnopqrsmnopqrstnopqrstu",
        repeat_a,
        all_characters
    };

    unsigned char digest[HASHER::k_DIGEST_SIZE];
    bsl::string   hexDigest;
    for (bsl::size_t n = 0; n != arraySize(messages); ++n)
    {
        const char *message = messages[n].c_str();
        HASHER      hasher;
        hasher.update(message, messages[n].size());
        hasher.loadDigest(digest);

        toHex(&hexDigest, digest);
        ASSERT(hexDigest == expected[n]);
    }
}

template<class HASHER>
void testIncremental()
    // Verify that an instance of the specified 'HASHER' produces the same hash
    // when hashing an entire message as it does when hashing the message in
    // pieces.
{
    const char       *in         = "abcdef";
    const bsl::size_t size       = bsl::strlen(in);
    const bsl::size_t digestSize = HASHER::k_DIGEST_SIZE;
    unsigned char     digest1[digestSize];
    unsigned char     digest2[digestSize];

    HASHER completeHasher;
    completeHasher.update(in, size);
    completeHasher.loadDigest(digest1);

    for (bsl::size_t stride = 1; stride <= 3; ++stride) {
        ASSERT(size % stride == 0);
        HASHER incrementalHasher;
        for (bsl::size_t index = 0; index != size; index += stride) {
            ASSERT(completeHasher != incrementalHasher);
            incrementalHasher.update(in + index, stride);
        }
        ASSERT(completeHasher == incrementalHasher);
        incrementalHasher.loadDigest(digest2);
        ASSERT(completeHasher == incrementalHasher);

        ASSERT(bsl::equal(digest1, digest1 + digestSize, digest2));
    }
}

}  // close unnamed namespace

//=============================================================================
//                                MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int        test = argc > 1 ? bsl::atoi(argv[1]) : 0;
    int     verbose = argc > 2;

    cout << "TEST " << __FILE__ << " CASE " << test << '\n';

    switch (test) { case 0:
      case 10: {
        // --------------------------------------------------------------------
        // TESTING USAGE EXAMPLE
        //   This will test the usage example provided in the component header
        //   file.
        //
        // Concerns:
        //   The usage example provided in the component header file must
        //   compile, link, and run on all platforms as shown.
        //
        // Plan:
        //   Run the usage example functions 'validatePassword'.
        //
        // Testing:
        //   Usage example.
        // --------------------------------------------------------------------

        if (verbose) cout << "Testing Usage Example" "\n"
                          << "=====================" "\n";


        // Prepare a message.
        const bsl::string message = "password";

        // Generate a digest for 'message'.
        bdlde::Sha512 hasher;
        hasher.update(message.data(), message.length());
        unsigned char       digest[bdlde::Sha512::k_DIGEST_SIZE];
        const unsigned char expected[bdlde::Sha512::k_DIGEST_SIZE] = {
            0xB1, 0x09, 0xF3, 0xBB, 0xBC, 0x24, 0x4E, 0xB8, 0x24, 0x41, 0x91,
            0x7E, 0xD0, 0x6D, 0x61, 0x8B, 0x90, 0x08, 0xDD, 0x09, 0xB3, 0xBE,
            0xFD, 0x1B, 0x5E, 0x07, 0x39, 0x4C, 0x70, 0x6A, 0x8B, 0xB9, 0x80,
            0xB1, 0xD7, 0x78, 0x5E, 0x59, 0x76, 0xEC, 0x04, 0x9B, 0x46, 0xDF,
            0x5F, 0x13, 0x26, 0xAF, 0x5A, 0x2E, 0xA6, 0xD1, 0x03, 0xFD, 0x07,
            0xC9, 0x53, 0x85, 0xFF, 0xAB, 0x0C, 0xAC, 0xBC, 0x86
        };
        hasher.loadDigest(digest);
        ASSERT(bsl::equal(digest,
                          digest + bdlde::Sha512::k_DIGEST_SIZE,
                          expected));
      } break;
      case 9: {
        // --------------------------------------------------------------------
        // KNOWN HASHES FOR SHA-512
        //   Ensure that values hash to their expected hashes
        //
        // Concerns:
        //: 1 Hashing a particular value provides the expected hash output when
        //:   using either the class interface or the function interface.
        //
        // Plan:
        //: 1 Verify a range of values with varying sizes and bytes against
        //:   known values. These known values came from running the Python
        //:   implementation of the SHA-2 algorithms.
        //
        // Testing:
        //   void Sha512::loadDigest(unsigned char *digest);
        // --------------------------------------------------------------------
        if (verbose) cout << "KNOWN HASHES FOR SHA-512" "\n"
                             "========================" "\n";
        const char *const results[6] =
        {
            "cf83e1357eefb8bdf1542850d66d8007d620e4050b5715dc83f4a921d36ce9ce"
            "47d0d13c5d85f2b0ff8318d2877eec2f63b931bd47417a81a538327af927da3e",
            "ddaf35a193617abacc417349ae20413112e6fa4e89a97ea20a9eeee64b55d39a"
            "2192992a274fc1a836ba3c23a3feebbd454d4423643ce80e2a9ac94fa54ca49f",
            "204a8fc6dda82f0a0ced7beb8e08a41657c16ef468b228a8279be331a703c335"
            "96fd15c13b1b07f9aa1d3bea57789ca031ad85c7a71dd70354ec631238ca3445",
            "8e959b75dae313da8cf4f72814fc143f8f7779c6eb9f7fa17299aeadb6889018"
            "501d289e4900f7e4331b99dec4b5433ac7d329eeb6dd26545e96e55b874be909",
            "e718483d0ce769644e2e42c7bc15b4638e1f98b13b2044285632a803afa973eb"
            "de0ff244877ea60a4cb0432ce577c31beb009c5c2c49aa2e4eadb217ad8cc09b",
            "f91a8584486a5f167ca103e390444e52fd294e10d43af7bd94402876954ae9b1"
            "d0ec65ab9aaf47a7ab7f8733a8d111c038ff78d1238e3aa32b58e9b63767f7d3"
        };
        testKnownHashes<bdlde::Sha512>(results);
      } break;
      case 8: {
        // --------------------------------------------------------------------
        // KNOWN HASHES FOR SHA-384
        //   Ensure that values hash to their expected hashes
        //
        // Concerns:
        //: 1 Hashing a particular value provides the expected hash output when
        //:   known values. These known values came from running the Python
        //:   implementation of the SHA-2 algorithms.
        //
        // Plan:
        //: 1 Verify a range of values with varying sizes and bytes against
        //:   known values.
        //
        // Testing:
        //   void Sha384::loadDigest(unsigned char *digest);
        // --------------------------------------------------------------------
        if (verbose) cout << "KNOWN HASHES FOR SHA-384" "\n"
                             "========================" "\n";
        const char *const results[6] =
        {
            "38b060a751ac96384cd9327eb1b1e36a21fdb71114be0743"
            "4c0cc7bf63f6e1da274edebfe76f65fbd51ad2f14898b95b",
            "cb00753f45a35e8bb5a03d699ac65007272c32ab0eded163"
            "1a8b605a43ff5bed8086072ba1e7cc2358baeca134c825a7",
            "3391fdddfc8dc7393707a65b1b4709397cf8b1d162af05ab"
            "fe8f450de5f36bc6b0455a8520bc4e6f5fe95b1fe3c8452b",
            "09330c33f71147e83d192fc782cd1b4753111b173b3b05d2"
            "2fa08086e3b0f712fcc7c71a557e2db966c3e9fa91746039",
            "9d0e1809716474cb086e834e310a4a1ced149e9c00f24852"
            "7972cec5704c2a5b07b8b3dc38ecc4ebae97ddd87f3d8985",
            "a7902aa7f28885d54c4dadbff0f721cd5532b1e56e6f7a4b"
            "b2baad0229e576da5902c1bf0cc809fa3efa6e6476e62696"
        };
        testKnownHashes<bdlde::Sha384>(results);
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // KNOWN HASHES FOR SHA-256
        //   Ensure that values hash to their expected hashes
        //
        // Concerns:
        //: 1 Hashing a particular value provides the expected hash output when
        //:   known values. These known values came from running the Python
        //:   implementation of the SHA-2 algorithms.
        //
        // Plan:
        //: 1 Verify a range of values with varying sizes and bytes against
        //:   known values.
        //
        // Testing:
        //   void Sha256::loadDigest(unsigned char *digest);
        // --------------------------------------------------------------------
        if (verbose) cout << "KNOWN HASHES FOR SHA-256" "\n"
                             "========================" "\n";
        const char *const results[6] =
        {
            "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855",
            "ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad",
            "248d6a61d20638b8e5c026930c3e6039a33ce45964ff2167f6ecedd419db06c1",
            "cf5b16a778af8380036ce59e7b0492370b249b11e8f07a51afac45037afee9d1",
            "cdc76e5c9914fb9281a1c7e284d73e67f1809a48a497200e046d39ccc7112cd0",
            "2bae3a9530e35152c19d73f13f6c0e22cb92f22ce8aa895796711f52b8f7f516"
        };
        testKnownHashes<bdlde::Sha256>(results);
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // KNOWN HASHES FOR SHA-224
        //   Ensure that values hash to their expected hashes
        //
        // Concerns:
        //: 1 Hashing a particular value provides the expected hash output when
        //:   known values. These known values came from running the Python
        //:   implementation of the SHA-2 algorithms.
        //
        // Plan:
        //: 1 Verify a range of values with varying sizes and bytes against
        //:   known values.
        //
        // Testing:
        //   void Sha224::loadDigest(unsigned char *digest);
        // --------------------------------------------------------------------
        if (verbose) cout << "KNOWN HASHES FOR SHA-224" "\n"
                             "========================" "\n";
        const char *const results[6] =
        {
            "d14a028c2a3a2bc9476102bb288234c415a2b01f828ea62ac5b3e42f",
            "23097d223405d8228642a477bda255b32aadbce4bda0b3f7e36c9da7",
            "75388b16512776cc5dba5da1fd890150b0c6455cb4f58b1952522525",
            "c97ca9a559850ce97a04a96def6d99a9e0e0e2ab14e6b8df265fc0b3",
            "20794655980c91d8bbb4c1ea97618a4bf03f42581948b2ee4ee7ad67",
            "5dfb7c35156ef5385de415bd1e04379c2fe5b7a002484d73730d4661"
        };

        testKnownHashes<bdlde::Sha224>(results);
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // INCREMENTAL UPDATES FOR SHA-512
        //   Test the update function
        //
        // Concerns:
        //: 1 Ensure that providing data one byte at a time is the same as
        //:   providing it all at once.
        //
        // Plan:
        //: 1 Pass each byte of a string to update and compare the result of
        //    passing the entire string to update.
        //
        // Testing:
        //   Sha512::Sha512();
        //   void Sha512::update(const void *message, bsl::size_t length);
        // --------------------------------------------------------------------
        if (verbose) cout << "INCREMENTAL UPDATES FOR SHA-512" "\n"
                             "===============================" "\n";
        testIncremental<bdlde::Sha512>();
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // INCREMENTAL UPDATES FOR SHA-384
        //   Test the update function
        //
        // Concerns:
        //: 1 Ensure that providing data one byte at a time is the same as
        //:   providing it all at once.
        //
        // Plan:
        //: 1 Pass each byte of a string to update and compare the result of
        //    passing the entire string to update.
        //
        // Testing:
        //   Sha384::Sha384();
        //   void Sha384::update(const void *message, bsl::size_t length);
        // --------------------------------------------------------------------
        if (verbose) cout << "INCREMENTAL UPDATES FOR SHA-384" "\n"
                             "===============================" "\n";
        testIncremental<bdlde::Sha384>();
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // INCREMENTAL UPDATES FOR SHA-256
        //   Test the update function
        //
        // Concerns:
        //: 1 Ensure that providing data one byte at a time is the same as
        //:   providing it all at once.
        //
        // Plan:
        //: 1 Pass each byte of a string to update and compare the result of
        //    passing the entire string to update.
        //
        // Testing:
        //   Sha256::Sha256();
        //   void Sha256::update(const void *message, bsl::size_t length);
        // --------------------------------------------------------------------
        if (verbose) cout << "INCREMENTAL UPDATES FOR SHA-256" "\n"
                             "===============================" "\n";
        testIncremental<bdlde::Sha256>();
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // INCREMENTAL UPDATES FOR SHA-224
        //   Test the update function
        //
        // Concerns:
        //: 1 Ensure that providing data one byte at a time is the same as
        //:   providing it all at once.
        //
        // Plan:
        //: 1 Pass each byte of a string to update and compare the result of
        //    passing the entire string to update.
        //
        // Testing:
        //   Sha224::Sha224();
        //   void Sha224::update(const void *message, bsl::size_t length);
        // --------------------------------------------------------------------
        if (verbose) cout << "INCREMENTAL UPDATES FOR SHA-224" "\n"
                             "===============================" "\n";
        testIncremental<bdlde::Sha224>();
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //   This case exercises (but does not fully test) basic functionality.
        //
        // Concerns:
        //: 1 The class is sufficiently functional to enable comprehensive
        //:   testing in subsequent test cases.
        //
        // Plan:
        //: 1 Verify default constructed hashes equal themselves.
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) cout << "\nBREATHING TEST"
                             "\n==============\n";
        {
            bdlde::Sha224 hasher;
            ASSERT(hasher == hasher);
        }
        {
            bdlde::Sha256 hasher;
            ASSERT(hasher == hasher);
        }
        {
            bdlde::Sha384 hasher;
            ASSERT(hasher == hasher);
        }
        {
            bdlde::Sha512 hasher;
            ASSERT(hasher == hasher);
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
// Copyright 2018 Bloomberg Finance L.P.
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
