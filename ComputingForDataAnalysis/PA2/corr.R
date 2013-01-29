source("complete.R")
# greater than
greater <- function(obj1, obj2) {
  obj1 > obj2
}
getCor <- function(id, directory) {
  data <- getmonitor(id, directory)[, c(2, 3)]
  data <- data[complete.cases(data), ]
  cor(data$sulfate, data$nitrate)
}
corr <- function(directory, threshold = 0) {
  data   <- complete(directory)
  select <- sapply(data[, 2], greater, obj2 = threshold)
  legal  <- data[, 1][select]
  sapply(legal, getCor, directory = directory)
}