rm(list=ls())
getwd()
setwd('H:\\PhD\\CODE\\Group_augmentation_Cplusplus\\Output_files\\20_replicas')
#setwd('C:\\Users\\igaru\\Documents\\GitHub\\Group_augmentation_Cplusplus\\Group_augmentation_RN\\Group_augmentation_RN')
GA<-read.table("group_augmentation_init.txt",header = TRUE,skip=27)

library("ggplot2")


#head(GA)
#str(GA)
#dim(GA)
#View(GA)
#summary(GA)


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


################## PLOTS ############################

##Help plot
ggplot(GA_means, aes(x=GA_means$Generation, y=GA_means$meanHelp)) +
  geom_ribbon(aes(ymin=GA_means$meanHelp-GA_SD$meanHelp, ymax=GA_means$meanHelp+GA_SD$meanHelp),
              alpha=0.3) +
  geom_line(color="red", size=1)+
  xlab("Generation")+ ylab("Help")+
  coord_cartesian(ylim = c(0.049, 1)) 


##Dispersal plot
ggplot(GA_means, aes(x=GA_means$Generation, y=GA_means$meanDispersal)) +
  geom_ribbon(aes(ymin=GA_means$meanDispersal-GA_SD$meanDispersal, ymax=GA_means$meanDispersal+GA_SD$meanDispersal),
              alpha=0.3) +
  geom_line(color="blue", size=1)+
  xlab("Generation")+ ylab("Dispersal")+
  coord_cartesian(ylim = c(0.049, 1)) 

##Relatedness plot
ggplot(GA_means, aes(x=GA_means$Generation, y=GA_means$Relatedness)) +
  geom_ribbon(aes(ymin=GA_means$Relatedness-GA_SD$Relatedness, ymax=GA_means$Relatedness+GA_SD$Relatedness),
              alpha=0.3) +
  geom_line(color="orange", size=1)+
  xlab("Generation")+ ylab("Relatedness")+
  coord_cartesian(ylim = c(0.049, 1)) 


##Population stability?
ggplot(GA_means, aes(x=GA_means$Generation, y=GA_means$Group_size)) +
  geom_ribbon(aes(ymin=GA_means$Group_size-GA_SD$Group_size, ymax=GA_means$Group_size+GA_SD$Group_size),
              alpha=0.3) +
  geom_line(color="purple", size=1)+
  xlab("Generation")+ ylab("Group size")
  


################## STATS LAST GENERATION ############################

##Means and SD of the means of the variables between the different replicas

do_mean_LG<-function(x){
  x<-round(mean(x[GA$Generation==100000]), digits = 4)
  return(x)
}

do_SD_LG<-function(x){
  x<-round(sd(x[GA$Generation==100000]), digits = 4)
  return(x)
}

meanAlpha<-do_mean_LG(GA$meanAlpha)
meanAlphaAge<-do_mean_LG(GA$meanAlphaAge)
meanAlphaAge2<-do_mean_LG(GA$meanAlphaAge2)
meanBeta<-do_mean_LG(GA$meanBeta)
meanBetaAge<-do_mean_LG(GA$meanBetaAge)
meanAge<-do_mean_LG(GA$Age)
meanHelp<-do_mean_LG(GA$meanHelp)
meanDispersal<-do_mean_LG(GA$meanDispersal)
meanRelatedness<-do_mean_LG(GA$Relatedness)
meanCorr_Help_Disp<-do_mean_LG(GA$corr_Help_Disp)

SD_Alpha<-do_SD_LG(GA$meanAlpha)
SD_AlphaAge<-do_SD_LG(GA$meanAlphaAge)
SD_AlphaAge2<-do_SD_LG(GA$meanAlphaAge2)
SD_Beta<-do_SD_LG(GA$meanBeta)
SD_BetaAge<-do_SD_LG(GA$meanBetaAge)
SD_Age<-do_SD_LG(GA$Age)
SD_Help<-do_SD_LG(GA$meanHelp)
SD_Dispersal<-do_SD_LG(GA$meanDispersal)
SD_Relatedness<-do_SD_LG(GA$Relatedness)
SDcorr_Help_Disp<-do_SD_LG(GA$corr_Help_Disp)

corr_Help_Relat<-(sum(GA$meanHelp[GA$Generation==100000]*GA$Relatedness[GA$Generation==100000]) / 20 - meanHelp * meanRelatedness) / (SD_Help*SD_Relatedness)
corr_Disp_Relat<-(sum(GA$meanDispersal[GA$Generation==100000]*GA$Relatedness[GA$Generation==100000]) / 20 - meanDispersal * meanRelatedness) / (SD_Dispersal*SD_Relatedness)

library(formattable)

descriptives <- data.frame(Variable=c("alpha", "alphaAge", "alphaAge2",
                                      "beta", "betaAge", "age", 
                                      "Help","Dispersal", "Relatedness","Help_Disp","Help_Relat","Disp_Relat"),
                           Sign=c("=","=","=","=","=","=","=","=","=","=","=","="),
                           Mean=c(meanAlpha, meanAlphaAge, meanAlphaAge2,
                                  meanBeta, meanBetaAge, meanAge,
                                  meanHelp,meanDispersal,meanRelatedness,meanCorr_Help_Disp,corr_Help_Relat,corr_Disp_Relat),
                           Sign=c("±","±","±","±","±","±","±","±","±","±"," "," "),
                           SD=c(SD_Alpha,SD_AlphaAge,SD_AlphaAge2,
                                SD_Beta,SD_BetaAge,SD_Age,
                                SD_Help,SD_Dispersal,SD_Relatedness,SDcorr_Help_Disp," "," "))

descriptives










# #Genes set as factors by default, change to doubles
# if (is.factor(GA$meanAlpha)) {
#   meanAlpha<-as.numeric(levels(GA$meanAlpha))[GA$meanAlpha]
# } else{meanAlpha<-GA$meanAlpha}

#plot(GA$Generation,help_Formula(1), type="l", col="red", lwd=2, xlab="Generation", ylab="Help",ylim=range(min=0, max=1))
#lines(GA$Generation,help_Formula(5), type="l", col="blue", lwd=1.8)
#lines(GA$Generation,help_Formula(7), type="l", col="green", lwd=1.8)
#title("Help as reaction norm to age")
#legend(locator(1),c("age=1","age=5","age=7"), lwd=c(2,2,2), col=c("red","blue","green"), y.intersp=0.8)

#Mean and SD of Relatedness for last generation
# 
# meanRelatedness<-round(mean(GA$Relatedness[GA$Generation==100000]), digits = 4)
# sdRelatedness<-round(sd(GA$Relatedness[GA$Generation==100000]), digits = 4)
# 
# descriptives <- data.frame(Variable=c("relatedness"),
#                            Sign=c("="),
#                            Mean=c(meanRelatedness),
#                            Sign=c("±"),
#                            SD=c(sdRelatedness))
# descriptives
