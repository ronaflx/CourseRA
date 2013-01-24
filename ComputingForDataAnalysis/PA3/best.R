best <- function(state, outcome) {
  data <- read.csv("outcome-of-care-measures.csv", colClasses = "character")
  select <- 0
  if(outcome == "heart attack") {
    select <- c(7, 11, 2)
  } else if(outcome == "heart failure") {
    select <- c(7, 17, 2)
  } else if(outcome == "pneumonia") {
    select <- c(7, 23, 2)
  } else {
    stop("invalid outcome")
  }
  data <- data[, select]
  ok   <- data[, 1] == state
  data <- data[ok, ]
  if(sum(ok) == 0) {
    stop("invalid state")
  }
  data[, 2] <- as.numeric(data[, 2])
  data <- na.omit(data)
  minV <- min(data[, 2])
  ok   <- data[, 2] == minV
  data <- data[ok, ]
  sort(data[, 3])
}