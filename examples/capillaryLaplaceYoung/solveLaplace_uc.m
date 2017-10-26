% J. Duriez (jerome.duriez@irstea.fr)

function stabSol = solveLaplace_uc(theta,rRatio,uStar,delta1Cons,deltaZ,save)
% Solves Laplace-Young equation for given r, uc* and theta
%   Making recursively use of solveLiqBridge function, the present function
%   identifies all possible bridges configurations for given r, uc*, and
%   theta

% Input attributes (see also solveLiqBridge function):
% theta: contact angle (deg)
% rRatio: rMax / rMin for the considered particles pair
% uStar: dimensionless capillary pressure
% delta1Cons: vector of delta1 (see solveLiqBridge) values to consider
% deltaZ: see solveLiqBridge
% save: 0/1 to save text files including the capillary bridges data (if 1)

% Returns a table (matrix) of capillary bridges data (see sol_u below),
% possibly (in case save=1) also written in text files.



% Removal of possible d1=0 since such bridge is physically impossible, and
% d1 = 0 might make trouble if theta = 0 (leads to inf. initial rhoPrime):
delta1Cons = delta1Cons(delta1Cons~=0);
% And of d1 values such that d1 + theta > 90
delta1Cons = delta1Cons(delta1Cons + theta <= 90);


%------------ All possible bridges for these r, uc*, theta: ---------------

% data table initialization:
sol_u = -ones(length(delta1Cons),9);
% 9 col. are [dist uc(=cst) V F delta1 delta2 E nn11 nn33]


minDzUsed = deltaZ;
% tic
for i=1:length(delta1Cons)
    dzUsed = deltaZ;
    delta1 = delta1Cons(i);
    [dist,vol,force,delta1,delta2,eStar,nn11,nn33,out] = solveLiqBridge(rRatio,theta,uStar,delta1,dzUsed,0,0,0);
    while out == 0
        dzUsed = dzUsed / 2;minDzUsed = min(dzUsed,minDzUsed);
%         disp(['Refining deltaZ to ',num2str(dzUsed),' in solveLaplace'])
        [dist,vol,force,delta1,delta2,eStar,nn11,nn33,out] = solveLiqBridge(rRatio,theta,uStar,delta1,dzUsed,0,0,0);
    end        
    sol_u(i,:) = [dist,uStar,vol,force,delta1,delta2,eStar,nn11,nn33];

end
% toc

% Get rid of unphysical solutions with negativ distances and/or volume:
physSol = sol_u( sol_u(:,1)>=0 & sol_u(:,3)>=0,: ); % NB: vol>=0 is true for vol = Inf

% Get rid of diverged solutions with infinite volume (should <=> infinite
% profile):
nonDvSol = physSol(isfinite(physSol(:,3)),:);
% nNonDvSol = length(nonDvSol(:,1));
% disp(['We had to suppress ',num2str((1-nNonDvSol/nPhysSol)*100),' % of diverged "solutions"'])

% Get rid of unstable physical solutions:(those with biggest volumes)
% We use volume values for this purpose, see e.g. Duriez2017
distRupt = max(nonDvSol(:,1));
% eRupt = nonDvSol(nonDvSol(:,1)==distRupt,7); % does not work since eRupt can be a global maximum
% (when two branches are increasing with d*)
vRupt = nonDvSol(nonDvSol(:,1)==distRupt,3);
stabSol = nonDvSol(nonDvSol(:,3)>=vRupt,:);


% ----------- Save of data in text files -----------
if save ~=0
% three text files are generated: one, "Raw", includes all computed data;
% another, "Phys", restricts to physical solutions (with positiv distance
% and volume); the last one, "Stab" restricts to the stable physical
% solutions
    if minDzUsed == deltaZ
        strDz = ['With deltaZ = ',num2str(deltaZ)];
    else
        strDz = ['With deltaZ between ',num2str(deltaZ),' and ',num2str(minDzUsed)];
    end
    nomFic = ['capDataRaw_r',num2str(rRatio),'_theta',num2str(theta),'_ucStar',num2str(uStar),'.txt'];
    head_wE = ['dist*\tuc*\tV*\tF*\tdelta1 (deg)\tdelta2(deg)\tE* (-)\tn11 (-)\tn33 (-)\t',strDz,'\n'];
    writeFile(nomFic,head_wE,sol_u)
    
    nomFic = ['capDataPhys_r',num2str(rRatio),'_theta',num2str(theta),'_ucStar',num2str(uStar),'.txt'];
    writeFile(nomFic,head_wE,nonDvSol)
    
    nomFic = ['capDataStab_r',num2str(rRatio),'_theta',num2str(theta),'_ucStar',num2str(uStar),'.txt'];
    head_woE = ['dist*\tuc*\tV*\tF*\tdelta1 (deg)\tdelta2(deg)\tn11 (-)\tn33 (-)\t',strDz,'\n'];
    writeFile(nomFic,head_woE,stabSol(:,[1:6,8:9]))
end
% ---------------------------------------------------

end


function writeFile(nomFic,headLine,data)

fic = fopen(nomFic,'w');
fprintf(fic,headLine);
fclose(fic);
dlmwrite(nomFic,data,'-append','delimiter','\t')

end

