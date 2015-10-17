
friend(kimmy, jimmy).

friend(jimmy, lin).

friend(X, Z) :- friend(X, Y), friend(Y, Z).

love(john, mary).
love(henry, mary).

jealous(X, Y) :- love(X, Z), love(Y, Z), X \= Y.

