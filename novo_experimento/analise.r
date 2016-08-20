library(plyr)

le.arquivos <- function(caminho) {
  arquivos <- list.files(caminho, pattern = ".*.txt")
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

result <- result[order(result$RPD.FO, result$RPD.Tempo),]
head(result, 5)

result$type = grepl("XoPMX", result$ID.Algoritmo)
res <- result[result$type == T,]
mean(res$Media.Tempo)

r3 <- result[c(1:5, 1150:1154, 2300:2304),]
#r3 <- frames[frames$ID.Algoritmo %in% r2$ID.Algoritmo,]
r3$ID.Algoritmo <- gsub("\\.", "\n", r3$ID.Algoritmo)
r3$ID.Algoritmo <- factor(r3$ID.Algoritmo, levels = r3$ID.Algoritmo)

ggplot(r3, aes(x = ID.Algoritmo, y = Media.FO)) +
  geom_point(size = 2) +
  geom_errorbar(aes(ymin = Min.FO, ymax = Max.FO))

ggplot(aes(y=FO, x=factor(ID.Algoritmo)), data=r3) + geom_boxplot()

ggplot(r3, aes(x = ID.Algoritmo, y = Media.Tempo)) + 
  geom_point(size = 2) + 
  geom_errorbar(aes(ymin = Min.Tempo, ymax = Max.Tempo))

ggplot(aes(y=Tempo.total, x=factor(ID.Algoritmo)), data=r3) 
  + geom_boxplot()

r3$nivel <- rep(1:3, each=5)
agrupado <- ddply(r3, ~nivel, summarise, 
                  Min.FO=min(Min.FO), 
                  Max.FO=max(Max.FO), 
                  Media.FO=mean(Media.FO),
                  Min.Tempo=min(Min.Tempo),
                  Max.Tempo=max(Max.Tempo),
                  Media.Tempo=mean(Media.Tempo))
agrupado$nivel <- c("Melhores", "Mediana", "Piores")

agrupado$nivel <- factor(agrupado$nivel, levels = agrupado$nivel)
ggplot(agrupado, aes(x = nivel, y = Media.FO)) +
  geom_point(size = 2) +
  geom_errorbar(aes(ymin = Min.FO, ymax = Max.FO)) +
  ylab("Função objetivo") +
  xlab("Grupos (5 configurações cada)")
  
ggplot(agrupado, aes(x = nivel, y = Media.Tempo)) +
  geom_point(size = 2) +
  geom_errorbar(aes(ymin = Min.Tempo, ymax = Max.Tempo)) +
  ylab("Tempo de execução") +
  xlab("Grupos (5 configurações cada)")
