/*
 * Copyright 2019-2020 Konstantinos Fragkiadakis
 * All rights reserved.
 *
 */

#pragma once

#include <vector>
#include <algorithm>
#include <numeric>

namespace LCS {

// This implements the most common algorithm for LCS. Time and space complexity are both O(N*M),
// where N = A.size() and M = B.size(). Returns two sorted vectors x,y of equal size s where
// A[x[i]] = B[y[i]] for all i
// TODO: There are better algorithms that reduce both time and space complexity, at least on
//       average, see:
//       https://en.wikipedia.org/wiki/Hirschberg%27s_algorithm
//       https://en.wikipedia.org/wiki/Hunt%E2%80%93Szymanski_algorithm
//       http://www.xmailserver.org/diff2.pdf

template<typename token>
std::tuple<std::vector<size_t>, std::vector<size_t>>
longestCommonSubsequence(const std::vector<token> &A, const std::vector<token> &B) {

    if(A.empty() || B.empty()) {
        return {};
    }
    std::vector<size_t> x;
    std::vector<size_t> y;
    std::vector<std::vector<size_t>> LCSTable;

    // Identify common prefixes and suffixes to reduce size of computation:
    size_t start;
    size_t minSize = std::min({A.size(), B.size()});
    for(start = 0; (start < minSize) && (A.at(start) == B.at(start)); start++);
    if (start == minSize) { // one is the prefix of the other
        x.resize(minSize);
        y.resize(minSize);
        std::iota(x.begin(), x.end(), 0);
        std::iota(y.begin(), y.end(), 0);
        return {x, y};
    }
    size_t end = 0;
    for(end = 0; (end < std::min({A.size(), B.size()})) && (A.at(A.size() - 1 - end) == B.at(B.size() - 1 - end)); end++);
    size_t Aend = A.size() - end;
    size_t Bend = B.size() - end;

    // We add one more row/column than actually needed. This is only to simplify the
    // code. This can be avoided with index arithmetic. The space complexity remains
    // O(N*M) regardless.
    size_t Asize = Aend - start;
    size_t Bsize = Bend - start;
    LCSTable.resize(Asize+1);
    for (auto &row : LCSTable) {
        row.resize(Bsize+1, 0);
    }

    // Fill the LCS table:
    for (size_t idx = 1; idx <= Asize; idx++) {
        for(size_t jdx = 1; jdx <= Bsize; jdx++) {
            if (A.at(start + idx-1) == B.at(start + jdx-1)) {
                LCSTable.at(idx).at(jdx) = 1 + LCSTable.at(idx-1).at(jdx-1);
            } else {
                LCSTable.at(idx).at(jdx) = std::max({LCSTable.at(idx-1).at(jdx),LCSTable.at(idx).at(jdx-1)});
            }
        }
    }

    // Find the LCS:
    size_t idx = Asize;
    size_t jdx = Bsize;
    const size_t LCSsize = LCSTable.at(Asize).at(Bsize);
    const size_t retSize = start + LCSsize + end;
    x.resize(retSize);
    y.resize(retSize);
    size_t kdx = start + LCSsize - 1;
    while(idx && jdx) {
        if (A.at(start + idx - 1)==B.at(start + jdx - 1)) {
            x.at(kdx) = start + idx - 1;
            y.at(kdx) = start + jdx - 1;
            idx--;
            jdx--;
            kdx--;
        } else if (LCSTable.at(idx-1).at(jdx) >= LCSTable.at(idx).at(jdx-1)){
            idx--;
        } else {
            jdx--;
        }
    }

    // pad the common prefix:
    for (idx = 0; idx < start; idx++) {
        x.at(idx) = idx;
        y.at(idx) = idx;
    }
    //pad the common suffix:
    for (idx = 0; idx < end; idx++ ) {
        x.at(start + LCSsize + idx) = Aend + idx;
        y.at(start + LCSsize + idx) = Bend + idx;
    }
    return {x,y};
}
} // LCS
