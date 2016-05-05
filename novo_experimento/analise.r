library(plyr)

le.arquivos <- function(caminho) {
  arquivos <- list.files(caminho)
  ldply(arquivos, read.csv)
}

rpd <- function(f.method, f.best) {
  abs((f.method - f.best)/f.best) * 100 
}

rpd.reduce <- function(f, col, f.best) {
  f(sapply(col, function(cur) rpd(cur, f.best)))
}

frames <- le.arquivos('.')

result <- ddply(frames, ~ID.Algoritmo, summarise, 
                Media.FO=mean(FO), Mediana.FO=median(FO), Max.FO=max(FO), Min.FO=min(FO),
                Media.Tempo=mean(Tempo.total), Mediana.Tempo=median(Tempo.total), Min.Tempo=min(Tempo.total), Max.Tempo=max(Tempo.total),
                RPD.FO=(function(x) { rpd.reduce(mean, x, max(frames$FO)) })(FO),
                RPD.Tempo=(function(x) { rpd.reduce(mean, x, min(frames$Tempo.total)) })(Tempo.total)
                )

result <- ddply(result, .(RPD.FO))
head(result, 5)
