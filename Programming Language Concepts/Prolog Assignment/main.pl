:- module(main, [is_movie_directed_by/2, total_awards_nominated/2, all_movies_directed_by/2, total_movies_released_in/3, all_movies_released_between/4]).
:- [kb].

% DO NOT CHANGE THE UPPER CONTENT, WRITE YOUR CODE AFTER THIS LINE


is_movie_directed_by(Title,Director):- movie(Title, Director, Year, OscarNoms, EmmyNoms, GoldenGlobeNoms).

total_awards_nominated(Title,Nominations):- movie(Title, Director, Year, OscarNoms, EmmyNoms, GoldenGlobeNoms),
Nominations is EmmyNoms+OscarNoms+GoldenGlobeNoms.


all_movies_directed_by(Director,Movies):- 
findall(Title,is_movie_directed_by(Title,Director), Movies).






total_movies_released_in(Movies,Year,Count):- 
findall(Title,movie(Title,Director,Year,OscarNoms,EmyNoms,GoldenGlobeNoms),NewList),
intersection(NewList,Movies,ResultList),
length(ResultList,Count).





is_it_between(Title,Year,MinYear,MaxYear):- movie(Title, Director, Year, OscarNoms, EmmyNoms, GoldenGlobeNoms),
MinYear=<Year, MaxYear>=Year.

all_movies_released_between( Movies , MinYear , MaxYear , MoviesBetweenGivenYears ):-
findall(Title,is_it_between(Title,Year,MinYear,MaxYear),Newlist),
intersection(Movies,Newlist,MoviesBetweenGivenYears).













