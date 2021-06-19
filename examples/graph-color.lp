
% PROBLEM INSTANCE

% 6 Nodes
node(1..6).

% 17 Directed Edges
% edge(X,Y) means X->Y
edge(1, 2).
edge(1, 3).
edge(1, 4).
edge(4, 1).
edge(4, 2).
edge(2, 4).
edge(2, 5).
edge(2, 6).
edge(6, 2).
edge(6, 3).
edge(6, 5).
edge(3, 1).
edge(3, 4).
edge(3, 5).
edge(5, 3).
edge(5, 4).
edge(5, 6).

% PROBLEM ENCODING

% Default n (override with gringo --const n=N)
#const n = 3.

% Generate
% Exactly one color to each node
1 { color(X, 1..n) } 1 :- node(X).

% Test
% No Edge can connect two same-colored Nodes
:- edge(X,Y), color(X,C), color(Y,C).

