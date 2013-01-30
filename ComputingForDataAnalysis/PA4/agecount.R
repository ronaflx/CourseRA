agecount <- function(age = NULL) {
  if(is.null(age)) {
    stop()
  }
  homicides <- readLines("homicides.txt")
  pattern <- sprintf("age(\\s)*:(\\s)*%d(\\D)|%d year[s]?( old)?", age, age)
  length(grep(pattern, homicides, ignore.case = TRUE))
}