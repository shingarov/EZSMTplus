/**************************************************************************************************
MiniSat -- Copyright (c) 2003-2005, Niklas Een, Niklas Sorensson

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and
associated documentation files (the "Software"), to deal in the Software without restriction,
including without limitation the rights to use, copy, modify, merge, publish, distribute,
sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or
substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT
NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT
OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
**************************************************************************************************/

#ifndef Solver_h
#define Solver_h

#include "SolverTypes.h"
#include "Constraints.h"
#include "Queue.h"
#include "Global.h"
#include "VarOrder.h"
#include <map>
using namespace std;


class testPartialSolutionInfo	/* [marcy 022812] */
{
public:
	virtual bool testPartialSolution(int *external_assignment,int num_ext_assigned)=0;
};

namespace ms1{
//=================================================================================================
// Solver -- the main class:


struct SolverStats {
    int64   starts, decisions, propagations, inspects, conflicts;
    int64   clauses, clauses_literals, learnts, learnts_literals;
    SolverStats(void) : starts(0), decisions(0), propagations(0), inspects(0), conflicts(0)
        , clauses(0), clauses_literals(0), learnts(0), learnts_literals(0) { }
};


struct SearchParams {
    double  var_decay, clause_decay, random_var_freq;    // (reasonable values are: 0.95, 0.999, 0.02)    
    SearchParams(double v = 1, double c = 1, double r = 0) : var_decay(v), clause_decay(c), random_var_freq(r) { }
};

class Solver {
public:
  //YU
  map<int,int>  externals_ids;
  map<int,int>::iterator  externals_ids_iterator;
  int * external_assignment;
  
  testPartialSolutionInfo *tpsi_;	/* [marcy 022812] */
	//end YU
    bool                ok;             // If FALSE, the constraints are already unsatisfiable. No part of the solver state may be used!
    vec<Constr*>        constrs;        // List of problem constraints.
    vec<Clause*>        learnts;        // List of learnt clauses.
    double              cla_inc;        // Amount to bump next clause with.
    double              cla_decay;      // INVERSE decay factor for clause activity: stores 1/decay.

    vec<double>         activity;       // A heuristic measurement of the activity of a variable.
    double              var_inc;        // Amount to bump next variable with.
    double              var_decay;      // INVERSE decay factor for variable activity: stores 1/decay. Use negative value for static variable order.
    VarOrder            order;          // Keeps track of the decision variable order.

    vec<vec<Constr*> >  watches;        // 'watches[lit]' is a list of constraints watching 'lit' (will go there if literal becomes true).
    vec<vec<Constr*> >  undos;          // 'undos[var]' is a list of constraints that will be called when 'var' becomes unbound.
    Queue<Lit>          propQ;          // Propagation queue.

    vec<char>           assigns;        // The current assignments (lbool:s stored as char:s).
    vec<Lit>            trail;          // List of assignments made. 
    vec<int>            trail_lim;      // Separator indices for different decision levels in 'trail'.
    vec<Constr*>        reason;         // 'reason[var]' is the clause that implied the variables current value, or 'NULL' if none.
    vec<int>            level;          // 'level[var]' is the decision level at which assignment was made.
    int                 root_level;     // Level of first proper decision.
    int                 last_simplify;  // Number of top-level assignments at last 'simplifyDB()'.

	

    // Temporaries (to reduce allocation overhead):
    //
    vec<char>           analyze_seen;

    // Main internal methods:
    //
    bool        assume       (Lit p);
    void        undoOne      (void);
    void        cancel       (void);
    void        cancelUntil  (int level);
    void        record       (const vec<Lit>& clause);

    void        analyze      (Constr* confl, vec<Lit>& out_learnt, int& out_btlevel); // (bt = backtrack)
    bool        enqueue      (Lit fact, Constr* from = NULL);
    Constr*     propagate    (void);
    void        reduceDB     (int percent);
   
    Lit         pickBranchLit(const SearchParams& params);
    lbool       search       (int nof_conflicts, int nof_learnts, const SearchParams& params);
    double      progressEstimate(void);

    // Activity:
    //
    void    varBumpActivity(Lit p) {
        if (var_decay < 0) return;     // (negative decay means static variable order -- don't bump)
        if ( (activity[var(p)] += var_inc) > 1e100 ) varRescaleActivity();
        order.update(var(p)); }
    void    varDecayActivity(void) { if (var_decay >= 0) var_inc *= var_decay; }
    void    varRescaleActivity(void);

    void    claBumpActivity(Clause* c) { if ( (c->activity() += cla_inc) > 1e20 ) claRescaleActivity(); }
    void    claDecayActivity(void) { cla_inc *= cla_decay; }
    void    claRescaleActivity(void);

    int     decisionLevel(void) { return trail_lim.size(); }

public:
    Solver(void) : ok               (true)
                 , cla_inc          (1)
                 , cla_decay        (1)
                 , var_inc          (1)
                 , var_decay        (1)
                 , order            (assigns, activity)
                 , last_simplify    (-1)
                 , progress_estimate(0)
                 , verbosity(0)
		 , tpsi_(NULL)	/* [marcy 022812] */
                 { }
   ~Solver(void) {
        for (int i = 0; i < learnts.size(); i++) learnts[i]->remove(*this, true);
        for (int i = 0; i < constrs.size(); i++) constrs[i]->remove(*this, true); }

    // Helpers: (semi-internal)
    //
    lbool   value(Var x) { return toLbool(assigns[x]); }
    lbool   value(Lit p) { return sign(p) ? ~toLbool(assigns[var(p)]) : toLbool(assigns[var(p)]); }

    int     nAssigns(void) { return trail.size(); }
    int     nConstrs(void) { return constrs.size(); }
    int     nLearnts(void) { return learnts.size(); }

    // Statistics: (read-only member variable)
    //
    SolverStats stats;

    // Problem specification:
    //
    Var     newVar (void);
    int     nVars  (void)  { return assigns.size(); }
    void    addUnit(Lit p) { if (ok) enqueue(p); }

    // -- constraints:
    friend class Clause;
    friend bool Clause_new(Solver& S, const vec<Lit>& ps, bool learnt, Clause*& out_clause);
	friend class External;
	
	  friend bool External_new(Solver& S, const Lit& ps, External*& out_constr);

    void    addClause(const vec<Lit>& ps, bool permanent)        
	{ if (ok){ Clause* c; ok = Clause_new(*this, ps, !permanent, c); 
	if (c != NULL) constrs.push(c); } }
    void    addExternal(const Lit& ps) { 
          if (ok){ 
			External* c; 
			ok = External_new(*this, ps, c); 
			if (c != NULL) constrs.push(c); 
		  } 
	}

    // Solving:
    //
    bool    okay(void) { return ok; }
    void    simplifyDB(void);
    bool    solve(const vec<Lit>& assumps,int percent = 0);
    bool    solve(int percent = 0); //{ vec<Lit> tmp; return solve(tmp); }

    double      progress_estimate;  // Set by 'search()'.
    vec<lbool>  model;              // If problem is solved, this vector contains the model (if any).
    int         verbosity;          // Verbosity level. 0=silent, 1=some progress report, 2=everything

	bool addClauseFromCmodels(int* clause, int size, bool= true);
	bool addExternalFromCmodels(int literal, int id, bool trueExternal);
	void retModelForCmodels(bool* assignments, int natoms);
	bool ExternalPropagation(vec<Lit>& out_reason);
	void initializeExternals(int num_lits);
	void deleteExternals();

	void setTestPartialSolutionInfo(testPartialSolutionInfo *tpsi); /* [marcy 022812] */

};

}

//=================================================================================================
#endif
