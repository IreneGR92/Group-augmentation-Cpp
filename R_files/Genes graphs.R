rm(list=ls())
getwd()
#setwd('H:\\PhD\\CODE\\Group_augmentation_Cplusplus\\Output_files')
setwd('C:\\Users\\igaru\\Documents\\GitHub\\Group_augmentation_Cplusplus\\Group_augmentation_RN\\Group_augmentation_RN')
GA<-read.table("group_augmentation_init.txt",header = TRUE,skip=27)

library("lattice")
library("Formula")
library("survival")
library("ggplot2")
library("Hmisc")

#head(GA)
str(GA)
#dim(GA)
#View(GA)
#summary(GA)

##Formulas
replace_with_zero_if_below_zero <- function(x) {
  x <- ifelse(x<0,0,x)
  return(x)
}

help_Formula<-function(){
  help<-GA$meanAlpha+GA$meanAlphaAge*GA$Age+GA$meanAlphaAge2*GA$Age*GA$Age
  help <- sapply(help, replace_with_zero_if_below_zero)
  return(help)}

GA$Help <- help_Formula()#includes breeders 

dispersal_Formula<-function(){
  dispersal<-1 / (1 + exp(GA$meanBetaAge*GA$Age - GA$meanBeta))
  return(dispersal)}

GA$Dispersal<-dispersal_Formula()


##Means between replicas

do_mean<-function(x){
  x<-aggregate(x, list(GA$Generation), mean) 
  return(x)
}
do_sd<-function(x){
  x<-aggregate(x, list(GA$Generation), sd) 
  return(x)
}

GA_means<-do_mean(GA)
GA_SD<-do_sd(GA)



##Help plot
ggplot(GA_means, aes(x=GA_means$Generation, y=GA_means$Help)) +
  geom_ribbon(aes(ymin=GA_means$Help-GA_SD$Help, ymax=GA_means$Help+GA_SD$Help),
              alpha=0.3) +
  geom_line(color="red", size=1)+
  xlab("Generation")+ ylab("Help")+title("Help")+
  coord_cartesian(ylim = c(0.049, 1)) 


##Dispersal plot
ggplot(GA_means, aes(x=GA_means$Generation, y=GA_means$Dispersal)) +
  geom_ribbon(aes(ymin=GA_means$Dispersal-GA_SD$Dispersal, ymax=GA_means$Dispersal+GA_SD$Dispersal),
              alpha=0.3) +
  geom_line(color="blue", size=1)+
  xlab("Generation")+ ylab("Dispersal")+title("Dispersal")+
  coord_cartesian(ylim = c(0.049, 1)) 

##Relatedness plot
ggplot(GA_means, aes(x=GA_means$Generation, y=GA_means$Relatedness)) +
  geom_ribbon(aes(ymin=GA_means$Relatedness-GA_SD$Relatedness, ymax=GA_means$Relatedness+GA_SD$Relatedness),
              alpha=0.3) +
  geom_line(color="orange", size=1)+
  xlab("Generation")+ ylab("Relatedness")+title("Relatedness")+
  coord_cartesian(ylim = c(0.049, 1)) 


##Population stability?
ggplot(GA_means, aes(x=GA_means$Generation, y=GA_means$Group_size)) +
  geom_ribbon(aes(ymin=GA_means$Group_size-GA_SD$Group_size, ymax=GA_means$Group_size+GA_SD$Group_size),
              alpha=0.3) +
  geom_line(color="purple", size=1)+
  xlab("Generation")+ ylab("Group size")+title("Group size")
  











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


#plot(GA$Generation,help_Formula(1), type="l", col="red", lwd=2, xlab="Generation", ylab="Help",ylim=range(min=0, max=1))
#lines(GA$Generation,help_Formula(5), type="l", col="blue", lwd=1.8)
#lines(GA$Generation,help_Formula(7), type="l", col="green", lwd=1.8)
#title("Help as reaction norm to age")
#legend(locator(1),c("age=1","age=5","age=7"), lwd=c(2,2,2), col=c("red","blue","green"), y.intersp=0.8)

#plot(GA_means$Generation,GA_means$Help, type="l", col="red",lwd=2, xlab="Generation", ylab="Help",ylim=range(min=0, max=1))
#title("Help")
