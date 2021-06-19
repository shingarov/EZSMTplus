/*
 * File main.cc 
 * Last modified on 2 19:34 2002
 * By Yuliya Babovich 
 *
 */

// Copyright 1998 by Patrik Simons
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston,
// MA 02111-1307, USA.
//
// Patrik.Simons@hut.fi

//3.53 version Aug 24, 05 removed hend1,pend1,nend1 from the rules
//that used to be copies of hen,pend,nend
//
//3.54 version Aug 25, 05:
//(1) bug at buildingLoopFormulas fixed so it could cause 
//problems for nontight programs
//(2) bug with rules ...v A v...:-..A... not being remove is fixed
//3.55 version Sep 21, 05:
//(1) Zchaff version 2004.11.15 is incorporated in place of Zchaff 2003
//(2) Cmodels is made compatable with gcc3.4/4.* thanks too Wolfgang Faber
//3.57 Includes
//        HCF check        
//        modularity in model checking
//        loopformula by default
//3.58 fixes the bug with loopformula from now on for disj programs only part of loop formula is added where head atom is sat
//    
//3.59 includes -eloop flag that performs elementary set computation
// and adds loop formulas of elementary loops when needed
// 
//3.60 tightness bug fix through away SCC of 1 element
//     now all the unsat loops are added 
//
//3.6x    Ilkka's circuit component's 
//        where implemented -bc flag is used plus bcircuit program
//        must be present  
//        impl is preliminary hence output is not readable
//        no positive time results were gained
//        as Cmodels CNF representation is more compact and seem to 
//        be better suit
//3.63 Datastructures added to support linear time WFM, and Reduct
//
//3.64-65 Bug fixes at WFM
//
//3.66 Bug fixes Zchaff to be portable into 64 bit archetecture
// Thanks to Yinlei Yu: 
// zchaff_dbase.cpp, line 324. Replacing "int displacement" with
//"long long displacement"
//3.67 Bug fixes Zchaff to be portable into 64 bit archetecture
// Thanks to Andre Neumann: on 64 bit archetecture UNSAT when SAT, due to no 
// return true; in the end of createClauses 
// zchaff_dbase.cpp, line 324. Replacing "int displacement" with
//"long long displacement"
// 
// 3.68 Esra's bug fix that could occur on constraint rules translation
// where new_atom was created instead of a false atom 
// + improved simplification
// + Added flag -ms that allows calling MINISAT
//    Minisat is now incorporated into Cmodels,
//    its code has been added as part of Cmodels
//    two functions added to SimpSolver to allow
//    adding clauses from cmodels, and getting feedback from a solver
//    once solution is found
//    Minisat and Zchaff communication is alike 
// + bug fixes for choice rules as {k}:-k.
// + bug fix with Weights that were not allowed to drop below negative value 
// version 3.69 
// + replaced zChaff version from 2004 with zChaff 2007.3.12
// + added check on what addClause from Minisat returns, since when
//   it returns false the instance is UNSAT.
// + -zca flag added only for comparison reasons of incremental SAT solving and 
//   assat method
// cmodels 3.70
// Esra's bug Fix
// WFM bug
//void ConstraintRule::HeadInOneRule(Atom* at)
//void     WeightRule::HeadInOneRule(Atom* at){
//some of the atoms could have been assigned values by now
//for instance a:-1{falseassigned1, notassigned}
//in such case only notassigned should be assigned computetrue
//Such bug could not appear in Basic, choice or disj rules
//
//
//3.71 improvement of clausification by not inroducing aux atom
//when -a occurs in the body i.e, avoiding intoruding clauses for aux_1==-a 
//+fixing intro two aux atoms for the same room
//
//3.73 atmost is computed (wfs--complete); sorting is introduced to disregard
//  same bodies in rules, and also the rules that are subsummed by others
//
//3.74 bug fixes due to Benjamin Kaufmann
// (i) weightrule Atmost computation
// (ii) completionchoice 
// (iii) weight rule elimination inefficiency in translation is removed 
//      (compare worked unproperly) cmodels worked correctly but inefficiently
//3.75 bug fix due to  Christian Drescher drescher@cs.uni-potsdam.de
//(occured in 3.73 not previously and only for the case of 
//disjunctive programs i.e.
// a v b:-c would be represented as two clauses: -c v a; -c v b 
// in place of -c v a v b
//
//+ following Benjamin suggestion changed minisatSolver->solve(true,false))
//  to minisatSolver->solve(true,true)) that disables farther than first time
//  preprocessing in minisat
//
//3.76 bug fixed in loop formula construction due to not cleanscc
//3.77 bug fixed in loop formula construction due to not cleanscc
//
//Minisat version 1 is added -ms1
//-loopformula1 flag (just one loopformula at a time is added
// instead of all
//-temp flag in case of Minisat v 1(-ms1) learns and forgets loop formulas
//
//
//3.78 bug fixed in read.cc 
//int Read::addWeightRule (FILE *f) 
// now reading is done with add_head() 
// in place of add_head_repetition 
// bug was reported by Roland Kaminski 26 Feb Fri 
//(it could occur only on programs with weight rules)
//3.79 
// 1) bug fixed in cmodels.cc 
// bool Cmodels::rec_weight_rule(Weight totalweight, int sizeC, Atom* headC,
//		     unsigned long atleast, int counter_body)
//
//weightrules translation had a bug by assigning the same atom to 
//two different expressions (see comments in the code that specify what was changed)
//
// bug was reported by  Raphael Finkel  10 Mar 2009
//(it could occur only on programs with weight rules)
// 2) in zchaff_dbase.cc displacement given a new type
//    to support both 32 and 64 bit archetectures in uniform manner.
//    The change is suggested by  Gurucharan Huchachar 11 Mar 2009
//    Line 329 zchaff_dbase.cc :
//    ptrdiff_t displacement = _lit_pool_start - old_start;
//    For 64 bit archetecture
//3.80
// minisat is now default instead of zchaff
//
//3.81
// capability for incremental ASP is added; please see ctable.h file 
// that contains all the details of the interface
//3.82
// under construction
// idea to implement an additional propagation of minisat 1 on cmodels side 
// so that non-lazy approach were possible with EZ-CSP or other system if it needs to
// 3.82g -> h bug fix due to Benjamin Kaufmann clearLoop added at cmodels.cc 4847
// 3.83 per Marcello's request change in partial assignment return to external solver. Now it is more flexible which atoms external solver wants to see. See ctable.h for Docs.
// 3.84 bug fix reported by Peter Schueller. Occurred in addDenial cmodels.cc
// 3.86 introduced -cdimacs flag to allow ezsmt 
//      to use cmodles capability compute completion
// 3.86.1 fixed declarations in Minisat so that gcc compiler version 5+ compiles the code (due to Marcello Balduccini)  
//      introduced -file option to pass filename as a parameter for reading (due to Ben Susman) 

#include <iostream>
#include <new>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "ctable.h"
#include "timer.h"
#include "interpret.h"
#include <time.h>
#include <sstream>
#include <ctype.h>
#include <fstream>

using namespace std;

Ctable ctable;
bool Atom::change = false;
bool Atom::conflict = false;

/* set parameters, equivalent to command line */
static void set_parameters() {
    /* how many models (default 1) */
    ctable.cmodels.param.many = 0;
    ctable.cmodels.param.manySet = true;
    // cmodels.param.extmany = XXX;

    /* SolverType (default MINISAT) */
    // cmodels.param.sys = RELSAT/DIMACS_PRODUCE/SCC_LEVEL_RANKING/
    //                     LEVEL_RANKING/SCC_LEVEL_RANKING_STRONG/
    //                     LEVEL_RANKING_STRONG/CASP_DIMACS_PRODUCE/
    //                     ZCHAFF/ASSAT_ZCHAFF/MINISAT/MINISAT1/
    //                     BCIRCUIT/SIMO

    /* rdcComp, mnmBd, PrintExtAS, NLLogic, sort, loopFormula, loopFormula1,
     * temp, eloop, stat, cm_wfm, disj, disjProgramLparse, verifyMethod,
     * out_f_c, timings, asparagus, dir, dirName, forgetPercent,
     * timeout, heur, blah-blah-blah
     */

    strcpy(ctable.cmodels.param.cmodelsname, "ezsmtPlus");
    strcpy(ctable.cmodels.param.config, "CONFIG");
}

int main (int argc, char *argv[])
{
  bool error = false;
  set_parameters(); 

  //preparsing
  //manually do:
  // ./tools/pre-parser INPUT > INPUT.preparsed
  //then, grep for "SYNTAX ERROR"

  //grounding
  //manually do:
  // ./tools/gringp INPUT.preparsed > INPUT.ground
  //then, grep for "ERROR: "

  //then pipe INPUT.ground into ezsmtPlus
  int bad = ctable.read();
  if (bad) return 10;
  //removes some setting that might be not fitting
  //for some specific SAT solver
  ctable.cmodels.param.finish();

   ctable.cmodels.cmodels();
   return 0;
}

