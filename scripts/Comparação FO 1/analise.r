ag <- read.csv('AG.csv')
ag$ID.Algoritmo <- rep("AG", nrow(ag))

hysst <- read.csv('HySST.csv')
hysst$ID.Algoritmo <- rep("HySST", nrow(hysst))

#modelo <- data.frame("Modelo", 0, 42*60*1000, 530.835)
#names(modelo) <- names(ag)

sails <- read.csv('SA-ILS.csv')
sails$ID.Algoritmo <- rep("SA-ILS", nrow(sails))

wdju <- read.csv('WDJU.csv')
wdju$ID.Algoritmo <- rep("WDJU", nrow(wdju))

total <- rbind(ag, hysst, sails, wdju)#, modelo)
#total <- rbind(sails, wdju)

total$ID.Algoritmo <- as.factor(total$ID.Algoritmo)
total$FO <- sapply(total$FO, abs)

shapiro.test(total$FO)

qqnorm(total$FO)
qqline(total$FO)

kruskal.test(FO~ID.Algoritmo, data=total)

boxplot(FO~ID.Algoritmo, data=total)
