x <- read.csv("hw1_data.csv", header = TRUE)
classType <- sapply(x, class)
nrows <- nrow(x)
ncols <- ncol(x)
Ozone <- x[, 1]

# question 1
names(x)
# question 2
x[1:2, ]
# question 3
nrows
# question 4
x[c(nrows - 1, nrows), ]
# question 5
Ozone[47]
# question 6
sum(is.na(Ozone))
# question 7
mean(Ozone[complete.cases(Ozone)])
# question 8
mean(x[(complete.cases(x) & x$Ozone > 31 & x$Temp > 90), ]$Solar.R)
# question 9 
mean(x$Temp[x$Month == 6])