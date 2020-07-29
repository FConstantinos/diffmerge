/*
 * Copyright 2019-2020 Konstantinos Fragkiadakis
 * All rights reserved.
 *
 */

#include<iostream>
#include<fstream>
#include<string>
#include<exception>
#include<unordered_map>
#include<experimental/filesystem> // TODO: do away with "experimental" for C++17 and g++ 8+

#include "lcs.h"


using namespace std;
using namespace LCS;
namespace fs = experimental::filesystem;

// Checks a files exists in the filesystem
bool file_exists(const fs::path& p, fs::file_status s = fs::file_status{}) {
    if (fs::status_known(s) ? fs::exists(s) : fs::exists(p)) {
        return true;
    }
    else {
        cout << "diff: " << p << ": No such file or directory" << endl;
        return false;
    }
}

// Parses file to a vector of strings representing the lines of the file.
// On failure, a runtime_error exception is thrown that indicates the error.
// TODO: For very large files, buffering and/or caching should be considered
//       Here we simply get the whole file to memory.
// TODO: For restricted "alphabets" (i.e the lines of the file can only come
//       from a preset of lines) we can consider perfect hashing to integers,
//       whose required operations are cheaper and faster than that of
//       strings.
vector<string> parse_file(const string &filename) {
    ifstream strm;

    auto finalize = [&]() {
        try {
            if(strm.is_open()) {
                strm.close();
            }
        } catch(const exception &e) {
            throw runtime_error("Exception closing " + filename + ": " + e.what());
        }
    };

    // Throw exception on IO error (see here: http://www.cplusplus.com/reference/ios/ios/exceptions/)
    strm.exceptions ( ifstream::failbit | ifstream::badbit );
    try {
        strm.open(filename);
    } catch(const exception &e) {
        throw runtime_error("Exception opening file " + filename + ": " + e.what());
    }

    vector<string> seq;
    try {
        string line;

        // The peek for eof is required if we want to catch other exceptions. See here:
        // https://stackoverflow.com/questions/11807804/stdgetline-throwing-when-it-hits-eof
        while(strm.peek() != EOF && getline(strm, line)) {
            seq.push_back(move(line));
        }
    } catch(const exception &e) {
        finalize();
        throw runtime_error("Exception reading or parsing file " + filename + ": " + e.what());
    }
    finalize();
    return seq;
}

int main(int argc, char** argv) {

    if( argc != 3 ) {
        cout << "Usage: ./diff file1 file2" << endl;
        return 1;
    }

    // Arguably, using scoped enums and unordered_map for indexing
    // just two vectors here is overengineering, but it is
    // certainly safer and possibly cleaner, depending on one's
    // perspective.
    // TODO:
    // C++20 will allow this:
    // using enum ID;
    // to avoid having to type the scope.
    // Should be implemented if we move to C++20.
    // see here:
    // http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2019/p1099r4.html
    enum class ID{from, to}; // Indexes for the input files.
    const vector<ID> IDIter = { ID::from, ID::to};

    const unordered_map<ID, string> filename = {{ID::from, argv[1]}, {ID::to  , argv[2]}};
    unordered_map<ID, vector<string>> seq = {{ID::from, vector<string>()}, {ID::to  , vector<string>()} };
    unordered_map<ID, vector<size_t>> lcsIdx = {{ID::from, vector<size_t>()}, {ID::to  , vector<size_t>()} };

    // check the files exist:
    const bool from_exists = file_exists(filename.at(ID::from));
    const bool to_exists = file_exists(filename.at(ID::to));
    if(! ( from_exists && to_exists )) {
        return 1;
    }

    // return if it's the same file:
    if(filename.at(ID::from) == filename.at(ID::to)) {
        return 0;
    }

    // parse files
    try {
        for (const auto &id : {ID::from, ID::to}) {
           seq.at(id) = parse_file(filename.at(id));
        }
    } catch (const exception &e) {
        cerr << e.what() << endl;
        return 1;
    }

    // Get the LCS:
    tie(lcsIdx.at(ID::from), lcsIdx.at(ID::to)) = longestCommonSubsequence<string>(seq.at(ID::from), seq.at(ID::to));

    // Present the LCS into diff Normal Format. One specification can be found here:
    // https://www.gnu.org/software/diffutils/manual/diffutils.html#Detailed-Normal
    // TODO: We will only use "add" and "delete" operation. Using the "change"
    //       operation is left for future updates. In general, choosing the right
    //       hunks is an optimization problem. See also here:
    //       https://www.gnu.org/software/diffutils/manual/diffutils.html#Hunks
    // TODO: This needs to be abstracted away and tested independently. However, the
    //       above TODO should be tackled first, otherwise tests will have to change
    //       every time we update the specifications.

    const size_t lcsSize = lcsIdx.at(ID::from).size(); // size of the LCS
    unordered_map<ID, size_t> sdx = {{ID::from, 0}, {ID::to, 0}}; // indexes on the sequences
    unordered_map<ID, string> direction = {{ID::from, "< "}, {ID::to, "> "}};
    unordered_map<ID, string> op = {{ID::from, "d"}, {ID::to, "a"}};

    // This will print the operation for the lines between init.at(id) inclusive and end exclusive
    auto print = [&](const unordered_map<ID, size_t> &init, const size_t &end, const ID &id) {
        if (init.at(id) >= end) {
            return;
        }
        unordered_map<ID, string> sides = {{ID::from, to_string(init.at(ID::from))},
                                           {ID::to, to_string(init.at(ID::to))} };
        sides.at(id) = to_string(init.at(id) + 1);
        if(init.at(id) + 1 != end) {
            sides.at(id) += "," + to_string(end);
        }
        cout << sides.at(ID::from) << op.at(id) << sides.at(ID::to) << endl;
        for (size_t jdx = init.at(id); jdx < end; jdx++) {
            cout << direction.at(id) << seq.at(id).at(jdx) << endl;
        }
    }; // prints an addition or a deletion

    // output the results
    for(size_t ldx = 0; ldx < lcsSize; ldx++) {
        const unordered_map<ID, size_t> init = {{ID::from, sdx.at(ID::from)}, {ID::to, sdx.at(ID::to)}};

        // deletions
        ID id = ID::from;
        size_t end = lcsIdx.at(id).at(ldx);
        print(init, end, id);

        // additions
        id = ID::to;
        end = lcsIdx.at(id).at(ldx);
        print(init, end, id);

        sdx.at(ID::from) = lcsIdx.at(ID::from).at(ldx) + 1;
        sdx.at(ID::to) = lcsIdx.at(ID::to).at(ldx) + 1;
    }

    // last deletion:
    ID id = ID::from;
    size_t end = seq.at(id).size();
    print(sdx, end, id);

    // last addition:
    id = ID::to;
    end = seq.at(id).size();
    print(sdx, end, id);

    return 0;
}

