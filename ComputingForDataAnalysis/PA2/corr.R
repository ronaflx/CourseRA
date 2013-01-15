getmonitor <- function(id, directory, summarize = FALSE) {
  len <- nchar(directory)
  if(substr(directory, len, len) == '/') {
    directory = substr(directory, 1, len - 1)
  }
  id  <- paste("00", id, sep = '')
  len <- nchar(id)
  id  <- substr(id, len - 2, len)
  filename <- paste(directory,    id, sep = '/')
  filename <- paste(filename , 'csv', sep = '.')
  data <- read.csv(filename, header <- TRUE)
  if(summarize) {
    print(data)
  }
  data
}
# use complete.cases
getCompleteLength <- function(id, directory) {
  data <- getmonitor(id, directory)
  data <- data[complete.cases(data), ]
  nrow(data)
}
complete <- function(directory, id = 1:332) {
  nobs <- sapply(id, getCompleteLength, directory = directory)
  data.frame(id = id, nobs = nobs)
}
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