#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include "interpret.h"
#include "defines.h"
using namespace std;
Output::Output (){
  asparagus==STANDARD;
  out_f_c=false;
  stat=false;
  timings= false;
  numLoops=0;
  numSameBodies=0;
  numRules=0;

  numELoops=0;
  sat=UNKNOWN;
  numSolutions=0;
  numModels=0;
  numModelsFirstSol=0;
  numSatVerifyCalls=0;
}

Output::~Output (){
}
Result
Output::interpret_relsat(char* solver_out, 
							bool* sol){
  ifstream solverOutFile (solver_out,ios::in); 
  int num_of_symbols=9;
  if(program->number_of_atoms <10)
	num_of_symbols = 3;
  else if(program->number_of_atoms <100)
	num_of_symbols = 4;
  else if(program->number_of_atoms <1000)
	num_of_symbols = 5;
  else if(program->number_of_atoms <10000)
	num_of_symbols = 6;
  else if(program->number_of_atoms <100000)
	num_of_symbols = 7;
  else if(program->number_of_atoms <1000000)
	num_of_symbols = 8;
  if(num_of_symbols ==9)
	{
	  cerr<<"Implementation restriction: "<<endl;
	  cerr<<"Implementation does not support so many atoms" <<endl;
	  exit(0);
	}
  char* buffer; 
  unsigned long buflen;  

  buflen = (unsigned long)(num_of_symbols*program->number_of_atoms)+12;
  if(buflen < 256)
	buflen = 256;
  buffer = new char[buflen]; 
  strcpy(buffer,"\0");
  
  if (! solverOutFile.is_open())
	{ 
	  cout << "Error opening "<<solver_out; 
	  exit (1); 
	  }
  //end of a common part for sys=3 and sys=2=1 (relsat1 and zchaff)

  while ( !solverOutFile.eof() ){
	if(solverOutFile.eof()) 
	  break;
	solverOutFile.getline (buffer,buflen-1);
	
	//cout << "buffer" << buffer << endl;
	if(buffer[0]=='c'||(buffer[0]=='S' && buffer[1]=='A'))
	  {}
	else if(buffer[0]=='U'&& buffer[1]=='N' ){
	  return UNSAT;}
	else if(buffer[0]=='E'&& buffer[1]=='r' ){ 
	  cout<<"Error with Relsat"<<endl;
	  return UNKNOWN;}
	else 
	  if (buffer[0]=='S' && buffer[1]=='o' && buffer[2]=='l'){
	  
		for( int j=0;j<program->number_of_atoms;j++){
		  sol[j]=false;
		}
		int p =0;
		while(buffer[p]!=':')
		  p++;
		long i=p+1;
		//		long i=11;
		while(buffer[i]!=0){
		  long atom_num= findNextAtom(buffer, i);
		  if(atom_num!=-1&&atom_num!=0){
			sol[atom_num-1]=true;
		  }
		}
      }
  }
  
  solverOutFile.close();   
  
  return SAT;

}
Result
Output::interpret_assat_zchaff(char* solver_out, 
						  	bool* sol){
  ifstream solverOutFile (solver_out,ios::in); 
  int num_of_symbols=9;
  if(program->number_of_atoms <10)
	num_of_symbols = 3;
  else if(program->number_of_atoms <100)
	num_of_symbols = 4;
  else if(program->number_of_atoms <1000)
	num_of_symbols = 5;
  else if(program->number_of_atoms <10000)
	num_of_symbols = 6;
  else if(program->number_of_atoms <100000)
	num_of_symbols = 7;
  else if(program->number_of_atoms <1000000)
	num_of_symbols = 8;
  if(num_of_symbols ==9)
	{
	  cerr<<"Implementation restriction: "<<endl;
	  cerr<<"Implementation does not support so many atoms" <<endl;
	  exit(0);
	}
  char* buffer; 
  unsigned long buflen;  

  buflen = (unsigned long)(num_of_symbols*program->number_of_atoms)+12;
  if(buflen < 256)
	buflen = 256;
  buffer = new char[buflen]; 
  strcpy(buffer,"\0");
  
  if (! solverOutFile.is_open())
	{ 
	  cout << "Error opening "<<solver_out; 
	  exit (1); 
	  }
  //end of a common part for sys=3 and sys=2=1 (relsat1 and zchaff)

  while ( !solverOutFile.eof() ){
	if(solverOutFile.eof()) 
	  break;
	solverOutFile.getline (buffer,buflen-1);
	
	if(! (buffer[0]=='I' && buffer[1]=='n' && buffer[2]=='s'))
	{ 

	}
	else if(buffer[9]=='U'&& buffer[10]=='n' ){
	  solverOutFile.close();   
	  return UNSAT;
	}
	else if(buffer[9]=='S'&& buffer[10]=='a' ){ 
	  for( int j=0;j<program->number_of_atoms;j++){
		sol[j]=false;
	  }
	  //get next line where the solution is contained
	  solverOutFile.getline (buffer,buflen-1);
	  long i=0;
	  //		long i=11;
	 
	  while(buffer[i]!=0){
		long atom_num= findNextAtom(buffer, i);
	  
		if(atom_num>0){
		  sol[atom_num-1]=true;
		}
	  }
	  solverOutFile.close();   
	  return SAT;
	
	}
	else{
	  solverOutFile.close();   
	  return UNKNOWN;
	} 
  }
  solverOutFile.close();   
  return UNKNOWN;

}
long 
Output::findNextAtom(char* buf, long &i){
  char num[10];
  if(buf[i]==0)
    return -1;
  if(buf[i] ==' ')
    i++;

  int k=0;
  while(buf[i]!=' '&& buf[i]!=0)
    {
      num[k]=buf[i];
      k++;

      i++;
    }
  num[k]=0;
  return atol(num);
}
