count <- function(cause = NULL) {
  allow <- c("asphyxiation", "blunt force", "other", "shooting", "stabbing", "unknown")
  if(is.null(cause) | sum(allow == tolower(cause)) == 0) {
    stop()
  }
  homicides <- readLines("homicides.txt")
  pattern   <- paste("cause:(\\s)*", cause, sep = "")
  length(grep(pattern, homicides, ignore.case = TRUE))
}