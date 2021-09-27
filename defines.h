/*
 * File defines.h 
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
#ifndef DEFINES_H
#define DEFINES_H

#define CMODELS_VERSION "1"

typedef enum {
  NOT_DEF,
  POS,
  NEG
} InClause;
typedef enum {
  LTH,
  EQ,
  SUBSET,
  SUPERSET,
  GTH
} CompareValues;

typedef enum {
  UNDEF,
  POSB,
  NEGB,
  HEAD
} InRule;
typedef enum {
  ENDRULE,
  BASICRULE,
  CONSTRAINTRULE,
  CHOICERULE,
  //
  GENERATERULE,
  WEIGHTRULE,
  OPTIMIZERULE,
  SOMETHING,
  DISJUNCTIONRULE,
  NESTED
  } RuleType;
typedef enum {
  AND,
  OR,
  EQUIV,
  IMPL
} Connector;

typedef enum {
	FD,
	R,
	MIXED
} LogicType;


 typedef enum {
  UNSAT = -1 , 
  UNKNOWN =0 ,
  SAT =1     
 } Result;
typedef enum {
  TIGHT=1, 
  NONDISJ=2,
  MINSCC=4,
  HCF=5
 } VerifyMethod;

 typedef enum {
  STANDARD=0, 
  ASPARAGUS=1,
  ASPCOMP2=2,
  SILENT=3
 } OutputMethod;

//#ifdef USEDOUBLE
//typedef double Weight;
//#define WEIGHT_MAX DBL_MAX
//#define WEIGHT_MIN -DBL_MAX
//#else
typedef long Weight;
#define WEIGHT_MAX LONG_MAX
#define WEIGHT_MIN -LONG_MAX
//#endif

#endif
