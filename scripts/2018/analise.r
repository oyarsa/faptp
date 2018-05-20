library(plyr)
library(ggplot2)

modelo.best <- 530.835

le.arquivos <- function(caminho) {
  arquivos <- list.files(caminho, pattern = ".*.csv")
  ldply(arquivos, .fun = function(x) {
    read.csv(file=x, stringsAsFactors=FALSE)
  })
}

join.nl <- function(...) {
  paste(list(...), collapse='\n')
}

modelo.rpd <- function(f.method) {
  abs((abs(f.method) - modelo.best)/modelo.best) * 100 
}

rpd <- function(f.method, f.best) {
  abs((abs(f.method) - f.best)/f.best) * 100 
}

rpd.reduce <- function(f, col, f.best) {
  f(sapply(col, function(cur) rpd(cur, f.best)))
}

modelo.rpd.reduce <- function(f, col) {
  f(sapply(col, function(cur) modelo.rpd(cur)))
}

coef.variacao <- function(x) {
  x <- abs(x)
  (sd(x) / mean(x)) * 100
}

frames <- le.arquivos('.')
result <- ddply(frames, ~ID_Algoritmo, summarise, 
                RPD.FO=(function(x) { modelo.rpd.reduce(mean, x) })(FO),
                Cv.FO=coef.variacao(FO),
                Media.FO=mean(FO), Mediana.FO=median(FO), Max.FO=max(FO), Min.FO=min(FO),
                Media.Tempo=mean(Tempo), Mediana.Tempo=median(Tempo), Min.Tempo=min(Tempo), Max.Tempo=max(Tempo),
                RPD.Tempo=(function(x) { rpd.reduce(mean, x, min(frames$Tempo)) })(Tempo)
                )

result <- result[order(result$RPD.Tempo),]
head(result, 5)