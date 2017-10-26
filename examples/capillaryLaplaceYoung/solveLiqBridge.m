% J. Duriez (jerome.duriez@irstea.fr)

function [dist,vol,force,delta1,delta2,eStar,nn11,nn33,out] = solveLiqBridge(rRatio,theta,uStar,delta1,deltaZ,plot,speak,plot3D)
%Computes one single liquid (capillary) bridge
%   Lian's method: iterative determination using Taylor expansion with
%   finite difference (FD) increment = deltaZ (a function attribute)

%   Attributes: rRatio = R2 / R1 >=1, theta = contact angle (deg), uStar =
%   dimensionless cap. pressure (uc*R2/gamma), delta1 = filling angle on
%   smallest particle (deg), deltaZ = increment of FD scheme (e.g. 2e-6)
%   Last ones = booleans to resp. plot the profile in 2D, output messages,
%   and plot the liq bridge in 3D

% Outputs: dist = interparticle distance ; 
% vol = dimensionless volume (= vol% /R2^3)
% force = dimensionless force = force/(2*pi*gamma*R2)
% delta1/2 = filling angles on the 2 particles (degrees)
% eStar = free energy/(gamma*% R2^2)
% nn11 = 11-term of integral(ni nj dS) / R2^2 = 22-term 
% nn33 = 33-term, with 3 axis = the meniscus orientation
% NB: surface / R2^2 = 2*nn11 + nn33
% out = 1 or 0 depending whether things went fine (see below): 1 if yes

% close all
global cstC
% cstC is the dimensionless capillary force: constant all along the profile
% see [9] Lian1993, (6) Duriez2017, or (2.51) Soulie2005 ~ (10) Soulie2006..
cstC = 1/rRatio * sin(radians(delta1)) * sin(radians(delta1+theta)) + 1/2 * uStar * rRatio^-2 * sin(radians(delta1))^2;

% Use of cstC to get the right filling angle delta2:
fun = @(delta2)cstC_delta2(delta2,theta,uStar);
delta2 = fzero(fun,[0 90]); % fzero does not catch multiple roots, if it occurs

% Discrete set of profile values:
rho = -ones(30000,1); % preallocating 26000 or 30000 makes 90 profile computations take ~ 10.8s instead of ~ 14.5
% Discrete set of slope values:
rhoPrime = -ones(30000,1);

step = 1;
% Boundary conditions on left contact line: see left (smallest) particle
rho(1) = 1/rRatio * sin(radians(delta1));
rhoPrime(1) = - 1 / tan(radians(delta1+theta));
% Boundary condition on right contact line: see right (biggest) particle
rhoRight = sin(radians(delta2));

%-------------------------------------------------------------------------
% Finite diff. scheme to compute whole profile ie compute rho and rhoPrime
% See Lian1993, Duriez2017, etc..

% Some remarks about next loop:
% - this loop may extend the size of rho without problem
% - just keep a simple and efficient test to stop computations. A more restrictive
% one may lead to go beyond the right contact line and lead to divergence..
while rho(step) < 1.00001*rhoRight
    rho2d = rhoSecond(rho(step),rhoPrime(step),uStar); % all terms are at "i"
    rho(step+1) = rho(step) + deltaZ * rhoPrime(step) + 1/2 * deltaZ^2 * rho2d;

    rhoPrime(step+1) = drho(rho(step+1),rho(step),deltaZ,rho2d); % i+1 value of rhoPrime, function of rho_{i+1}, rho_i, and rho2d_i. Used next time in loop.
    step=step+1;
end
%-------------------------------------------------------------------------

% Two following lines to suppress extra data due to preallocation. Keep
% this order !
rhoPrime = rhoPrime(rho>0);
rho = rho(rho>0);

% --------------- Output computations from now on: ------------------------

d1 = radians(delta1); d2 = radians(delta2);

% Inter particle dimensionless distance:
% see (7) Duriez2017, (33) Scholtes2008, etc. (not (5) Soulie2006..)
lastZ = deltaZ*(length(rho) -1);
dist = lastZ - 1/rRatio * ( 1-cos(d1) ) - ( 1-cos(d2) );

% Dimensionless capillary force:
force = cstC;

% Dimensionless volume:
% Cf (4) Soulie2006, (34) Appendix Scholtes2008, (8) Duriez2017 etc. :
vol = pi*sum(rho(2:length(rho)).^2)*deltaZ;
vol = vol - pi/3 * rRatio^(-3) * ( 1-cos(d1) )^2 * ( 2+cos(d1) );
vol = vol - pi/3 * ( 1-cos(d2) )^2 * ( 2+cos(d2) );

% Capillary bridge dimensionless free energy:
% (12) Duriez2017 rather than [35] Lian1993 (was for cst volume stability)
dArea = rho .* ( 1+rhoPrime.^2).^(1/2); % ~ infinitesimal liquid gas area
eStar = 2*pi * sum(dArea(2:length(dArea))) * deltaZ + uStar * vol; %+/- uStar changes the values but not the shape
eStar = eStar - 2*pi*cos(radians(theta)) * ( (1-cos(d1))/rRatio^2 + 1 - cos(d2) );

rhoRed = rho(2:length(rho)); rhoPrRed = rhoPrime(2:length(rho));

% Surface:
surf = 2*pi * sum( rhoRed .* (1 + rhoPrRed.^2).^0.5 ) * deltaZ;

% Surface integral of dyadic product n*n (diagonal axisymmetric matrix):
nn11 = pi * sum( rhoRed ./ (1 + rhoPrRed.^2).^0.5 ) * deltaZ; % = n22
nn33 = 2*pi * sum( rhoRed .* rhoPrRed.^2 ./ (1 + rhoPrRed.^2).^0.5 ) * deltaZ;


% --------------- Miscellaneous checks and plots --------------------------

% Check whether surf = tr(integral n*n)
if (surf - (2*nn11 + nn33) ) / surf > 0.0025 % strict equality beyond reach
    disp(surf)
    disp(2*nn11 + nn33)
    startStr = ['r = ',num2str(rRatio),';theta = ',num2str(theta),';u* = ',num2str(uStar)];
    error(['Liq-gas area # tr(integral over liq-gas surf. n*n) for ',startStr,';delta1 = ',num2str(delta1),';dZ = ',num2str(deltaZ)])
end

if plot==1 % && dist >=0 && vol >=0 %&& isfinite(vol)
    plotProfile(rho,deltaZ,rRatio,delta1,delta2,theta,cstC,uStar)
end

if plot3D==1
    plot3Dbridge(rho,deltaZ,rRatio)
end

% Check if the neck has been reached: (there is a risk because of divergence possibility)
if uStar~= 0 % "y0" computation according to [10] Lian1993, (or (2.54) Soulie2005 - (11) Soulie2006)
    rhoNeck = ( -1 + sqrt(1+2*uStar*cstC) ) / uStar;
else
    rhoNeck = cstC;
end

if (min(rho) - rhoNeck) / rhoNeck > 0.002
    strU = num2str(uStar); strD1 = num2str(delta1); strDz = num2str(deltaZ);
    if speak ~= 0
        disp(['Profile with u*=',strU,', delta1=',strD1,' and deltaZ=',strDz,' has not reached its neck !'])
    end
    out = 0;
else
    out = 1;
end

end


function angleRad = radians(angle)
angleRad = angle * pi / 180;
end

function y = cstC_delta2(delta2,theta,uStar) % from e.g. (2.52) Soulie 2005
global cstC
y = sin(radians(delta2)) * sin(radians(delta2+theta)) + 1/2 * uStar * sin(radians(delta2))^2 - cstC;
end


function drho = drho(rho,prevRho,deltaZ,rho2d)
% returns rhoPrime at i+1, see (4) Duriez2017

%drho = sqrt( ( rho/(cstK - 1/2*uStar*rho^2) )^2 - 1 );% [11] Lian1993 always positiv, which is not true
drho = (rho - prevRho) / deltaZ + 1/2*deltaZ * rho2d; % NB: the rho2d term has a real influence
end

function rho2d = rhoSecond(rho,rhoP,uStar)
% see e.g. (5) Duriez2017
rho2d = (1+rhoP^2) / rho + uStar*(1+rhoP^2)^1.5;
end

function plotProfile(rho,deltaZ,rRatio,delta1,delta2,theta,cstC,uStar)

if uStar~= 0 % "y0" computation, see e.g. [10] Lian1993, (2.54) Soulie2005, (11) Soulie2006..
    rhoNeck = ( -1 + sqrt(1+2*uStar*cstC) ) / uStar;
else
    rhoNeck = cstC;
end
lastZ = deltaZ*(length(rho) -1);
vecZ = 0:deltaZ:lastZ;

% Toroidal profile parameters: see [15]-[17] Lian1993, for r=1
d1Rad = radians(delta1);d2Rad = radians(delta2); thRad = radians(theta);
dist = lastZ - (1-cos(d1Rad)) - (1-cos(d2Rad)); % TODO: only for r=1
rho1 = ( dist/2. + 1 - cos(d1Rad) )/cos(d2Rad+thRad);
rho2 = sin(d2Rad) - (1-sin(d2Rad+thRad)) * ( dist/2 + 1-cos(d2Rad) ) / cos(d2Rad+thRad);

figure();
plot(vecZ,rho,'o');hold on;
plot([0;lastZ],[1/rRatio * sin(radians(delta1));1/rRatio * sin(radians(delta1))],'r')
plot([0;lastZ],[rhoNeck;rhoNeck],'m')
plot([0;lastZ],[sin(radians(delta2));sin(radians(delta2))],'g')
plot(vecZ,rho1 + abs(rho2) - ( rho1^2 - (vecZ-lastZ/2).^2 ).^(1/2),'b');
legend('Profile','Left value','Neck value','Right value','Toroidal profile')
xlabel('Interparticle axis, z')
ylabel('\rho(z)')
title(['\delta_1 = ',num2str(delta1)])
if rRatio ~= 1
    disp('Plotted toroidal profile is obviously wrong because rRatio # 1')
end
% disp([num2str(delta1),' profile plotted with ',num2str(length(rho)),' rho values'])

end

function plot3Dbridge(rho,deltaZ,rRatio)

nPtsAlongAxis = 200 ; % grid-step along "rho" and z axis (the x,y plane of the 3D figure in fact)
nRhoVal =length(rho);
lastZ = deltaZ*(nRhoVal -1);
x = -max(rho):max(rho)/nPtsAlongAxis:max(rho);

y = 0:lastZ / (nPtsAlongAxis-1):lastZ; % will then reach exactly lastZ
alt = zeros(length(y),length(x));
for i = 1:length(y)
    indRho = floor( (i-1)*(nRhoVal-1)/(nPtsAlongAxis) ) +1;
    for j = 1:length(x)
        if rho(indRho)^2 - x(j)^2 >=0
            alt(i,j) = rho(indRho) * sqrt(rho(indRho)^2 - x(j)^2) / rho(indRho);
%         else
%             alt(i,j) = NaN; % not pretty... 2d step with matAltToPlot
%             necessary
        end
    end
end

figure();set(gca,'FontSize',14);
surf(x,y,matAltToPlot(alt),'LineStyle','none') % filled surface, with color according to altitude
% surf(x,y,matAltToPlot(alt),'FaceColor','none') % just the mesh
ylabel('Interparticle axis, z')

hold on

delta1 = asin(rRatio*rho(1));
yC1 = - cos(delta1)/rRatio;
delta2 = asin(rho(nRhoVal));
yC2 = lastZ + cos(delta2); % Center of right sphere:
x = -max(rho):max(rho)/40:max(rho);
y = x + lastZ/2;
altSph1 = zeros(length(x),length(x));
altSph2 = zeros(length(x),length(x));
for i=1:length(x)
    for j=1:length(x)
        alt1 = 1/rRatio^2 - x(j)^2 - (y(i)-yC1)^2;
        if alt1>=0
            altSph1(i,j) = alt1^(1/2);
        end
        alt2 = 1 - x(j)^2 - (y(i)- yC2)^2;
        if alt2>=0
            altSph2(i,j) = alt2^(1/2);
        end
    end
end

surf(x,y,matAltToPlot(altSph1),'FaceColor','none')
surf(x,y,matAltToPlot(altSph2),'FaceColor','none')
caxis([0,max(rho)])
grid off
axis square
end

function retMat = matAltToPlot(matAlt)

sizeY = size(matAlt,1);sizeX = size(matAlt,2);
plotAlt = ones(sizeY,sizeX);
for i = 2:sizeY-1
    for j = 2:sizeX-1
        if ~matAlt(i,j)
            test = matAlt(i-1,j-1) || matAlt(i-1,j) || matAlt(i-1,j+1);
            test = test || matAlt(i,j-1) ||  matAlt(i,j+1);
            test = test || matAlt(i+1,j-1) || matAlt(i+1,j) || matAlt(i+1,j+1);
            if ~test
                plotAlt(i,j) = 0;
            end
        end
    end
end

retMat = zeros(sizeY,sizeX);
for i = 1:sizeY*sizeX
    if ~plotAlt(i)
        retMat(i) = NaN;
    else
        retMat(i) = matAlt(i);
    end
end


end

