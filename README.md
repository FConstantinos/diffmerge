# diffmerge
A diff and a diff3 toolset I develop for my own education/enjoyment. It aims to be a stripped down version of the standard diff and diff3 commands on Linux.

**Currently, only a diff tool is implemented**. A library that provides an algorithm to compute the Longest Common Subsequence between two sequences is also created (liblcs.so). 

# Requirements:
 * Linux programming environment
 * cmake (>=3.10)
 * goolgetest
   * only if intending to compile and run the tests
   
# Build and Installation:
 * Clone this repository to your local machine
 * cd /your/clone/directory
 * mkdir build && cd build
 * cmake -DCMAKE_INSTALL_PREFIX=/your/install/directory
 * make all install
 * you can now find the diff executable under /your/install/directory/bin/
 * you can now find the liblcs.so library under /your/install/directory/lib/
 * you can now find the lcs.h header under /your/install/directory/include

If you also want to build the tests, in the cmake step above you can add -DENABLE_TESTS=ON. You can find an executable that runs the tests under /your/clone/repository/build/unittests. Currently, only liblcs.so is tested.

# Usage:
To use:

````
diff A B
````
where A and B are the files to apply the diff to.

Currently, the only output format supported is the diff Normal Format. One specification can be found here:
Present the LCS into diff Normal Format. One specification can be found here:
    // https://www.gnu.org/software/diffutils/manual/diffutils.html#Detailed-Normal

Currently, we only use the "add" and "delete" operations of the Normal Format which are necessary and sufficient to describe any diff. Using the "change" operation is left for future updates.


# Examples:
Consider the files:

A.txt:
````
This part of the
document has stayed the
same from version to
version.  It shouldn't
be shown if it doesn't
change.  Otherwise, that
would not be helping to
compress the size of the
changes.

This paragraph contains
text that is outdated.
It will be deleted in the
near future.

It is important to spell
check this dokument. On
the other hand, a
misspelled word isn't
the end of the world.
Nothing in the rest of
this paragraph needs to
be changed. Things can
be added after it.
````

B.txt:
````
This is an important
notice! It should
therefore be located at
the beginning of this
document!

This part of the
document has stayed the
same from version to
version.  It shouldn't
be shown if it doesn't
change.  Otherwise, that
would not be helping to
compress the size of the
changes.

It is important to spell
check this document. On
the other hand, a
misspelled word isn't
the end of the world.
Nothing in the rest of
this paragraph needs to
be changed. Things can
be added after it.

This paragraph contains
important new additions
to this document.
````

Using the diff in this project, we get this result:

````
diff A.txt B.txt
0a1,6
> This is an important
> notice! It should
> therefore be located at
> the beginning of this
> document!
> 
10,14d15
< 
< This paragraph contains
< text that is outdated.
< It will be deleted in the
< near future.
17d17
< check this dokument. On
16a18
> check this document. On
24a26,29
> 
> This paragraph contains
> important new additions
> to this document.
````
Do notice that the output is not exactly the same as that of the linux diff. Using the linux diff, we get this:
 
````
0a1,6
> This is an important
> notice! It should
> therefore be located at
> the beginning of this
> document!
> 
11,15d16
< This paragraph contains
< text that is outdated.
< It will be deleted in the
< near future.
< 
17c18
< check this dokument. On
---
> check this document. On
24a26,29
> 
> This paragraph contains
> important new additions
> to this document.
````
However, both outputs are valid. In the general case, there are multiple valid diff results; this occurs because there are multiple ways an old version of a file could have been modified to a new version.

# TODO:
 * Add a diff3 executable.
 * Set a specification on when to consider hunk to be a "change" operation of the Normal Format instead of a pair of "add" and "delete".
 * Test the frontend (IO operations before computing the LCS)
 * Test the backend (liblcs.so) with performance tests
