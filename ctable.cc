/*
 * File ctable.cc 
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
#include <iostream>
#include <iomanip>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#if defined _BSD_SOURCE || defined _SVID_SOURCE
#include <sys/time.h>
#include <sys/resource.h>
#endif
#include "ctable.h"
#include "time.h" 
 
Ctable::Ctable ()
  : api (&cmodels.program),
    reader (&cmodels.program, &api)
{
  cmodels.api=&api;
  
}

Ctable::~Ctable ()
{
}

int
Ctable::read ()
{ 
  int r = reader.read (stdin);
  //the program has just been read; no modifications on the program were performed 
  //so we can safely set the value of program.original_number_of_atoms
  cmodels.program.original_number_of_atoms=cmodels.program.number_of_atoms;
  return r;
}

void
Ctable::numberExpected(char* option){
  if(option==NULL||!isdigit(option[0])){
    usage();
    exit(1);
  }       
}

int 
Ctable::getNumberGroundedAtoms(){
  return cmodels.program.original_number_of_atoms;
}

void 
Ctable::Initialize(int* answerset_lits, int& num_atoms, const char **&symbolTable, int &symbolTableEntries){
  cmodels.output.asparagus= SILENT;
  cmodels.init(answerset_lits, num_atoms,symbolTable,symbolTableEntries);
  //  cout<<"After initialize: num_atoms: "<<num_atoms<<endl;
  if(num_atoms>=-1)
    solved=true;
  else 
    solved=false;
}

void 
Ctable::Initialize(int* answerset_lits, int& num_atoms){
  const char **symbolTable;
  int symbolTableEntries;

  Initialize(answerset_lits,num_atoms,symbolTable,symbolTableEntries);
  
}

void
Ctable::print_lits(int* constraint_lits, int num_lits, bool denial){
  int cur;
  bool cur_sign;
  int inner_count=0;
  long indA=0;
 
  if(!denial){
    cout<<"Solution: ";
  }
  else 
    cout<<"Denial: :- ";
  
  for (int i=0; i<num_lits; i++){
    if(!denial){
      cur=constraint_lits[i];
      cur_sign=true;
    }else{
      if(constraint_lits[i]%2){//if it is odd      
        cur=(constraint_lits[i]-1)/2;
        cur_sign=true;
      }
      else{
        cur=constraint_lits[i]/2;
        cur_sign=false;
      }
    }
    
    for(indA=inner_count; indA<cmodels.program.cmodelsAtomsFromThisId; indA++){
      
      if(cur==cmodels.program.atoms[indA]->get_lparse_id()){
        
        if(cur_sign)
          cmodels.program.atoms[indA]->print();
        else{
          cout<<"-";
          cmodels.program.atoms[indA]->print();
        }
        cout<<", ";
  
        break;
      }
    }
    if(indA==cmodels.program.cmodelsAtomsFromThisId-1&&i!=num_lits-1){
      cerr<<"Cmodels: Error with denial print ";
      exit(20);
    }

  }
  cout<<" done."<<endl;

}

void 
Ctable::usage ()
{
  cerr << "Usage: ./ezsmtPlus -file <path> [-file <path>] [-file <path>] <num1> <num2> [-PrintExtAS] [-levelRanking|-levelRankingStrong|-SCClevelRanking|-SCClevelRankingStrong] [-reducedCompletion] [-minimalUpperBound] [-cvc4|-yices|-z3] [-non-linear]"
           << endl << "<path> is the path to the input program (file should be in ezcsp language)"
           << endl << "<num1> is the number of answer sets to be computed. 0 stands for all and 1 is default."
           << endl << "<num2> is the number of extended answer sets to be computed. 0 stands for all and 1 is default."
           << endl << "[-cvc4|-z3|-yices] instructs EZSMTPLUS to call the corresponding SMT solver. By default, -z3 is chosen."
           << endl << "[-PrintExtAS] instructs EZSMT+ to print out extended answer sets. By default, only answer sets will be printed."
           << endl << "[-levelRanking|-levelRankingStrong|-SCClevelRanking|-SCClevelRankingStrong] selects the typy of level ranking formulas produced for non-tight programs. By default, -SCClevelRanking is chosen."
           << endl << "[-reducedCompletion] will instruct EZSMTPLUS to remove the part of Clark's completion that is captured by a level ranking formula."
           << endl << "[-minimalUpperBound] sets the upper bound of level ranking variables to the number of atoms inside the Strongly Connected Component containing that variable. A bigger upper bound (the total number of atoms) would be selected by default."
           << endl << "[-non-linear] selects non-linear logics for SMT solvers."
           << endl;

}

;


