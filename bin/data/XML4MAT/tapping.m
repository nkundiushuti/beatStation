function results = tapping(xmlinput,xmloutput);
%read the xml

warning("off");
[y,varname]=xml2cell(xmlinput,0);


load('gtdata');
numTapped = size(y,2)-3;
M = 1;N = 100;

%write the results
results=[];
results.Score = round((N - M) * rand(1,1) + M)+ round((N - M) * rand(1,1) + M)/100;
results.OverallMean = 80.23;
results.numTapped = numTapped;
xmlwrite=mat2xml(results,'results');
file_id = fopen(xmloutput, 'w');
fprintf(file_id, "%s",xmlwrite);
fclose(file_id);
%xml_write(xmloutput, results);
%exit;
%keyboard

end
