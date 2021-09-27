// {{{ GPL License 

// This file is part of gringo - a grounder for logic programs.
// Copyright (C) 2013  Roland Kaminski

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

// }}}

#ifndef _GNU_SOURCE
#  define _GNU_SOURCE
#endif
#include <cstdlib>
#ifdef __USE_GNU
#  include <libgen.h>
#  include <sys/types.h>
#  include <sys/stat.h>
#endif
#include "gringo/input/nongroundparser.hh"
#include "gringo/lexerstate.hh"
#include "gringo/value.hh"
#include "gringo/logger.hh"
#include "src/input/nongroundgrammar.hh"
#include <cstddef>
#include <climits>
#include <memory>
#include <fstream>
#include <vector>
#include <algorithm>


namespace Gringo { namespace Input {

namespace {

struct Free {
    void operator ()(char *ptr) { free(ptr); }
};

template <typename T>
void report_included(T const &loc, std::string const &filename) {
    GRINGO_REPORT(W_FILE_INCLUDED) << loc << ": warning: already included file:\n"
        << "  " << filename << "\n";
}

template <typename T>
void report_not_found(T const &loc, std::string const &filename) {
    GRINGO_REPORT(E_ERROR) << loc << ": error: file could not be opened:\n"
        << "  " << filename << "\n";
}

// NOTE: is there a better way?
#ifdef __USE_GNU

std::string check_file(std::string const &filename) {
    if (filename == "-") { return filename; }
    struct stat sb;
    if (stat(filename.c_str(), &sb) != -1) { 
        if ((sb.st_mode & S_IFMT) == S_IFIFO) { 
            return filename;
        }
        else {
            std::unique_ptr<char, Free> x(canonicalize_file_name(filename.c_str()));
            if (x) { return x.get(); }
        }
    }
    return "";
}

std::pair<std::string, std::string> check_file(std::string const &filename, std::string const &source) {
    struct stat sb;
    if (stat(filename.c_str(), &sb) != -1) {
        if ((sb.st_mode & S_IFMT) == S_IFIFO) {
            return {filename, filename};
        }
        else {
            std::unique_ptr<char, Free> x(canonicalize_file_name(filename.c_str()));
            if (x) { return {x.get(), filename}; }
        }
    }
    else if (filename.compare(0, 1, "/", 1) != 0) {
        std::unique_ptr<char, Free> x(strdup(source.c_str()));
        std::string path = dirname(x.get());
        path.push_back('/');
        path.append(filename);
        if (stat(path.c_str(), &sb) != -1) { 
            if ((sb.st_mode & S_IFMT) == S_IFIFO) { 
                return {path, path};
            }
            else {
                x.reset(canonicalize_file_name(path.c_str()));
                if (x) { return {x.get(), path}; }
            }
        }
    }
    return {"", ""};
}

#else

std::string check_file(std::string const &filename) {
    if (filename == "-" && std::cin.good()) {
        return filename; 
    }
    if (std::ifstream(filename).good()) {
        return filename;
    }
    return "";
}

std::pair<std::string, std::string> check_file(std::string const &filename, std::string const &source) {
    if (std::ifstream(filename).good()) {
        return {filename, filename};
    }
    else {
#if defined _WIN32 || defined __WIN32__ || defined __EMX__ || defined __DJGPP__
        const char *SLASH = "\\";
#else
        const char *SLASH = "/";
#endif
        size_t slash = source.find_last_of(SLASH);
        if (slash != std::string::npos) {
            std::string path = source.substr(0, slash + 1);
            path.append(filename);
            if (std::ifstream(path).good()) {
                return {path, path};
            }
        }
    }
    return {"", ""};
}

#endif

}

// {{{ defintion of NonGroundParser

NonGroundParser::NonGroundParser(INongroundProgramBuilder &pb)
    : not_(FWString::uid("not"))
    , pb_(pb)
    , _startSymbol(0) { }

void NonGroundParser::parseError(Location const &loc, std::string const &msg) {
    GRINGO_REPORT(E_ERROR) << loc << ": error: " << msg << "\n";
}

void NonGroundParser::lexerError(std::string const &token) {
    GRINGO_REPORT(E_ERROR) << filename() << ":" << line() << ":" << column() << ": error: lexer error, unexpected " << token << "\n";
}

bool NonGroundParser::push(std::string const &filename, bool include) {
    return (include && !empty()) ? 
        LexerState::push(filename, {filename, LexerState::data().second}) : 
        LexerState::push(filename, {filename, {"base", {}}});
}

bool NonGroundParser::push(std::string const &filename, std::unique_ptr<std::istream> in) {
    FWString data = filename;
    return LexerState::push(std::move(in), {data, {"base", {}}});
}

void NonGroundParser::pop() { LexerState::pop(); }

FWString NonGroundParser::filename() const { return LexerState::data().first; }

void NonGroundParser::pushFile(std::string &&file) {
    auto checked = check_file(file);
    if (!checked.empty() && !filenames_.insert(checked).second) {
        report_included("<cmd>", file);
    }
    else if (checked.empty() || !push(file)) {
        report_not_found("<cmd>", file);
    }
}

void NonGroundParser::pushStream(std::string &&file, std::unique_ptr<std::istream> in) {
    auto res = filenames_.insert(std::move(file));
    if (!res.second) {
        report_included("<cmd>", *res.first);
    }
    else if (!push(*res.first, std::move(in))) {
        report_not_found("<cmd>", *res.first);
    }
}

void NonGroundParser::pushBlock(std::string const &name, IdVec const &vec, std::string const &block) {
    LexerState::push(gringo_make_unique<std::istringstream>(block), {"<block>", {name, vec}});
}

void NonGroundParser::_init() {
    if (!empty()) {
        Location loc(filename(), 1, 1, filename(), 1, 1);
        IdVecUid params = pb_.idvec();
        for (auto &x : data().second.second) { params = pb_.idvec(params, x.first, x.second); }
        pb_.block(loc, data().second.first, params);
    }
}

int NonGroundParser::lex(void *pValue, Location &loc) {
    if (_startSymbol) {
        auto ret = _startSymbol;
        _startSymbol = 0;
        return ret;
    }
    while (!empty()) {
        int minor = lex_impl(pValue, loc);
        loc.endFilename = filename();
        loc.endLine     = line();
        loc.endColumn   = column();
        if (minor) { return minor; }
        else       { 
            pop();
            _init();
        }
    }
    return 0;
}

void NonGroundParser::include(unsigned sUid, Location const &loc, bool inbuilt) {
    if (inbuilt) {
        if (sUid == FWString("incmode").uid()) {
            if (incmodeIncluded_) {
                report_included(loc, "<incmode>");
            }
            else {
                push("<incmode>", gringo_make_unique<std::istringstream>(R"(
#script (lua) 

function get(val, default)
    if val ~= nil then 
        return val 
    else 
        return default 
    end
end

function main(prg)
    local imin   = get(prg:get_const("imin"), 0)
    local imax   = prg:get_const("imax")
    local istop  = get(prg:get_const("istop"), "SAT")

    local step, ret = 0, gringo.SolveResult.UNKNOWN
    while (imax == nil or step < imax) and
          (step == 0   or step < imin or (
              (istop == "SAT"     and ret ~= gringo.SolveResult.SAT) or
              (istop == "UNSAT"   and ret ~= gringo.SolveResult.UNSAT) or 
              (istop == "UNKNOWN" and ret ~= gringo.SolveResult.UNKNOWN))) do
        local parts = {}
        table.insert(parts, {"check", {step}})
        if step > 0 then
            prg:release_external(gringo.Fun("query", {step-1}))
            prg:cleanup_domains()
            table.insert(parts, {"step", {step}})
        else
            table.insert(parts, {"base", {}})
        end
        prg:ground(parts)
        prg:assign_external(gringo.Fun("query", {step}), true)
        ret, step = prg:solve(), step+1
    end
end

#end.

#program check(t).
#external query(t).
)"));
                incmodeIncluded_ = true;
            }
        }
        else {
            report_not_found(loc, "<" + *FWString(sUid) + ">");
        }
    }
    else {
        auto paths = check_file(*FWString(sUid), *loc.beginFilename);
        if (!paths.first.empty() && !filenames_.insert(paths.first).second) {
            report_included(loc, *FWString(sUid));
        }
        else if (paths.first.empty() || !push(paths.second, true)) {
            report_not_found(loc, *FWString(sUid));
        }
    }
}

bool NonGroundParser::parseDefine(std::string const &define) {
    pushStream("<" + define + ">", gringo_make_unique<std::stringstream>(define));
    _startSymbol = NonGroundGrammar::parser::token::PARSE_DEF;
    NonGroundGrammar::parser parser(this);
    auto ret = parser.parse();
    filenames_.clear();
    return ret == 0;
}

bool NonGroundParser::parse() {
    if (empty()) { return true; }
    _startSymbol = NonGroundGrammar::parser::token::PARSE_LP;
    NonGroundGrammar::parser parser(this);
    _init();
    auto ret = parser.parse();
    filenames_.clear();
    return ret == 0;
}

INongroundProgramBuilder &NonGroundParser::builder() { return pb_; }

unsigned NonGroundParser::aggregate(AggregateFunction fun, unsigned choice, unsigned elems, BoundVecUid bounds) {
    return _aggregates.insert({fun, choice, elems, bounds});
}

HdLitUid NonGroundParser::headaggregate(Location const &loc, unsigned hdaggr) {
    auto aggr = _aggregates.erase(hdaggr);
    if (aggr.choice) { return builder().headaggr(loc, aggr.fun, aggr.bounds, CondLitVecUid(aggr.elems)); }
    else { return builder().headaggr(loc, aggr.fun, aggr.bounds, HdAggrElemVecUid(aggr.elems)); }
}

BdLitVecUid NonGroundParser::bodyaggregate(BdLitVecUid body, Location const &loc, NAF naf, unsigned bdaggr) {
    auto aggr = _aggregates.erase(bdaggr);
    if (aggr.choice) { return builder().bodyaggr(body, loc, naf, aggr.fun, aggr.bounds, CondLitVecUid(aggr.elems)); }
    else { return builder().bodyaggr(body, loc, naf, aggr.fun, aggr.bounds, BdAggrElemVecUid(aggr.elems)); }
}

BoundVecUid NonGroundParser::boundvec(Relation ra, TermUid ta, Relation rb, TermUid tb) {
    auto bound(builder().boundvec());
    auto undef = TermUid(-1);
    if (ta != undef) { builder().boundvec(bound, inv(ra), ta); }
    if (tb != undef) { builder().boundvec(bound, rb, tb); }
    return bound;
}

NonGroundParser::~NonGroundParser() { }

// }}}

} } // namespace Input Gringo

#include "src/input/nongroundlexer.hh"

