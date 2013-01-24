rankall <- function(outcome, num = "best") {
  data <- read.csv("outcome-of-care-measures.csv", colClasses = "character")
  if(outcome == "heart attack") {
    data <- data[, c(7, 11, 2)]
  } else if(outcome == "heart failure") {
    data <- data[, c(7, 17, 2)]
  } else if(outcome == "pneumonia") {
    data <- data[, c(7, 23, 2)]
  } else {
    stop("invalid outcome")
  }
  data[, 2] <- as.numeric(data[, 2])
  data <- na.omit(data)
  # state, rate, name
  data   <- data[order(data[, 1], data[, 2], data[, 3]), ]
  newState <- c()
  newName  <- c()
  i <- 1
  n <- nrow(data)
  while(i <= n) {
    j <- i
    while(j <= n && data[i, 1] == data[j, 1]) {
      j <- j + 1
    }
    select <- num
    if(select == "best") {
      select <- 1
    } else if(select == "worst") {
      select <- j - i
    }
    newState <- rbind(newState, data[i, 1])
    if(i + select <= j) {
      newName <- rbind(newName, data[i + select - 1, 3])
    } else {
      newName <- rbind(newName, NA)
    }
    i <- j
  }
  result <- data.frame(hospital = newName,  state = newState, row.names = newState)
  result
}