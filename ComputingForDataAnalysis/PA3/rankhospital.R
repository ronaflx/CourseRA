rankhospital <- function(state, outcome, num = "best") {
  data <- read.csv("outcome-of-care-measures.csv", colClasses = "character")
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
  if(sum(ok) == 0) {
    stop("invalid state")
  }
  names(data) <- c("State", "Rate", "HospitalName")
  data <- data[ok, ]
  data[, 2] <- as.numeric(data[, 2])
  data <- na.omit(data)
  data <- data[order(data$Rate, data$HospitalName), ]
  if(num == "best") {
    num <- 1
  } else if(num == "worst") {
    num <- nrow(data) 
  }
  data[num, 3]
}
