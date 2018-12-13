############# Quadratic reaction norms #########################################

library("lattice")

# Univariate reaction norm -----------------------------------------------------

QuadReactNormUni<-function(parameters,x){
  y<-parameters[1]+parameters[2]*x+parameters[3]*x^2
  y<-ifelse(y<0,0,y) #not allow negative values
  return(y)
}

rangX<-seq(0,20,length.out = 100)

rangQuad<-c(0.5,-0.5,0)

varThird<-data.frame(third.1=QuadReactNormUni(parameters = c(5,10,rangQuad[1]),
                                           rangX),
                     third.2=QuadReactNormUni(parameters = c(5,10,rangQuad[2]),
                                           rangX),
                     third.3=QuadReactNormUni(parameters = c(5,10,rangQuad[3]),
                                           rangX))

matplot(cbind(rangX,rangX,rangX),varThird,type='l',ylab="Help",xlab="Age")

legend("topleft",legend = c(0.5,-0.5,0),col = 1:3,lty = 1:3,
       title = "Quadratic parameter")


# Bivariate reaction norm -----------------------------------------------------

QuadReactNormBi<-function(parameters,x,y){
  y<-parameters[1]+parameters[2]*x+parameters[3]*x^2+parameters[4]*y+
    parameters[5]*y^2
  y<-ifelse(y<0,0,y)
  return(y)
}

rangx<-seq(0,20,length.out = 100)

rangy<-seq(0,20,length.out = 100)

param<-c(5,10,-5,10,-5)

gridxy<-expand.grid(rangx,rangy)
names(gridxy)<-c('x',"y")

gridxy$help<-QuadReactNormBi(param,gridxy$x,gridxy$y)

levelplot(help~x*y,data=gridxy)

