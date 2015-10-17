

len([], 0).

len([H|T], X) :- len(T, Y), X is Y+1.

