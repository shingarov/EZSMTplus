/*
 * File program.h 
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
#ifndef PROGRAM_H
#define PROGRAM_H


#include "atomrule.h"
#include <vector>
#include <queue>
using namespace std;

class OptimizeRule;
struct sortedLE;
class Program
{
public:
  Program ();
  ~Program ();
  void init ();
  void print ();
  void print_completion ();   
  void print_clauses ();
  void print_atoms ();
  void print_atoms_wf ();



  queue <Atom*>q; 
  vector<Atom*>atoms; 


  list<Rule*> rules;

  //Vector which will have all the complitions
  vector<Completion*> completions;

  //Vector which will have all the clauses
  vector<Clause*> clauses;

  int number_of_atoms;
  int number_of_rules;
  int number_of_complitions; 
  int number_of_clauses;
  int number_of_nestedRules;
  int cmodelsAtomsFromThisId;

  Atom* false_atom; // since we need to work with atom which stands for false
                    //differently creating it's completions false_atom is a pointer
                    // to false atom  

  bool conflict;


  bool basic;//true if program is translated into basic
             //false if program is translated into nested basic program (if it has choice rules)
  bool tight; //true if the program is tight
  bool hcf; //true if the program is hcf

  bool disjProgramLparse;//true if the program is disjunctive and lparse is used

  void clearInM();
  void clearInLoop();
  void clearInMminus();
  void clearInCons();
  bool wellfounded();

  void atleast();
  void atmost();

 
};

#endif



