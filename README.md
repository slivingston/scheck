# scheck -- finite automata from LTL safety formulae

## Introduction

[scheck](http://tcs.legacy.ics.tkk.fi/users/tlatvala/scheck/) is a tool for translating safety LTL formulae to finite automata.
The algorithm used in the tool was presented in the paper:

[Timo Latvala](http://www.tcs.hut.fi/~timo/)
[Efficient Model Checking of Safety Properties.](https://spinroot.com/spin/symposia/ws03/Latvala.pdf)
In: T. Ball and S.K. Rajamani (eds.), Model Checking Software.
10th International SPIN Workshop. Volume 2648 of LNCS, pp. 74-88,
Springer, 2003.
DOI: [10.1007/3-540-44829-2_5](https://doi.org/10.1007/3-540-44829-2_5)

## Usage

Usage: scheck [options] {inputfile}. Without any options scheck reads the input
from the standard input outputs to the standard output a non-deterministic
automaton corresponding to the given safety formula.

### options
<table>
    <tr>
      <td>-o</td>
      <td>file</td>
      <td>specify outputfile</td>
    </tr>
    <tr>
      <td>-d</td>
      <td> </td>
      <td>produce a deterministic automaton</pd>
    </tr>
    <tr>
      <td>-s</td>
      <td> </td>
      <td>check for syntactic safety</td>
    </tr>
    <tr>
      <td>-p</td>
      <td>translator</td>
      <td>check if formula is pathologic</td>
    </tr>
    <tr>
      <td>-v</td>
      <td> </td>
      <td>output version and exit</td>
      </tr>
</table>

Checking if a formula is pathologic requires an external translator which
translates and LTL formula to a Büchi automaton. scheck expects the translator
to accept the scheck prefix notation for LTL formulae and return the result
in the automata format of scheck. One such tool is
[lbt](http://www.tcs.hut.fi/maria/tools/lbt). If you only have
a tool which accepts the format of the
[Spin model checker](https://spinroot.com) you can use a utility
by [Heikki Tauriainen](https://web.archive.org/web/20091001125942/http://www.tcs.hut.fi/~htauriai/) which
is distributed with his excellent LTL to Büchi translator testing tool
[lbtt](https://web.archive.org/web/20080605232012/http://www.tcs.hut.fi/Software/lbtt/).
scheck calls the external transator in the following way:
*translator inputfile outputfile*.

## Compiling scheck

scheck has been written using strict ANSI C++. It, however, uses some SGI
extensions to the Standard Template Library. For compilers supporting templates
and the SGI extensions to the STL compiling scheck should be as simple as typing 'make'.
Examples of such compilers are gcc, version 2.96 and version 3.3.

## Interfacing the Translator

The tool reads input from the standard input and outputs the result
to the standard output by default. Both the input and the output
format is a form of prefix notation. A precise description can
be found [here](http://www.tcs.hut.fi/Software/maria/tools/lbt/).

## Wrappers in other languages

* [readlbt.py](https://github.com/tulip-control/tulip-control/blob/master/contrib/readlbt.py) is available with [TuLiP](http://tulip-control.org/). It can load output from `scheck` into a NetworkX graph in Python.

## Version History

New in version 1.2

* Implemented the QM-algorithm to generate shorter expressions for the arcs
* Fixed a bug which could result in errors for the pathologic check

New in version 1.1.

* Eliminated BDDs from the code. The code uses bitvectors instead.
* Added a Hopcroft style minimisation as a last step when outputting deterministic automatons.
* scheck performs some simple rewriting of the formulas before it proceeds to create the finite automaton.

Version 1.0 -- orginal release.

## Copying

The tool is licenced under the
[GNU Public License](http://www.gnu.org/licenses/gpl.html).
See the file "COPYING" for details.


## Acknowledgements

[Keijo Heljanko](http://www.tcs.hut.fi/~kepa/) contributed with
valuable ideas for scheck. Some of the utility classes and functions were
originally written by [Marko Mäkelä](http://www.tcs.hut.fi/~msmakela/)
for [Maria](http://www.tcs.hut.fi/maria/).
