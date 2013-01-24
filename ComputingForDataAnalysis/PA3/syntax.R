# d for density, r forrandom number genreation
# p for cumulative distribution, q for quantile fucntion
str(rnorm) # random normal variate 
str(dnorm) # normal probability density 
str(pnorm) # probability of a random  normal fcuntion less than p
str(qnorm) # probability of a inverse normal function less than q
str(rpois) # random poission variate

# n is the number of variables you want to generate
rnorm(10)

# set.seed, the results of case 1 and case 3 are equal
set.seed(1)
rnorm(10) # case 1
rnorm(10) # case 2
set.seed(1) 
rnorm(10) # case 3

# p & q
ppois(2, 2)
ppois(4, 2)
ppois(6, 2)

# simulate a fucntion y = B0 + B1 * x + e
# e ~ N(0, 2 ^ 2), x ~ N(0, 1 ^ 2), B0 = 0.5, B1 = 2
set.seed(20)
x <- rnorm(100, 0, 1)
e <- rnorm(100, 0, 2)
y <- 0.5 + 2 * x + e
summary(y)
plot(x, y)

# sample function, generate a number of value for a sample
str(sample)
sample(1:10, 10) # a permutation
sample(1:10) # a permutation
sample(1:10, 4) # 4 numbers 
sample(1:10, 11) # error
sample(1:10, replace = TRUE) # may have multiply value
sample(1:10, 11, replace = TRUE) # no error

# base plot
str(plot)
str(hist)
?par
x <- rnorm(100)
y <- rnorm(100)
z <- rnorm(100)
# axis'name is the name of the object if you don't rename it
plot(x, y)
plot(x, z)
hist(x)
# margin size
par(mar = c(2, 2, 2, 2))
plot(x, y)
par(mar = c(4, 4, 2, 2))
# pch symbol
plot(x, y)
plot(x, y , pch = 20)
plot(x, y , pch = 15)
plot(x, y , pch = 10)
plot(x, y , pch = 5)
plot(x, y , pch = 0)
# search for the symbol pch stand for
example(points)
# annotate the plot
plot(x, y)
title("plot")
text(-2, -2, "Label")
legend("topleft", legend = "data", pch = 20)
fit <- lm(y ~ x)
# add a linear model fit
abline(fit)
# change line width and color
abline(fit, lwd = 3, col = 'blue')
# use ... more parameter
plot(x, y, xlab = 'weight', ylab = 'height', main = 'plot', pch = 20)
z <- rpois(100, 2)
par(mfrow = c(2, 1))
plot(x, y, pch = 20)
plot(x, z, pch = 10)
# make plot without data
par(mfrow = c(1, 1))
plot(x, y, type = 'n')
g <- gl(2, 50, label = c("Male", "Female"))
points(x[g == 'Male'], y[g == 'Male'], col = 'red')
points(x[g == 'Female'], y[g == 'Female'], col = 'blue')
# quiz
library(nlme)
library(lattice)
xyplot(weight ~ Time | Diet, BodyWeight)

# lattice plot
x <- rnorm(100)
y <- x + rnorm(100, sd = 0.5)
f <- gl(2, 50, labels = c("Group 1", "Group 2"))
str(xyplot)
xyplot(y ~ x | f)
str(y ~ x | f)
str(y ~ x)

library(lattice)
data(environmental)
?environmental
xyplot(ozone ~ radiation, data = environmental, main = "Ozone vs. radiation")
temp.cut <- equal.count(environmental$temperature, 4)
temp.cut # split data with 4 ranges, may overlap
xyplot(ozone ~ radiation | temp.cut, data = environmental)
xyplot(ozone ~ radiation | temp.cut, data = environmental, layout = c(1, 4))
xyplot(ozone ~ radiation | temp.cut, data = environmental,  as.table = TRUE)
xyplot(ozone ~ radiation | temp.cut, data = environmental,  as.table = TRUE,
       panel = function(x, y, ...) {
         panel.xyplot(x, y, ...)
         fit <- lm(y ~ x)
         panel.abline(fit)
       })
splom(~ environmental)
histogram(~ temperature, data = environmental)

# quiz
library(nlme)
library(lattice)
xyplot(weight ~ Time | Diet, BodyWeight)
# math annotataion
plot(0, 0, main = expression(theta == 0),
     ylab = expression(hat(gamma) == 0), 
     xlab = expression(sum(x[i] * y[i], i == 1, n)))
plot(x, y,
     xlab = substitute(bar(x) == k, list(k = mean(x))),
     ylab = substitute(bar(x) == k, list(k = mean(y))))