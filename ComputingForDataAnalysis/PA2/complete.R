source("getmonitor.R")
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