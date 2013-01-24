assignment.1 <- function(outcome) {
  par(mfrow = c(1, 1))
  heartAttack <- as.numeric(outcome[, 11])
  hist(heartAttack, xlab = "30-day Death Rate",
       main = "Heart Attack 30-day Death Attack")
}
## 11, 17, 23 cols
assignment.2 <- function(outcome) {
  heartAttack  <- as.numeric(outcome[, 11])
  heartFailure <- as.numeric(outcome[, 17])
  pneumonia    <- as.numeric(outcome[, 23])
  par(mfrow = c(3, 1))
  lim = range(heartAttack, heartFailure, pneumonia, na.rm = TRUE)
  hist(heartAttack,  xlab = "30-day Death Rate",
       xlim = lim, main = "Heart Attack")
  hist(heartFailure, xlab = "30-day Death Rate",
       xlim = lim, main = "Heart Failure")
  hist(pneumonia,    xlab = "30-day Death Rate",
       xlim = lim, main = "Pneumonia")
}
assignment.3 <- function(outcome) {
  death  <- as.numeric(outcome[, 11])
  state  <- outcome$State
  par(mfrow = c(1, 1), las = 2)
  boxplot(death ~ state, ylab = "30-day Death Rate",
          main = "Heart Attack 30-day Death Rate by State")
}

library(lattice)
?xyplot
assignment.4 <- function(outcome.hospital) {
  death <- as.numeric(outcome.hospital[, 11]) ## Heart attack outcome
  npatient <- as.numeric(outcome.hospital[, 15])
  owner <- factor(outcome.hospital$Hospital.Ownership)
  xyplot(death ~ npatient | owner, 
         xlab = "Number of Patients Seen", 
         ylab = "30-day Death Rate",
         main = "“Heart Attack 30-day Death Rate by Ownership",
         layout = c(3, 3)
         panel = function(x, y, ...) {
           panel.xyplot(x, y, ...)
           panel.lmline(x, y, ...)
         })
}
outcome  <- read.csv("outcome-of-care-measures.csv", colClasses = "character")
hospital <- read.csv("hospital-data.csv", colClasses = "character")
outcome.hospital <- merge(outcome, hospital, by = "Provider.Number")
assignment.1(outcome)
assignment.2(outcome)
assignment.3(outcome) # how to sort
assignment.4(outcome.hospital) # order is wrong

source("submitscript.R")
submit()