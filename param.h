// yuliya@cs.utexas.edu
#include <string.h>
#ifndef PARAM_H
#define PARAM_H
class Param{
 public:
  bool loopFormula;//true if loop formula is built
                   //false is reason is built
  bool loopFormula1;//true if only one loop formula is built
                   //false otherwise
  bool temp; //true if added clauses for loop formula or
  // reason are only temporary learned (relevent to minisat v.1)
  //_________________  
  //parameters that can be passed at command line
  //or set by functions
  bool sort;
  bool rdcComp;
  bool mnmBd;
  bool wf;
  bool nowellfounded;//skip well founded computation
  bool param;
  bool cm_wfm; //default 0 stops at prinitng WFM
  bool dir;
  bool keep;
  bool bt;
  bool bj;
  bool le;
  bool bmodes;
  bool shortr; 
  int numLFClauses;
  //_________________
  Param(){
	numLFClauses=1;
	dir = false;
	keep = false;
	bt = false;
	bj =false;
	le = true;
	bmodes = false;
	shortr = false;
	loopFormula = true;
	loopFormula1 = false;
	temp = false;
	wf   =true;
	cm_wfm = false;
	sort=false;
	nowellfounded=false; 
  }
}

;
#endif
