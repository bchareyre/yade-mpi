% J. Duriez (jerome.duriez@irstea.fr)

function writesCapFile()
% Builds capillary files for given radius ratio and contact angle
%   The historical template of YADE capillary files is kept: each line of
%   the capillary file is a bridge configuration. Configurations are
%   grouped by distance values, then (for any distance value) by capillary
%   pressure values.
%   The user is expected to modify l. 10 to 27 to taylor the generation to
%   his needs.

% ---------------- Parameters to define by the user -----------------------

% Laplace-Young resolution parameters:
deltaZ = 2e-6; % use 2e-6, typically
theta = 0; % contact angle, in degrees
dDelta1 = 0.3; % delta1 (see solveLiqBridge.m) increment between two configurations

% Capillary files construction parameters:
nValUc = 350; % # (at most) of capillary pressure values to consider for 
% one given distance
nValDist = 80; % # of distance values (linearly spaced between 0 and some 
% rupture distance computed in Preliminary A below) to consider
% See also if needed l. 147 that defines the capillary file names

% List of radius ratio to consider:
rRatioVec = [1 1.1 1.25 1.5 1.75 2 3 4 5 10]; % historical rRatio values in YADE
% As many capillary files as length(rRatioVec) will be generated

suffixe = ''; % string to append at the end of generated files names

% ---- End of parameters definitions, no further action is required ! -----




% ---------------- Some data vector initializations -----------------------

% maximum possible interparticle distances for given rRatio:
maxD = -ones(length(rRatioVec),1);
% Private (J. Duriez) remark: for theta=0; dz=1e-5 or 2e-6; delta1=15:0.2:90;
% and rRatioVec = [1;1.1;1.25;1.5;1.75;2;3;4;5], we get here:
% maxD = [0.3995;0.3808;0.3567;0.3241;0.2983;0.2772;0.2199;0.1852;0.1615];
% Compares favourably with historical (L. Scholtes') data: dRupt_rMeSch.eps

% initial guess (e.g. from L. Scholtes data) of maximum uc values for given rRatio:
maxUc_r = [1 1493;1.1 1567;1.25 1781;1.5 2137;1.75 2469;2 2849;3 4274;4 5644;5 6987;10 14370];
% these initial guess are not mandatory but may help save some time below


% ---------- Generation of all capillary files really starts here ---------
for i =1:length(rRatioVec)
    rRatio = rRatioVec(i);
    
% ##### Preliminary A: find maxD values #####
% It is considered to be for zero suction (corresponds ~ to Scholtes' data)
% And assumed to occur for delta1 greater than 15 deg (> 30 deg in Scholtes)
    fprintf(['\nFor r = ',num2str(rRatio),', starting search for maximum distance\n'])
%     fflush(stdout); % Octave command for flush display
    d1here = 15:0.5:90-theta; % I got the exact same results with 15:1:90 or 15:0.2:90
    data = solveLaplace_uc(theta,rRatio,0,d1here,deltaZ,0);
    maxD(i) = max(data(:,1));
    disp(['For r = ',num2str(rRatio),', maximum distance found: maxD = ',num2str(maxD(i))])
    disp('');
%    fflush(stdout); % Octave command for flush display
    
% #### Preliminary B: computing the set of d* values to consider, valDist ####

    % -- Following lines are for geometric exponential sequence -- 
    % valDist was a ~ 10-exponential sequence in L. Scholtes' data, but I
    % (JD) think there was too much small values    
%     valDist = -ones(nValDist,1);
%     valDist(1)=0;valDist(2) = 5e-5;% 1e-4 in Scholtes maybe too high (cf e.g. F* for r=3)
%     cst = 1 / (nValDist-2) * log(maxD(i) / valDist(2));
%     valDist(3:nValDist) = valDist(2) * exp( ((3:nValDist)'-2) * cst);
    
    % -- Anyway, I (JD) think a linear sequence is just as good, or better --
    incDist = maxD(i) / (nValDist - 1);
    valDist = 0:incDist:maxD(i);
    if length(valDist) ~= nValDist
        error('We did not construct a consistent set of distances')
    end
    
% #### Preliminary C: find the greatest admissible suction for r,theta ####
% It is considered to be the suction leading, for contacting particles, to
% a mean filling angle <= 1 deg (was ~ 2 deg in L. Scholtes' data).

    d1MaxUc = 0:dDelta1:min(45,90-theta);
    disp('Starting search for maximum suction');%fflush(stdout);
    if isempty(find(maxUc_r(:,1)==rRatio,1))
        index = find(maxUc_r(:,1) < rRatio,1,'last');
        ucTried = floor(maxUc_r(index,2));
    else
        ucTried = floor(maxUc_r(maxUc_r(:,1)==rRatio,2));
    end
    data = solveLaplace_uc(0,rRatio,ucTried,d1MaxUc,deltaZ,0);

% Filling angles for contact situation are, if necessary, extrapolated:
    deltaC = guessDeltaContact(data);
    while deltaC > 1
        prevMaxDelta1 = max(data(:,5));
        d1MaxUc = 0:dDelta1:prevMaxDelta1;
        ucTried = floor(ucTried*1.2); % floor because no need to take care of digits after comma here
        data = solveLaplace_uc(0,rRatio,ucTried,d1MaxUc,deltaZ,0);
        deltaC = guessDeltaContact(data);
    end
    maxUc = ucTried ;
    disp(['For r = ',num2str(rRatio),', maximum suction found: final uc = ',num2str(maxUc)])
    disp('');%fflush(stdout);
% #### End of Preliminary C ####


% #### Preliminary D: choosing list of uc* values, valUc #####
% we define valUc as a geometric sequence --with 0 as first term, though--
% so that log(uc*_{i+1}) - log(uc*_i) = cst, and reaching maxUc as the last
% (i.e. nValUc-th) term
    valUc = -ones(nValUc,1);
    valUc(1)=0;valUc(2) = 0.002;% 0.002 as first positiv uc* value corresponds ~ to Scholtes
    cst = 1 / (nValUc-2) * log(maxUc / valUc(2))/log(10);
    valUc(3:nValUc) = valUc(2) * 10.^( ((3:nValUc)'-2) * cst);


% #### Preliminaries completed: computing data for all uc values in valUc ! #####
    bigData = -ones(nValDist*nValUc,8); % will gather all data
    lineInBigData = 1;
    disp('Starting solving for all uc values');tic
%     fflush(stdout);
    output = zeros(10);iOut = 1; % these things are just to control some message display
    for indUc = 1:nValUc
        % message display:
        if indUc >= iOut * nValUc / length(output) && output(iOut) == 0
            disp([num2str(iOut / length(output)* 100),'% of uc values just completed'])
%             fflush(stdout);
            toc;tic;
            output(iOut) = 0;
            iOut = iOut+1;
        end
        if indUc == 1
            delta1Int = 0:dDelta1:90-theta;
        end
        smallData = solveLaplace_uc(theta,rRatio,valUc(indUc),delta1Int,deltaZ,0);
        delta1Int = 0:dDelta1:max(smallData(:,5));
        % Filling bigData:
        nLinesSmallData = length(smallData(:,1));
        bigData(lineInBigData:lineInBigData+nLinesSmallData-1,:) = smallData(:,[1:6,8:9]);
        lineInBigData = lineInBigData + nLinesSmallData;
    end
    disp('All uc values computed !');toc
%     fflush(stdout);
    
    % Cleaning and sorting the computed data:
    bigData = bigData(bigData(:,1)>=0,:); % removal of possible extra data, due to preallocation
    bigData = sortrows(bigData); % sorts according to increasing distance
    
    nomCapFic = ['M(r=',num2str(rRatio),')',suffixe];
    fic = fopen(nomCapFic,'w');
    % first line of the file=rRatio value:
    fprintf(fic,[num2str(rRatio),'\n',num2str(nValDist),'\n']);% for direct use in YADE
    % alternatively, you may want to save some details about the
    % construction. If yes, replace with the 2 following lines
%     fprintf(fic,[num2str(rRatio),' ; theta = ',num2str(theta),' ; dDelta1 = ',num2str(dDelta1)]);
%     fprintf(fic,[' ; deltaZ less or equal than ',num2str(deltaZ),'\n',num2str(nValDist),'\n']);
    % in this case, you may need to manually suppress these extra
    % characters before using the capillary file in YADE
    fclose(fic);
    
    % the final data have to conform valDist data. This is achieved through
    % interpolation, below:
    for indD =1:nValDist
        dWanted = valDist(indD);
        dataCapFile = interpolate(dWanted,valUc,bigData);
        dlmwrite(nomCapFic,dataCapFile,'-append','delimiter','\t')
    end
    

    
end

end

function deltaC = guessDeltaContact(data)

data = sortrows(data); % sorts the data according to increasing distance
dist1 = data(1,1);dist2 = data(2,1); % the two lowest computed distances
data1 = data(1,:); data2 = data(2,:); % the data for these two distances

data_contact =  data1 - ( data2 - data1 ) / (dist2 - dist1) * dist1;
deltaC = mean(data_contact(5:6)); % mean delta1, delta2
end

function outData = interpolate(dist,valUc,bigData)
% input bigData is a distance sorted bridges data table (matrix), obtained
% directly from solving Laplace-Young equ. It hence shows no specific
% distance values pattern. 
% Output outData is a data table for the given distance "dist" (and many uc
% values) which is obtained distance-interpolating from bigData

dataGreaterDist = bigData(bigData(:,1)>= dist,:);

if isempty(dataGreaterDist) % may happen for maxD, see for instance delta1 
    % increment = 0.5 for maxD determination (with uc*=0), vs possibly 
    % another value for computing bigData
    outData = [];
    out = '\nWARNING: Reaching the last distance value was not possible here. Before use in YADE, you need to';
    out = [out,' modify by hand the 2d line (where nValDist appears) of this capillary file:\n'];
    out = [out,'Just substract 1 to the nValDist value appearing therein\n'];
    fprintf([out,'Sorry about that..\n\n']);
    return
end
maxUc_dist = max(dataGreaterDist(:,2));
valUcOK = valUc(valUc <= maxUc_dist);
nValUcOK = length(valUcOK);

data_dist = -ones(nValUcOK,8); % all data for one given distance

for i = 1:nValUcOK
    currUc = valUcOK(i);
    data_uc = bigData(bigData(:,2) == currUc,:);
    if size(data_uc,1) == 1 % one single liq bridge data is useless
        nValUcOK = i-1;
        break
    end
    if dist < min(data_uc(:,1))
        data1 = data_uc(1,:);
        data2 = data_uc(2,:);
        dist1 = data1(1) ; dist2 = data2(1);
        data_dist(i,:) =  data1 + ( data2 - data1 ) / (dist2 - dist1) * (dist - dist1);
    else
        data_dInf = data_uc(data_uc(:,1) <= dist,:); % not empty after previous if
        data_dInf = data_dInf(size(data_dInf,1),:);
        dInf = data_dInf(1);
        
        data_dSup = data_uc(data_uc(:,1) >= dist,:); % not empty either
        data_dSup = data_dSup(1,:);
        dSup = data_dSup(1);
        
        data_dist(i,:) = data_dInf + (data_dSup-data_dInf)/(dSup-dInf) * (dist-dInf);        
    end
end

data_dist = data_dist(data_dist(:,2) >= 0,:); % in case we faced the break above

if size(data_dist,1) > nValUcOK
    error('Problem here !')
end
if size(data_dist,1) == 0 % may happen for dist = maxD
    out = '\nWARNING: Reaching the last distance value was not possible here. Before use in YADE, you need to';
    out = [out,' modify by hand the 2d line (where nValDist appears) of this capillary file:\n'];
    out = [out,'Just substract 1 to the nValDist value appearing therein\n'];
    fprintf([out,'Sorry about that..\n\n']);
end

outData = [nValUcOK 0 0 0 0 0 0 0;data_dist];

end
