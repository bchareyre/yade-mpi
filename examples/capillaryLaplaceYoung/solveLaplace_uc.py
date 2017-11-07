# J. Duriez (jerome.duriez@irstea.fr)

# to import with yade/python/ipython solveLaplace_uc.py, or with execfile('solveLaplace_uc.py',globals()) once inside a yade/python/ipython session

execfile('solveLiqBridge.py',globals())

def solveLaplace_uc(theta,rRatio,uStar,delta1Cons,deltaZ,save):
    # Solves Laplace-Young equation for given r=rRatio, uc*=uStar and theta
    #   Making recursively use of solveLiqBridge function, the present function
    #   identifies all possible bridges configurations for given r, uc*, and
    #   theta

    # Input attributes (see also solveLiqBridge function):
    # theta: contact angle (deg)
    # rRatio: rMax / rMin for the considered particles pair
    # uStar: dimensionless capillary pressure
    # delta1Cons: numpy.array of delta1 (see solveLiqBridge) values to consider
    # deltaZ: see solveLiqBridge
    # save: 0/1 to save text files including the capillary bridges data (if 1)

    # Returns a 2D numpy.array of capillary bridges data (see sol_u below),
    # possibly (in case save=1) also written in text files.



    # Removal of possible d1=0 since such bridge is physically impossible, and
    # d1 = 0 might make trouble if theta = 0 (leads to inf. initial rhoPrime):
    delta1Cons = delta1Cons[delta1Cons!=0]
    # And of d1 values such that d1 + theta > 90
    delta1Cons = delta1Cons[delta1Cons + theta <= 90]


    #------------ All possible bridges for these r, uc*, theta: ---------------

    # initialization of bridge configurations data: as many lines as configurations
    # and 9 columns being [dist uc(=cst) V F delta1 delta2 E nn11 nn33]
    sol_u = -numpy.ones((len(delta1Cons),9))
    # list [[- 1 for j in range(9)] for j in range(len(delta1Cons)) ] would be a better choice than arrays in terms of sys.getsizeof (list of 100000*9 lines*columns~0.1 * numpy.array of 100000*9 lines*columns)
    # but seems much less convenient for indexing and testing signs below (list[4][:] is the same as list[:][4]....)

    minDzUsed = deltaZ
    # timeStartLoop = time.clock() 
    for i in range(0,len(delta1Cons)):
        dzUsed = deltaZ
        delta1 = delta1Cons[i]
        [dist,vol,force,delta1,delta2,eStar,nn11,nn33,out] = solveLiqBridge(rRatio,theta,uStar,delta1,dzUsed,0,0,0)
        while out == 0:
            dzUsed = dzUsed / 2.
            minDzUsed = min(dzUsed,minDzUsed)
            [dist,vol,force,delta1,delta2,eStar,nn11,nn33,out] = solveLiqBridge(rRatio,theta,uStar,delta1,dzUsed,0,0,0)
        sol_u[i,:] = [dist,uStar,vol,force,delta1,delta2,eStar,nn11,nn33]
    # timeEndLoop = time.clock() # gives the same measurements than time.time()
    # print 'Here, loop took '+str(timeEndLoop-timeStartLoop)+' s'

    # Get rid of unphysical solutions with negativ distances and/or volume:
    physSol = sol_u[ numpy.logical_and(sol_u[:,0]>=0,sol_u[:,2]>=0),: ]

    # Get rid of unstable physical solutions:
    # We use volume values for this purpose, see e.g. Duriez2017
    distRupt = max(physSol[:,0])
    # eRupt = physSol(physSol(:,1)==distRupt,7) # would not work since eRupt can be a global maximum
    # (when two branches are increasing with d*)
    vRupt = physSol[physSol[:,0]==distRupt,2]
    stabSol = physSol[physSol[:,2]>=vRupt,:]


    # ----------- Save of data in text files -----------
    # three text files are generated: one, "Raw", includes all computed data;
    # another, "Phys", restricts to physical solutions (with positiv distance
    # and volume); the last one, "Stab" restricts to the stable physical solutions
    if save !=0:
        if minDzUsed == deltaZ:
            strDz = 'With deltaZ = ' + str(deltaZ)
        else:
            strDz = 'With deltaZ between '+str(deltaZ)+' and '+str(minDzUsed)
        # Raw data: all liquid bridge configurations that have been computed
        nameFile = 'capDataRaw_r'+str(rRatio)+'_theta'+str(theta)+'_ucStar'+str(uStar)+'.txt'
        head_wE = 'dist*\tuc*\tV*\tF*\tdelta1 (deg)\tdelta2(deg)\tE* (-)\tn11 (-)\tn33 (-)\t' + strDz + '\n'
        writeFile(nameFile,head_wE,sol_u)

        # Physical data: after removing the configurations with negative distance or volume
        nameFile = 'capDataPhys_r'+str(rRatio)+'_theta'+str(theta)+'_ucStar'+str(uStar)+'.txt'
        writeFile(nameFile,head_wE,physSol)

        # Stab data: only the stable configurations, i.e. biggest volumes
        nameFile = 'capDataStab_r'+ str(rRatio)+'_theta'+str(theta)+'_ucStar'+str(uStar)+'.txt'
        head_woE = 'dist*\tuc*\tV*\tF*\tdelta1 (deg)\tdelta2(deg)\tn11 (-)\tn33 (-)\t'+strDz+'\n'
        columns = range(6) # just an intermediate variable for below, as range(5).extend([7,8]) is impossible..
        columns.extend([7,8]) # columns.extend() as an argument of numpy.ix() would fail
        writeFile(nameFile,head_woE,stabSol[ numpy.ix_( range(stabSol.shape[0]) , columns ) ])

    # --------------------------------------------------
    return stabSol


def writeFile(nomFic,headLine,data):

    fic = open(nomFic,'w')
    numpy.savetxt(fic,data,delimiter='\t',header=headLine)
    fic.close()
