function tapping(xmlfile)
[tree treeName] = xml_read (xmlfile);
disp([treeName{1} ' ='])
gen_object_display(tree)
