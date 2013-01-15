# Q1
cube <- function(x, n) {
  x^3
}
cube(3)
# Q2
pow <- function(x = 4, n = 3) {
  x^n
}
pow()
# Q3
x <- 1:10
if(x > 5) {
  x <- 0
}
# Q4
library(datasets)
data(iris)
?iris # get help
result <- iris[iris$Species == "virginica", ]
lapply(result[1], mean)
# Q5
apply(iris[, 1:4], 2, mean)
# Q6
library(datasets)
data(mtcars)
?mtcars
mtcars
print(tapply(mtcars$mpg, mtcars$cyl, mean))
result <- sapply(split(mtcars$mpg, mtcars$cyl), mean)
print(result)
# Q7
result <- tapply(mtcars$hp, mtcars$cyl, mean)
print(abs(result[1] - result[3]))
# Q9
f <- function(x) {
  g <- function(y) {
    y + z
  }
  z <- 4
  x + g(x)
}
f(3)
# Q10
debug(ls)
ls(result)