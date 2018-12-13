rm(list=ls())
getwd()
setwd('H:\\PhD\\CODE\\Group_augmentation_Cplusplus\\Output_files')
GA<-read.table("group_augmentation_m=0.2.txt",header = TRUE,nrows=3003)

#head(GA)
str(GA)
#dim(GA)
#View(GA)
#summary(GA)

##Formulas

help_Formula<-function(age){
  help<-GA$meanAlpha+GA$meanAlphaAge*age+GA$meanAlphaAge2*age*age
  return(help)}

dispersal_Formula<-function(age){
  dispersal<-1 / (1 + exp(GA$meanBetaAge*age - GA$meanBeta))
  return(dispersal)}


##Help plot
plot(GA$Generation,help_Formula(1), type="l", col="red", lwd=2, xlab="Generation", ylab="Help",ylim=range(min=0, max=1))
lines(GA$Generation,help_Formula(5), type="l", col="blue", lwd=1.8)
lines(GA$Generation,help_Formula(7), type="l", col="green", lwd=1.8)
title("Help as reaction norm to age")
legend(locator(1),c("age=1","age=5","age=7"), lwd=c(2,2,2), col=c("red","blue","green"), y.intersp=0.8)

 
##Dispersal plot

plot(GA$Generation,dispersal_Formula(1), type="l", col="red", lwd=2, xlab="Generation", ylab="Dispersal", ylim=range(min=0, max=1)) 
lines(GA$Generation,dispersal_Formula(5), type="l", col="blue", lwd=2)
title("Dispersal as reaction norm to age")
#legend(locator(1),c("age=1","age=5"), lwd=c(2,2), col=c("red","blue"), y.intersp=1)

##Relatedness plot

plot(GA$Generation, GA$Relatedness, type="l", col="orange", lwd=2, xlab="Generation", ylab="Relatedness", ylim=range(min=0, max=1))
title("Relatedness")


##Population stability?

plot(GA$Generation, GA$Group_size, type="l", col="blue", lwd=2, xlab="Generation", ylab="Group size")
title("Group size")











# #Genes set as factors by default, change to doubles
# if (is.factor(GA$meanAlpha)) {
#   meanAlpha<-as.numeric(levels(GA$meanAlpha))[GA$meanAlpha]
# } else{meanAlpha<-GA$meanAlpha}
# 
# if (is.factor(GA$meanAlphaAge)) {
#   meanAlphaAge<-as.numeric(levels(GA$meanAlphaAge))[GA$meanAlphaAge]
# } else{meanAlphaAge<-GA$meanAlphaAge}
# 
# if (is.factor(GA$meanAlphaAge2)) {
#   meanAlphaAge2<-as.numeric(levels(GA$meanAlphaAge2))[GA$meanAlphaAge2]
# } else{meanAlphaAge2<-GA$meanAlphaAge2}
# 
# if (is.factor(GA$meanBeta)) {
#   meanBeta<-as.numeric(levels(GA$meanBeta))[GA$meanBeta]
# } else{meanBeta<-GA$meanBeta}
# 
# if (is.factor(GA$meanBetaAge)) {
#   meanBetaAge<-as.numeric(levels(GA$meanBetaAge))[GA$meanBetaAge]
# } else{meanBetaAge<-GA$meanBetaAge}
# 
# if (is.factor(GA$Relatedness)) {
#   Relatedness<-as.numeric(levels(GA$Relatedness))[GA$Relatedness]
# } else{Relatedness<-GA$Relatedness}
# 
# 
# #Attention with the NA introduced!
# summary(meanAlpha)
# summary(meanAlphaAge)
# summary(meanAlphaAge2)
# summary(meanBeta)
# summary(meanBetaAge)
# summary(Relatedness)
