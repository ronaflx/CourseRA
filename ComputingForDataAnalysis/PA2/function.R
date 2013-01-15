# call function is similar with python
# args show the definition of funtcion
print(args(sd))
# the parameter can be missing even if it hasn't a default value(lazy evaluation)
# Q1
cube <- function(x, n) {
  x^3
}
# defalt value
# Q2
cube(3)
pow <- function(x = 4, n = 3) {
  x^n
}
pow()
# ... like C
# argument after ... must be named explicitly!!!
# search the environment
search()
# global environment and user's workspace is the first element to search
# package:base is always the last.
print(args(lm))
print(environment(lm))
lm <- function(x) {x ^ 2}
print(args(lm))
print(environment(lm))
# show free variable
ls(environment(lm))
# library() import a package, dan it will be the second to serach, the rest go down
library(grid)
search()
# documentation
library(datasets)
data(iris)