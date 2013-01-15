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