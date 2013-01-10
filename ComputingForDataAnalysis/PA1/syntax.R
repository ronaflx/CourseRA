csv <- read.csv("hw1_data.csv", header <- TRUE)
#csv is a data.frame
className <- class(csv)
print(className)
# get names of each column
dataName <- names(csv)
print(dataName)
# subsetting(single)
# [] return the same data type as original objects.
# here, firstColumn is also a data.frame
firstColumn <- csv[1]
className   <- class(firstColumn)
print(firstColumn)
print(className)
# [[]] return the necessarily data type.
# here, firstColumn is a vector of integer
firstColumn <- csv[[1]]
className   <- class(firstColumn)
print(firstColumn)
print(className)
# the return value of [, 1] is same as [[]]
firstColumn <- csv[, 1]
className   <- class(firstColumn)
print(firstColumn)
print(className)
# [1, ] return the first row and the same data type as original objects.
# here, firstRow is a data.frame of multi data type
firstRow  <- csv[1, ]
className <- class(firstRow)
print(firstRow)
print(className)
# subset by name
# the return value is same as [[]]
firstColumn <- csv$Ozone
className   <- class(firstColumn)
print(firstColumn)
print(className)
# subset by vector of boolean
firstColumn <- csv$Ozone
isNotNa     <- !is.na(firstColumn)
className   <- class(firstColumn)
validColumn <- firstColumn[isNotNa]
print(firstColumn)
print(validColumn)
# some function mean , min, max 
# parameter is a vector of some data type.
# the return value is NA if any of parmeter is NA
# mean with NA
print(mean(firstColumn))
print(mean(validColumn))
# min with NA
print(min(firstColumn))
print(min(validColumn))
# max with NA
print(max(firstColumn))
print(max(validColumn))
# fistColumn and isNotNa is data.frame, but validColumn is not.
firstColumn <- csv[c(1, 2)]
isNotNa     <- !is.na(firstColumn)
validColumn <- firstColumn[isNotNa]
# colMeans function
columns     <- csv[c('Temp', 'Month')]
print(colMeans(columns))