x <- read.csv("hw1_data.csv", header = TRUE)
classType <- sapply(x, class)
nrows <- nrow(x)
ncols <- ncol(x)
# question 1
print(names(x))
# question 2
print(x[1:2, ])
# question 3
print(nrows)
# question 4
print(x[c(nrows - 1, nrows), ])
Ozone = x[, 1]
# question 5
print(Ozone[47])
# question 6
bad = is.na(Ozone)
validOzone <- Ozone[!bad]
print(nrows - length(validOzone))
# question 7
print(mean(validOzone))
# question 8
OzoneGreater <- x$Ozone > 31 & !is.na(x$Ozone)
TempEqual    <- x$Temp  > 90 & !is.na(x$Temp)
notNa        <- !is.na(x$Solar.R)
validBool    <- OzoneGreater & TempEqual & notNa
solar = x$Solar.R[validBool]
print(mean(solar))
# question 9
MonthEqualSix <- x$Month == 6
TempValue = x$Temp[MonthEqualSix]
print(mean(TempValue))