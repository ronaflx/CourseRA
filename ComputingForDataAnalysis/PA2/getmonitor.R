getmonitor <- function(id, directory, summarize = FALSE) {
  path <- sprintf("%s/%03d.csv", directory, strtoi(id))
  if(summarize) {
    print(path)
  }
  read.csv(path, header <- TRUE)
}