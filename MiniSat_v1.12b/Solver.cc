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
#include <iostream>

#include <typeinfo>

#include "Solver.h"
#include "Sort.h"
#include <cmath>
using namespace ms1;

//=================================================================================================
// Debug:


// For derivation output (verbosity level 2)
#define L_IND    "%-*d"
#define L_ind    decisionLevel()*3+3,decisionLevel()
#define L_LIT    "%sx%d"
#define L_lit(p) sign(p)?"~":"", var(p)

// Just like 'assert()' but expression will be evaluated in the release version as well.
inline void check(bool expr) { assert(expr); }


//=================================================================================================
// Minor methods:


// Creates a new SAT variable in the solver. If 'decision_var' is cleared, variable will not be
// used as a decision variable (NOTE! This has effects on the meaning of a SATISFIABLE result).
//
Var Solver::newVar(void)
{
    int     index;
    index = nVars();
    watches .push();          // (list for positive literal)
    watches .push();          // (list for negative literal)
    undos   .push();
    reason  .push(NULL);
    assigns .push(toInt(l_Undef));
    level   .push(-1);
    activity.push(0);
    order   .newVar();
    return index;
}


// Returns FALSE if immediate conflict.
bool Solver::assume(Lit p) {
    assert(propQ.size() == 0);
    if (verbosity >= 2) printf(L_IND"assume("L_LIT")\n", L_ind, L_lit(p));
    trail_lim.push(trail.size());
    return enqueue(p); }


// Revert one variable binding on the trail.
//
inline void Solver::undoOne(void)
{
    if (verbosity >= 2){ Lit p = trail.last(); printf(L_IND"unbind("L_LIT")\n", L_ind, L_lit(p)); }
    Lit     p  = trail.last(); trail.pop();
    Var     x  = var(p);
    assigns[x] = toInt(l_Undef);
    reason [x] = NULL;
    order.undo(x);
    while (undos[x].size() > 0)
        undos[x].last()->undo(*this, p),
        undos[x].pop();
}


// Reverts to the state before last 'assume()'.
//
void Solver::cancel(void)
{
    assert(propQ.size() == 0);
    if (verbosity >= 2){ if (trail.size() != trail_lim.last()){ Lit p = trail[trail_lim.last()]; printf(L_IND"cancel("L_LIT")\n", L_ind, L_lit(p)); } }
    for (int c = trail.size() - trail_lim.last(); c != 0; c--)
        undoOne();
    trail_lim.pop();
}


// Revert to the state at given level.
//
void Solver::cancelUntil(int level) {
    while (decisionLevel() > level) cancel(); }


// Record a clause and drive backtracking. 'clause[0]' must contain the asserting literal.
//
void Solver::record(const vec<Lit>& clause)
{
    assert(clause.size() != 0);
    Clause* c;
    check(Clause_new(*this, clause, true, c));
    check(ok);
    check(enqueue(clause[0], c));
    if (c != NULL) learnts.push(c);
}


//=================================================================================================
// Major methods:


/*_________________________________________________________________________________________________
|
|  analyze : (confl : Constr*) (out_learnt : vec<Lit>&) (out_btlevel : int&)  ->  [void]
|  
|  Description:
|    Analyze conflict and produce a reason clause.
|  
|    Pre-conditions:
|      * 'out_learnt' is assumed to be cleared.
|      * Current decision level must be greater than root level.
|  
|    Post-conditions:
|      * 'out_learnt[0]' is the asserting literal at level 'out_btlevel'.
|  
|  Effect:
|    Will undo part of the trail, upto but not beyond the assumption of the current decision level.
|________________________________________________________________________________________________@*/
void Solver::analyze(Constr* confl, vec<Lit>& out_learnt, int& out_btlevel)
{
    vec<char>&  seen = analyze_seen;
    int         pathC    = 0;
    Lit         p = lit_Undef;
    vec<Lit>    p_reason;
    seen.growTo(nVars(), 0);

    // Generate conflict clause:
    //
    out_learnt.push();      // (leave room for the asserting literal)
    out_btlevel = 0;
    do{
        assert(confl != NULL);          // (otherwise should be UIP)

        p_reason.clear();
        confl->calcReason(*this, p, p_reason);

        for (int j = 0; j < p_reason.size(); j++){
            Lit q = p_reason[j];
            if (!seen[var(q)] && level[var(q)] > 0){
                seen[var(q)] = 1;
                varBumpActivity(q);
                if (level[var(q)] == decisionLevel())
                    pathC++;
                else{
                    out_learnt.push(~q),
                    out_btlevel = max(out_btlevel, level[var(q)]);
                }
            }
        }

        // Select next clause to look at:
        do{
            p = trail.last();
            confl = reason[var(p)];
            undoOne();
        }while (!seen[var(p)]);
        pathC--;
    seen[var(p)] = 0;
    }while (pathC > 0);
    out_learnt[0] = ~p;

    for (int j = 0; j < out_learnt.size(); j++) seen[var(out_learnt[j])] = 0;    // ('seen[]' is now cleared)

    if (verbosity >= 2){
        printf(L_IND"Learnt {", L_ind);
        for (int i = 0; i < out_learnt.size(); i++) printf(" "L_LIT, L_lit(out_learnt[i]));
        printf(" } at level %d\n", out_btlevel); }
}


/*_________________________________________________________________________________________________
|
|  enqueue : (p : Lit) (from : Constr*)  ->  [bool]
|  
|  Description:
|    Puts a new fact on the propagation queue as well as immediately updating the variable's value.
|    Should a conflict arise, FALSE is returned.
|  
|  Input:
|    p    - The fact to enqueue
|    from - [Optional] Fact propagated from this (currently) unit clause. Stored in 'reason[]'.
|           Default value is NULL (no reason).
|  
|  Output:
|    TRUE if fact was enqueued without conflict, FALSE otherwise.
|________________________________________________________________________________________________@*/
bool Solver::enqueue(Lit p, Constr* from)
{
   if (value(p) != l_Undef){
        if (value(p) == l_False){
            // Conflicting enqueued assignment
            if (decisionLevel() == 0)
                ok = false;
            return false;
        }else{
            // Existing consistent assignment -- don't enqueue
            return true;
        }
    }else{
        // New fact -- store it.
        if (verbosity >= 2) printf(L_IND"bind("L_LIT")\n", L_ind, L_lit(p));
        assigns[var(p)] = toInt(lbool(!sign(p)));
        level  [var(p)] = decisionLevel();
        reason [var(p)] = from;
        trail.push(p);
        propQ.insert(p);
        return true;
    }
}


/*_________________________________________________________________________________________________
|
|  propagate : [void]  ->  [Constr*]
|  
|  Description:
|    Propagates all enqueued facts. If a conflict arises, the conflicting clause is returned,
|    otherwise NULL.
|  
|    Post-conditions:
|      * the propagation queue is empty, even if there was a conflict.
|________________________________________________________________________________________________@*/
Constr* Solver::propagate(void)
{

	bool extpPresent=false;
    Constr* confl = NULL;
	Constr        *extConstr;
	bool          keep_watch;
	/*//here for debugging

	cout<<	"Trail Before Propagate"<<endl;  
	for(int i=0;i<trail.size();i++){
	  externals_ids_iterator=externals_ids.find(index(trail[i]));

	  if (externals_ids_iterator!=externals_ids.end()){
		
		if(!sign(trail[i])){
		 cout<< "Assignment "<<externals_ids_iterator->second<<endl;

		}
		else{
		 cout<< "Assignment "<<-externals_ids_iterator->second<<endl;

		}
	  }
	}
	cout<<":End1:";  
	*/  

    while (propQ.size() > 0){
        stats.propagations++;
        Lit           p  = propQ.dequeue();        // 'p' is enqueued fact to propagate.
		
		vec<Constr*>& ws = watches[index(p)];
		Constr        **i, **j, **end = (Constr**)ws + ws.size();
		for (i = j = (Constr**)ws; confl == NULL && i < end; i++){
		  //		  cout<<endl<<typeid(**i).name()<<endl;
		  if((*i)->externalConstraint(*this)){
			//			cout<<"external"<<endl;
			if(!extpPresent){			  
			  extpPresent=true;
			  extConstr=(Constr *)*i;
			}
			//external atom's constraints should always be watched upon
			*j++ = *i;
		  }
		  else{
			stats.inspects++;
			keep_watch = false;
			if (!(*i)->propagate(*this, p, keep_watch))
			  confl = *i,
				propQ.clear();
			if (keep_watch)
			  *j++ = *i;
		  }
		}
		
		// Copy the remaining watches:
		while (i < end)
		  *j++ = *i++;
		
		ws.shrink(i - j);
   
    }
	/*//here for debugging
	cout<<	"Trail After Propagate"<<endl;  
	for(int i=0;i<trail.size();i++){
	  externals_ids_iterator=externals_ids.find(index(trail[i]));

	  if (externals_ids_iterator!=externals_ids.end()){
		
		if(!sign(trail[i])){
		 cout<< "Assignment "<<externals_ids_iterator->second<<endl;

		}
		else{
		 cout<< "Assignment "<<-externals_ids_iterator->second<<endl;

		}
	  }
	}
	cout<<":End2:";  


	if(confl!=NULL)
	  cout<<"conflict is reached"<<endl;

	if(extpPresent)
	  cout<<"present"<<endl;
	*/

	//if conflict has not been reached and there are
	// external propagations to be made then here we go
	if(extpPresent&&confl == NULL){
	  stats.inspects++;
	  //the lit_Undef, keep_watch are dummies here; inessential
	  //Solver's state is used to propagate
	  //and remember reason
	  if (!(extConstr)->propagate(*this, lit_Undef, keep_watch))
		confl = extConstr,
		  propQ.clear();
	}

    return confl;
}


/*_________________________________________________________________________________________________
|
|  reduceDB : (int percent)  ->  [void]
|  
|  Description:
|    Remove percent of the learnt clauses, minus the clauses locked by the current assignment. Locked
|    clauses are clauses that are reason to a some assignment.
|________________________________________________________________________________________________@*/
struct reduceDB_lt { bool operator () (Clause* x, Clause* y) { return x->activity() < y->activity(); } };
void Solver::reduceDB(int percent)
{
  assert(percent<=100&percent>=0);
    int     i, j;
    double  extra_lim = cla_inc / learnts.size();    // Remove any clause below this activity

    sort(learnts, reduceDB_lt());
    for (i = j = 0; i < learnts.size() *percent/ 100; i++){
        if (!learnts[i]->locked(*this))
            learnts[i]->remove(*this);
        else
            learnts[j++] = learnts[i];
    }
    for (; i < learnts.size(); i++){
        if (!learnts[i]->locked(*this) && learnts[i]->activity() < extra_lim)
            learnts[i]->remove(*this);
        else
            learnts[j++] = learnts[i];
    }
    learnts.shrink(i - j);
}

/*_________________________________________________________________________________________________
|
|  simplifyDB : [void]  ->  [bool]
|  
|  Description:
|    Simplify all constraints according to the current top-level assigment (redundant constraints
|    may be removed altogether).
|________________________________________________________________________________________________@*/
void Solver::simplifyDB(void)
{
    if (!ok) return;    // GUARD (public method)
    assert(decisionLevel() == 0);

    if (propagate() != NULL){
        ok = false;
        return; }
    if (nAssigns() == last_simplify)
        return;

    last_simplify = nAssigns();

    for (int type = 0; type < 2; type++){
        vec<Constr*>& cs = type ? (vec<Constr*>&)learnts : constrs;

        int     j = 0;
        for (int i = 0; i < cs.size(); i++){
            if (cs[i]->simplify(*this))
                cs[i]->remove(*this);
            else
                cs[j++] = cs[i];
        }
        cs.shrink(cs.size()-j);
    }
}


/*_________________________________________________________________________________________________
|
|  search : (nof_conflicts : int) (nof_learnts : int) (params : const SearchParams&)  ->  [lbool]
|  
|  Description:
|    Search for a model the specified number of conflicts, keeping the number of learnt clauses
|    below the provided limit. NOTE! Use negative value for 'nof_conflicts' or 'nof_learnts' to
|    indicate infinity.
|  
|  Output:
|    'l_True' if a partial assigment that is consistent with respect to the clauseset is found. If
|    all variables are decision variables, this means that the clause set is satisfiable. 'l_False'
|    if the clause set is unsatisfiable. 'l_Undef' if the bound on number of conflicts is reached.
|________________________________________________________________________________________________@*/
lbool Solver::search(int nof_conflicts, int nof_learnts, const SearchParams& params)
{
    if (!ok) return l_False;    // GUARD (public method)
    assert(root_level == decisionLevel());

    stats.starts++;
    int     conflictC = 0;
    var_decay = 1 / params.var_decay;
    cla_decay = 1 / params.clause_decay;
    model.clear();

    for (;;){
        Constr* confl = propagate();
        if (confl != NULL){
            // CONFLICT

            if (verbosity >= 2) printf(L_IND"**CONFLICT**\n", L_ind);
            stats.conflicts++; conflictC++;
            vec<Lit>    learnt_clause;
            int         backtrack_level;
            if (decisionLevel() == root_level)
                return l_False;
            analyze(confl, learnt_clause, backtrack_level);
            cancelUntil(max(backtrack_level, root_level));
            record(learnt_clause);
            varDecayActivity(); claDecayActivity();

        }else{
            // NO CONFLICT

            if (nof_conflicts >= 0 && conflictC >= nof_conflicts){
                // Reached bound on number of conflicts:
                progress_estimate = progressEstimate();
                propQ.clear();
                cancelUntil(root_level);
                return l_Undef; }

            if (decisionLevel() == 0)
                // Simplify the set of problem clauses:
                simplifyDB(), assert(ok);

            if (nof_learnts >= 0 && learnts.size()-nAssigns() >= nof_learnts)
                // Reduce the set of learnt clauses:
                reduceDB(50);

            // New variable decision:
            stats.decisions++;
            Var next = order.select(params.random_var_freq);

            if (next == var_Undef){
                // Model found:
                model.growTo(nVars());
                for (int i = 0; i < nVars(); i++) model[i] = value(i);
                cancelUntil(root_level);
                return l_True;
            }

            check(assume(~Lit(next)));
        }
    }
}


// Return search-space coverage. Not extremely reliable.
//
double Solver::progressEstimate(void)
{
    double  progress = 0;
    double  F = 1.0 / nVars();
    for (int i = 0; i < nVars(); i++)
        if (value(i) != l_Undef)
            progress += pow(F, level[i]);
    return progress / nVars();
}


// Divide all variable activities by 1e100.
//
void Solver::varRescaleActivity(void)
{
    for (int i = 0; i < nVars(); i++)
        activity[i] *= 1e-100;
    var_inc *= 1e-100;
}


// Divide all constraint activities by 1e100.
//
void Solver::claRescaleActivity(void)
{
    for (int i = 0; i < learnts.size(); i++)
        learnts[i]->activity() *= 1e-20;
    cla_inc *= 1e-20;
}


/*_________________________________________________________________________________________________
|
|  solve : (assumps : const vec<Lit>&)  ->  [bool]
|  
|  Description:
|    Top-level solve. If using assumptions (non-empty 'assumps' vector), you must call
|    'simplifyDB()' first to see that no top-level conflict is present (which would put the solver
|    in an undefined state).
|________________________________________________________________________________________________@*/
bool Solver::solve(int percent){
  vec<Lit> tmp; return solve(tmp,percent);
}
bool Solver::solve(const vec<Lit>& assumps,int percent)
{
    simplifyDB();
    if (!ok) return false;

    SearchParams    params(0.95, 0.999, 0.02);
    double  nof_conflicts = 100;
    double  nof_learnts   = nConstrs() / 3;
    lbool   status        = l_Undef;
    reduceDB(percent);
    for (int i = 0; i < assumps.size(); i++)
        if (!assume(assumps[i]) || propagate() != NULL){
            propQ.clear();
            cancelUntil(0);
            return false; }
    root_level = decisionLevel();

    if (verbosity >= 1){
        printf("==================================[MINISAT]===================================\n");
        printf("| Conflicts |     ORIGINAL     |              LEARNT              | Progress |\n");
        printf("|           | Clauses Literals |   Limit Clauses Literals  Lit/Cl |          |\n");
        printf("==============================================================================\n");
    }

    while (status == l_Undef){
        if (verbosity >= 1){
            printf("| %9d | %7d %8d | %7d %7d %8d %7.1f | %6.3f %% |\n", (int)stats.conflicts, nConstrs(), (int)stats.clauses_literals, (int)nof_learnts, nLearnts(), (int)stats.learnts_literals, (double)stats.learnts_literals/nLearnts(), progress_estimate*100);
            fflush(stdout);
        }
        status = search((int)nof_conflicts, (int)nof_learnts, params);
        nof_conflicts *= 1.5;
        nof_learnts   *= 1.1;
    }
    if (verbosity >= 1)
        printf("==============================================================================\n");

    cancelUntil(0);
    return status == l_True;
}


// YULIYA's additions for Cmodels
//
void Solver::retModelForCmodels(bool* assignments, int natoms){ 
  assert(natoms==nVars());
  for (int i = 0; i < nVars(); i++){
	if (model[i] == l_Undef||model[i]==l_False)//solver also returns partial models
	  //we by default take for undef -- false
	  assignments[i]=false;
	else 
	  assignments[i]=true;

  }
}
bool Solver::addClauseFromCmodels(int* clause, int size, bool permanent)
{
  vec<Lit> ps;
  ps.clear();
  int     parsed_lit, var;
  for(int i=0;i<size;i++){
	parsed_lit =clause[i];
	var = abs(parsed_lit)-1;
	while (var >= nVars()) 
	  newVar();
	ps.push( (parsed_lit > 0) ? Lit(var) : ~Lit(var) );	
  }
  addClause(ps, permanent);
  return ok;
}

bool Solver::addExternalFromCmodels(int literal, int id, bool trueExternal)
{
  Lit ps;
  int var;

  var = abs(literal)-1;
  assert(var < nVars()); 
  //add for positive atom
  ps=Lit(var);
  externals_ids.insert(pair<int,int>(index(ps),id));
  //  setExternal(ps);
  if (trueExternal)
    addExternal(ps);

  //add for negative atom
  ps=~Lit(var);
  externals_ids.insert(pair<int,int>(index(ps),id));
  if (trueExternal)
    addExternal(ps);
  //	setExternal(ps);
  return ok;
}


bool Solver::ExternalPropagation(vec<Lit>& out_reason){
  // Vector "trail" of literals contains the partial assignment
  // We populate external_assignment array so that it contains 
  // external ids that coincide with ones from lparse/gringo
  // num_ext_assigned will contain a number of assigned external atoms in the current trail

  int num_ext_assigned=0;  
  for(int i=0;i<trail.size();i++){
	externals_ids_iterator=externals_ids.find(index(trail[i]));

	if (externals_ids_iterator!=externals_ids.end()){
	//if conflict is reached then out_reason will contain the external atoms as a reason
	//for the conflict. They in fact build the  reason (defined externally for a conflict)
	  out_reason.push(trail[i]);

	  if(!sign(trail[i])){
		external_assignment[num_ext_assigned]=externals_ids_iterator->second;
	  }
	  else{
		external_assignment[num_ext_assigned]=-externals_ids_iterator->second;
	  }
	  num_ext_assigned++;
	}
  }
  /* //Here for debugging
  for (int i=0;i<num_ext_assigned;i++)
	{
	  cout<< "*Assignment* "<<external_assignment[i]<<endl;

	}
  	  cout<<":End3:";  
	*/  
	  bool ret=true;
  	/* [marcy 022812] */
	if (tpsi_!=NULL)
	{	
	  ret=tpsi_->testPartialSolution(external_assignment,num_ext_assigned);
	}

	return ret;
}

void Solver::setTestPartialSolutionInfo(testPartialSolutionInfo *tpsi) { /* [marcy 022812] */
	tpsi_=tpsi;
}

void Solver::initializeExternals(int num_lits){
  external_assignment= new int[num_lits];
}

void Solver::deleteExternals(){

  delete external_assignment;
}
