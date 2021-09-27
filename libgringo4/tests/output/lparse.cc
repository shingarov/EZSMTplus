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

#include "tests/tests.hh"
#include "tests/term_helper.hh"
#include "tests/output/solver_helper.hh"

namespace Gringo { namespace Output { namespace Test {

// {{{ declaration of TestLparse

class TestLparse : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(TestLparse);
        CPPUNIT_TEST(test_empty);
        CPPUNIT_TEST(test_projectionBug);
        CPPUNIT_TEST(test_recAntiAggr);
        CPPUNIT_TEST(test_minMax);
        CPPUNIT_TEST(test_recCondBug);
        CPPUNIT_TEST(test_aggregateBug);
        CPPUNIT_TEST(test_aggregateNotNot);
        CPPUNIT_TEST(test_aggregateRecBug);
        CPPUNIT_TEST(test_headAggregateBug);
        CPPUNIT_TEST(test_headAggrPropagateBug);
        CPPUNIT_TEST(test_recHeadAggregateBug);
        CPPUNIT_TEST(test_disjunctionBug);
        CPPUNIT_TEST(test_symTabBug);
        CPPUNIT_TEST(test_mutexBug);
        CPPUNIT_TEST(test_head);
        CPPUNIT_TEST(test_assign);
        CPPUNIT_TEST(test_conjunction);
        CPPUNIT_TEST(test_disjunction);
        CPPUNIT_TEST(test_show);
        CPPUNIT_TEST(test_aggregates);
        CPPUNIT_TEST(test_minimize);
        CPPUNIT_TEST(test_invert);
        CPPUNIT_TEST(test_csp);
        CPPUNIT_TEST(test_cspbound);
        CPPUNIT_TEST(test_disjoint);
        CPPUNIT_TEST(test_queens);
        CPPUNIT_TEST(test_python);
        CPPUNIT_TEST(test_lua);
        CPPUNIT_TEST(test_unpool);
        CPPUNIT_TEST(test_undefinedRule);
        CPPUNIT_TEST(test_undefinedCSP);
        CPPUNIT_TEST(test_undefinedCSPDisjoint);
        CPPUNIT_TEST(test_undefinedBodyAggregate);
        CPPUNIT_TEST(test_undefinedAssignmentAggregate);
        CPPUNIT_TEST(test_undefinedHeadAggregate);
        CPPUNIT_TEST(test_undefinedConjunction);
        CPPUNIT_TEST(test_undefinedDisjunction);
        CPPUNIT_TEST(test_undefinedScript);
        CPPUNIT_TEST(test_nonmon);
        CPPUNIT_TEST(test_bugRewriteCond);
    CPPUNIT_TEST_SUITE_END();
    using S = std::string;

public:
    virtual void setUp();
    virtual void tearDown();

    void test_empty();
    void test_projectionBug();
    void test_recCondBug();
    void test_recAntiAggr();
    void test_aggregateBug();
    void test_aggregateNotNot();
    void test_aggregateRecBug();
    void test_headAggregateBug();
    void test_headAggrPropagateBug();
    void test_recHeadAggregateBug();
    void test_disjunctionBug();
    void test_symTabBug();
    void test_mutexBug();
    void test_head();
    void test_assign();
    void test_conjunction();
    void test_disjunction();
    void test_show();
    void test_aggregates();
    void test_csp();
    void test_cspbound();
    void test_disjoint();
    void test_queens();
    void test_python();
    void test_lua();
    void test_minimize();
    void test_invert();
    void test_unpool();
    void test_undefinedRule();
    void test_undefinedCSP();
    void test_undefinedCSPDisjoint();
    void test_undefinedBodyAggregate();
    void test_undefinedAssignmentAggregate();
    void test_undefinedHeadAggregate();
    void test_undefinedConjunction();
    void test_undefinedDisjunction();
    void test_undefinedScript();
    void test_minMax();
    void test_nonmon();
    void test_bugRewriteCond();
    virtual ~TestLparse();
};

// }}}

// {{{ definition of TestLparse

void TestLparse::setUp() {
}

void TestLparse::tearDown() {
}

void Test::TestLparse::test_unpool() {
    CPPUNIT_ASSERT_EQUAL(
        S("[[a(1),a(2),a(4)]]"),
        IO::to_string(solve(
            "a(X) :- X=(1;2;4)."
        ))
    );
    CPPUNIT_ASSERT_EQUAL(
        S("[[p(1),p(2),q(3)],[p(1),p(2),q(4)]]"),
        IO::to_string(solve(
            "1 {q(3;4)} 1."
            "p(1;2) :- q(3;4)."
        ))
    );
    CPPUNIT_ASSERT_EQUAL(
        S("[]"),
        IO::to_string(solve(
            "1 {q(3;4)} 1."
            "#disjoint { (1;2) : (2;3) : q(3;4) }."
        ))
    );
    CPPUNIT_ASSERT_EQUAL(
        S("[[a,p(1),q(3)],[a,p(1),q(4)],[a,p(2),q(3)],[a,p(2),q(4)]]"),
        IO::to_string(solve(
            "1 { p(1;2) } 1."
            "1 { q(3;4) } 1."
            "a :- p(1;2) : q(3;4)."
        ))
    );
    CPPUNIT_ASSERT_EQUAL(
        S("[[p(1),p(2),q(3)],[p(1),p(2),q(4)]]"),
        IO::to_string(solve(
            "1 { q(3;4) } 1."
            "p(1;2) : q(3;4)."
        ))
    );
    CPPUNIT_ASSERT_EQUAL(
        S("[[a,p(1),q(3)],[a,p(1),q(4)],[a,p(2),q(3)],[a,p(2),q(4)],[q(3)],[q(4)]]"),
        IO::to_string(solve(
            "1 { q(3;4) } 1."
            "0 { p(1;2) } 1."
            "a :- (1;2) { q(3;4) : p(1;2) }."
        ))
    );
    CPPUNIT_ASSERT_EQUAL(
        S("[[],[a,p(1)],[a,p(2)]]"),
        IO::to_string(solve(
            "0 { p(1;2) } 1."
            "a :- (2;3) #count { q(3;4) : p(1;2) }."
        ))
    );
    CPPUNIT_ASSERT_EQUAL(
        S("[[p(1),q(3),q(4)],[p(2),q(3),q(4)]]"),
        IO::to_string(solve(
            "0 { p(1;2) } 1."
            "(1;2) { q(3;4) : p(1;2) }."
        ))
    );
    CPPUNIT_ASSERT_EQUAL(
        S("[[p(1),q(3)],[p(1),q(3),q(4)],[p(1),q(4)],[p(2),q(3)],[p(2),q(3),q(4)],[p(2),q(4)]]"),
        IO::to_string(solve(
            "0 { p(1;2) } 1."
            "(1;2) #count { t(5;6) : q(3;4) : p(1;2) }."
        ))
    );
}

void TestLparse::test_recCondBug() {
    CPPUNIT_ASSERT_EQUAL(
        S("[[0,2,4,5,9],[0,2,5,9],[0,4,9],[0,9]]"),
        IO::to_string(solve(
            "holds(atom(A)) :- rule(lit(pos,atom(A)),B); holds(B).\n"
            "{ holds(atom(A)) : head_aggregate_element_set(I, head_aggregate_element(_,lit(pos,atom(A)),C))\n"
            "                 , holds(C)\n"
            "} :- rule(head_aggregate(left(#inf,less_equal),count,head_aggregate_element_set(I),right(less_equal,#sup)),C)\n"
            "   ; holds(C)\n"
            "   .\n"
            "% :- rule(lit(pos,false),B); holds(B) .\n"
            "% NOTE: there are more heads but this is enough to support --lparse-rewrite\n"
            "\n"
            "holds(conjunction(I)) :- conjunction(I)\n"
            "                       ;         holds(A) : conjunction(I,lit(pos,A))\n"
            "                       ; not     holds(A) : conjunction(I,lit(neg,A))\n"
            "                       ; not not holds(A) : conjunction(I,lit(neg_neg,A))\n"
            "                       .\n"
            "\n"
            "body_aggregate(L,F,S,U) :- conjunction(_,lit(_,body_aggregate(L,F,S,U))). % TODO: generate in meta\n"
            "\n"
            "holds(body_aggregate(left(#inf,less_equal),sump,body_aggregate_element_set(S),right(greater_equal,U)))\n"
            "    :- body_aggregate(left(#inf,less_equal),sump,body_aggregate_element_set(S),right(greater_equal,U))\n"
            "     ; #sum+ { W,T : body_aggregate_element_set(S,body_aggregate_element((W,T),conjunction(C))), holds(C) } >= U % TODO: if the holds is ommitted strange things appear to happen\n"
            "     .\n"
            "% NOTE: there are more body aggregates but this is enough to support --lparse-rewrite\n"
            "\n"
            "% TODO: handle minimize constraint\n"
            "\n"
            "#show.\n"
            "#show I : holds(conjunction(I)), conjunction(I).\n"
            "rule(head_aggregate(left(#inf,less_equal),count,head_aggregate_element_set(0),right(less_equal,#sup)),conjunction(0)).\n"
            "conjunction(0).\n"
            "head_aggregate_element_set(0).\n"
            "head_aggregate_element_set(0,head_aggregate_element((),lit(pos,atom(p(1))),conjunction(0))).\n"
            "head_aggregate_element_set(0,head_aggregate_element((),lit(pos,atom(p(2))),conjunction(0))).\n"
            "rule(lit(pos,atom(__aux(1))),conjunction(1)).\n"
            "conjunction(1).\n"
            "conjunction(1,lit(pos,atom(p(2)))).\n"
            "conjunction(1,lit(neg,atom(p(2)))).\n"
            "rule(lit(pos,atom(__aux(2))),conjunction(2)).\n"
            "conjunction(2).\n"
            "conjunction(2,lit(pos,atom(p(2)))).\n"
            "rule(lit(pos,atom(__aux(2))),conjunction(3)).\n"
            "conjunction(3).\n"
            "conjunction(3,lit(pos,atom(__aux(1)))).\n"
            "rule(lit(pos,atom(__aux(4))),conjunction(6)).\n"
            "conjunction(4).\n"
            "conjunction(4,lit(pos,atom(p(1)))).\n"
            "conjunction(5).\n"
            "conjunction(5,lit(pos,atom(__aux(2)))).\n"
            "body_aggregate_element_set(0).\n"
            "body_aggregate_element_set(0,body_aggregate_element((1,(0,())),conjunction(4))).\n"
            "body_aggregate_element_set(0,body_aggregate_element((2,(1,())),conjunction(5))).\n"
            "conjunction(6).\n"
            "conjunction(6,lit(pos,body_aggregate(left(#inf,less_equal),sump,body_aggregate_element_set(0),right(greater_equal,1)))).\n"
            "rule(lit(pos,atom(__aux(5))),conjunction(7)).\n"
            "conjunction(7).\n"
            "conjunction(7,lit(pos,body_aggregate(left(#inf,less_equal),sump,body_aggregate_element_set(0),right(greater_equal,3)))).\n"
            "rule(lit(pos,atom(__aux(3))),conjunction(8)).\n"
            "conjunction(8).\n"
            "conjunction(8,lit(pos,atom(__aux(4)))).\n"
            "conjunction(8,lit(neg,atom(__aux(5)))).\n"
            "rule(lit(pos,false),conjunction(9)).\n"
            "conjunction(9).\n"
            "conjunction(9,lit(neg,atom(__aux(3)))).\n"
            "show_atom(p(1)).\n"
            "show_atom(p(2)).\n"
        ))
    );
}

void TestLparse::test_empty() {
    CPPUNIT_ASSERT_EQUAL(S("[[]]"), IO::to_string(solve("")));
}

void TestLparse::test_projectionBug() {
    CPPUNIT_ASSERT_EQUAL(S("[[p(1),p(2)]]"), IO::to_string(solve(
        "q((1,x),2).\n"
        "p(A) :- q((A,_),_).\n"
        "p(B) :- q((A,_),B).\n"
        , {"p("})));
}
void TestLparse::test_aggregateBug() {
    CPPUNIT_ASSERT_EQUAL(
        S("[[a(1),a(2),b(1)],[a(1),a(2),b(1),b(2)]]"), 
        IO::to_string(solve(
        "a(1)."
        "a(2)."
        "b(1)."
        "{ b(X) } :- a(X).")));
}
void TestLparse::test_recHeadAggregateBug() {
    CPPUNIT_ASSERT_EQUAL(
        S(
        "["
        "[r(v(x13),e(r1,n(a11))),r(v(x13),e(r1,v(x3))),r(v(x13),e(r1,v(x7))),r(v(x13),n(a11)),r(v(x7),e(r1,n(a11))),r(v(x7),e(r1,v(x4))),r(v(x7),n(a11)),r(v(x9),e(r1,n(a11))),r(v(x9),e(r1,v(x3))),r(v(x9),e(r1,v(x7)))],"
        "[r(v(x13),e(r1,n(a11))),r(v(x13),e(r1,v(x3))),r(v(x13),e(r1,v(x7))),r(v(x13),n(a11)),r(v(x7),e(r1,n(a11))),r(v(x7),e(r1,v(x4))),r(v(x9),e(r1,v(x3))),r(v(x9),e(r1,v(x7)))],"
        "[r(v(x13),e(r1,n(a11))),r(v(x13),e(r1,v(x3))),r(v(x13),e(r1,v(x7))),r(v(x7),e(r1,n(a11))),r(v(x7),e(r1,v(x4))),r(v(x9),e(r1,v(x3))),r(v(x9),e(r1,v(x7)))],"
        "[r(v(x13),e(r1,n(a11))),r(v(x13),e(r1,v(x3))),r(v(x13),n(a11)),r(v(x7),e(r1,n(a11))),r(v(x7),e(r1,v(x4))),r(v(x7),n(a11)),r(v(x9),e(r1,n(a11))),r(v(x9),e(r1,v(x3)))],"
        "[r(v(x13),e(r1,n(a11))),r(v(x13),e(r1,v(x3))),r(v(x13),n(a11)),r(v(x7),e(r1,n(a11))),r(v(x7),e(r1,v(x4))),r(v(x7),n(a11)),r(v(x9),e(r1,n(a11))),r(v(x9),e(r1,v(x3))),r(v(x9),e(r1,v(x7)))],"
        "[r(v(x13),e(r1,n(a11))),r(v(x13),e(r1,v(x3))),r(v(x13),n(a11)),r(v(x7),e(r1,n(a11))),r(v(x7),e(r1,v(x4))),r(v(x9),e(r1,v(x3)))],"
        "[r(v(x13),e(r1,n(a11))),r(v(x13),e(r1,v(x3))),r(v(x13),n(a11)),r(v(x7),e(r1,n(a11))),r(v(x7),e(r1,v(x4))),r(v(x9),e(r1,v(x3))),r(v(x9),e(r1,v(x7)))],"
        "[r(v(x13),e(r1,n(a11))),r(v(x13),e(r1,v(x3))),r(v(x7),e(r1,n(a11))),r(v(x7),e(r1,v(x4))),r(v(x9),e(r1,v(x3)))],"
        "[r(v(x13),e(r1,n(a11))),r(v(x13),e(r1,v(x3))),r(v(x7),e(r1,n(a11))),r(v(x7),e(r1,v(x4))),r(v(x9),e(r1,v(x3))),r(v(x9),e(r1,v(x7)))],"
        "[r(v(x13),e(r1,n(a11))),r(v(x13),n(a11)),r(v(x7),e(r1,n(a11))),r(v(x7),e(r1,v(x4))),r(v(x7),n(a11)),r(v(x9),e(r1,n(a11))),r(v(x9),e(r1,v(x3)))],"
        "[r(v(x13),e(r1,n(a11))),r(v(x13),n(a11)),r(v(x7),e(r1,n(a11))),r(v(x7),e(r1,v(x4))),r(v(x7),n(a11)),r(v(x9),e(r1,n(a11))),r(v(x9),e(r1,v(x3))),r(v(x9),e(r1,v(x7)))],"
        "[r(v(x13),e(r1,n(a11))),r(v(x13),n(a11)),r(v(x7),e(r1,n(a11))),r(v(x7),e(r1,v(x4))),r(v(x9),e(r1,v(x3)))],"
        "[r(v(x13),e(r1,n(a11))),r(v(x13),n(a11)),r(v(x7),e(r1,n(a11))),r(v(x7),e(r1,v(x4))),r(v(x9),e(r1,v(x3))),r(v(x9),e(r1,v(x7)))],"
        "[r(v(x13),e(r1,n(a11))),r(v(x7),e(r1,n(a11))),r(v(x7),e(r1,v(x4))),r(v(x9),e(r1,v(x3)))],"
        "[r(v(x13),e(r1,n(a11))),r(v(x7),e(r1,n(a11))),r(v(x7),e(r1,v(x4))),r(v(x9),e(r1,v(x3))),r(v(x9),e(r1,v(x7)))],"
        "[r(v(x13),e(r1,v(x3))),r(v(x13),e(r1,v(x7))),r(v(x13),n(a11)),r(v(x7),e(r1,n(a11))),r(v(x7),e(r1,v(x4))),r(v(x9),e(r1,v(x3))),r(v(x9),e(r1,v(x7)))],"
        "[r(v(x13),e(r1,v(x3))),r(v(x13),e(r1,v(x7))),r(v(x7),e(r1,n(a11))),r(v(x7),e(r1,v(x4))),r(v(x9),e(r1,v(x3))),r(v(x9),e(r1,v(x7)))],"
        "[r(v(x13),e(r1,v(x3))),r(v(x13),n(a11)),r(v(x7),e(r1,n(a11))),r(v(x7),e(r1,v(x4))),r(v(x7),n(a11)),r(v(x9),e(r1,n(a11))),r(v(x9),e(r1,v(x3)))],"
        "[r(v(x13),e(r1,v(x3))),r(v(x13),n(a11)),r(v(x7),e(r1,n(a11))),r(v(x7),e(r1,v(x4))),r(v(x7),n(a11)),r(v(x9),e(r1,n(a11))),r(v(x9),e(r1,v(x3))),r(v(x9),e(r1,v(x7)))],"
        "[r(v(x13),e(r1,v(x3))),r(v(x13),n(a11)),r(v(x7),e(r1,n(a11))),r(v(x7),e(r1,v(x4))),r(v(x9),e(r1,v(x3)))],"
        "[r(v(x13),e(r1,v(x3))),r(v(x13),n(a11)),r(v(x7),e(r1,n(a11))),r(v(x7),e(r1,v(x4))),r(v(x9),e(r1,v(x3))),r(v(x9),e(r1,v(x7)))],"
        "[r(v(x13),e(r1,v(x3))),r(v(x7),e(r1,n(a11))),r(v(x7),e(r1,v(x4))),r(v(x9),e(r1,v(x3)))],"
        "[r(v(x13),e(r1,v(x3))),r(v(x7),e(r1,n(a11))),r(v(x7),e(r1,v(x4))),r(v(x9),e(r1,v(x3))),r(v(x9),e(r1,v(x7)))],"
        "[r(v(x13),n(a11)),r(v(x7),e(r1,n(a11))),r(v(x7),e(r1,v(x4))),r(v(x7),n(a11)),r(v(x9),e(r1,n(a11))),r(v(x9),e(r1,v(x3)))],"
        "[r(v(x13),n(a11)),r(v(x7),e(r1,n(a11))),r(v(x7),e(r1,v(x4))),r(v(x7),n(a11)),r(v(x9),e(r1,n(a11))),r(v(x9),e(r1,v(x3))),r(v(x9),e(r1,v(x7)))],"
        "[r(v(x13),n(a11)),r(v(x7),e(r1,n(a11))),r(v(x7),e(r1,v(x4))),r(v(x9),e(r1,v(x3)))],"
        "[r(v(x13),n(a11)),r(v(x7),e(r1,n(a11))),r(v(x7),e(r1,v(x4))),r(v(x9),e(r1,v(x3))),r(v(x9),e(r1,v(x7)))],"
        "[r(v(x7),e(r1,n(a11))),r(v(x7),e(r1,v(x4))),r(v(x9),e(r1,v(x3)))],"
        "[r(v(x7),e(r1,n(a11))),r(v(x7),e(r1,v(x4))),r(v(x9),e(r1,v(x3))),r(v(x9),e(r1,v(x7)))]"
        "]"), IO::to_string(solve(
        "atom(A) :- hasatom(A,_,_).\n"
        "atom(A) :- hasatom(e(_,A),_,_).\n"
        "nonvatom(X) :- atom(X), X!=v(V):atom(v(V)).\n"
        "\n"
        "triviallyfalse(n(X),n(Y)) :- atom(n(X)), atom(n(Y)), X!=Y.\n"
        "triviallyfalse(e(R1,C1),e(R2,C2)) :- atom(e(R1,C1)),\n"
        "                                               atom(e(R2,C2)), R1!=R2. \n"
        "triviallyfalse(n(C),e(R,C1)) :- atom(n(C)), atom(e(R,C1)).\n"
        "triviallyfalse(e(R,C1),n(C)) :- atom(n(C)), atom(e(R,C1)).\n"
        "\n"
        "subs(X,Y) :- subs(e(R,X),e(R,Y)).\n"
        "\n"
        "1 { subs(Y,X):hasatom(Y,|P-1|,E),not triviallyfalse(Y,X) } :- hasatom(X,P,E), nonvatom(X).\n"
        "\n"
        "1 { subs(Y,C):hasatom(Y,|P-1|,E),not triviallyfalse(Y,C) } :- subs(X,C), hasatom(X,P,E),\n"
        "                  nonvatom(C), not hasatom(C,PX,E):hasatom(C,PX,E).\n"
        "\n"
        "subs(X,Z) :- subs(X,Y), subs(Y,Z), X!=Z, X!=Y, Z!=Y.\n"
        "\n"
        ":- subs(X,Y), triviallyfalse(X,Y).\n"
        "\n"
        "greater(X,Y) :- subs(v(X),e(R,v(Y))).\n"
        ":- greater(X,X).\n"
        "greater(X,Z) :- greater(X,Y), greater(Y,Z), X!=Y, X!=Z, Y!=Z.\n"
        "\n"
        "atom(C) :- diseq(C,D).\n"
        "atom(D) :- diseq(C,D).\n"
        "\n"
        "1 { dissubs(X,Y); dissubs(Y,X) } :- diseq(X,Y).\n"
        "ok(C,D) :- dissubs(C,D), subs(D,D'), nonvatom(D'), not subs(C,D'), not subs(C',D'):subs(C,C').\n"
        ":- dissubs(C,D), not ok(C,D).\n"
        "\n"
        "r(v(X),C) :- subs(v(X),C), relevant(X), nonvatom(C).\n"
        "\n"
        "%equation 1\n"
        "hasatom(v(x3), 0, 1).\n"
        "hasatom(e(r1, n(a11)), 1, 1).\n"
        "\n"
        "%equation 2\n"
        "hasatom(v(x9), 0, 2).\n"
        "hasatom(v(x5), 1, 2).\n"
        "hasatom(v(x9), 1, 2).\n"
        "\n"
        "%equation 3\n"
        "hasatom(v(x6), 0, 3).\n"
        "hasatom(v(x4), 1, 3).\n"
        "hasatom(v(x9), 1, 3).\n"
        "\n"
        "%equation 4\n"
        "hasatom(v(x1), 0, 4).\n"
        "hasatom(v(x1), 1, 4).\n"
        "hasatom(v(x9), 1, 4).\n"
        "\n"
        "%equation 5\n"
        "hasatom(v(x1), 0, 5).\n"
        "hasatom(e(r1, v(x7)), 1, 5).\n"
        "\n"
        "%equation 6\n"
        "hasatom(v(x4), 0, 6).\n"
        "hasatom(v(x3), 1, 6).\n"
        "hasatom(v(x13), 1, 6).\n"
        "\n"
        "%equation 7\n"
        "hasatom(v(x2), 0, 7).\n"
        "hasatom(e(r1, v(x4)), 1, 7).\n"
        "\n"
        "%equation 8\n"
        "hasatom(v(x7), 0, 8).\n"
        "hasatom(v(x2), 1, 8).\n"
        "hasatom(v(x7), 1, 8).\n"
        "\n"
        "%equation 9\n"
        "hasatom(v(x5), 0, 9).\n"
        "hasatom(e(r1, v(x3)), 1, 9).\n"
        "\n"
        "%disequation 1\n"
        "diseq(v(x6), v(x9)).\n"
        "\n"
        "relevant(x7).\n"
        "relevant(x9).\n"
        "relevant(x13).\n"
        , {"r("})));

}
void TestLparse::test_disjunctionBug() {
    Gringo::Test::Messages msg;
    CPPUNIT_ASSERT_EQUAL(
        S("[]"),
        IO::to_string(solve(
            "b."
            "c :- b."
            "a :- c."
            "c : d :- a."
        )));
}
void TestLparse::test_aggregateNotNot() {
    CPPUNIT_ASSERT_EQUAL(
        S("[[p(2)],[p(2),p(3)],[p(2),p(3),p(4)],[p(2),p(3),p(4),p(5)],[p(2),p(3),p(5)],[p(2),p(4)],[p(2),p(4),p(5)],[p(2),p(5)],[p(4)],[p(4),p(5)]]"),
        IO::to_string(solve(
        "{ p(1..5) }."
        ":- not not 2 != #min { X:p(X) } != 4.")));
    CPPUNIT_ASSERT_EQUAL(
        S("[[p(2)],[p(2),p(3)],[p(2),p(3),p(4)],[p(2),p(3),p(4),p(5)],[p(2),p(3),p(5)],[p(2),p(4)],[p(2),p(4),p(5)],[p(2),p(5)],[p(4)],[p(4),p(5)]]"),
        IO::to_string(solve(
        "{ p(1..5) }."
        ":- 2 != #min { X:p(X) } != 4.")));
    CPPUNIT_ASSERT_EQUAL(
        S("[[p(2)],[p(2),p(3)],[p(2),p(3),p(4)],[p(2),p(3),p(4),p(5)],[p(2),p(3),p(5)],[p(2),p(4)],[p(2),p(4),p(5)],[p(2),p(5)],[p(4)],[p(4),p(5)]]"),
        IO::to_string(solve(
        "{ p(1..5) }."
        "h :- not 2 != #min { X:p(X) } != 4."
        ":- not h.", {"p("})));
    CPPUNIT_ASSERT_EQUAL(
        S("[[p(2)],[p(2),p(3)],[p(2),p(3),p(4)],[p(2),p(3),p(4),p(5)],[p(2),p(3),p(5)],[p(2),p(4)],[p(2),p(4),p(5)],[p(2),p(5)],[p(4)],[p(4),p(5)]]"),
        IO::to_string(solve(
        "{ p(1..5) }."
        ":- not 2 = #min { X:p(X) }, not #min { X:p(X) } = 4.")));
}
void TestLparse::test_aggregateRecBug() {
    Gringo::Test::Messages msg;
    CPPUNIT_ASSERT_EQUAL(
        S("[]"), 
        IO::to_string(solve(
        "a :- {a}!=1."
        )));
    CPPUNIT_ASSERT_EQUAL(
        S("[]"), 
        IO::to_string(solve(
        "a :- #sum {1:a}!=1."
        )));
    CPPUNIT_ASSERT_EQUAL(
        S("[[b]]"), 
        IO::to_string(solve(
            "b :- 0  #sum+ { 1: b }."
        )));
    CPPUNIT_ASSERT_EQUAL(
        S("[[b]]"), 
        IO::to_string(solve(
            "b :- 0  #sum { 1: b }."
        )));
}
void TestLparse::test_symTabBug() {
    Gringo::Test::Messages msg;
    CPPUNIT_ASSERT_EQUAL(
        S("[[does(a,0),does(a,1)],[does(a,0),does(b,1)]]"),
        IO::to_string(solve(
            "time(0..1).\n"
            "1 { does(M,T) : legal(M,T) } 1 :- time(T).\n"
            "legal(a,T) :- time(T).\n"
            "legal(b,T) :- does(a,0), time(T).\n", 
            {"does"})));
}
void TestLparse::test_recAntiAggr() {
    CPPUNIT_ASSERT_EQUAL(
        S("[[p],[r]]"),
        IO::to_string(solve(
            "r :- #sum { 1:p } < 1.\n"
            "p :- not r.")));
    CPPUNIT_ASSERT_EQUAL(
        S("[[],[p]]"),
        IO::to_string(solve("p :- #sum { 1:not p } < 1.")));
    CPPUNIT_ASSERT_EQUAL(
        S("[[],[p]]"),
        IO::to_string(solve("p :- not #sum { 1:p } < 1.")));
    CPPUNIT_ASSERT_EQUAL(
        S("[[],[p]]"),
        IO::to_string(solve("p :- not #sum { 1:not p } > 0.")));
    CPPUNIT_ASSERT_EQUAL(
        S("[[],[p]]"),
        IO::to_string(solve("p :- not not #sum { 1:p } > 0.")));
    CPPUNIT_ASSERT_EQUAL(
        S("[[],[p]]"),
        IO::to_string(solve("p :- not not #sum { 1:not p } < 1.")));
}
void TestLparse::test_headAggrPropagateBug() {
    Gringo::Test::Messages msg;
    CPPUNIT_ASSERT_EQUAL(
        S(
            "[[a(c),a(p),b(c,d),b(c,e),b(c,f),b(c,g),b(p,d),b(p,e),b(p,f),b(p,g)],"
            "[a(c),a(p),b(c,d),b(c,e),b(c,g),b(p,d),b(p,e),b(p,g),e(3)],"
            "[b(c,d),b(c,e),b(p,d),b(p,e),e(2),e(3)]]"), 
        IO::to_string(solve(
            "b(S,h) :- b(S,X), c.\n"
            "b(c,d).\n"
            "b(p,X) :- b(c,X).\n"
            "b(c,e).\n"
            "a(S)   :- b(S,g).\n"
            "1 = { e(3); b(S,f) : a(S) } :- b(S,e).\n"
            "1 = { e(2); b(S,g) } :- b(S,d).\n")));
    CPPUNIT_ASSERT_EQUAL(S("[-:1:19-20: info: atom does not occur in any rule head:\n  c\n]"), IO::to_string(msg.messages));
}
void TestLparse::test_mutexBug() {
#ifdef WITH_LUA
    CPPUNIT_ASSERT_EQUAL(
        S("[[]]"), 
        IO::to_string(solve(
            "#script (lua) \n"
            "function main(prg)\n"
            "	prg:ground(\"base\", {})\n"
            "	prg:ground(\"plan_graph_base\", {})\n"
            "\n"
            "	for step = 1,3,1 do\n"
            "		prg:ground(\"plan_graph_step\", {step})\n"
            "		prg:solve()\n"
            "	end\n"
            "--	Comment line 8 and uncomment line 11\n"
            "--	prg:solve()\n"
            "end\n"
            "#end.\n"
            "\n"
            "#program plan_graph_base.\n"
            "valid_f(FLUENT, 1) :- init(FLUENT).\n"
            "valid_f1(FLUENT, 1) :- init(FLUENT).\n"
            "\n"
            "#program plan_graph_step(time).\n"
            "\n"
            "%Encoding 1\n"
            "\n"
            "valid_a(ACT, time) :- action(ACT); valid_f(FLUENT, time) : pre(ACT, FLUENT);\n"
            "			not mutex(F1, F2, time) : req_both(ACT, F1, F2).\n"
            "\n"
            "valid_pre(ACT, FLUENT, time) :- valid_a(ACT, time), pre(ACT, FLUENT).\n"
            "valid_add(ACT, FLUENT, time) :- valid_a(ACT, time), addadd(ACT, FLUENT).\n"
            "valid_f(FLUENT, time + 1) :- valid_add(_, FLUENT, time).\n"
            "\n"
            "mutex_actions(A1, A2, time) :- valid_a(A1, time), valid_a(A2, time), conflicting(A1, A2).\n"
            "\n"
            "mutex_a_with_f(A1, P2, time) :- mutex(P1, P2, time), valid_pre(A1, P1, time).\n"
            "mutex_actions(A1, A2, time) :- mutex_a_with_f(A1, P2, time), valid_pre(A2, P2, time).\n"
            "%mutex_actions(A1, A2, time) :- mutex(P1, P2, time), valid_pre(A1, P1, time), valid_pre(A2, P2, time).\n"
            "\n"
            "mutex_a_sym(A1, A2, time; A2, A1, time) :- mutex_actions(A1, A2, time).\n"
            "\n"
            "closes(A2, F1, time) :- valid_a(A2, time), valid_f(F1, time+1); mutex_a_sym(A1, A2, time) : valid_add(A1, F1, time).\n"
            "mutex(F1, F2, time+1) :- F1 < F2, valid_f(F1, time+1), valid_f(F2, time+1);\n"
            "			closes(A2, F1, time) : valid_add(A2, F2, time).\n"
            "\n"
            "% Encoding 2:\n"
            "% Identical to encoding 1 except for lines 32 and 33 vs. line 57.\n"
            "% However these should generate identical groundings, but they don't for some reason\n"
            "\n"
            "valid_a1(ACT, time) :- action(ACT); valid_f1(FLUENT, time) : pre(ACT, FLUENT);\n"
            "			not mutex1(F1, F2, time) : req_both(ACT, F1, F2).\n"
            "\n"
            "valid_pre1(ACT, FLUENT, time) :- valid_a1(ACT, time), pre(ACT, FLUENT).\n"
            "valid_add1(ACT, FLUENT, time) :- valid_a1(ACT, time), addadd(ACT, FLUENT).\n"
            "valid_f1(FLUENT, time + 1) :- valid_add1(_, FLUENT, time).\n"
            "\n"
            "mutex_actions1(A1, A2, time) :- valid_a1(A1, time), valid_a1(A2, time), conflicting(A1, A2).\n"
            "\n"
            "%mutex_a_with_f1(A1, P2, time) :- mutex1(P1, P2, time), valid_pre1(A1, P1, time).\n"
            "%mutex_actions1(A1, A2, time) :- mutex_a_with_f1(A1, P2, time), valid_pre1(A2, P2, time).\n"
            "mutex_actions1(A1, A2, time) :- mutex1(P1, P2, time), valid_pre1(A1, P1, time), valid_pre1(A2, P2, time).\n"
            "\n"
            "mutex_a_sym1(A1, A2, time; A2, A1, time) :- mutex_actions1(A1, A2, time).\n"
            "\n"
            "closes1(A2, F1, time) :- valid_a1(A2, time), valid_f1(F1, time+1); mutex_a_sym1(A1, A2, time) : valid_add1(A1, F1, time).\n"
            "mutex1(F1, F2, time+1) :- F1 < F2, valid_f1(F1, time+1), valid_f1(F2, time+1);\n"
            "			closes1(A2, F1, time) : valid_add1(A2, F2, time).\n"
            "\n"
            "% diff reports if the two encodings are not the same\n"
            "\n"
            "diff(A1, A2, time) :- mutex_actions1(A1, A2, time), not mutex_actions(A1, A2, time).\n"
            "diff(A1, A2, time) :- mutex_actions(A1, A2, time), not mutex_actions1(A1, A2, time).\n"
            "\n"
            "#program base.\n"
            "\n"
            "#show diff/3.\n"
            "\n"
            "preserve_action(preserve(F)) :- fact(F).\n"
            "action(A) :- preserve_action(A).\n"
            "pre(preserve(F), F) :- fact(F).\n"
            "addadd(preserve(F), F) :- fact(F).\n"
            "\n"
            "prepre(A, F) :- pre(A, F), not del(A, F).\n"
            "deldel(A, F) :- del(A, F), not pre(A, F).\n"
            "predel(A, F) :- pre(A, F), del(A, F).\n"
            "addadd(A, F) :- add(A, F), not pre(A, F), not del(A, F).\n"
            "\n"
            "conflicting(A1, A2) :- pre(A1, F), del(A2, F), A1 != A2.\n"
            "\n"
            "req_both(ACT, F1, F2) :- pre(ACT, F1), pre(ACT, F2), F1 < F2.\n"
            "\n"
            "type(object).\n"
            "type(ferry, object).\n"
            "fact(at_ferry(X)) :- type(X, object).\n"
            "fact(at(X, Y)) :- type(X, object), type(Y, object).\n"
            "fact(on(X, Y)) :- type(X, object), type(Y, object).\n"
            "fact(empty_ferry).\n"
            "action(debark(X, Y)) :- type(X, object), type(Y, object), auto__(X), place__(Y).\n"
            "pre(debark(X, Y), on(X, ferry)) :- action(debark(X, Y)).\n"
            "pre(debark(X, Y), at_ferry(Y)) :- action(debark(X, Y)).\n"
            "add(debark(X, Y), at(X, Y)) :- action(debark(X, Y)).\n"
            "add(debark(X, Y), empty_ferry) :- action(debark(X, Y)).\n"
            "del(debark(X, Y), on(X, ferry)) :- action(debark(X, Y)).\n"
            "action(sail(X, Y)) :- type(X, object), type(Y, object), place__(X), place__(Y).\n"
            "pre(sail(X, Y), at_ferry(X)) :- action(sail(X, Y)).\n"
            "add(sail(X, Y), at_ferry(Y)) :- action(sail(X, Y)).\n"
            "del(sail(X, Y), at_ferry(X)) :- action(sail(X, Y)).\n"
            "action(board(X, Y)) :- type(X, object), type(Y, object), place__(Y), auto__(X).\n"
            "pre(board(X, Y), at(X, Y)) :- action(board(X, Y)).\n"
            "pre(board(X, Y), at_ferry(Y)) :- action(board(X, Y)).\n"
            "pre(board(X, Y), empty_ferry) :- action(board(X, Y)).\n"
            "add(board(X, Y), on(X, ferry)) :- action(board(X, Y)).\n"
            "del(board(X, Y), at(X, Y)) :- action(board(X, Y)).\n"
            "del(board(X, Y), empty_ferry) :- action(board(X, Y)).\n"
            "type(c1, object).\n"
            "type(b, object).\n"
            "type(c2, object).\n"
            "type(a, object).\n"
            "place__(a).\n"
            "place__(b).\n"
            "auto__(c1).\n"
            "auto__(c2).\n"
            "init(at(c1, a)).\n"
            "init(at(c2, a)).\n"
            "init(at_ferry(a)).\n"
            "init(empty_ferry).\n"
            "goal(at(c1, b)).\n"
            "goal(at(c2, b)).\n"
            )));
#endif // WITH_LUA
}
void TestLparse::test_headAggregateBug() {
    Gringo::Test::Messages msg;
    CPPUNIT_ASSERT_EQUAL(S("[[q(a),r(a)]]"), IO::to_string(solve("1 { q(a); p(X) : z(X) }. r(X) :- q(X).")));
    CPPUNIT_ASSERT_EQUAL(S("[[q(a),r(a)]]"), IO::to_string(solve("1 { p(X) : z(X); q(a) }. r(X) :- q(X).")));
}
void TestLparse::test_head() {
    CPPUNIT_ASSERT_EQUAL(S("[[],[a],[a,b],[b]]"), IO::to_string(solve("{a;b}.")));
    CPPUNIT_ASSERT_EQUAL(S("[[a],[b]]"), IO::to_string(solve("1{a;b}1.")));
    CPPUNIT_ASSERT_EQUAL(S("[[p(1)],[p(1),p(2)],[p(1),p(3)],[p(1),p(4)],[p(2)],[p(2),p(3)],[p(2),p(4)],[p(3)],[p(3),p(4)],[p(4)]]"), IO::to_string(solve("1#count{X:p(X):X=1..4}2.")));
    CPPUNIT_ASSERT_EQUAL(S("[[p(1)],[p(2)]]"), IO::to_string(solve("1#sum+{X:p(X):X=1..4}2.")));
    CPPUNIT_ASSERT_EQUAL(S("[[p(1)],[p(2)]]"), IO::to_string(solve("1#sum {X:p(X):X=1..4}2.")));
    CPPUNIT_ASSERT_EQUAL(S(
        "[[p(1)],[p(1),p(2)],[p(1),p(2),p(3)],[p(1),p(2),p(3),p(4)],[p(1),p(2),p(4)],[p(1),p(3)],[p(1),p(3),p(4)],[p(1),p(4)],"
        "[p(2)],[p(2),p(3)],[p(2),p(3),p(4)],[p(2),p(4)]]"),
        IO::to_string(solve("1#min{X:p(X):X=1..4}2.")));
    CPPUNIT_ASSERT_EQUAL(S("[[p(1)],[p(1),p(2)],[p(2)]]"), IO::to_string(solve("1#max{X:p(X):X=1..4}2.")));
    CPPUNIT_ASSERT_EQUAL(S("[[c,p]]"), IO::to_string(solve("{p}. 1 {c:p}.")));
}

void TestLparse::test_assign() {
    CPPUNIT_ASSERT_EQUAL(S("[[p,q(1)],[q(0)]]"),IO::to_string(solve("{p}. q(M):-M=#count{1:p}.")));
    CPPUNIT_ASSERT_EQUAL(S("[[p,q(1)],[q(0)]]"),IO::to_string(solve("{p}. q(M):-M=#sum+{1:p}.")));
    CPPUNIT_ASSERT_EQUAL(S("[[p,q(1)],[q(0)]]"),IO::to_string(solve("{p}. q(M):-M=#sum{1:p}.")));
    CPPUNIT_ASSERT_EQUAL(S("[[p,q(p)],[q(#sup)]]"),IO::to_string(solve("{p}. q(M):-M=#min{p:p}.")));
    CPPUNIT_ASSERT_EQUAL(S("[[p,q(p)],[q(#inf)]]"),IO::to_string(solve("{p}. q(M):-M=#max{p:p}.")));
    CPPUNIT_ASSERT_EQUAL(S(
        "[[p(1),p(2),q(1)],"
        "[p(1),p(3),q(1)],"
        "[p(1),p(4),q(1)],"
        "[p(2),p(3),q(2)],"
        "[p(2),p(4),q(2)],"
        "[p(3),p(4),q(3)]]"), 
        IO::to_string(solve("2{p(1..4)}2. q(M):-M=#min{X:p(X)}.")));
    CPPUNIT_ASSERT_EQUAL(S(
        "[[p(1),p(2),q(2)],"
        "[p(1),p(3),q(3)],"
        "[p(1),p(4),q(4)],"
        "[p(2),p(3),q(3)],"
        "[p(2),p(4),q(4)],"
        "[p(3),p(4),q(4)]]"), 
        IO::to_string(solve("2{p(1..4)}2. q(M):-M=#max{X:p(X)}.")));
}

void TestLparse::test_conjunction() {
    Gringo::Test::Messages msg;
    CPPUNIT_ASSERT_EQUAL(
        S("[]"),
        IO::to_string(solve(
            "a:-b:c.\n"
            "c:-a.\n")));
    CPPUNIT_ASSERT_EQUAL(S("[-:1:4-5: info: atom does not occur in any rule head:\n  b\n]"), IO::to_string(msg));
    CPPUNIT_ASSERT_EQUAL(
        S("[[a,b,c]]"),
        IO::to_string(solve(
            "a:-b:c.\n"
            "c:-a.\n"
            "b:-c.\n", {"a", "b", "c"})));
    CPPUNIT_ASSERT_EQUAL(
        S("[[a,b,c,d]]"),
        IO::to_string(solve(
            "a:-b:c,d.\n"
            "c:-a.\n"
            "d:-a.\n"
            "b:-c.\n"
            "b:-d.\n", {"a","b","c","d"})));
    CPPUNIT_ASSERT_EQUAL(
        S("[[a(1),a(2),a(3),c,q],[a(3)]]"),
        IO::to_string(solve(
            "{c}.\n"
            "a(1):-c.\n"
            "a(2):-c.\n"
            "a(3).\n"
            "q:-a(X):X=1..3.\n")));
    CPPUNIT_ASSERT_EQUAL(
        S("[[p],[q]]"),
        IO::to_string(solve(
            "p :- p:q.\n"
            "q :- q:p.\n")));
    CPPUNIT_ASSERT_EQUAL(
        S("[[p,q]]"),
        IO::to_string(solve(
            "p :- p:q.\n"
            "q :- q:p.\n"
            "p :- q.\n"
            "q :- p.\n")));
    CPPUNIT_ASSERT_EQUAL(
        S("[[],[p(1),r,s],[p(2),q,r],[p(3),r,s]]"),
        IO::to_string(solve(
            "{ p(1..3) } 1.\n"
            "q :- p(Y..X) : Y = 2, X = 2.\n"
            "r :- p(X) : #true.\n"
            "s :- p(X-1;2*(X..X+1)-3) : X=2.\n"
            )));
    CPPUNIT_ASSERT_EQUAL(
        S("[[d(a),d(b),q(1,a),q(2,a)],[d(a),d(b),q(1,b),q(2,b)]]"),
        IO::to_string(solve(
            "d(a;b).\n"
            "2 { q(1,a); q(1,b); q(2,a); q(2,b) } 2.\n"
            " :- q(1,A;2,A) : d(A).\n"
            )));
}
void TestLparse::test_disjunction() {
    CPPUNIT_ASSERT_EQUAL(
        S("[[],[a,b,c,d],[c,x,y]]"),
        IO::to_string(solve(
            "{ y; d } 1.\n"
            "c :- y.\n"
            "c :- d.\n"
            "b :- d.\n"
            "x:y | a:b :- c.\n"
            "b :- a.\n"
            )));
    CPPUNIT_ASSERT_EQUAL(
        S("[[a,b,c,d,x5]]"),
        IO::to_string(solve(
            "x5:-b.\n"
            "x5:-not c.\n"
            "d:-c.\n"
            "c:-d.\n"
            "\n"
            "x5|d:-not not b.\n"
            "a:-x5.\n"
            "\n"
            "c:-a.\n"
            "b:-c.\n"
            )));

    CPPUNIT_ASSERT_EQUAL(
        S("[[a,b,c],[b],[b,c],[c]]"),
        IO::to_string(solve(
            "1{b;c}.\n"
            "a:b,c;not a.\n")));
    CPPUNIT_ASSERT_EQUAL(
        S("[[],[p(1)],[p(1),p(2)],[p(2)]]"),
        IO::to_string(solve(
            "q(1..2).\n"
            "p(X); not p(X) :- q(X).\n", {"p("})));
    CPPUNIT_ASSERT_EQUAL(
        S("[[],[p(1),r(1)],[r(1)]]"),
        IO::to_string(solve(
            "q(1).\n"
            "p(X); not p(X); not r(X) :- q(X).\n"
            "r(X); not r(X) :- q(X).\n", {"p(", "r("})));
    CPPUNIT_ASSERT_EQUAL(
        S("[[a,aux,b,c,p,s_b1,s_b2,s_c2]]"),
        IO::to_string(solve(
            "aux :- { a; b; c } > 2.\n"

            "p :- aux.\n"
            "p :- not s_c2.\n"

            "s_b1 :- a.\n"
            "s_b1 :- b.\n"
            "a; b :- s_b1.\n"

            "s_b2 :- a, b.\n"
            "a :- s_b2.\n"
            "b :- s_b2.\n"

            "s_c2 :- c, s_b1.\n"
            "s_c2 :- s_b2.\n"
            "c; s_b2 :- s_c2.\n"
            "s_b1; s_b2 :- s_c2.\n"

            "p; s_c2 :- not not aux.\n"

            "a :- p.\n"
            "b :- p.\n"
            "c :- p.\n"

            "p :- a.\n"
            "p :- b.\n"
            "p :- c.\n")));
    CPPUNIT_ASSERT_EQUAL(
        S("[[p(1,a),p(2,a)],[p(1,b),p(2,b)]]"),
        IO::to_string(solve(
            "d(a;b).\n"
            "p(1,A;2,A) : d(A).\n", {"p("})));
    CPPUNIT_ASSERT_EQUAL(
        S("[[],[p(1),p(2)]]"),
        IO::to_string(solve(
            "r(1).\n"
            "q(1,2).\n"
            "p(1..Y) : q(X,Y) | not p(1..Y) : q(X,Y) :- r(X).\n", {"p("})));
}

void TestLparse::test_show() {
    CPPUNIT_ASSERT_EQUAL(
        S(
            "["
            "[(1,2,3),-q(1),42],"
            "[(1,2,3),-q(1),42],"
            "[(1,2,3),-q(1),42,p(1)],"
            "[(1,2,3),42],"
            "[(1,2,3),42],"
            "[(1,2,3),42,boo(1)],"
            "[(1,2,3),42,boo(1)],"
            "[(1,2,3),42,boo(1),p(1)],"
            "[(1,2,3),42,p(1)]"
            "]"),
        IO::to_string(solve(
            "#show p/1.\n"
            "#show -q/1.\n"
            "#show boo(X):q(X).\n"
            "#show -p/-1.\n"
            "#show (1,2,3).\n"
            "\n"
            "{p(1); q(1); -p(1); -q(1)}.\n"
            "\n"
            "#const p=42.\n")));
    CPPUNIT_ASSERT_EQUAL(
        S(
            "[[a,c,x=1,y=1]]"),
        IO::to_string(solve(
            "a. b.\n"
            "$x $= 1. $y $= 1. $z $= 1.\n"
            "#show a/0.\n"
            "#show c.\n"
            "#show $x/0.\n"
            "#show $y.\n"
            )));
    CPPUNIT_ASSERT_EQUAL(
        S(
            "[[x=1],[y=1]]"),
        IO::to_string(solve(
            "{b}.\n"
            "$x $= 1. $y $= 1.\n"
            "#show.\n"
            "#show $x:b.\n"
            "#show $y:not b.\n"
            )));
}

void TestLparse::test_aggregates() {
    CPPUNIT_ASSERT_EQUAL(
        S(
            "["
            "[]"
            "]"),
        IO::to_string(solve(
            "#sum { 1:b; 2:c } < 1.\n"
            )));
    CPPUNIT_ASSERT_EQUAL(S("[[p(1),p(2)],[p(1),p(3)],[p(2),p(3)]]"), IO::to_string(solve("{p(1..3)}.\n:-{p(X)}!=2.")));
    CPPUNIT_ASSERT_EQUAL(S("[[],[a,b],[b]]"), IO::to_string(solve("#sum { -1:a; 1:b } >= 0.")));
    CPPUNIT_ASSERT_EQUAL(S("[[],[a,b],[b]]"), IO::to_string(solve("#sum { 1:a; 2:b } != 1.")));
    CPPUNIT_ASSERT_EQUAL(S("[]"), IO::to_string(solve("a. {a} 0.")));
}

void TestLparse::test_invert() {
    std::string prg = 
        "p(-1;2).\n"
        "q(X) :- p((-(X+3))+2).\n"
        "a(a;-b).\n"
        "b(X) :- a(---X).\n"
        "c(X) :- a(----X).\n";
    CPPUNIT_ASSERT_EQUAL(S("[[a(-b),a(a),b(-a),b(b),c(-b),c(a),p(-1),p(2),q(-3),q(0)]]"), IO::to_string(solve(S(prg))));
}

void TestLparse::test_minimize() {
    std::string prg;
    prg =
        "{a; b; c; d}.\n"
        "#minimize {1,a:a; 1,b:b; 1,c:c; 1,d:d}.\n"
        "ok :- a,    b,not c,not d.\n"
        "ok :- not a,b,    c,    d.\n"
        "ok :- a,not b,    c,    d.\n"
        ":- not ok.\n";
    CPPUNIT_ASSERT_EQUAL(S("[[a,b]]"), IO::to_string(solve(S(prg), {"a", "b", "c", "d"}, {2})));
    CPPUNIT_ASSERT_EQUAL(S("[]"), IO::to_string(solve(S(prg), {"a", "b", "c", "d"}, {1})));
    prg =
        "{a; b; c; d}.\n"
        ":~ a. [1,a]\n"
        ":~ b. [1,b]\n"
        ":~ c. [1,c]\n"
        ":~ d. [1,d]\n"
        "ok :- a,    b,not c,not d.\n"
        "ok :- not a,b,    c,    d.\n"
        "ok :- a,not b,    c,    d.\n"
        ":- not ok.\n";
    CPPUNIT_ASSERT_EQUAL(S("[[a,b]]"), IO::to_string(solve(S(prg), {"a", "b", "c", "d"}, {2})));
    CPPUNIT_ASSERT_EQUAL(S("[]"), IO::to_string(solve(S(prg), {"a", "b", "c", "d"}, {1})));
    prg =
        "{a; b; c; d}.\n"
        "#maximize {-1,a:a; -1,b:b; -1,c:c; -1,d:d}.\n"
        "ok :- a,    b,not c,not d.\n"
        "ok :- not a,b,    c,    d.\n"
        "ok :- a,not b,    c,    d.\n"
        ":- not ok.\n";
    CPPUNIT_ASSERT_EQUAL(S("[[a,b]]"), IO::to_string(solve(S(prg), {"a", "b", "c", "d"}, {2})));
    CPPUNIT_ASSERT_EQUAL(S("[]"), IO::to_string(solve(S(prg), {"a", "b", "c", "d"}, {1})));
    prg = 
        "{a; b; c; d}.\n"
        "#minimize {3,a:a; 3,b:b; 1,x:c; 1,x:d}.\n"
        "ok :- a,    b,not c,not d.\n"
        "ok :- not a,b,    c,    d.\n"
        "ok :- a,not b,    c,    d.\n"
        ":- not ok.\n"
        ;
    CPPUNIT_ASSERT_EQUAL(S("[[a,c,d],[b,c,d]]"), IO::to_string(solve(S(prg), {"a", "b", "c", "d"}, {4})));
    CPPUNIT_ASSERT_EQUAL(S("[]"), IO::to_string(solve(S(prg), {"a", "b", "c", "d"}, {3})));
    prg = 
        "{a; b; c; d}.\n"
        "#minimize {3@2,a:a; 3@2,b:b; 1@2,x:c; 1@2,x:d}.\n"
        "#minimize {1@1,x:a; 1@1,x:c; 1@1,y:b}.\n"
        "ok :- a,    b,not c,not d.\n"
        "ok :- not a,b,    c,    d.\n"
        "ok :- a,not b,    c,    d.\n"
        ":- not ok.\n"
        ;
    CPPUNIT_ASSERT_EQUAL(S("[[a,c,d]]"), IO::to_string(solve(S(prg), {"a", "b", "c", "d"}, {4, 1})));
    CPPUNIT_ASSERT_EQUAL(S("[]"), IO::to_string(solve(S(prg), {"a", "b", "c", "d"}, {4, 0})));
    CPPUNIT_ASSERT_EQUAL(S("[[]]"), IO::to_string(solve(S("{p}. #maximize{1:not p}."), {"p"}, {0})));
}

void TestLparse::test_csp() {
    CPPUNIT_ASSERT_EQUAL(
        S("[[p(1)=1,p(2)=1,x=1],[p(1)=1,p(2)=2,x=1],[p(1)=2,p(2)=1,x=1],[p(1)=2,p(2)=2,x=1]]"),
        IO::to_string(solve(
            "1 $<= $p(1..2) $<= 2.\n"
            "$x $= 1.\n"
            )));
    CPPUNIT_ASSERT_EQUAL(
        S("[[x=0,y=0,z=2],[x=0,y=0,z=3],[x=0,y=1,z=3],[x=1,y=0,z=3]]"),
        IO::to_string(solve(
            "0 $<= $(x;y;z) $<= 3.\n"
            "$x $+ $y $+ -1$*$z $<= -2.\n"
            )));
    CPPUNIT_ASSERT_EQUAL(
        S("[[x=0,y=0,z=2],[x=0,y=0,z=3],[x=0,y=1,z=3],[x=1,y=0,z=3]]"),
        IO::to_string(solve(
            "0 $<= $(x;y;z) $<= 3.\n"
            "p:-$x $+ $y $+ -1$*$z $<= -2.\n"
            ":- not p.\n", {"x", "y", "z"}
            )));
}

void TestLparse::test_cspbound() {
    CPPUNIT_ASSERT_EQUAL(
        S("[[x=4],[x=5]]"),
        IO::to_string(solve(
            "$x $<= 5.\n"
            ":- $x $<= 3, $x $<=4.\n", {"x="}
            )));
}

void TestLparse::test_disjoint() {
    CPPUNIT_ASSERT_EQUAL(
        S("[[x=2]]"),
        IO::to_string(solve(
            "1 $<= $x $<= 2.\n"
            "#disjoint{ 1:1; 2:$x }.\n"
            )));
    CPPUNIT_ASSERT_EQUAL(
        S("[[a,x=1],[b,x=1],[x=1]]"),
        IO::to_string(solve(
            "$x $= 1.\n"
            "{ a; b }.\n"
            "#disjoint{ a:$x:a; b:$x:b }.\n"
            )));
    CPPUNIT_ASSERT_EQUAL(
        S("[[a,b,y=2],[a,y=2],[b,y=2],[y=1],[y=2]]"),
        IO::to_string(solve(
            "1 $<= $y $<= 2.\n"
            "{ a; b }.\n"
            "#disjoint{ 1:1:a; 1:1:b; 2:$y }.\n"
            )));
    CPPUNIT_ASSERT_EQUAL(
        S("[[p(1)=1,p(2)=1,p(3)=1,q(1)=2,q(2)=2,q(3)=2],[p(1)=2,p(2)=2,p(3)=2,q(1)=1,q(2)=1,q(3)=1]]"),
        IO::to_string(solve(
            "1 $<= $(p(1..3);q(1..3)) $<= 2.\n"
            "#disjoint{ 1:$p(1..3); 2:$q(1..3) }.\n"
            )));
    CPPUNIT_ASSERT_EQUAL(
        S("[[x=6,y=35]]"),
        IO::to_string(solve(
            "6  $<= $x $<=  7.\n"
            "35 $<= $y $<= 36.\n"
            "not #disjoint{ 1:6$*$y; 2:35$*$x }.\n"
            )));
    CPPUNIT_ASSERT_EQUAL(
        S(
            "[[x=1,y=1,z=1]"
            ",[x=2,y=2,z=2]"
            ",[x=3,y=3,z=3]]"),
        IO::to_string(solve(
            "1  $<= $(x;y;z) $<=  3.\n"
            "not #disjoint{ 1:2$*$x $+ 3$*$y; 2:2$*$y $+ 3$*$z; 3:2$*$z $+ 3$*$x }.\n"
            )));
    CPPUNIT_ASSERT_EQUAL(
        S("[[x=6,y=35]]"),
        IO::to_string(solve(
            "6  $<= $x $<=  7.\n"
            "35 $<= $y $<= 36.\n"
            "not #disjoint{ 1:6$*$y; 2:35$*$x }.\n"
            )));
}

void TestLparse::test_queens() {
    CPPUNIT_ASSERT_EQUAL(
        S("[[q(1,2),q(2,4),q(3,6),q(4,1),q(5,3),q(6,5)],"
          "[q(1,3),q(2,6),q(3,2),q(4,5),q(5,1),q(6,4)],"
          "[q(1,4),q(2,1),q(3,5),q(4,2),q(5,6),q(6,3)],"
          "[q(1,5),q(2,3),q(3,1),q(4,6),q(5,4),q(6,2)]]"),
        IO::to_string(solve(
            "#const n = 6.\n"
            "n(1..n).\n"
            "\n"
            "q(X,Y) :- n(X), n(Y), not not q(X,Y).\n"
            "\n"
            "        c(r,X; c,Y) :- q(X,Y).\n"
            "not not c(r,N; c,N) :- n(N).\n"
            "\n"
            "n(r,X,Y-1,X,Y; c,X-1,Y,X,Y; d1,X-1,Y-1,X,Y;     d2,X-1,Y+1,X,Y      ) :- n(X), n(Y).\n"
            "c(r,N,0;       c,0,N;       d1,N-1,0; d1,0,N-1; d2,N-1,n+1; d2,0,N+1) :- n(N).\n"
            "\n"
            "c(C,XX,YY) :-     c(C,X,Y), n(C,X,Y,XX,YY), not q(XX,YY).\n"
            "           :- not c(C,X,Y), n(C,X,Y,XX,YY),     q(XX,YY).\n", {"q("})));
    CPPUNIT_ASSERT_EQUAL( 
        48,
        (int)solve(
            "#const n=4.\n"
            "1 $<= $(row(X);col(X)) $<= n :- X=1..n.\n"
            "$row(X) $!= $row(Y) :- X=1..n, Y=1..n, X<Y.\n"
            "$col(X) $!= $col(Y) :- X=1..n, Y=1..n, X<Y.\n"
            "$row(X) $+ $col(Y) $!= $row(Y) $+ $col(X) :- X=1..n, Y=1..n, X<Y.\n"
            "$row(X) $+ $col(X) $!= $row(Y) $+ $col(Y) :- X=1..n, Y=1..n, X<Y.\n"
            ).size());
    S q5 = 
        "[[q(1)=1,q(2)=3,q(3)=5,q(4)=2,q(5)=4]"
        ",[q(1)=1,q(2)=4,q(3)=2,q(4)=5,q(5)=3]"
        ",[q(1)=2,q(2)=4,q(3)=1,q(4)=3,q(5)=5]"
        ",[q(1)=2,q(2)=5,q(3)=3,q(4)=1,q(5)=4]"
        ",[q(1)=3,q(2)=1,q(3)=4,q(4)=2,q(5)=5]"
        ",[q(1)=3,q(2)=5,q(3)=2,q(4)=4,q(5)=1]"
        ",[q(1)=4,q(2)=1,q(3)=3,q(4)=5,q(5)=2]"
        ",[q(1)=4,q(2)=2,q(3)=5,q(4)=3,q(5)=1]"
        ",[q(1)=5,q(2)=2,q(3)=4,q(4)=1,q(5)=3]"
        ",[q(1)=5,q(2)=3,q(3)=1,q(4)=4,q(5)=2]]";
    CPPUNIT_ASSERT_EQUAL( 
        S(q5),
        IO::to_string(solve(
            "#const n=5.\n"
            "1 $<= $q(1..n) $<= n.\n"
            "$q(X) $!= $q(Y) :- X=1..n, Y=1..n, X<Y.\n"
            "X $+ $q(Y) $!= Y $+ $q(X) :- X=1..n, Y=1..n, X<Y.\n"
            "X $+ $q(X) $!= Y $+ $q(Y) :- X=1..n, Y=1..n, X<Y.\n"
            )));
    CPPUNIT_ASSERT_EQUAL( 
        S(q5),
        IO::to_string(solve(
            "#const n=5.\n"
            "1 $<= $q(1..n) $<= n.\n"
            "#disjoint { X : $q(X)$+0 : X=1..n }.\n"
            "#disjoint { X : $q(X)$+X : X=1..n }.\n"
            "#disjoint { X : $q(X)$-X : X=1..n }.\n"
            )));
    CPPUNIT_ASSERT_EQUAL( 
        S(q5),
        IO::to_string(solve(
            "#const n=5.\n"
            "1 $<= $q(1..n) $<= n.\n"
            "#disjoint { X : $q(X)     : X=1..n }.\n"
            ":- not #disjoint { X : $q(X)$+ X : X=1..n }.\n"
            "not not #disjoint { X : $q(X)$+-X : X=1..n }.\n"
            )));
}

void TestLparse::test_python() {
#ifdef WITH_PYTHON
    CPPUNIT_ASSERT_EQUAL(
        S(
            "["
            "[p(39),q(\"a\"),q(1),q(2),q(a),r(2),r(3),s((1,2)),s((1,3)),s((2,1))]"
            "]"),
        IO::to_string(solve(
            "#script (python)\n"
            "import gringo\n"
            "def gcd(a, b): return b if a == 0 else gcd(b % a, a)\n"
            "def test():    return [1, 2, gringo.Fun(\"a\"), \"a\"]\n"
            "def match():   return [(1,2),(1,3),(2,1)]\n"
            "#end.\n"
            "\n"
            "p(@gcd(2*3*7*13,3*11*13)).\n"
            "q(@test()).\n"
            "r(X) :- (1,X)=@match().\n"
            "s(X) :- X=@match().\n"
            )));
#endif // WITH_PYTHON
}

void TestLparse::test_lua() {
#ifdef WITH_LUA
    CPPUNIT_ASSERT_EQUAL(
        S(
            "["
            "[p(39),q(\"a\"),q(1),q(2),q(a),r(2),r(3),s((1,2)),s((1,3)),s((2,1))]"
            "]"),
        IO::to_string(solve(
            "#script (lua)\n"
            "function gcd(a, b) if a == 0 then return b else return gcd(b % a, a) end end\n"
            "function test()    return {1, 2, gringo.Fun(\"a\"), \"a\"} end\n"
            "function match()   return {gringo.Tuple({1,2}),gringo.Tuple({1,3}),gringo.Tuple({2,1})} end\n"
            "#end.\n"
            "\n"
            "p(@gcd(2*3*7*13,3*11*13)).\n"
            "q(@test()).\n"
            "r(X) :- (1,X)=@match().\n"
            "s(X) :- X=@match().\n"
            )));
#endif // WITH_LUA
}

void TestLparse::test_undefinedRule() {
    Gringo::Test::Messages msg;
    CPPUNIT_ASSERT_EQUAL(
        S("[[q(2)]]"),
        IO::to_string(solve(
            "a(a).\n"
            "a(1).\n"
            "b(1).\n"
            "q(A+B) :- a(A), b(B).\n", {"q("})));
    CPPUNIT_ASSERT_EQUAL(S("[-:4:3-6: info: operation undefined:\n  (A+B)\n]"), IO::to_string(msg.messages));
    msg.clear();
    CPPUNIT_ASSERT_EQUAL(
        S("[[q(1,1,2)]]"),
        IO::to_string(solve(
            "a(a).\n"
            "a(1).\n"
            "b(1).\n"
            "q(A,B,X) :- a(A), b(B), X=A+B.\n", {"q("})));
        S("[[q(1,1,2)]]"),
    CPPUNIT_ASSERT_EQUAL(S("[-:4:27-30: info: operation undefined:\n  (A+B)\n]"), IO::to_string(msg.messages));
    msg.clear();
    CPPUNIT_ASSERT_EQUAL(
        S("[[q(1,1)]]"),
        IO::to_string(solve(
            "a(a).\n"
            "a(1).\n"
            "b(1).\n"
            "q(A,B) :- a(A), b(B), not b(A+B).\n", {"q("})));
    CPPUNIT_ASSERT_EQUAL(S("[-:4:29-32: info: operation undefined:\n  (A+B)\n]"), IO::to_string(msg.messages));
    msg.clear();
    CPPUNIT_ASSERT_EQUAL(
        S("[[q(1)]]"),
        IO::to_string(solve(
            "a(a).\n"
            "a(1).\n"
            "b(1).\n"
            "q(A..B) :- a(A), b(B).\n", {"q("})));
    CPPUNIT_ASSERT_EQUAL(S("[-:4:3-7: info: interval undefined:\n  A..B\n]"), IO::to_string(msg.messages));
    msg.clear();
}

void TestLparse::test_undefinedCSP() {
    Gringo::Test::Messages msg;
    CPPUNIT_ASSERT_EQUAL(
        S("[[a(1),a(a),b(1)]]"),
        IO::to_string(solve(
            "a(a).\n"
            "a(1).\n"
            "b(1).\n"
            "A $<= B :- a(A), b(B).\n")));
    CPPUNIT_ASSERT_EQUAL(S("[-:4:1-2: info: number expected:\n  A\n]"), IO::to_string(msg.messages));
    msg.clear();
    CPPUNIT_ASSERT_EQUAL(
        S("[[a(1),a(a),b(1)]]"),
        IO::to_string(solve(
            "a(a).\n"
            "a(1).\n"
            "b(1).\n"
            ":- a(A), b(B), A $< B.\n")));
    CPPUNIT_ASSERT_EQUAL(S("[-:4:16-17: info: number expected:\n  A\n]"), IO::to_string(msg.messages));
    msg.clear();
    CPPUNIT_ASSERT_EQUAL(
        S("[[a(1),a(2)=1,a(a),b(1)]]"),
        IO::to_string(solve(
            "a(a).\n"
            "a(1).\n"
            "b(1).\n"
            "$a(2) $<= 1.\n"
            ":- a(A), b(B), $a(A+1) $< B.\n")));
    CPPUNIT_ASSERT_EQUAL(S("[-:5:19-20: info: operation undefined:\n  (1*A+1)\n]"), IO::to_string(msg.messages));
    msg.clear();
}

void TestLparse::test_undefinedCSPDisjoint() {
    Gringo::Test::Messages msg;
    CPPUNIT_ASSERT_EQUAL(
        S("[[a(1),a(a)]]"),
        IO::to_string(solve(
            "a(a).\n"
            "a(1).\n"
            "#disjoint { X : X : a(X) }.\n")));
    CPPUNIT_ASSERT_EQUAL(S("[-:3:17-18: info: number expected:\n  X\n]"), IO::to_string(msg.messages));
    msg.clear();
    CPPUNIT_ASSERT_EQUAL(
        S("[[a(1),a(1)=1,a(a)]]"),
        IO::to_string(solve(
            "a(a).\n"
            "a(1).\n"
            "$a(1) $= 1.\n"
            "#disjoint { X : $a(X+1) : a(X) }.\n")));
    CPPUNIT_ASSERT_EQUAL(S("[-:4:20-21: info: operation undefined:\n  (1*X+1)\n]"), IO::to_string(msg.messages));
    msg.clear();
}

void TestLparse::test_undefinedBodyAggregate() {
    Gringo::Test::Messages msg;
    CPPUNIT_ASSERT_EQUAL(
        S("[[a(1),a(a),h]]"),
        IO::to_string(solve(
            "a(a).\n"
            "a(1).\n"
            "h :- #count { X+1 : a(X) } < 2.\n")));
    CPPUNIT_ASSERT_EQUAL(S("[-:3:15-16: info: operation undefined:\n  (1*X+1)\n]"), IO::to_string(msg.messages));
    msg.clear();
    CPPUNIT_ASSERT_EQUAL(
        S("[[a(1),a(a),h]]"),
        IO::to_string(solve(
            "a(a).\n"
            "a(1).\n"
            "h :- { not a(X+1) : a(X) } < 2.\n")));
    CPPUNIT_ASSERT_EQUAL(S("[-:3:14-15: info: operation undefined:\n  (1*X+1)\n]"), IO::to_string(msg.messages));
    msg.clear();
    CPPUNIT_ASSERT_EQUAL(
        S("[[a(1),a(a),g(1)]]"),
        IO::to_string(solve(
            "a(a).\n"
            "a(1).\n"
            "g(X) :- not X < #count { } < X+1, a(X).\n"
            "h(X) :-     X < #count { } < X+1, a(X).\n")));
    CPPUNIT_ASSERT_EQUAL(S("[-:3:30-31: info: operation undefined:\n  (1*X+1)\n,-:4:30-31: info: operation undefined:\n  (1*X+1)\n]"), IO::to_string(msg.messages));
}

void TestLparse::test_undefinedAssignmentAggregate() {
    Gringo::Test::Messages msg;
    CPPUNIT_ASSERT_EQUAL(
        S("[[a(1),a(a),h(1)]]"),
        IO::to_string(solve(
            "a(a).\n"
            "a(1).\n"
            "h(C) :- C = #count { X+1 : a(X) }.\n")));
    CPPUNIT_ASSERT_EQUAL(S("[-:3:22-23: info: operation undefined:\n  (1*X+1)\n]"), IO::to_string(msg.messages));
    msg.clear();
    CPPUNIT_ASSERT_EQUAL(
        S("[[a(1),a(a),h(1)]]"),
        IO::to_string(solve(
            "a(a).\n"
            "a(1).\n"
            "h(C) :- C = { not a(X+1) : a(X) }.\n")));
    CPPUNIT_ASSERT_EQUAL(S("[-:3:21-22: info: operation undefined:\n  (1*X+1)\n]"), IO::to_string(msg.messages));
    msg.clear();
    CPPUNIT_ASSERT_EQUAL(
        S("[[]]"),
        IO::to_string(solve(
            "h(C) :- C+1 = #min { a }.\n")));
    CPPUNIT_ASSERT_EQUAL(S("[]"), IO::to_string(msg.messages));
    msg.clear();
}

void TestLparse::test_undefinedHeadAggregate() {
    Gringo::Test::Messages msg;
    CPPUNIT_ASSERT_EQUAL(
        S("[[a(1),a(a)],[a(1),a(a),p(1)]]"),
        IO::to_string(solve(
            "a(a).\n"
            "a(1).\n"
            "#count { X+1 : p(X) : a(X) }.\n")));
    CPPUNIT_ASSERT_EQUAL(S("[-:3:10-11: info: operation undefined:\n  (1*X+1)\n]"), IO::to_string(msg.messages));
    msg.clear();
    CPPUNIT_ASSERT_EQUAL(
        S("[[a(1),a(a)],[a(1),a(a),p(2)]]"),
        IO::to_string(solve(
            "a(a).\n"
            "a(1).\n"
            "#count { X : p(X+1) : a(X) }.\n")));
    CPPUNIT_ASSERT_EQUAL(S("[-:3:16-17: info: operation undefined:\n  (1*X+1)\n]"), IO::to_string(msg.messages));
    msg.clear();
    CPPUNIT_ASSERT_EQUAL(
        S("[[a(1),a(a)],[a(1),a(a),p(2)]]"),
        IO::to_string(solve(
            "a(a).\n"
            "a(1).\n"
            "{ p(X+1) : a(X) }.\n")));
    CPPUNIT_ASSERT_EQUAL(S("[-:3:5-6: info: operation undefined:\n  (1*X+1)\n]"), IO::to_string(msg.messages));
    msg.clear();
    CPPUNIT_ASSERT_EQUAL(
        S("[[p(1)]]"),
        IO::to_string(solve(
            "a(a).\n"
            "a(1).\n"
            "X-1 < { p(X) } < X+1 :- a(X).\n", {"p("})));
    CPPUNIT_ASSERT_EQUAL(S("[-:3:1-2: info: operation undefined:\n  (1*X+-1)\n]"), IO::to_string(msg.messages));
}

void TestLparse::test_undefinedConjunction() {
    Gringo::Test::Messages msg;
    CPPUNIT_ASSERT_EQUAL(
        S("[[a(a)],[h]]"),
        IO::to_string(solve(
            "{a(a)}.\n"
            "a(1..2).\n"
            "p(2..3).\n"
            "h :- p(1+A):a(A).\n", {"h", "a(a)"})));
    CPPUNIT_ASSERT_EQUAL(S("[-:4:10-11: info: operation undefined:\n  (1*A+1)\n]"), IO::to_string(msg.messages));
    msg.clear();
    CPPUNIT_ASSERT_EQUAL(
        S("[[a(a)],[a(a),p(2),p(3)],[h],[p(2),p(3)]]"),
        IO::to_string(solve(
            "{a(a)}.\n"
            "a(1..2).\n"
            "{p(2..3)} != 1.\n"
            "h :- not p(1+A):a(A).\n", {"h", "a(a)", "p("})));
    CPPUNIT_ASSERT_EQUAL(S("[-:4:14-15: info: operation undefined:\n  (1*A+1)\n]"), IO::to_string(msg.messages));
    msg.clear();
    CPPUNIT_ASSERT_EQUAL(
        S("[[a(a),h],[a(a),p(2),p(3)],[h],[p(2),p(3)]]"),
        IO::to_string(solve(
            "{a(a)}.\n"
            "a(1..2).\n"
            "{p(2..3)} != 1.\n"
            "h :- not p(X):a(A),X=1+A.\n", {"h", "a(a)", "p("})));
    CPPUNIT_ASSERT_EQUAL(S("[-:4:24-25: info: operation undefined:\n  (1*A+1)\n]"), IO::to_string(msg.messages));
}

void TestLparse::test_undefinedDisjunction() {
    Gringo::Test::Messages msg;
    CPPUNIT_ASSERT_EQUAL(
        S("[[]]"),
        IO::to_string(solve(
            "a(a).\n"
            "a(1..2).\n"
            "p(1+A):a(A).\n", {"p("})));
    CPPUNIT_ASSERT_EQUAL(S("[-:3:5-6: info: operation undefined:\n  (1*A+1)\n]"), IO::to_string(msg.messages));
    msg.clear();
    CPPUNIT_ASSERT_EQUAL(
        S("[[p(2)],[p(3)]]"),
        IO::to_string(solve(
            "a(a).\n"
            "a(1..2).\n"
            "p(X):a(A),X=1+A.\n", {"p("})));
    CPPUNIT_ASSERT_EQUAL(S("[-:3:15-16: info: operation undefined:\n  (1*A+1)\n]"), IO::to_string(msg.messages));
    msg.clear();
}

void TestLparse::test_undefinedScript() {
    Gringo::Test::Messages msg;
    CPPUNIT_ASSERT_EQUAL(
        S("[[]]"),
        IO::to_string(solve(
            "a(@failure()).\n")));
    CPPUNIT_ASSERT_EQUAL(S("[-:1:3-13: info: operation undefined:\n  function 'failure' not found\n]"), IO::to_string(msg.messages));
    msg.clear();
}

void TestLparse::test_minMax() {
    CPPUNIT_ASSERT_EQUAL(
        S("[[a],[b]]"),
        IO::to_string(solve(
            "a :- #min { 1:a; 2:b } != 2.\n"
            "b :- #max { 1:a; 2:b } != 1.\n")));
    CPPUNIT_ASSERT_EQUAL(
        S("[[a,b]]"),
        IO::to_string(solve(
            "a :- b.\n"
            "b :- a.\n"
            "a :- #min { 1:a; 2:b } != 2.\n"
            "b :- #max { 1:a; 2:b } != 1.\n")));
    CPPUNIT_ASSERT_EQUAL(
        S("[[a,b,c,w,z],[a,b,w,z],[a,c,w,z],[a,w,z],[b,c,x,y,z],[b,x,y,z],[c,w,y],[w,y]]"),
        IO::to_string(solve(
            "{a;b;c}.\n"
            "w :- #min { 1:a;2:b;3:c } != 2.\n"
            "x :- #min { 1:a;2:b;3:c } = 2.\n"
            "y :- #min { 1:a;2:b;3:c } >= 2.\n"
            "z :- #min { 1:a;2:b;3:c } <= 2.\n")));
}

void TestLparse::test_nonmon() {
    CPPUNIT_ASSERT_EQUAL(
        S("[[true(e,x1,1)]]"),
        IO::to_string(solve(
            "{ true(e,L,C) } :- var(e,L,C).\n"
            "true(a,L,C) :- unequal, var(a,L,C).\n"
            "unequal :- int(V), #sum{ C,Q,L : true(Q,L,C) } != V.\n"
            ":- not unequal.\n"

            "var(e,x1,1).\n"
            "var(e,x2,2).\n"
            "var(a,y1,2).\n"
            "var(a,y2,3).\n"
            "int(5).\n", {"true(e"})));
    CPPUNIT_ASSERT_EQUAL(
        S("[[true(e,x1,1)]]"),
        IO::to_string(solve(
            "{ true(e,L,C) } :- var(e,L,C).\n"
            "true(a,L,C) :- unequal, var(a,L,C).\n"
            "unequal :- int(V), #sum{ C,Q,L : true(Q,L,C) } != V.\n"
            ":- not unequal.\n"

            "var(e,x1,1).\n"
            "var(e,x2,2).\n"
            "var(a,y1,2).\n"
            "var(a,y2,3).\n"
            "int(5).\n", {"true(e"})));

    CPPUNIT_ASSERT_EQUAL(
        S("[]"),
        IO::to_string(solve(
            "{ true(e,L,C) } :- var(e,L,C).\n"
            "true(a,L,C) :- unequal, var(a,L,C).\n"
            "unequal :- int(V), #sum{ C,Q,L : true(Q,L,C) } != V.\n"
            ":- not unequal.\n"

            "var(e,x1,2).\n"
            "var(e,x2,4).\n"
            "var(a,y1,-2).\n"
            "var(a,y2,4).\n"
            "int(4).\n", {"true(e"})));

    CPPUNIT_ASSERT_EQUAL(
        S("[[],[true(e,x1,6),true(e,x2,3)]]"),
        IO::to_string(solve(
            "{ true(e,L,C) } :- var(e,L,C).\n"
            "true(a,L,C) :- unequal, var(a,L,C).\n"
            "unequal :- int(V), #sum{ C,Q,L : true(Q,L,C) } != V.\n"
            ":- not unequal.\n"

            "var(e,x1,6).\n"
            "var(e,x2,3).\n"
            "var(a,y1,-2).\n"
            "var(a,y2,1).\n"
            "int(4).\n", {"true(e"})));
}

void TestLparse::test_bugRewriteCond() {
    CPPUNIT_ASSERT_EQUAL(
        S("[[p(1)],[p(1),p(2)],[p(2)]]"),
        IO::to_string(solve(
            "{p(1..2)}.\n"
            ":- #false:p(X).\n", {"p("})));
}

TestLparse::~TestLparse() { }

// }}}

CPPUNIT_TEST_SUITE_REGISTRATION(TestLparse);

} } } // namespace Test Output Gringo

