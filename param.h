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
  bool eloop; //elementary loop
  bool eloopSwitch; //elementary loop
  //_________________  
  //parameters that can be passed at command line
  //or set by functions
  char config[100];
  char dirName[256];
  char cmodelsname[256];
  char dimacsFileName[512];
  char solverOutputFileName[512];
  int many; 
  bool manySet;
  bool NLLogic;
  int extmany;
  bool sort;
  bool rdcComp;
  bool mnmBd;
  LogicType SMTLogic;
  bool PrintExtAS;
  bool wf;
  bool nowellfounded;//skip well founded computation
  int verifyMethod;  
  bool param;
  bool cm_wfm; //default 0 stops at prinitng WFM
  bool dir;
  int timeout; //default is 0 and this meens no timeout
               //otherwise the value meens that in timeout 
               //seconds the process will timeout
  int heur;

  bool hf; //first false  heuristic for Simo
  bool keep;
  bool bt;
  bool bj;
  bool le;
  bool bmodes;
  bool shortr; 
  int numLFClauses;
  int forgetPercent; //percentage of learnt clauses to be forgottem when denial comes back from external solver. Default is 0. No clauses forgot.
  //_________________
  Param(){
    forgetPercent=0;
	verifyMethod;
	numLFClauses=1;
	dir = false;
	timeout = 0;
	heur = 0;//default is 0 and this meens the default heuristics for 
	//simo to do search, all other numbers are specified within simo 
	//for different heuristics
	keep = false;
	bt = false;
	bj =false;
	le = true;
	bmodes = false;
	hf = false;
	shortr = false;
	many = 1;
	manySet = false;
	extmany = 1;
	NLLogic = false;
	loopFormula = true;
	loopFormula1 = false;
	temp = false;
	strcpy(config,"\0");
	strcpy(dirName,"\0");
	strcpy(cmodelsname,"\0");
	strcpy(dimacsFileName,"\0");
	strcpy(solverOutputFileName,"\0");
	eloop=false;
	wf   =true;
	cm_wfm = false;
	sort=false;
	nowellfounded=false; 
  }
  void finish(){
	if(numLFClauses<0)
	  numLFClauses=256;
	if(many<0)
	  many=1;
	if(eloop){
	  eloopSwitch = false;
	}
  }
}

;
#endif
