// {{{ GPL License 

// This file is part of gringo - a grounder for logic programs.
// Copyright (C) 2013  Roland Kaminski

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

// }}}

#include "gringo/ground/program.hh"
#include "gringo/input/nongroundparser.hh"
#include "gringo/input/program.hh"
#include "gringo/output/output.hh"
#include "gringo/scripts.hh"

#include "tests/tests.hh"
#include "tests/gringo_module.hh"

namespace Gringo { namespace Ground { namespace Test {

// {{{ declaration of TestProgram

class TestProgram : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(TestProgram);
        CPPUNIT_TEST(test_toGround);
        CPPUNIT_TEST(test_analyze);
    CPPUNIT_TEST_SUITE_END();

public:
    virtual void setUp();
    virtual void tearDown();

    void test_toGround();
    void test_analyze();

    virtual ~TestProgram();
};

// }}}

using namespace Gringo::IO;

// {{{ definition of auxiliary functions

namespace {

typedef std::string S;

Program parse(std::string const &str) {
    std::ostringstream oss;
    Output::OutputBase out({}, oss);
    Input::Program prg;
    Defines defs;
    Scripts scripts(Gringo::Test::getTestModule());
    Input::NongroundProgramBuilder pb{ scripts, prg, out, defs };
    Input::NonGroundParser ngp{ pb };
    ngp.pushStream("-", gringo_make_unique<std::stringstream>(str));
    ngp.parse();
    prg.rewrite(defs);
    return prg.toGround(out.domains);
}

std::string toString(Program const &p) {
    std::string str = to_string(p);
    replace_all(str, ",#inc_base", "");
    replace_all(str, ":-#inc_base.", ".");
    replace_all(str, ":-#inc_base,", ":-");
    replace_all(str, ":-#inc_base;", ":-");
    return str;
}

} // namespace

// }}}
// {{{ definition of TestProgram

void TestProgram::setUp() {
}

void TestProgram::tearDown() {
}

void TestProgram::test_toGround() {
    Gringo::Test::Messages msg;
    CPPUNIT_ASSERT_EQUAL(S(
        "% component\n"
        "#external."),
        toString(parse("p(1;2).")));
    CPPUNIT_ASSERT_EQUAL(S(
        "% component\n#external.\n"
        "% positive component\n"
        "p(X):-q(X)."),
        toString(parse("p(X):-q(X).")));
    CPPUNIT_ASSERT_EQUAL(S(
        "% component\n#external.\n"
        "% positive component\n"
        "p(#Range0):-#Range0=1..2."),
        toString(parse("p(1..2).")));
    CPPUNIT_ASSERT_EQUAL(S(
        "% component\n#external.\n"
        "% positive component\n"
        "p(X):-X=1."),
        toString(parse("p(X):-X=1.")));
    CPPUNIT_ASSERT_EQUAL(S(
        "% component\n#external.\n"
        "% component\n"
        "#false:-0=0."),
        toString(parse(":-.")));
    CPPUNIT_ASSERT_EQUAL(S(
        "% component\n#external.\n"
        "% component\n"
        "#false:-not not p."),
        toString(parse("not p.")));
    CPPUNIT_ASSERT_EQUAL(S(
        "% component\n#external.\n"
        "% positive component\n"
        "p(X,Y):-p(Y),p(X),X=Y,Y=X."),
        toString(parse("p(X,Y):-X=Y,p(X),p(Y).")));
    CPPUNIT_ASSERT_EQUAL(S(
        "% component\n"
        "#external.\n"
        "% positive component\n"
        "#accu(#d0(Z,X,Y),tuple(#special)):-:-p(X,Y,Z),0>Z.\n"
        "% positive component\n"
        "#accu(#d0(Z,X,Y),tuple(A)):-q(A),r(A,X):-p(X,Y,Z).\n"
        "% positive component\n"
        "#accu(#d0(Z,X,Y),tuple(B,Y)):-a(B,Y):-p(X,Y,Z).\n"
        "% positive component\n"
        "#d0(Z,X,Y):-#accu(#d0(Z,X,Y),tuple(#special)),#accu(#d0(Z,X,Y),tuple(A)),#accu(#d0(Z,X,Y),tuple(B,Y)).\n"
        "% positive component\n"
        "x:-p(X,Y,Z),Z<#count{#d0(Z,X,Y)}."),
        toString(parse("x:-p(X,Y,Z),Z<#count{A:q(A),r(A,X);B,Y:a(B,Y)}.")));
    CPPUNIT_ASSERT_EQUAL(S(
        "% component\n"
        "#external.\n"
        "% positive component\n"
        "#accu(#d0(Z,X,Y),tuple(#special)):-:-p(X,Y,Z),0=Z.\n"
        "% positive component\n"
        "#accu(#d0(Z,X,Y),tuple(A)):-q(A),r(A,X):-p(X,Y,Z).\n"
        "% positive component\n"
        "#accu(#d0(Z,X,Y),tuple(B,Y)):-a(B,Y):-p(X,Y,Z).\n"
        "% positive component\n"
        "#d0(Z,X,Y):-#accu(#d0(Z,X,Y),tuple(#special)),#accu(#d0(Z,X,Y),tuple(A)),#accu(#d0(Z,X,Y),tuple(B,Y)).\n"
        "% positive component\n"
        "x:-p(X,Y,Z),Z=#count{#d0(Z,X,Y)}."),
        toString(parse("x:-p(X,Y,Z),Z=#count{A:q(A),r(A,X);B,Y:a(B,Y)}.")));
    CPPUNIT_ASSERT_EQUAL(S(
        "% component\n"
        "#external.\n"
        "% positive component\n"
        "#accu(#d0(X,Y,ZZ),tuple(#special)):-:-p(X,Y,Z).\n"
        "% positive component\n"
        "#accu(#d0(X,Y,ZZ),tuple(A)):-q(A),r(A,X):-p(X,Y,Z).\n"
        "% positive component\n"
        "#accu(#d0(X,Y,ZZ),tuple(B,Y)):-a(B,Y):-p(X,Y,Z).\n"
        "% positive component\n"
        "#d0(X,Y,ZZ):-#accu(#d0(X,Y,ZZ),tuple(#special));#accu(#d0(X,Y,ZZ),tuple(A));#accu(#d0(X,Y,ZZ),tuple(B,Y)).\n"
        "% positive component\n"
        "x:-p(X,Y,Z),ZZ=#count{#d0(X,Y,ZZ)}."),
        toString(parse("x:-p(X,Y,Z),ZZ=#count{A:q(A),r(A,X);B,Y:a(B,Y)}.")));
    CPPUNIT_ASSERT_EQUAL(S(
        "% component\n"
        "#external.\n"
        "% component\n"
        "Z<#count(#d0(Z,X,Y)):-p(X,Y,Z).\n"
        "% component\n"
        "#accu(#d0(Z,X,Y),q(A),tuple(A)):-r(A,X),#d0(Z,X,Y)!.\n"
        "% component\n"
        "#accu(#d0(Z,X,Y),x,tuple(B,Y)):-a(B,Y),#d0(Z,X,Y)!.\n"
        "% component\n"
        "Z<#count{A:q(A):#accu(#d0(Z,X,Y),q(A),tuple(A));B,Y:x:#accu(#d0(Z,X,Y),x,tuple(B,Y))}:-#d0(Z,X,Y)!."),
        toString(parse("Z<#count{A:q(A):r(A,X);B,Y:x:a(B,Y)}:-p(X,Y,Z).")));
    CPPUNIT_ASSERT_EQUAL(S(
        "% component\n"
        "#external.\n"
        "% positive component\n"
        "#accu(#d0(Z,X,Y),tuple(#special)):-:-p(X,Y,Z),0>Z.\n"
        "% positive component\n"
        "#accu(#d0(Z,X,Y),tuple(1,q(A))):-r(A,X),not q(A):-p(X,Y,Z).\n"
        "% positive component\n"
        "#accu(#d0(Z,X,Y),tuple(0,a(B,Y))):-a(B,Y):-p(X,Y,Z).\n"
        "% positive component\n"
        "#accu(#d0(Z,X,Y),tuple(3,X,1)):-q(X),X>1:-p(X,Y,Z).\n"
        "% positive component\n"
        "#d0(Z,X,Y):-#accu(#d0(Z,X,Y),tuple(#special)),#accu(#d0(Z,X,Y),tuple(1,q(A))),#accu(#d0(Z,X,Y),tuple(0,a(B,Y))),#accu(#d0(Z,X,Y),tuple(3,X,1)).\n"
        "% positive component\n"
        "x:-p(X,Y,Z),Z<#count{#d0(Z,X,Y)}."),
        toString(parse("x:-p(X,Y,Z),Z<{not q(A):r(A,X);a(B,Y);X>1:q(X)}.")));
    CPPUNIT_ASSERT_EQUAL(S(
        "% component\n"
        "#external.\n"
        "% component\n"
        "Z<#count(#d0(Z,X,Y)):-p(X,Y,Z).\n"
        "% component\n"
        "#accu(#d0(Z,X,Y),#true):-r(A,X),not q(A),#d0(Z,X,Y)!.\n"
        "% component\n"
        "#accu(#d0(Z,X,Y),a(B,Y),tuple(0,a(B,Y))):-#d0(Z,X,Y)!.\n"
        "% component\n"
        "#accu(#d0(Z,X,Y),#true):-q(X),X>1,#d0(Z,X,Y)!.\n"
        "% component\n"
        "Z<#count{1,q(A):#true:#accu(#d0(Z,X,Y),#true);0,a(B,Y):a(B,Y):#accu(#d0(Z,X,Y),a(B,Y),tuple(0,a(B,Y)));3,X,1:#true:#accu(#d0(Z,X,Y),#true)}:-#d0(Z,X,Y)!."),
        toString(parse("Z<{not q(A):r(A,X);a(B,Y);X>1:q(X)}:-p(X,Y,Z).")));
    CPPUNIT_ASSERT_EQUAL(S(
        "% component\n"
        "#external.\n"
        "% component\n"
        "#accu(empty,#d0(Y,X),()):-p(X,Y,Z).\n"
        "% component\n"
        "#accu(cond,#d0(Y,X),(0,)):-:-#accu(empty,#d0(Y,X),())!.\n"
        "% component\n"
        "#accu(head(a(B,Y)),#d0(Y,X),(0,)):-:-#accu(cond,#d0(Y,X),(0,))!.\n"
        "% component\n"
        "#accu(cond,#d0(Y,X),(A,1)):-r(A,X):-#accu(empty,#d0(Y,X),())!.\n"
        "% component\n"
        "#accu(head(q(A)),#d0(Y,X),(A,1)):-:-#accu(cond,#d0(Y,X),(A,1))!.\n"
        "% component\n"
        "a(B,Y)|q(A):-#accu(#Any1,#d0(Y,X),#Any2)!"),
        toString(parse("q(A):r(A,X);a(B,Y):-p(X,Y,Z).")));
    CPPUNIT_ASSERT_EQUAL(S(
        "% component\n"
        "#external.\n"
        "% component\n"
        "#accu(empty,#d0(Y,X),()):-p(X,Y,Z).\n"
        "% component\n"
        "#accu(cond,#d0(Y,X),(0,)):-:-#accu(empty,#d0(Y,X),())!.\n"
        "% component\n"
        "#accu(head(a(B,Y)),#d0(Y,X),(0,)):-:-#accu(cond,#d0(Y,X),(0,))!.\n"
        "% component\n"
        "#accu(cond,#d0(Y,X),(1,)):-q(X):-#accu(empty,#d0(Y,X),())!.\n"
        "% component\n"
        "#accu(head,#d0(Y,X),(1,)):-X<=1:-#accu(cond,#d0(Y,X),(1,))!.\n"
        "% component\n"
        "#accu(cond,#d0(Y,X),(A,2)):-r(A,X):-#accu(empty,#d0(Y,X),())!.\n"
        "% component\n"
        "#accu(head,#d0(Y,X),(A,2)):-not not q(A):-#accu(cond,#d0(Y,X),(A,2))!.\n"
        "% component\n"
        "a(B,Y):-#accu(#Any1,#d0(Y,X),#Any2)!"),
        toString(parse("not q(A):r(A,X);a(B,Y);X>1:q(X):-p(X,Y,Z).")));
    CPPUNIT_ASSERT_EQUAL(S(
        "% component\n"
        "#external.\n"
        "% positive component\n"
        "#accu(#d0(Z,X,Y),tuple(#special)):-:-p(X,Y,Z),0>Z.\n"
        "% positive component\n"
        "#accu(#d0(Z,X,Y),tuple(A)):-q(A),r(A,X):-p(X,Y,Z).\n"
        "% positive component\n"
        "#accu(#d0(Z,X,Y),tuple(B,Y)):-a(B,Y):-p(X,Y,Z).\n"
        "% positive component\n"
        "#d0(Z,X,Y):-#accu(#d0(Z,X,Y),tuple(#special)),#accu(#d0(Z,X,Y),tuple(A)),#accu(#d0(Z,X,Y),tuple(B,Y)).\n"
        "% positive component\n"
        "x:-p(X,Y,Z),Z<#count{#d0(Z,X,Y)}."),
        toString(parse("x:-p(X,Y,Z),Z<#count{A:q(A),r(A,X);B,Y:a(B,Y)}.")));
}

void TestProgram::test_analyze() {
    Gringo::Test::Messages msg;
    CPPUNIT_ASSERT_EQUAL(S(
        "% component\n#external.\n"
        "% positive component\n"
        "x:-x?.\n"
        "% positive component\n"
        "b:-a?.\n"
        "a:-not y,x,b?.\n"
        "% positive component\n"
        "c:-b,a."),
        toString(parse("x:-x.a:-b,x,not y.b:-a.c:-a,b.")));
    CPPUNIT_ASSERT_EQUAL(S(
        "% component\n#external.\n"
        "% positive component\n"
        "x:-x?.\n"
        "% component\n"
        "a:-not b?.\n"
        "% component\n"
        "b:-x,a!,not a!."),
        toString(parse("x:-x.a:-not b.b:-not a,a,x.")));
    // TODO: all the other statements are missing
}

TestProgram::~TestProgram() { }

// }}}

CPPUNIT_TEST_SUITE_REGISTRATION(TestProgram);

} } } // namespace Test Ground Gringo
