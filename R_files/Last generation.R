rm(list=ls())
library(data.table)
getwd()
setwd('H:\\PhD\\CODE\\Group_augmentation_Cplusplus\\Output_files')
GA2<-read.table("group_augmentation_last_generation_m=0.2.txt",header = TRUE, skip=27)
GA2 <- subset(GA2, age>0)
setDF(GA2)


#head(GA2)
str(GA2)
#View(GA2)

##FORMULAS

replace_with_zero_if_below_zero <- function(x) {
  x <- ifelse(x<0,0,x)
  return(x)
}

###Help

help_Formula<-function(){
  help <- GA2$alpha+GA2$alphaAge*GA2$age+GA2$alphaAge2*GA2$age*GA2$age
  help <- sapply(help, replace_with_zero_if_below_zero)
  return(help)}
 
GA2$Help <- help_Formula()
GA2[GA2$type==0,]$Help<-NA
    

###Dispersal
dispersal_Formula<-function(){
  dispersal<-1 / (1 + exp(GA2$betaAge*GA2$age - GA2$beta))
  return(dispersal)}

GA2$Dispersal<-dispersal_Formula()

#View(GA2)


##DESCRIPTIVE STATISTICS

###Means and SD by replicas
means<-round(aggregate(GA2[, 4:12], list(GA2$replica), mean, na.rm=TRUE), digits = 4)

SD<-round(aggregate(GA2[, 4:12], list(GA2$replica), sd, na.rm=TRUE), digits = 4)


library(formattable)

#3 replicas in this case (change values when more replicas applied)
descriptives <- data.frame(Variable=c("alpha", "alphaAge", "alphaAge2",
                                      "beta", "betaAge", "age", "Help","Dispersal"),
                 Sign=c("=","=","=","=","=","=","=","="),
                 Mean=c(means[mean(1:3),3],means[mean(1:3),4],means[mean(1:3),5],
                        means[mean(1:3),6],means[mean(1:3),7],means[mean(1:3),2],
                        means[mean(1:3),9],means[mean(1:3),10]),
                 Sign=c("±","±","±","±","±","±","±","±"),
                 SD=c(SD[mean(1:3),3],SD[mean(1:3),4],SD[mean(1:3),5],
                      SD[mean(1:3),6],SD[mean(1:3),7],SD[mean(1:3),2],
                      SD[mean(1:3),9],SD[mean(1:3),10]))

descriptives


##PLOTS

GA2$color <- ifelse(GA2$type == 1, "red", "blue")

plot(GA2$Help, GA2$Dispersal,col=GA2$color, xlab="Help", ylab="Dispersal")
title("Dispersal vs Help")
























###Means and SD across all replicates

# meanAlpha<-round(mean(GA2$alpha), digits=4)
# meanAlphaAge<-round(mean(GA2$alphaAge), digits=4)
# meanAlphaAge2<-round(mean(GA2$alphaAge2), digits=4)
# meanBeta<-round(mean(GA2$beta), digits=4)
# meanBetaAge<-round(mean(GA2$betaAge), digits=4)
# meanAge<-round(mean(GA2$age), digits=4)
# meanHelp<-round(mean(GA2$Help,na.rm=TRUE), digits=4)
# meanDispersal<-round(mean(GA2$Dispersal), digits=4)
# 
# SD_Alpha<-round(sd(GA2$alpha), digits=4)
# SD_AlphaAge<-round(sd(GA2$alphaAge), digits=4)
# SD_AlphaAge2<-round(sd(GA2$alphaAge2), digits=4)
# SD_Beta<-round(sd(GA2$beta), digits=4)
# SD_BetaAge<-round(sd(GA2$betaAge), digits=4)
# SD_Age<-round(sd(GA2$age), digits=4)
# SD_Help<-round(sd(GA2$Help, na.rm=TRUE), digits=4)
# SD_Dispersal<-round(sd(GA2$Dispersal), digits=4)
# 
# descriptives <- data.frame(Variable=c("alpha", "alphaAge", "alphaAge2",
#                                       "beta", "betaAge", "age", "Help","Dispersal"),
#                            Sign=c("=","=","=","=","=","=","=","="),
#                            Mean=c(meanAlpha, meanAlphaAge, meanAlphaAge2,
#                                   meanBeta, meanBetaAge, meanAge,
#                                   meanHelp,meanDispersal),
#                            Sign=c("±","±","±","±","±","±","±","±"),
#                            SD=c(SD_Alpha,SD_AlphaAge,SD_AlphaAge2,
#                                 SD_Beta,SD_BetaAge,SD_Age,SD_Help,SD_Dispersal))

