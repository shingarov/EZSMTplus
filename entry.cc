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

#include <climits>
#include <stdexcept>
#include <iostream>
#include <float.h>
#include <limits.h>
#include <string.h>
#include <stdio.h>

#include <gringo/input/nongroundparser.hh>
#include <gringo/input/groundtermparser.hh>
#include <gringo/input/programbuilder.hh>
#include <gringo/input/program.hh>
#include <gringo/ground/program.hh>
#include <gringo/output/output.hh>
#include <gringo/logger.hh>
#include <gringo/scripts.hh>
#include <gringo/version.hh>
#include <gringo/control.hh>

#include "atomrule.h"
#include "api.h"
#include "program.h"
#include "cmodels.h"

class EzSMT
{
public:
  EzSMT ();
  ~EzSMT ();
  bool gringo(const char*);
  char* clausify(const char *program);
  void grow ();
  Atom *getAtom (long n);
  Atom *getFalseAtom (long n);
  Atom **atoms;
  long size;
  long linenumber;
  Program* program;
  Api* api;
  Cmodels cmodels;
  char *gringoError;
};

struct IncrementalControl : Gringo::Control, Gringo::GringoModule {
    using StringVec = std::vector<std::string>;
    IncrementalControl(Gringo::Output::OutputBase &out, std::string program) 
        : out(out)
        , scripts(*this)
        , pb(scripts, prg, out, defs, false /*rewriteMinimize*/)
        , parser(pb) {
        parser.pushStream("~~", Gringo::gringo_make_unique<std::stringstream>(program));
    }
    bool parse() {
        if (parser.empty()) return false;
        parser.parse();
        if (Gringo::message_printer()->hasError()) return false;
        defs.init();
        parsed = true;
        return true;
    }
    virtual void ground(Gringo::Control::GroundVec const &parts, Gringo::Any &&context) { 
        // NOTE: it would be cool to have assumptions in the lparse output
        auto exit = Gringo::onExit([this]{ scripts.context = Gringo::Any(); });
        scripts.context = std::move(context);
        parse();
        if (parsed) {
            prg.rewrite(defs);
            prg.check();
            if (Gringo::message_printer()->hasError()) {
                throw std::runtime_error("grounding stopped because of errors");
            }
            parsed = false;
        }
        if (!grounded) {
            if (incremental) { out.incremental(); }
            grounded = true;
        }
        if (!parts.empty()) {
            Gringo::Ground::Parameters params;
            for (auto &x : parts) { params.add(x.first, x.second); }
            Gringo::Ground::Program gPrg(prg.toGround(out.domains));
            gPrg.ground(params, scripts, out, false);
        }
    }
    virtual void add(std::string const &name, Gringo::FWStringVec const &params, std::string const &part) {
        Gringo::Location loc("<block>", 1, 1, "<block>", 1, 1);
        Gringo::Input::IdVec idVec;
        for (auto &x : params) { idVec.emplace_back(loc, x); }
        parser.pushBlock(name, std::move(idVec), part);
        parse();
    }
    virtual Gringo::Value getConst(std::string const &name) {
        parse();
        auto ret = defs.defs().find(name);
        if (ret != defs.defs().end()) {
            bool undefined = false;
            Gringo::Value val = std::get<2>(ret->second)->eval(undefined);
            if (!undefined) { return val; }
        }
        return Gringo::Value();
    }
    virtual void load(std::string const &filename) {
        parser.pushFile(std::string(filename));
        parse();
    }
    virtual void onModel(Gringo::Model const &) { }
    virtual bool blocked() { return false; }
    virtual Gringo::SolveResult solve(ModelHandler, Assumptions &&ass) {
        if (!ass.empty()) { std::cerr << "warning: the lparse format does not support assumptions" << std::endl; }
        if (!grounded) {
            if (incremental) { out.incremental(); }
        }
        grounded = false;
        out.finish();
        return Gringo::SolveResult::UNKNOWN;
    }
    virtual Gringo::SolveIter *solveIter(Assumptions &&) { 
        throw std::runtime_error("solving not supported in gringo");
    }
    virtual Gringo::SolveFuture *solveAsync(ModelHandler, FinishHandler, Assumptions &&) { throw std::runtime_error("solving not supported in gringo"); }
    virtual Gringo::Statistics *getStats() { throw std::runtime_error("statistics not supported in gringo (yet)"); }
    virtual void assignExternal(Gringo::Value ext, Gringo::TruthValue val) { 
        Gringo::PredicateDomain::element_type *atm = out.find2(ext);
        if (atm && atm->second.hasUid()) {
            out.assignExternal(*atm, val);
        }
    }
    virtual Gringo::DomainProxy &getDomain() { throw std::runtime_error("domain introspection not supported"); }
    virtual Gringo::ConfigProxy &getConf() { throw std::runtime_error("configuration not supported"); }
    virtual void useEnumAssumption(bool) { }
    virtual bool useEnumAssumption() { return false; }
    virtual ~IncrementalControl() { }
    virtual Gringo::Value parseValue(std::string const &str) { return termParser.parse(str); }
    virtual Control *newControl(int, char const **) { throw std::logic_error("creating new control instances not supported in gringo"); }
    virtual void freeControl(Control *) { }
    virtual void cleanupDomains() { }

    Gringo::Input::GroundTermParser        termParser;
    Gringo::Output::OutputBase            &out;
    Gringo::Scripts                        scripts;
    Gringo::Defines                        defs;
    Gringo::Input::Program                 prg;
    Gringo::Input::NongroundProgramBuilder pb;
    Gringo::Input::NonGroundParser         parser;
    bool                                   parsed = false;
    bool                                   grounded = false;
    bool                                   incremental = false;
};

struct EZMessagePrinter : Gringo::DefaultMessagePrinter {
    EzSMT *r;
    EZMessagePrinter(EzSMT *r) : r(r) {}
    virtual void print(std::string const &msg) {
        r->gringoError = strdup(msg.data());
    }
    virtual ~EZMessagePrinter() { }
};

EzSMT::EzSMT () {
    atoms = 0;
    size = 0;
    program = &cmodels.program;
    api = new Api(program);
    cmodels.api = api;
}

EzSMT::~EzSMT () { delete[] atoms; }

void EzSMT::grow () {
    long sz = size*2;
    if (sz == 0)
         sz = 256;
    Atom **array = new Atom *[sz];
    long i;
    for (i = 0; i < size; i++)
         array[i] = atoms[i];
    size = sz;
    for (; i < size; i++)
         array[i] = 0;
    delete[] atoms;
    atoms = array;
}

Atom * EzSMT::getAtom (long n) {
  while (n >= size)
    grow ();
  if (atoms[n] == 0)
    atoms[n] = api->new_atom (n);
  return atoms[n];
}

Atom * EzSMT::getFalseAtom (long n) {
  while (n >= size)
    grow ();
  if (atoms[n] == 0)
    atoms[n] = api->new_atom (n);
  if(n==1 && strcmp("#noname#",atoms[n]->atom_name ())==0){
	//    atoms[n]->Bneg=true;
	//   api->set_name (atoms[n], 0);   
    program->false_atom=atoms[n];
  }
  return atoms[n];
}

#define PANICK(msg) std::cerr << msg << "\n"; exit(1);
struct EZOutputter : Gringo::Output::LparseOutputter {
    EZOutputter(std::ostream &out, EzSMT *r) : out(out), r(r) {}
    virtual void incremental() { PANICK("No Incremental in EZ.") }
    virtual void printBasicRule(unsigned head, LitVec const &body) {
       r->api->begin_rule(BASICRULE);
       Atom *a = r->getAtom(head);
       r->api->add_head_repetition (a);
       for(const int& lit: body) {
           Atom *a = r->getAtom(abs(lit));
           r->api->add_body_repetition (a, lit>=0, BASICRULE);
       }
       r->api->end_rule();
    }
    virtual void printChoiceRule(AtomVec const &head, LitVec const &body) {
       r->api->begin_rule(CHOICERULE);
       for(const unsigned& at: head) {
            Atom *a = r->getAtom(at);
            r->api->add_head_repetition (a);
       }
       for(const int& lit: body) {
           Atom *a = r->getAtom(abs(lit));
           r->api->add_body_repetition (a, lit>=0, CHOICERULE);
       }
       r->api->end_rule();
    }
    virtual void printCardinalityRule(unsigned head, unsigned lower, LitVec const &body) {
       r->api->begin_rule(CONSTRAINTRULE);
       Atom *a = r->getAtom(head);
       r->api->add_head(a);
       r->api->set_atleast_body(lower);
       for(const int& lit: body) {
           Atom *a = r->getAtom(abs(lit));
           a->pbInd.clear();
           a->nbInd.clear();
           r->api->add_body (a, lit>=0);
       }
       r->api->end_rule();
    }
    virtual void printWeightRule(unsigned head, unsigned lower, LitWeightVec const &body) {
       PANICK("NOT IMPLEMENTED YET!")
    }
    virtual void printMinimize(LitWeightVec const &body) {
       PANICK("MINIMIZE NOT SUPPORTED BY CMODELS")
    }
    virtual void printDisjunctiveRule(AtomVec const &head, LitVec const &body) {
       PANICK("API WILL CAUSE EXIT 55")
    }
    virtual unsigned falseUid() { return 1; }
    virtual unsigned newUid() { return uids++; }
    virtual void finishRules() { }
    virtual void printSymbol(unsigned atomUid, Gringo::Value v) {
       std::ostringstream oss;
       v.print(oss);
       const char *s = strdup(oss.str().c_str());
       Atom *a = r->getAtom(atomUid);
       r->api->set_name(a, s);
    }
    virtual void printExternal(unsigned atomUid, Gringo::TruthValue type) { PANICK("No External in EZ.") }
    virtual void finishSymbols() {
       Atom *a = r->getFalseAtom(falseUid());
       r->api->set_compute (a, false,true);
    }
    virtual bool &disposeMinimize() { return disposeMinimize_; }
    virtual ~EZOutputter() {}

    std::ostream &out;
    EzSMT *r;
    unsigned      uids             = 2;
    bool          disposeMinimize_ = true;
};
/* return true if success.
 * if fail, return false and gringoError will point to a copy of error msg
 */
bool EzSMT::gringo(const char *program) {
    EZMessagePrinter *msgPrt = new EZMessagePrinter(this);
    Gringo::message_printer().reset(msgPrt);
    Gringo::Output::OutputPredicates outPreds;
    EZOutputter plo(std::cout, this);
    Gringo::Output::OutputBase out(std::move(outPreds), plo);
    std::string p(program);
    IncrementalControl inc(out, p);
    gringoError = NULL;
    if (!inc.parse()) return false;

    Gringo::Control::GroundVec parts;
    parts.emplace_back("base", Gringo::FWValVec{});
    inc.ground(parts, Gringo::Any());
    inc.solve(nullptr, {});
    return true;
}

char* EzSMT::clausify(const char *program) {
    bool good = gringo(program);
    if (!good) return NULL;
    return cmodels.cmodels();
}

extern "C" {
void* ezsmt_mk() {
    return (void*) new EzSMT;
}

char* ezsmt_clausify(void *self, const char* program) {
    return ((EzSMT*)self)->clausify(program);
}

char* ezsmt_get_error(void *self) {
    return ((EzSMT*)self)->gringoError;
}

void ezsmt_del(void* self) {
    delete (EzSMT*)self;
}
}
