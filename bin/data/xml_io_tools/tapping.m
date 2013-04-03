function tapping(xmlinput, xmloutput)
%read the xml 
[tree treeName] = xml_read (xmlinput);
%disp([treeName{1} ' =']);
gen_object_display(tree);

%do something with the results


%write the results
results=[];
results.Score = 10;
results.Results = 'results';
xml_write(xmloutput, results);
exit;