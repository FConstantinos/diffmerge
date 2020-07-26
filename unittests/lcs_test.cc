/*
 * Copyright 2019-2020 Konstantinos Fragkiadakis
 * All rights reserved.
 *
 */

// TODO: This framework lacks performance tests. It would be desirable to add them.
#include <algorithm>
#include <iostream>
#include <cassert>
#include <gtest/gtest.h>

#include "lcs.h"

using namespace std;
using namespace LCS;

typedef int token;

struct TestCase {
    vector<token> arr1;     // first array of tokens
    vector<token> arr2;     // second array of tokens
    const size_t LCSLength; // the length of LCS between arr1 and arr2
    vector<size_t> idx1;    // first return value of the LCS algorithm
    vector<size_t> idx2;    // second return value of the LCS algorithm
};

class lcsTestFixture : public ::testing::TestWithParam<TestCase> {};

TEST_P(lcsTestFixture, lcsTest) {
    TestCase tcase = GetParam();

    auto testLCS = [&]() {
        auto runAsserts = [&]() {
            // test sizes:
            const size_t lcsSize = tcase.idx1.size();
            ASSERT_EQ(lcsSize, tcase.idx2.size());
            ASSERT_EQ(lcsSize, tcase.LCSLength);

            // test for increasing sequences:
            for(size_t idx = 1; idx < lcsSize; idx++) {
                ASSERT_GT(tcase.idx1.at(idx), tcase.idx1.at(idx-1));
                ASSERT_GT(tcase.idx2.at(idx), tcase.idx2.at(idx-1));
            }

            // test equality of elements
            for(size_t idx = 0; idx < lcsSize; idx++) {
                ASSERT_EQ(tcase.arr1.at(tcase.idx1.at(idx)), tcase.arr2.at(tcase.idx2.at(idx)));
            }
        };

        // Check calls with both original and reverse argument order.
        // One could argue that it would suffice to check lcs == revLcs.
        // However, this need not be the case; there are multiple LCSs
        // and the reverse argument order might return, in principle, a
        // different LCS than the original, depending on the algorithm
        // used.
        tie(tcase.idx1, tcase.idx2) = longestCommonSubsequence<token>(tcase.arr1, tcase.arr2);
        runAsserts();
        tie(tcase.idx2, tcase.idx1) = longestCommonSubsequence<token>(tcase.arr2, tcase.arr1);
        runAsserts();
    };

    testLCS();

    // Also try reversing the arrays:
    reverse(tcase.arr1.begin(), tcase.arr1.end());
    reverse(tcase.arr2.begin(), tcase.arr2.end());
    testLCS();
}

vector<TestCase> testCases = {
        {{}           , {}               , 0}, // all empty
        {{}           , {1}              , 0}, // one non-empty
        {{}           , {1,1,1}          , 0},
        {{0}          , {0}              , 1}, // exactly the same
        {{1,2,3}      , {1,2,3}          , 3},
        {{1,2,3}      , {1,2,3,4}        , 3}, // one is prefix to the other
        {{0,1,2,3}    , {1,2,3}          , 3}, // one is suffix to the other
        {{1,2,3}      , {4,5,6,7}        , 0}, // completely different
        {{0,1,2}      , {1,2,3}          , 2}, // different
        {{0,1,2,4,5}  , {1,2,3,6,7,4,5,8}, 4},
        {{1,4,5,2,3,6}, {1,2,4,5,3,6}    , 5},
};

INSTANTIATE_TEST_CASE_P(
    lcsTest,
    lcsTestFixture,
    ::testing::ValuesIn(testCases)
);

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
