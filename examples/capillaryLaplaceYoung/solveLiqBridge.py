# J. Duriez (jerome.duriez@irstea.fr)

# to import with yade/python/ipython solveLiqBridge.py, or with execfile('solveLiqBridge.py',globals()) once inside a yade/python/ipython session

# -- Necessary imports when launching the script directly with python/ipython, not necessary within Yade --
# import numpy
# from math import sin,pi,cos,sqrt,tan
# --- End of "ipython necessary imports" ---
import scipy # for root finding function (see below), requires installing python-scipy package
from scipy import optimize


def solveLiqBridge(rRatio,theta,uStar,delta1,deltaZ,plot,plot3D,speak):
    #Computes one single liquid (capillary) bridge
    #   Lian's method: iterative determination using Taylor expansion with
    #   finite difference (FD) increment = deltaZ (a function attribute)

    #   Attributes: rRatio = R2 / R1 >=1, theta = contact angle (deg), uStar =
    #   dimensionless cap. pressure (uc*R2/gamma), delta1 = filling angle on
    #   smallest particle (deg), deltaZ = increment of FD scheme (e.g. 2e-6)
    #   Last ones = booleans to resp. plot the profile in 2D, output messages,
    #   and plot the liq bridge in 3D

    # Outputs: dist = interparticle distance ; 
    # vol = dimensionless volume (= vol /R2^3)
    # force = dimensionless force = force/(2*pi*gamma*R2)
    # delta1/2 = filling angles on the 2 particles (degrees)
    # eStar = free energy/(gamma* R2^2)
    # nn11 = 11-term of integral(ni nj dS) / R2^2 = 22-term 
    # nn33 = 33-term, with 3 axis = the meniscus orientation
    # NB: surface / R2^2 = 2*nn11 + nn33
    # out = 1 or 0 depending whether things went fine (see below): 1 if yes

    # cstC is the dimensionless capillary force: constant all along the profile
    # see [9] Lian1993, (6) Duriez2017, or (2.51) Soulie2005 ~ (10) Soulie2006..
    cstC = 1./rRatio * sin(radians(delta1)) * sin(radians(delta1+theta)) + 1./2. * uStar * rRatio**-2 * sin(radians(delta1))**2;

    # Use of cstC to get the right filling angle delta2 (# delta1 for rRatio # 1), according to e.g. (2.52) Soulie2005
    if rRatio !=1:
        delta2 = scipy.optimize.brentq(func_delta2,0.,90.,(cstC,theta,uStar))
    else:
        delta2 = delta1

    # Discrete set of profile values:
    rho = [] # a list, should be faster than an array for following append operations, see e.g. https://stackoverflow.com/a/10122262 or http://akuederle.com/create-numpy-array-with-for-loop
    # Discrete set of slope values:
    rhoPrime = []

    step = 0
    # Boundary conditions on left contact line: see left (smallest) particle
    rho.append( 1./rRatio * sin(radians(delta1)) )
    rhoPrime.append( - 1 / tan(radians(delta1+theta)) )
    # Boundary condition on right contact line: see right (biggest) particle
    rhoRight = sin(radians(delta2));

    #-------------------------------------------------------------------------
    # Finite diff. scheme to compute whole profile ie compute rho and rhoPrime
    # See Lian1993, Duriez2017, etc..

    # One remark about next loop:
    # - just keep a simple and efficient test to stop computations. A more restrictive one may take you beyond the right contact line and lead to divergence..
    # tps = time.time()
    while rho[step] < 1.00001*rhoRight:
        rho2d = rhoSecond(rho[step],rhoPrime[step],uStar) # all terms are at "i"
        rho.append( rho[step] + deltaZ * rhoPrime[step] + 1./2. * deltaZ**2 * rho2d )

        rhoPrime.append( drho(rho[step+1],rho[step],deltaZ,rho2d) ) # i+1 value of rhoPrime, function of rho_{i+1}, rho_i, and rho2d_i. Used next time in loop.
        step=step+1
    #----------------------------------------------------------------------
    # tpsEnd = time.time()
    # print 'This took'+str(tpsEnd-tps)

    # --------------- Output computations from now on: --------------------
    # tps = time.time()
    # will have element-wise operations, seems convenient to switch now to arrays
    rho,rhoPrime = numpy.array(rho),numpy.array(rhoPrime)

    d1,d2 = radians(delta1),radians(delta2)

    # Inter particle dimensionless distance:
    # see (7) Duriez2017, (33) Scholtes2008, etc. (not (5) Soulie2006..)
    lastZ = deltaZ*(len(rho) -1)
    dist = lastZ - 1./rRatio * ( 1-cos(d1) ) - ( 1-cos(d2) )

    # Dimensionless capillary force
    force = cstC

    # Dimensionless volume:
    # Cf (4) Soulie2006, (34) Appendix Scholtes2008, (8) Duriez2017 etc. :
    vol = pi*sum( rho[1:]**2 )*deltaZ
    vol = vol - pi/3 * rRatio**(-3) * ( 1-cos(d1) )**2 * ( 2+cos(d1) )
    vol = vol - pi/3 * ( 1-cos(d2) )**2 * ( 2+cos(d2) )

    # Capillary bridge dimensionless free energy:
    # (12) Duriez2017 rather than [35] Lian1993 (was for cst volume stability)
    dArea = rho * ( 1+rhoPrime**2)**0.5 # ~ infinitesimal liquid gas area
    eStar = 2*pi * sum(dArea[1:]) * deltaZ + uStar * vol #+/- uStar changes the values but not the shape
    eStar = eStar - 2*pi*cos(radians(theta)) * ( (1-cos(d1))/rRatio**2 + 1 - cos(d2) )

    rhoRed,rhoPrRed = rho[1:len(rho)], rhoPrime[1:len(rho)]

    # Surface:
    surf = 2*pi * sum( rhoRed * (1 + rhoPrRed**2)**0.5 ) * deltaZ

    # Surface integral of dyadic product n*n (diagonal axisymmetric matrix):
    nn11 = pi * sum( rhoRed / (1 + rhoPrRed**2)**0.5 ) * deltaZ # = n22
    nn33 = 2*pi * sum( rhoRed * rhoPrRed**2 / (1 + rhoPrRed**2)**0.5 ) * deltaZ


    # --------------- Miscellaneous checks and plots --------------------------

    # Check whether surf = tr(integral n*n)
    if (surf - (2*nn11 + nn33) ) / surf > 0.0025: # strict equality beyond reach
        print 'surf =', surf
        print 'Vs 2*nn11 + nn33 =', 2*nn11 + nn33
        startStr = 'r = ' + str(rRatio) + ';theta = '+ str(theta) + ';u* = ' + str(uStar)
        raise BaseException('Liq-gas area # tr(integral over liq-gas surf. n*n) for ' + startStr + ';delta1 = '+ str(delta1),';dZ = '+ str(deltaZ))

    if plot==1 or plot3D==1:
        import matplotlib # allows to call all functions f from matplotlib with matplotlib.f
        from matplotlib import pyplot # as pyplot is a submodule (and not a function), this is also necessary to call pyplot functions f with pyplot.f
        pyplot.close('all')
#        matplotlib.rc('text', usetex=True) # http://matplotlib.org/api/matplotlib_configuration_api.html#matplotlib.rc and https://stackoverflow.com/questions/13338550/typing-greek-letters-etc-in-python-plots
        pyplot.ion()
        
    if plot==1:
        plotProfile(rho,deltaZ,rRatio,delta1,delta2,theta,cstC,uStar)
        
    if plot3D==1:
        plot3Dbridge(rho,deltaZ,rRatio)

    # Check if the neck has been reached: (there is a risk because of divergence possibility)
    if uStar!= 0: # "y0" computation according to [10] Lian1993, (or (2.54) Soulie2005 - (11) Soulie2006)
        rhoNeck = ( -1 + sqrt(1+2*uStar*cstC) ) / uStar
    else:
        rhoNeck = cstC

    if (min(rho) - rhoNeck) / rhoNeck > 0.002:
        strU,strD1,strDz = str(uStar),str(delta1),str(deltaZ)
        if speak != 0:
            print 'Profile with u*='+strU+', delta1='+strD1+' and deltaZ='+strDz+' has not reached its neck !'
        out = 0
    else:
        out = 1

    # tpsEnd = time.time()
    # print 'This took'+str(tpsEnd-tps)

    return dist,vol,force,delta1,delta2,eStar,nn11,nn33,out



def radians(angle):
    return angle * pi / 180

def func_delta2(delta2,cstC,theta,uStar): # from e.g. (2.52) Soulie2005
    return sin(radians(delta2)) * sin(radians(delta2+theta)) + 1./2. * uStar * sin(radians(delta2))**2 - cstC

def drho(rho,prevRho,deltaZ,rho2d):
    # returns rhoPrime at i+1, see (4) Duriez2017

    #drho = sqrt( ( rho/(cstK - 1./2.*uStar*rho**2) )**2 - 1 ) # [11] Lian1993 is always positiv, which is not true
    return (rho - prevRho) / deltaZ + 1./2.*deltaZ * rho2d; # NB: the rho2d term has a real influence

def rhoSecond(rho,rhoP,uStar):
    # see e.g. (5) Duriez2017
    return (1+rhoP**2) / rho + uStar*(1+rhoP**2)**1.5


def plotProfile(rho,deltaZ,rRatio,delta1,delta2,theta,cstC,uStar):

    from matplotlib import pyplot # could not achieve calling pyplot.plot from here without putting this line directly here.

    if uStar != 0: # "y0" computation, see e.g. [10] Lian1993, (2.54) Soulie2005, (11) Soulie2006..
        rhoNeck = ( -1 + sqrt(1+2*uStar*cstC) ) / uStar
    else:
        rhoNeck = cstC

    lastZ = deltaZ*(len(rho) -1)
    vecZ = deltaZ*numpy.arange(len(rho)) #numpy.arange(a=0,b) does not include b

    # Toroidal profile: see [15]-[17] Lian1993, for r=1
    if rRatio ==1:
        d1Rad,d2Rad,thRad = radians(delta1),radians(delta2),radians(theta)
        dist = lastZ - (1-cos(d1Rad)) - (1-cos(d2Rad))
        rho1 = ( dist/2. + 1 - cos(d1Rad) )/cos(d2Rad+thRad)
        rho2 = sin(d2Rad) - (1-sin(d2Rad+thRad)) * ( dist/2 + 1-cos(d2Rad) ) / cos(d2Rad+thRad)
        torProfile = rho1 + abs(rho2) - ( rho1**2 - (vecZ-lastZ/2)**2 )**0.5 # toroidal equation        

    pyplot.figure()
    pyplot.plot(vecZ,rho,label='Profile')
    pyplot.plot([0,lastZ],[1./rRatio * sin(radians(delta1)),1./rRatio * sin(radians(delta1))],'r',label='Left value')
    pyplot.plot([0,lastZ],[rhoNeck,rhoNeck],'m',label='Neck value')
    pyplot.plot([0,lastZ],[sin(radians(delta2)),sin(radians(delta2))],'g',label='Right value')
    if rRatio ==1:
        pyplot.plot(vecZ[::int(len(rho)/50.)],torProfile[::int(len(rho)/50.)],'ob',label='Toroidal profile');
    pyplot.legend()
    pyplot.xlabel('Interparticle axis, z')
    pyplot.ylabel('Profile height, rho(z)')
    pyplot.title('Filling angle delta1 = '+str(delta1)+' degrees')
    pyplot.show()


def plot3Dbridge(rho,deltaZ,rRatio):

    from matplotlib import pyplot # same remark as above
    from mpl_toolkits.mplot3d import Axes3D # to plot 3D figure, using projection='3d' below, in particular
    from matplotlib import cm

    nPtsAlongAxis = 200 # grid-step along "rho" and z axis (the x,y plane of the 3D figure in fact)
    nRhoVal,rhoMax = len(rho),max(rho)
    lastZ = deltaZ*(nRhoVal -1)

    # discretization of x axis of 3D figure
    x = numpy.arange(-rhoMax,rhoMax,rhoMax/nPtsAlongAxis)
    if not rhoMax in x: # most often rhoMax won't be in x, as numpy.arange usually does not include the stop attribute
        x=numpy.append(x,rhoMax)
    # discretization of y axis of 3D figure (= z axis from Fig. 1 Duriez2017), nPtsAlongAxis values, from 0 to lastZ:
    y = numpy.arange(nPtsAlongAxis)*lastZ/(nPtsAlongAxis-1)

    # the matrix, with elevation data (MATLAB-like)
    alt = numpy.zeros((len(y),len(x)))
    for i in range(len(y)):
        indRho = int( (i-1)*(nRhoVal-1)/(nPtsAlongAxis) ) + 1
        for j in range(len(x)):
            if rho[indRho]**2 - x[j]**2 >=0:
                alt[i,j] = rho[indRho] * sqrt(rho[indRho]**2 - x[j]**2) / rho[indRho]

    fig = pyplot.figure()
    ax = fig.gca(projection='3d')
    x,y = numpy.meshgrid(x,y)
    surf = ax.plot_surface(x,y,alt,linewidth=0,cmap=cm.coolwarm,vmin = 0,vmax=rhoMax)
    pyplot.ylabel('Interparticle axis, z')
    # TODO: I can not include a colorbar..
    # fig.colorbar(surf)
    #    pyplot.colorbar(surf,ax=ax)
    pyplot.show()
    
    # TODO: plot the spherical particle caps, like in MATLAB...
# delta1 = asin(rRatio*rho[1]);
# yC1 = - cos(delta1)/rRatio;
# delta2 = asin(rho(nRhoVal));
# yC2 = lastZ + cos(delta2); # Center of right sphere:
# x = -rhoMax:rhoMax/40:rhoMax;
# y = x + lastZ/2;
# altSph1 = zeros(len(x),len(x));
# altSph2 = zeros(len(x),len(x));
# for i=1:len(x)
#     for j=1:len(x)
#         alt1 = 1./rRatio**2 - x[j]**2 - (y[i]-yC1)**2;
#         if alt1>=0
#             altSph1[i,j] = alt1**0.5;
#         end
#         alt2 = 1 - x[j]**2 - (y[i]- yC2)**2;
#         if alt2>=0
#             altSph2[i,j] = alt2**0.5;
#         end
#     end
# end

# surf(x,y,matAltToPlot(altSph1),'FaceColor','none')
# surf(x,y,matAltToPlot(altSph2),'FaceColor','none')
# caxis([0,rhoMax])
# grid off
# axis square
# end

def matAltToPlot(matAlt): # should help plotting like in MATLAB TODO: actually use it...
    sizeY,sizeX = numpy.size(matAlt,0), numpy.size(matAlt,1)
    plotAlt = numpy.ones((sizeY,sizeX))
    for i in range(1,sizeY-1):
        for j in range(1,sizeX-1):
            if not matAlt[i,j]:
                test = matAlt[i-1,j-1] or matAlt[i-1,j] or matAlt[i-1,j+1]
                test = test or matAlt[i,j-1] or  matAlt[i,j+1]
                test = test or matAlt[i+1,j-1] or matAlt[i+1,j] or matAlt[i+1,j+1]
                if not test:
                    plotAlt[i,j] = 0

    retMat = numpy.zeros((sizeY,sizeX))
    for i in range(sizeY-1):
        for j in range(sizeX-1):
            if not plotAlt[i,j]:
                retMat[i,j] = numpy.nan
            else:
                retMat[i,j] = matAlt[i,j]
    return retMat


