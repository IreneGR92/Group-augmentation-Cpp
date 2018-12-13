rm(list=ls())
getwd()
setwd('H:\\PhD\\CODE\\Group_augmentation_Cplusplus\\Output_files')
GA<-read.table("group_augmentation_last_generation_m=0.2.txt",header = TRUE,nrow=7969)
GA <- subset(GA, age>0)

#head(GA)
str(GA)
#View(GA)

##FORMULAS

replace_with_zero_if_below_zero <- function(x) {
  x <- ifelse(x<0,0,x)
  #x<-ifelse(GA$type==0,NA,x)
  return(x)
}

###Help

help_Formula<-function(){
  help <- GA$alpha+GA$alphaAge*GA$age+GA$alphaAge2*GA$age*GA$age
  help <- sapply(help, replace_with_zero_if_below_zero)
  return(help)}
 
GA$Help <- help_Formula()
GA[GA$type==0,]$Help<-NA
    

###Dispersal
dispersal_Formula<-function(){
  dispersal<-1 / (1 + exp(GA$betaAge*GA$age - GA$beta))
  return(dispersal)}

GA$Dispersal<-dispersal_Formula()

View(GA)


##DESCRIPTIVE STATISTICS

#summary(GA)

meanAlpha<-round(mean(GA$alpha), digits=4)
meanAlphaAge<-round(mean(GA$alphaAge), digits=4)
meanAlphaAge2<-round(mean(GA$alphaAge2), digits=4)
meanBeta<-round(mean(GA$beta), digits=4)
meanBetaAge<-round(mean(GA$betaAge), digits=4)
meanAge<-round(mean(GA$age), digits=4)
meanHelp<-round(mean(GA$Help,na.rm=TRUE), digits=4)
meanDispersal<-round(mean(GA$Dispersal), digits=4)

SD_Alpha<-round(sd(GA$alpha), digits=4)
SD_AlphaAge<-round(sd(GA$alphaAge), digits=4)
SD_AlphaAge2<-round(sd(GA$alphaAge2), digits=4)
SD_Beta<-round(sd(GA$beta), digits=4)
SD_BetaAge<-round(sd(GA$betaAge), digits=4)
SD_Age<-round(sd(GA$age), digits=4)
SD_Help<-round(sd(GA$Help,na.rm=TRUE), digits=4)
SD_Dispersal<-round(sd(GA$Dispersal), digits=4)

library(formattable)
descriptives <- data.frame(Variable=c("alpha", "alphaAge", "alphaAge2", 
                        "beta", "betaAge", "age", "Help","Dispersal"),
                 Sign=c("=","=","=","=","=","=","=","="),
                 Mean=c(meanAlpha, meanAlphaAge, meanAlphaAge2, 
                        meanBeta, meanBetaAge, meanAge,
                        meanHelp,meanDispersal),
                 Sign=c("±","±","±","±","±","±","±","±"),
                 SD=c(SD_Alpha,SD_AlphaAge,SD_AlphaAge2,
                      SD_Beta,SD_BetaAge,SD_Age,SD_Help,SD_Dispersal))

descriptives


##PLOTS

GA$color <- ifelse(GA$type == 1, "red", "blue")

plot(GA$Help, GA$Dispersal,col=GA$color, xlab="Help", ylab="Dispersal")
title("Dispersal vs Help")



