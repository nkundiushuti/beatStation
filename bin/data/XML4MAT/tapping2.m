function results = tapping2(xmlinput,xmloutput);
%read the xml

warning("off");
[y,varname]=xml2cell(xmlinput,0);



% load gt mat file
% gt{1}.name = 'T1238732984234.txt'
% gt{1}.beats = 0.5 1.0 1.5 etc - beats times
load('gtdata');

params.minBeatTime = 3;
% size of tolerance window for beat phase in continuity based evaluation
params.continuityBased.phaseThresh = 0.175;
% size of tolerance window for beat period in continuity based evaluation
params.continuityBased.periodThresh = 0.175;
% standard deviation of gaussian error function
params.cemgilAcc.sigma = 0.04;

% identify appropriate gt sequence for all files with taps
% evaluate taps vs gt per file
% try to do it for all files present
numTapped = size(y,2)-3;
amlT = zeros(1,numTapped); % don't know how many we'll get

easyEval = 1; % use less strict AMLt, else use AmlCem which punishes poor accuracy more.

songOrder = str2num(y{3}.songIndexes)+1;

for i=1:numTapped,

   tappedBeats{songOrder(i)} = str2num(y{i+3}.song{4}.transcription)/1000;


   if (easyEval)
       [tmp,tmp,tmp,amlT(songOrder(i))]= be_continuityBased(gtdata{songOrder(i)}.beats,tappedBeats{songOrder(i)},params);
   else
amlT(songOrder(i))=be_amlCem(gtdata{songOrder(i)}.beats,tappedBeats{songOrder(i)},params);
   end

end

% take average of best five
[sortedscores,inds]=sort(amlT,'descend');
bestFive = mean(sortedscores(1:5));



%write the results
results=[];
results.Score = bestFive;
results.OverallMean = sum(amlT);
results.numTapped = numTapped;
results.Results = 'results';
xmlwrite=mat2xml(results,'results');
file_id = fopen(xmloutput, 'w');
fprintf(file_id, "%s",xmlwrite);
fclose(file_id);
%xml_write(xmloutput, results);

exit;
%keyboard


function [cmlC,cmlT,amlC,amlT] = be_continuityBased(anns,beats,params)

if nargin<3
 params = be_params;
end


% put the beats and annotations into column vectors
anns = anns(:);
beats = beats(:);

% remove beats and annotations that are within the first 5 seconds
anns(anns<params.minBeatTime) = [];
beats(beats<params.minBeatTime) = [];

% Check if there are any beats, if not then exit
if( or( (length(beats)<6), (length(anns)<6) ) ),
 disp('beat or annotation sequence is empty or too short, assigning zero to all outputs [cmlC,cmlT,amlC,amlT]');

 cmlC = 0;
 cmlT = 0;
 amlC = 0;
 amlT = 0;

 return;
end

% also check that the beat times are in seconds and not any other time units
if( or( (max(beats)>2000) , (max(anns)>2000) ) )
 error('either beats or annotations are not in seconds, please rectify.');
 return;
end


p = params.continuityBased.phaseThresh;
t = params.continuityBased.periodThresh;

doubleAnns = interp1([1:length(anns)],anns,[1:0.5:length(anns)]);

% make different variants of annotations
% normal annotations
testAnns{1} = anns;
% off-beats
testAnns{2} = doubleAnns(2:2:end);
% double tempo
testAnns{3} = doubleAnns;
% half tempo odd-beats (i.e. 1,3,1,3)
testAnns{4} = anns(1:2:end);
% half tempo even-beat (i.e. 2,4,2,4)
testAnns{5} = anns(2:2:end);


% loop analysis over number of variants on annotations
for j=1:size(testAnns,2),

 [totAcc(j),contAcc(j)] = ContinuityEval(testAnns{j},beats,t,p);

end


% assign the accuracy scores
cmlC = contAcc(1);
cmlT = totAcc(1);
amlC = max(contAcc);
amlT = max(totAcc);


function [totAcc,contAcc] = ContinuityEval(anns,beats,t,p)
% sub-function for calculating continuity-based accuracy


checkanns = zeros(1,max(length(anns),length(beats)));
checkbeats = checkanns;

cond = 0;



for i=1:length(beats),

   cond = 0;
   [a,b] = min(abs(beats(i)-anns)); % look for nearest annotation to current beat

   if(checkanns(b)==1) % if we've already used this annotation..
       cond = 0;
   else

       if or(i==1,b==1), % either first beat or first annotation, look forward on both

           if (and((abs(a/(anns(b+1)-anns(b)))<p), ...
(abs(1-((beats(i+1)-beats(i))/(anns(b+1)-anns(b))))<t)));
               checkanns(b) = 1;
               cond = 1;
           end

       else % not first beat or first annotation, so can look backwards

           if (and((abs(a/(anns(b)-anns(b-1)))<p), ...
(abs(1-((beats(i)-beats(i-1))/(anns(b)-anns(b-1))))<t)));
               checkanns(b) = 1;
               cond = 1;
           end

       end

   end

   % for i^th beat what is the condition
   checkbeats(i) = double(cond);

end

checkbeats = [0 checkbeats(:)' 0];

[d1,d2,d3] = find(checkbeats==0);
checkbeats = checkbeats(2:end-1);

% in best case, d2 = 1 & length(checkbeats)
contAcc = 100*(max(diff(d2))-1)/length(checkbeats);
totAcc = 100*sum(checkbeats)/length(checkbeats);


function [amlcem] = be_amlCem(anns,beats,params)


if nargin<3
 params = be_params;
end


% put the beats and annotations into column vectors
anns = anns(:);
beats = beats(:);

% remove beats and annotations that are within the first 5 seconds
anns(anns<params.minBeatTime) = [];
beats(beats<params.minBeatTime) = [];

% Check if there are any beats, if not then exit
if or(isempty(beats),isempty(anns))
 disp('beat sequence is empty, assigning zero to amlCem');

   amlcem = 0;

 return;
end

% also check that the beat times are in seconds and not any other time units
if( or( (max(beats)>2000) , (max(anns)>2000) ) )
 error('either beats or annotations are not in seconds, please rectify.');
 return;
end


p = params.continuityBased.phaseThresh;
t = params.continuityBased.periodThresh;

doubleAnns = interp1([1:length(anns)],anns,[1:0.5:length(anns)]);

% make different variants of annotations
% normal annotations
testAnns{1} = anns;
% off-beats
testAnns{2} = doubleAnns(2:2:end);
% double tempo
testAnns{3} = doubleAnns;
% half tempo odd-beats (i.e. 1,3,1,3)
testAnns{4} = anns(1:2:end);
% half tempo even-beat (i.e. 2,4,2,4)
testAnns{5} = anns(2:2:end);


% loop analysis over number of variants on annotations
for j=1:size(testAnns,2),
   amlcem(j) = be_cemgilAcc(testAnns{j},beats,params);
end

amlcem = max(amlcem);


function [a] = be_cemgilAcc(anns,beats,params)


if nargin<3
 params = be_params;
end


% put the beats and annotations into column vectors
anns = anns(:);
beats = beats(:);

% remove beats and annotations that are within the first 5 seconds
anns(anns<params.minBeatTime) = [];
beats(beats<params.minBeatTime) = [];


% Check if there are any beats, if not then exit
if or(isempty(beats),isempty(anns))
 disp('beat sequence is empty, assigning zero to all outputs [a]');
 a = 0;
 return;
end

% also check that the beat times are in seconds and not any other time units
if( or( (max(beats)>2000) , (max(anns)>2000) ) )
 error('either beats or annotations are not in seconds, please rectify.');
 return;
end


% get standard deviation for Gaussian error function
sigma = params.cemgilAcc.sigma;



% beat accuracy initially zero
a = 0;


for i=1:length(anns)

 % find the closest beat to each annotation
 beatDiff = min(abs(anns(i) - beats));

 % work out the value on the gaussian error function and add to the cumulative sum
 a = a + exp(-(beatDiff.^2)/(2*(sigma^2)));

end

% normalise by the mean of the number of annotations and beats
a = a/(0.5*(length(beats)+length(anns)));

% put into the range 0 to 100%
a = a*100;