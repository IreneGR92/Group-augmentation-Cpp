rm(list=ls())

#DISPERSAL

age<-seq(from=1,by=1, length=15)

dispersal_Formula<-function(beta, betaAge){
  dispersal<-1 / (1 + exp(betaAge*age - beta))
  return(dispersal)}

dispersal<- 1 / (1 + exp(betaAge*age - beta))

plot(age,dispersal_Formula(0,0), type="l", col="red", lwd=3, xlab="age", ylab="dispersal", ylim=range(min=0, max=1))
lines(age,dispersal_Formula(0.25,0.25), type="l", col="blue", lwd=3)
lines(age,dispersal_Formula(-0.25,0.25), type="l", col="green", lwd=4)
lines(age,dispersal_Formula(0.25,-0.25), type="l", col="purple", lwd=4)
lines(age,dispersal_Formula(-0.25,-0.25), type="l", col="black", lwd=4)
title("Dispersal")

#HELP

age<-seq(from=1,by=1, length=15)

help_Formula<-function(alpha, alphaAge, alphaAge2){
  help<-alpha + alphaAge*age + alphaAge2*age*age
  return(help)}

plot(age, help_Formula(2,0.3,0.03), type="l", col="red", lwd=4, xlab="Age", ylab="Help", ylim=range(min=0, max=10))
lines(age,help_Formula(2,0.3,-0.03), type="l", col="blue", lwd=4)
lines(age,help_Formula(2,-0.3,0.03), type="l", col="green", lwd=4)
lines(age,help_Formula(2,-0.3,-0.03), type="l", col="purple", lwd=4)
title("Help")
legend(locator(1),c("linear= +, quadratic= +",
                    "linear= +, quadratic= -", 
                    "linear= -, quadratic= +",
                    "linear= -, quadratic= -"), 
       lwd=c(2,2,2,2), col=c("red","blue","green","purple"), y.intersp=1)


#SURVIVAL

m<-0.2

##Survival(help)

help<-seq(from=0,by=0.3, length=11)
groupSizeC<-10
Xsn<-1

survivalH_F <- function(m,Xsh,Xsn) {
  survivalH <- (1-m)/(1 + exp(Xsh*help - Xsn*(groupSizeC+1)))
  return(survivalH)
}

plot(help,survivalH_F(m,1,Xsn), type="l", col="red", lwd=3, xlab="help", ylab="survival", ylim=range(min=0, max=1))
lines(help,survivalH_F(m,5,Xsn), type="l", col="blue", lwd=3)
lines(help,survivalH_F(m,7,Xsn), type="l", col="green", lwd=3)
title("Survival(h)")
legend(locator(1),c("Xsh=1","Xsh=5","Xsh=7"), lwd=c(2,2,2), col=c("red","blue","green"), y.intersp=1)


##Survival(group size)

groupSize<-c(0:10)
helpC<-2
Xsh<-1

survivalN_F <- function(m,Xsh,Xsn,C) {
  survivalN <- (1-m)/(1 + exp(Xsh*helpC - Xsn*(groupSize+C)))
  return(survivalN)
}

plot(groupSize,survivalN_F(m,Xsh,0.5,1), type="l", col="red", lwd=3, xlab="groupSize", ylab="survival", ylim=range(min=0, max=1))
lines(groupSize,survivalN_F(m,Xsh,1,1), type="l", col="blue", lwd=3)
lines(groupSize,survivalN_F(m,Xsh,2,1), type="l", col="green", lwd=3)
lines(groupSize,survivalN_F(m,Xsh,0.5,-1), type="l", col="purple", lwd=3)
title("Survival(n)")
#legend(locator(1),c("Xsn=0.5/C=1","Xsn=2/C=1","Xsn=0.5/C=0", "Xsn=0.5/C=-1"), lwd=c(2,2), col=c("red","blue","green","purple"), y.intersp=1)



#FECUNDITY

K0<-1
K1<-1 # red
K2<-2 #blue
K3<-0.5 #green
cumhelp<-c(0:10)

#Linear function
#fecundity<-K0 + K1 * cumhelp 

#Logaritmic function
#fecundity1<-K0 + K1 * log(cumhelp+1) 
#fecundity2<-K0 + K2 * log(cumhelp+1)

#Type II functional response
fecundity1<-K0 + cumhelp*K1 / (1 + cumhelp*K1) 
fecundity2<-K0 + cumhelp*K2 / (1 + cumhelp*K2)
fecundity3<-K0 + cumhelp*K3 / (1 + cumhelp*K3)


plot(cumhelp,fecundity1, type="l", col="red", lwd=3, xlab="cumhelp", ylab="fecundity", ylim=range(fecundity1, fecundity2, fecundity3))
lines(cumhelp, fecundity2, type="l", col="blue", lwd=3)
lines(cumhelp, fecundity3, type="l", col="green", lwd=3)
title("Fecundity: Type II functional response")
legend(locator(1),c("K1=1","K1=2", "K1=0.5"), lwd=c(2,2,2), col=c("red","blue","green"), y.intersp=1)
