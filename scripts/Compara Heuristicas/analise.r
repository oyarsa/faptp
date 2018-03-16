library(plyr)
library(ggplot2)

best <- 530.835

le.arquivos <- function(caminho) {
  arquivos <- list.files(caminho, pattern = ".*.csv")
  ldply(arquivos, .fun = function(x) {
    read.csv(file=x, stringsAsFactors=FALSE)
  })
}

join.nl <- function(...) {
  paste(list(...), collapse='\n')
}

rpd <- function(f.method, f.best) {
  abs((abs(f.method) - best)/best) * 100 
}

rpd.reduce <- function(f, col, f.best) {
  f(sapply(col, function(cur) rpd(cur, f.best)))
}

coef.variacao <- function(x) {
  x <- abs(x)
  (sd(x) / mean(x)) * 100
}

frames <- le.arquivos('.')
result <- ddply(frames, ~ID.Algoritmo, summarise, 
                RPD.FO=(function(x) { rpd.reduce(mean, x, max(frames$FO)) })(FO),
                Cv.FO=coef.variacao(FO),
                Media.FO=mean(FO), Mediana.FO=median(FO), Max.FO=max(FO), Min.FO=min(FO),
                Media.Tempo=mean(Tempo.total), Mediana.Tempo=median(Tempo.total), Min.Tempo=min(Tempo.total), Max.Tempo=max(Tempo.total),
                RPD.Tempo=(function(x) { rpd.reduce(mean, x, min(frames$Tempo.total)) })(Tempo.total)
                )

result <- result[order(result$RPD.FO, result$RPD.Tempo),]
head(result, 5)