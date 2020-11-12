isto::test
==========

A ``C++20`` headers-only library.

Aims at being to ``std::array`` what ``std::valarray`` is to ``srtd::vector``.

Requirements
------------

- A ``C++20`` capable compiler.

To run the tests:

- `GNU Make <https://www.gnu.org/software/make/>`_.
- The `onqtam/doctest library <https://github.com/onqtam/doctest>`_.

Installation
------------

This is a headers-only library. Just put it where your compiler can find it.

Tests
-----

The tests require the `onqtam/doctest library`_.
Edit the ``config.mk`` file to make the ``DOCTEST_HEADERS`` variable point to 
the directory containing ``doctest/doctest.h``. 

To execute the tests run

    $ make check

in the root directory of the project.


License
-------

SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
