library(plyr)
library(ggplot2)

le.arquivos <- function(caminho) {
  arquivos <- list.files(caminho, pattern = ".*.txt")
  ldply(arquivos, .fun = function(x) {
    read.csv(file=x, stringsAsFactors=FALSE)
  })
}

join.nl <- function(...) {
  paste(list(...), collapse='\n')
}

rpd <- function(f.method, f.best) {
  abs((f.method - f.best)/f.best) * 100 
}

rpd.reduce <- function(f, col, f.best) {
  f(sapply(col, function(cur) rpd(cur, f.best)))
}

setwd("C:\\Users\\Italo Silva\\Google Drive\\Faculdade\\IC\\Arquivos\\Experimentos\\2015-2016\\resultados3");

frames <- le.arquivos('.')
result <- ddply(frames, ~ID.Algoritmo, summarise, 
                Media.FO=mean(FO), Mediana.FO=median(FO), Max.FO=max(FO), Min.FO=min(FO),
                Media.Tempo=mean(Tempo.total), Mediana.Tempo=median(Tempo.total), Min.Tempo=min(Tempo.total), Max.Tempo=max(Tempo.total),
                RPD.FO=(function(x) { rpd.reduce(mean, x, max(frames$FO)) })(FO),
                RPD.Tempo=(function(x) { rpd.reduce(mean, x, min(frames$Tempo.total)) })(Tempo.total)
                )

result <- result[order(result$RPD.FO, result$RPD.Tempo),]
head(result, 5)

result$type = grepl("XoPMX", result$ID.Algoritmo)
res <- result[result$type == T,]
mean(res$Media.Tempo)

r3 <- result[c(1:5, 1150:1154, 2300:2304),]
r3$Media.FO <- r3$Media.FO * 0.9848133397522868
r3$Mediana.FO <- r3$Mediana.FO * 0.9848133397522868
r3$Max.FO <- r3$Max.FO * 0.9848133397522868
r3$Min.FO <- r3$Min.FO * 0.9848133397522868
#r3 <- frames[frames$ID.Algoritmo %in% r2$ID.Algoritmo,]
r3$ID.Algoritmo <- gsub("\\.", "\n", r3$ID.Algoritmo)
r3$ID.Algoritmo <- gsub("AI", "ag_max_it=", r3$ID.Algoritmo)
r3$ID.Algoritmo <- gsub("TM", "probMutacao=", r3$ID.Algoritmo)
r3$ID.Algoritmo <- gsub("NI", "PopTam=", r3$ID.Algoritmo)
r3$ID.Algoritmo <- gsub("PC", "pCruz=", r3$ID.Algoritmo)
r3$ID.Algoritmo <- gsub("Xo", "op_cruz=", r3$ID.Algoritmo)
r3$ID.Algoritmo <- gsub("NM", "ntMut=", r3$ID.Algoritmo)
r3$ID.Algoritmo <- gsub("NT", "tor_tam=", r3$ID.Algoritmo)
r3$ID.Algoritmo <- gsub("GI", "grasp_max_it=", r3$ID.Algoritmo)
r3$ID.Algoritmo <- gsub("GV", "num_vizinhos=", r3$ID.Algoritmo)
r3$ID.Algoritmo <- gsub("GA", "alfa=", r3$ID.Algoritmo)
r3$ID.Algoritmo <- factor(r3$ID.Algoritmo, levels = r3$ID.Algoritmo)

ggplot(r3, aes(x = ID.Algoritmo, y = Media.FO)) +
  geom_point(size = 2) +
  geom_errorbar(aes(ymin = Min.FO, ymax = Max.FO))

#ggplot(aes(y=FO, x=factor(ID.Algoritmo)), data=r3) + geom_boxplot()

ggplot(r3, aes(x = ID.Algoritmo, y = Media.Tempo)) + 
  geom_point(size = 2) + 
  geom_errorbar(aes(ymin = Min.Tempo, ymax = Max.Tempo))

#ggplot(aes(y=Tempo.total, x=factor(ID.Algoritmo)), data=r3) 
#  + geom_boxplot()
# 
# r3$nivel <- rep(1:3, each=5)
# agrupado <- ddply(r3, ~nivel, summarise, 
#                   Min.FO=min(Min.FO), 
#                   Max.FO=max(Max.FO), 
#                   Media.FO=mean(Media.FO),
#                   Min.Tempo=min(Min.Tempo),
#                   Max.Tempo=max(Max.Tempo),
#                   Media.Tempo=mean(Media.Tempo))
# agrupado$nivel <- c("Melhores", "Mediana", "Piores")
# 
# agrupado$nivel <- factor(agrupado$nivel, levels = agrupado$nivel)
# ggplot(agrupado, aes(x = nivel, y = Media.FO)) +
#   geom_point(size = 2) +
#   geom_errorbar(aes(ymin = Min.FO, ymax = Max.FO)) +
#   ylab("Função objetivo") +
#   xlab("Grupos (5 configurações cada)")
#   
# ggplot(agrupado, aes(x = nivel, y = Media.Tempo)) +
#   geom_point(size = 2) +
#   geom_errorbar(aes(ymin = Min.Tempo, ymax = Max.Tempo)) +
#   ylab("Tempo de execução") +
#   xlab("Grupos (5 configurações cada)")

agrupa_2 <- r3[c(0, 10:15),]
agrupa_2$ID.Algoritmo <- c(
  'op_cruz={CX, OX}',
  join.nl('op_cruz=PMX', 'ag_max_it=40', 'alfa=60', 'grasp_max_it=25'),
  join.nl('op_cruz=PMX', 'alfa=60', 'grasp_max_it=25'),
  join.nl('op_cruz=PMX', 'alfa=60', 'probMutacao=30', 'grasp_max_it=25'),
  join.nl('op_cruz=PMX', 'alfa=40', 'probMutacao=30', 'grasp_max_it=25', 'nMut=4'),
  join.nl('op_cruz=PMX', 'alfa=40', 'probMutacao=30', 'grasp_max_it=25')
  )

ggplot(agrupa_2, aes(x = ID.Algoritmo, y = Media.FO)) +
  theme(text=element_text(size=18)) +
  geom_point(size = 2) +
  geom_errorbar(aes(ymin = Min.FO, ymax = Max.FO))

ggplot(agrupa_2, aes(x = ID.Algoritmo, y = Media.Tempo)) +
  theme(text=element_text(size=18)) +
  geom_point(size = 2) + 
  geom_errorbar(aes(ymin = Min.Tempo, ymax = Max.Tempo))
  
