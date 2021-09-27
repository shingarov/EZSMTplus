% Instance
disk(1..4).
peg(a;b;c).

init_on(1..4,a).
goal_on(1..4,c).

% fuel for moves/1
moves(15).

% Encoding: Generate -> Define -> Test.

% Generate
1 { move(D,P,T) : disk(D), peg(P) } 1 :- T = 1..M, moves(M).

% Define
move(D,T) :- move(D,_,T).
on(D,P,0) :- init_on(D,P).
on(D,P,T) :- move(D,P,T). % focus
on(D,P,T+1) :- on(D,P,T), not move(D,T+1), not moves(T). % frame
blocked(D-1,P,T+1) :- on(D,P,T), not moves(T).
blocked(D-1,P,T) :- blocked(D,P,T), disk(D).

% Test
:- move(D,P,T), blocked(D-1,P,T).
:- move(D,T), on(D,P,T-1), blocked(D,P,T).
:- not 1 { on(D,P,T) : peg(P) } 1, disk(D), T=1..M, moves(M).

:- goal_on(D,P), not on(D,P,M), moves(M).

#show move/3.
