# if statement
# assign inside
cond <- 1
val  <- NA
if (cond == 1) {
  val <- 1
} else {
  val <- 0
}
print(val)
# assign outside
val <- if(cond == 1) {
  0
} else {
  1
}
print(val)
# if length(cond) is greater than 1, only the first element is used
cond <- 1:10
if(cond < 5) {
  cond <- 0
}
print(cond)
# for loop
# each i is a numeric 
for(i in 1:10) {
  print(i)
}
x <- c("a", "b", "c", "d")
# index 
for(i in 1:4) {
  print(x[i])
}
# seq_along return a vector(1:length(x))
for(i in seq_along(x)) {
  print(x[i])
}
# similar with python
for(ele in x) {
  print(ele)
}
# nrow(mat) is a integer equal to 2
# seq_len(x) generate a vector 1:x
mat <- matrix(1:6, 2, 3)
rows = nrow(mat)
cols = ncol(mat)
# follow two for loop is same
print(seq_len(rows))
for(i in seq_len(rows)) {
  for(j in seq_len(cols)) {
    print(mat[i, j])
  }
}
for(i in 1:rows) {
  for(j in 1:cols) {
    print(mat[i, j])
  }
}
# while loop
count <- 0
while(count < 10) {
  print(count)
  count <- count + 1
}
# rbinom generate a Binomial distribution rbinom(n, size, p)
# size is number of trials
count <- 5
while(count >= 3 && count <= 10) {
  print(count)
  coin <- rbinom(1, 1, 0.5)
  if(coin == 1) {
    count <- count + 1
  } else {
    count <- count - 1
  }
}
# repeat statement & break statement
repeat {
  coin <- rbinom(1, 1, 0.5)
  if(coin == 1) {
    break
  } else {
    print(coin)
  }
}
# next statement
for(i in 1: 100) {
  if(i < 90) {
    print(i)
    next
  }
  print(1: i)
}
