#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup() {
	//ofSetVerticalSync(true);
	//ofBackground(255, 255, 255);
	//ofSetLogLevel(OF_LOG_VERBOSE);
    ofSetEscapeQuitsApp(FALSE);
    
    
    //LOAD SETTINGS
    loadXmlSettings("settings.xml");    
    
        
    //LOAD USERS 
    nrUsers = 0;
    maxID = 0;
    loadXmlUser("data/users.xml");     
    //usert.initTranscription(numSounds);
    usert.setName("");
    usert.setID(-1);
    
    
    //things to be done when the application is launched
    ofSetFullscreen(1);  
    clickTime = ofGetElapsedTimef();
    
    
    //LOAD SOUNDS
    //DIR.setVerbose(false);
    numSounds = DIR.listDir("sounds");
    songNames = new string[numSounds];
    for(int i = 0; i < numSounds; i++){
        songNames[i] = DIR.getPath(i);
    }  
    
    
    ///////MIDI	
	midiIn.openPort(midiPort);
	//midiIn.openPort("IAC Pure Data In");	// by name
	midiIn.ignoreTypes(false, false, false);
    midiIn.addListener((ofxMidiListener *)this);	
    
    
    /////////// INITIALIZE GUI 
    float dim = itemDimGUI; 
    float xInit = OFX_UI_GLOBAL_WIDGET_SPACING; 
    float length = ofGetHeight()-xInit;
    vector<string> names; 
    
    
    //GUI USER
    gui1 = new ofxUICanvas(ofGetWidth()-length,ofGetHeight()/2,ofGetWidth(),ofGetHeight());		//ofxUICanvas(float x, float y, float width, float height)    
    gui1->addWidgetDown(new ofxUILabel("INTRODUCE YOUR INITIALS OR YOUR ID IF YOU ARE ALREADY REGISTERED", OFX_UI_FONT_LARGE));     
    gui1->addSpacer(length-xInit, 2); 
    
    vector<string> vnames; vnames.push_back("NEW"); vnames.push_back("EXISTING"); 
    ofxUIRadio *radio = (ofxUIRadio *) gui1->addWidgetDown(new ofxUIRadio("USER", vnames, OFX_UI_ORIENTATION_VERTICAL, dim, dim )); 
    radio->activateToggle("NEW"); 
    newUser = TRUE;    
    
    gui1->addSpacer(length-xInit, 2); 
    gui1->addWidgetDown(new ofxUIButton("START",false, dim*2, dim*2, OFX_UI_FONT_MEDIUM)); 
    ofAddListener(gui1->newGUIEvent, this, &testApp::guiEvent1); 
    
    gui1->addWidgetEastOf(new ofxUILabel("INITIALS", OFX_UI_FONT_MEDIUM),"USER");
    gui1->addWidgetSouthOf(new ofxUITextInput("ID", "", (length-xInit)/4),"INITIALS");
    ofxUILabel *errors = (ofxUILabel*) new ofxUILabel("ERRORS", OFX_UI_FONT_MEDIUM);
    errors->setVisible(FALSE);
    gui1->addWidgetEastOf(errors,"START"); 
    
    //gui1->centerWidgets();
    gui1->setDrawBack(false);
    //gui1->disable(); 
    
    
    //GUI TEST
    gui2 = new ofxUICanvas(ofGetWidth()-length,ofGetHeight()/2,ofGetWidth(),ofGetHeight());
    gui2->disable();      
    gui2->addWidgetDown(new ofxUILabel("PLAY SOUND AND TAP ALONG", OFX_UI_FONT_LARGE)); 
    gui2->addWidgetEastOf(new ofxUILabel("ID", OFX_UI_FONT_LARGE),"PLAY SOUND AND TAP ALONG"); 
    gui2->addSpacer(length-xInit, 2); 
    
    gui2->addWidgetDown(new ofxUIButton("PLAY",false, dim*2, dim*2));
    gui2->addWidgetDown(new ofxUIButton("NEXT",false, dim*2, dim*2));
    
    gui2->addSpacer(length-xInit, 2);  
    
    gui2->addWidgetDown(new ofxUIButton("INSTRUCTIONS",false, dim*2, dim*2));
    //gui2->addSpacer(length-xInit, 2);  
    gui2->addWidgetEastOf(new ofxUIButton("QUIT",false, dim*2, dim*2),"INSTRUCTIONS");
    
    
    
    gui2->addWidgetEastOf(new ofxUILabel("SONG ID", OFX_UI_FONT_MEDIUM),"PLAY");
    gui2->addWidgetSouthOf(new ofxUIRotarySlider(dim*3, 0, 100, 0.0, "POS"),"SONG ID");
    gui2->addWidgetEastOf(new ofxUISlider("VOL", 0.0, 99.0, 75.0, dim, dim * 3),"POS");
    
    
    ofAddListener(gui2->newGUIEvent, this, &testApp::guiEvent2); 
    //gui2->centerWidgets();
    gui2->setDrawBack(false);
    
    
    //GUI RESULTS
    gui3 = new ofxUICanvas(ofGetWidth()-length,ofGetHeight()/2,ofGetWidth(),ofGetHeight());
    gui3->disable(); 
    gui3->addWidgetDown(new ofxUILabel("THANK YOU!", OFX_UI_FONT_LARGE)); 
    gui3->addSpacer(length-xInit, 2); 
    
    gui3->addWidgetDown(new ofxUIButton("FINISH",false, dim*2, dim*2)); 
    ofAddListener(gui3->newGUIEvent, this, &testApp::guiEvent3);     
    gui3->setDrawBack(false);
    
    
    
    //LOAD INSTRUCTIONS
    toggleInstructions = FALSE;
    ofBuffer buffer = ofBufferFromFile("instructions.txt"); // reading into the buffer
    instructions.init("GUI/NewMedia Fett.ttf", 12);
    instructions.setText(buffer.getText());
    instructions.wrapTextX(length);     
    instructions.setColor(240, 240, 240, 180);
}

//--------------------------------------------------------------
void testApp::update() {
    if (beats.getIsPlaying())
    {
        if (midiMessage.status == MIDI_CONTROL_CHANGE)
        {        
            //add time stamp to the transcription vector
            usert.sounds[usert.currentSound].time.push_back( beats.getPositionMS() ); 
        }
    
        //update playing widget
        ofxUIRotarySlider *rotary = (ofxUIRotarySlider *) gui2->getWidget("POS");
        if (beats.getPosition() > 0.94) rotary->setValue(100);
        else rotary->setValue(beats.getPosition()*100);
    }
    
    ofxUISlider *volume = (ofxUISlider *) gui2->getWidget("VOL");
    beats.setVolume(volume->getValue());
}

//--------------------------------------------------------------
void testApp::draw() {
	ofSetColor(0);
    
    // update the sound playing system:
	ofSoundUpdate();	
    
    if (toggleInstructions)
    {
        //draw instructions
        //instructions.drawJustified(0, 0, instructions.getWidth());
        ofxUIButton *button = (ofxUIButton *) gui2->getWidget("INSTRUCTIONS");
        ofxUIRectangle *rect = (ofxUIRectangle *) button->getRect(); 
        instructions.drawLeft(rect->getX(), rect->getY() + rect->getHeight() + button->getPadding());
    }
}

//--------------------------------------------------------------
void testApp::exit() {
    
    
    delete gui1;
    delete gui2;
	delete gui3;
    delete[] uIDs;
    delete[] uNames;
    
	// clean up
	midiIn.closePort();
	midiIn.removeListener(this);
}

//--------------------------------------------------------------
void testApp::newMidiMessage(ofxMidiMessage& msg) {

	// make a copy of the latest message
	midiMessage = msg;
}

//--------------------------------------------------------------
void testApp::keyPressed(int key) {
    
    switch (key) {
        case ' ':
            //add time stamp to the transcription vector
            if ((tapWithSpace) && (beats.getIsPlaying()))  
                usert.sounds[usert.currentSound].time.push_back( beats.getPositionMS() ); 
            break;
        case 1:
            OF_EXIT_APP(0); 
            //cout << "1 pressed " << endl;
            break; 
        case 26:
            OF_EXIT_APP(0); 
            //cout << "z pressed " << endl;
            break; 
        case OF_KEY_ESC:
            //cout << "esc pressed " << endl;
            OF_EXIT_APP(0); 
            break;
        default:
            break;
    }
}

//--------------------------------------------------------------
void testApp::keyReleased(int key) {
}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y) {
}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button) {
}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button) {
}

//--------------------------------------------------------------
void testApp::mouseReleased() {
}

//////////////-------LOAD SETTINGS FOR THE APP-----------
void testApp::loadXmlSettings(string fileName)
{
    //save user to the xml file
    ofxXmlSettings xmlSet;    
    
    if(xmlSet.loadFile(fileName))
    {        
        if (xmlSet.tagExists("settings"))
        {   
            xmlSet.pushTag("settings");               
             
            //load midi settings
            midiPort = xmlSet.getValue("midiPort", 0);
            midiChannel = xmlSet.getValue("midiChannel", 10);
            midiNote = xmlSet.getValue("midiNote", 46); 
            noPlays = xmlSet.getValue("noPlays", 2);
            if (xmlSet.getValue("tapWithSpace", 1)) tapWithSpace = TRUE;
            else tapWithSpace = FALSE;
            itemDimGUI = xmlSet.getValue("itemDimGUI", 24);
                    
            xmlSet.popTag();  
        }
        else {
            midiPort = 0;
            midiChannel = 10;
            midiNote = 46;
            noPlays = 2;
            tapWithSpace = TRUE;
            itemDimGUI = 24;
        }
    }
    else {
        midiPort = 0;
        midiChannel = 10;
        midiNote = 46;
        noPlays = 2;
        tapWithSpace = TRUE;
        itemDimGUI = 24;
    }
    
}




//////////////---------USER functions---------------------

///USER LIST SAVE/LOAD
void testApp::saveXmlUser(string fileName)
{
    //save user to the xml file
    ofxXmlSettings xmlUser;
    xmlUser.addTag( "users" );
    xmlUser.pushTag( "users" );
    
    xmlUser.setValue("records", nrUsers + 1);
    xmlUser.setValue("maxID", maxID + 1);
    
    //add all the existing users
    for(int i = 0; i <= nrUsers; i++){
        //each position tag represents one user
        xmlUser.addTag( "user" );
        xmlUser.pushTag( "user" , i);
        //set the values 
        xmlUser.setValue("ID", uIDs[i]);
        xmlUser.setValue("name", uNames[i]);
        xmlUser.popTag();//pop position        
    } 
    
    xmlUser.popTag();
    xmlUser.saveFile( fileName );
    
}


void testApp::loadXmlUser(string fileName)
{
    //users.clear();
    //load all users from the xml file into the users vector
    ofxXmlSettings xmlUser;
    
    if(xmlUser.loadFile(fileName))
    {
        
        if (xmlUser.tagExists("users"))
        {
            xmlUser.pushTag("users");
            
            if (nrUsers==0) //when entering the program we have to add all users stored
            {
                //get records from the xml
                nrUsers = xmlUser.getNumTags("user");  
                uIDs = new int[nrUsers+1];
                uNames = new string[nrUsers+1];
                for(int i = 0; i < nrUsers; i++){
                    xmlUser.pushTag("user", i);            
                    uIDs[i] = xmlUser.getValue("ID", 0);
                    uNames[i] = xmlUser.getValue("name", ""); 
                    xmlUser.popTag();            
                }      
                maxID = uIDs[nrUsers-1];
            }
            else //we need to syncronize the users we have with the ones from xml database
            {
                //get records from the xml
                int xmlRecords = xmlUser.getNumTags("user");   
                int * xmlIDs = new int[xmlRecords];
                string * xmlNames = new string[xmlRecords];
                for(int i = 0; i < xmlRecords; i++){
                    xmlUser.pushTag("user", i);            
                    xmlIDs[i] = xmlUser.getValue("ID", 0);
                    xmlNames[i] = xmlUser.getValue("name", "");        
                    xmlUser.popTag();            
                }  
                //merge existing data with the xml data
                array_union(xmlIDs, xmlNames, xmlRecords);                
                maxID = uIDs[nrUsers-1];
                
                //free memory
                delete[] xmlIDs;
                delete[] xmlNames;
            }
            
            xmlUser.popTag(); //pop position
        }
        else
        {
            //first time, xml is empty
            uIDs = new int[1];
            uNames = new string[1];
            maxID = -1;
            nrUsers = 0;
        }
    }
    else{
        //first time, xml doesn't exist
        uIDs = new int[1];
        uNames = new string[1];
        maxID = -1;
        nrUsers = 0;
    }
    
}


////TAPPING SAVE/LOAD
void testApp::saveXmlTap(string fileName)
{
    //save user to the xml file
    ofxXmlSettings xmlTap;
    int nrRec;
    
    xmlTap.addTag( "tapping" );
    xmlTap.addAttribute("tapping","userID", usert.getID(),0);
    xmlTap.addAttribute("tapping","currentSound", usert.currentSound,0);
    xmlTap.pushTag( "tapping" );
    
    //add random indexes 
    text.clear();text.str("");
    for(int i = 0; i < numSounds; i++){
        text << usert.sounds[i].songID << " ";
    }
    xmlTap.setValue("songIndexes",text.str().c_str());
    
    if (usert.currentSound >= numSounds) nrRec=numSounds; else nrRec=usert.currentSound+1;
    for(int i = 0; i < nrRec; i++){
        //each position tag represents one user
        xmlTap.addTag( "song" );
        xmlTap.addAttribute("song","songID", usert.sounds[i].songID,i);
        xmlTap.pushTag( "song" , i);
        
        //set the values 
        xmlTap.setValue("fileName",songNames[usert.sounds[i].songID]);
        
        //write the time stamps in a string
        std::vector<int>::const_iterator it;
        text.clear();text.str("");
        //copy(usert.sounds[i].time.begin(), usert.sounds[i].time.end(), ostreambuf_iterator<int>(ss, " "));
        for( it = usert.sounds[i].time.begin(); it != usert.sounds[i].time.end(); ++it )
        {
            text << *it << " ";
        }
        xmlTap.setValue("transcription", text.str().c_str());
        
        xmlTap.popTag();//pop position        
    } 
    
    xmlTap.popTag();
    xmlTap.saveFile( fileName );
    
}


void testApp::loadXmlTap(string fileName)
{
    //save user to the xml file
    ofxXmlSettings xmlTap;    
    
    if(xmlTap.loadFile(fileName))
    {        
        if (xmlTap.tagExists("tapping"))
        {   
            //cout << usert.getID() << " " << 
            if (usert.getID() != xmlTap.getAttribute("tapping", "userID", 0)) usert.currentSound=0;
            else 
            {
                xmlTap.pushTag("tapping");
                usert.currentSound = xmlTap.getNumTags("song");
                //usert.currentSound=xmlTap.getAttribute("tapping", "currentSound", 0); //or xmlTap.getNumTags("song")
                
                string transcript="";
                int t;
                for(int i = 0; i < xmlTap.getNumTags("song"); i++){                    
                    xmlTap.pushTag("song", i);
                    usert.sounds[i].songID = xmlTap.getAttribute("song", "songID", i);                    
                    //xmlTap.getValue("filename", "");
                    
                    //load transcription
                    transcript = xmlTap.getValue("transcription", "");      
                    text.clear();text.str("");
                    text << transcript;
                    vector<string> tokens;
                    while (text >> t)                        
                        usert.sounds[i].time.push_back(t);
                    
                    xmlTap.popTag();            
                }  
            }
            
        }
        else usert.currentSound=0;
    }
    else usert.currentSound=0;
    
}


//union of arrays
void testApp::array_union(int* arr, string* sarr, int n)
{
    int i = 0, j = 0, k = 0;
    tempIDs = new int[n+nrUsers];
    tempNames = new string[n+nrUsers];
    while ((i < n ) || (j < nrUsers))
    {
        if(arr[i] == uIDs[j]) 
        { // found a common element.
            tempIDs[k] = arr[i];
            tempNames[k] = sarr[i];
            i++; // move on.
            j++;
        }
        else if(arr[i] > uIDs[j])
        {
            tempIDs[k] = uIDs[j];
            tempNames[k] =  uNames[j];
            j++;// don't change i, move j.
        }
        else
        {
            tempIDs[k] = arr[i];
            tempNames[k] = sarr[i];
            i++;// don't change j, move i.
        }
        k++;
    }
    
    delete[] uIDs;
    delete[] uNames;
    nrUsers = k;
    uIDs = new int[nrUsers+1]; //nrUsers+1 because after this we need to add a new user
    uNames = new string[nrUsers+1];
    for(int i = 0; i < nrUsers; i++){
        uIDs[i] = tempIDs[i];
        uNames[i] = tempNames[i];
    }
    delete[] tempIDs;
    delete[] tempNames;
    
}


//returns the id of the user with the given initials
int testApp::checkUser(string initials)
{
    int code=0;
    int nameID=0;
    for(int i = 0; i < nrUsers; i++)
    {       
       if (uNames[i].substr(0,2) == initials)
       {
           text.clear();text.str("");
           text << uNames[i].substr(2,uNames[i].length()-2);
           text >> nameID; 
           if (nameID > code) code = nameID;           
       }
    }
    code++;
    return code;
}


//returns the unique ID of the user with the uName
int testApp::getUserID(string uName)
{
    int code=-1;
    
    //cout << uName << endl; 
    for(int i = 0; i < nrUsers; i++)
    {
        if (uNames[i] == uName) code = uIDs[i];
    }
    return code;
}


//randomize the playing order for a specific user id
void testApp::randomOrder(int uid)
{
    int t, i = numSounds;
    srand(uid);//unique random order for each user
    int* deck = new int[numSounds];
    for(int i = 0; i < numSounds; i++)  deck[i] = i+1;
    
    while (i > 1) {
        int k = rand() % numSounds;
        i--;
        t = deck[i];
        deck[i] = deck[k];
        deck[k] = t;
    }
    for(int i = 0; i < numSounds; i++){ 
        usert.sounds[i].songID = deck[i]-1;
    }  
    delete[] deck;
}




/////////////-----------GUI----------

void testApp::guiEvent1(ofxUIEventArgs &e)
{
    int wkind = e.widget->getKind(); 
    string wname = e.widget->getName();
    ofxUIButton *button = (ofxUIButton *) e.widget;    
    
    //START TEST
	if ((wname == "START") && (button->getValue()==1))	
    {           
        bool forward=TRUE;
        
        //prevent starting if clicked right away - solved a bug of button class
        //if ( (ofGetElapsedTimef() - clickTime ) < 1)  {clickTime = ofGetElapsedTimef(); return;}   
        
        ofxUILabel *errors = (ofxUILabel*) gui1->getWidget("ERRORS");
        
        //GATHER USER DATA AND CHECK IT
        ofxUITextInput *uName = (ofxUITextInput *) gui1->getWidget("ID");        
        string name = uName->getTextString(); 
        uName->setTextString("");
        
        //check for errors 
        
        //check log-in errors, refresh the data
        if (forward)
        {
            //load and refresh the xml data
            loadXmlUser("data/users.xml"); 
            
            if (newUser) 
            {
                //cout << "NEW USER" << endl;
                //check semantic errors
                if ( (name.length() != 2) || (name == "  ") )
                {
                    errors->setLabel("ERROR! PLEASE INTRODUCE YOUR INITIALS AGAIN");
                    errors->setVisible(TRUE);
                    forward = FALSE;
                }
                else
                {                    
                    //get the max id with the existing user with same initials
                    int code = checkUser(name);
                
                    //assign an ID to the new user, the maximum ID + 1 
                    uIDs[nrUsers] = maxID+1;
                    text.clear();text.str("");
                    text << name << code;
                    uNames[nrUsers] = text.str();
                    usert.setID(maxID+1);
                    usert.setName(text.str()); 
                
                    //save the new user
                    saveXmlUser("data/users.xml");
                
                    //init transcription
                    //usert.deleteTranscription();
                    usert.initTranscription(numSounds);  
                    usert.currentSound = 0;
                    randomOrder(usert.getID());   
                    
                }
            }
            else
            {                
                //check semantic errors
                if ( (name.length() < 3) || (name.substr(0,1) == " ") )
                {
                    errors->setLabel("ERROR! PLEASE INTRODUCE YOUR ID AGAIN");
                    errors->setVisible(TRUE);
                    forward = FALSE;
                }
                else
                {
                    //get the max id with the existing user with same initials
                    int code = getUserID(name);
                    if (code == -1) 
                    {
                        errors->setLabel("ERROR! THE USER WITH THIS ID DOESN'T EXIST IN THE DATABASE");
                        errors->setVisible(TRUE);
                        forward = FALSE;
                    }
                    else 
                    {
                        usert.setID(code);
                        usert.setName(name); 
                        //load transcription
                        //usert.deleteTranscription();
                        usert.initTranscription(numSounds);  
                        text.clear();text.str("");
                        text << "data/" << name << ".xml";
                        loadXmlTap(text.str());                   
                        randomOrder(usert.getID());
                    }
                }
                
            }
        }            
        
        
        //if the user tapped everything before we just present him the results
        if (usert.currentSound >= numSounds)
        {           
            //load gui
            gui1->disable();
            gui3->enable();
            forward = FALSE;              
        }
        //MOVE FORWARD TO THE TAPPING        
        if (forward)
        {                
            //LOAD THE FIRST SOUND
            beats.loadSound(songNames[usert.sounds[usert.currentSound].songID]);
            beats.setMultiPlay(false); 
            beats.setVolume(75);
                        
            //ADD WIDGETS
            //user
            text.clear();text.str("");
            text << "ID: " << usert.getName() << " ";
            ofxUILabel *userL = (ofxUILabel *) gui2->getWidget("ID");
            userL->setLabel(text.str());
            
            //song nr
            char numstr[21]; // enough to hold all numbers up to 64-bits
            sprintf(numstr, "%d", usert.currentSound+1);
            string label1 = "SONG ID: ", label2 = label1 + numstr;
            ofxUILabel *songN = (ofxUILabel *) gui2->getWidget("SONG ID");
            songN->setLabel(label2);
            
            //LOAD THE TAPPING GUI
            errors->setVisible(FALSE);            
            gui1->disable();
            gui2->enable();
            button->setValue(FALSE);
        }
    }  
    
    //change new/existing user
    else if(wkind == OFX_UI_WIDGET_TOGGLE)
    {
        ofxUIToggle *toggle = (ofxUIToggle *) e.widget; 
        ofxUILabel *label = (ofxUILabel *) gui1->getWidget("INITIALS");
        if ((wname == "NEW") && (toggle->getValue() == 1)) {
            newUser = TRUE;
            label->setLabel("INITIALS");
        }
        else if ((wname == "EXISTING") && (toggle->getValue() == 1)) {
            newUser = FALSE;
            label->setLabel("ID");
        }          
    }
    
}


void testApp::guiEvent2(ofxUIEventArgs &e)
{  
    ofxUIButton *button = (ofxUIButton *) e.widget;
    
    //play a song
	if ((e.widget->getName() == "PLAY") && (button->getValue()==1))	
    {         
        if ((!beats.getIsPlaying()) && (played<(noPlays+1))) {
            //update playing widget
            ofxUIRotarySlider *rotary = (ofxUIRotarySlider *) gui2->getWidget("POS");
            rotary->setValue(0.0);
            
            //clear transcription vector for the current sound
            usert.sounds[usert.currentSound].time.clear();
            beats.play();  
            played++;
        }
    }   
    
    //next song
    if ((e.widget->getName() == "NEXT") && (button->getValue()==1))	
    {             
        if (beats.getIsPlaying()) {return;} //everything is inactive when sound is playing
        else if ((usert.currentSound == (numSounds-1)) && (played>0)) //move to the results stage
        {  
            usert.currentSound++;
            //save the tapping to xml
            text.clear();text.str("");
            text << "data/" << usert.getName() << ".xml";
            saveXmlTap(text.str());                       
            
            //load gui
            gui2->disable();
            gui3->enable();
            button->setValue(FALSE);
        }
        else if (played>0) //move to the next sound if the current sound has been played
        {       
            usert.currentSound++;
            
            //load the next sound     
            beats.loadSound(songNames[usert.sounds[usert.currentSound].songID]);            
            beats.setMultiPlay(false);
            played = 0;
            
            //change song number label
            char numstr[21]; // enough to hold all numbers up to 64-bits
            sprintf(numstr, "%d", usert.currentSound+1);
            string label1 = "SONG ID: ", label2 = label1 + numstr;
            ofxUILabel *songN = (ofxUILabel *) gui2->getWidget("SONG ID");
            songN->setLabel(label2);
            
            //update playing widget at zero
            ofxUIRotarySlider *rotary = (ofxUIRotarySlider *) gui2->getWidget("POS");
            rotary->setValue(0.0);
            
        }        
    }
    
    //quit tapping to the log-in
	if ((e.widget->getName() == "QUIT") && (button->getValue()==1))	
    { 
        if (usert.currentSound > 0)
        {
            usert.currentSound--;
            //save the tapping to xml
            text.clear();text.str("");
            text << "data/" << usert.getName() << ".xml";
            saveXmlTap(text.str());
        }
        
        //we move go the beggining
        //clear the data
        usert.setName("");
        usert.setID(-1);
        usert.currentSound = 0;
        usert.deleteTranscription();
        
        //load gui
        gui2->disable();
        gui1->enable();
        button->setValue(FALSE);        
    }
    
    if ((e.widget->getName() == "INSTRUCTIONS") && (button->getValue()==1))	
    { 
        //show/hide the instructions
        toggleInstructions = !toggleInstructions;
    }
    
}


void testApp::guiEvent3(ofxUIEventArgs &e)
{    
    ofxUIButton *button = (ofxUIButton *) e.widget;    
    
    //finish
    if ((e.widget->getName() == "FINISH") && (button->getValue()==1))	
    {
        //we move go the beggining
        //clear the data
        usert.setName("");
        usert.setID(-1);
        usert.currentSound = 0;
        usert.deleteTranscription();
        
        //syncronize data
        //loadXmlUser("data/users.xml");              
        
        //load the gui1            
        gui3->disable();
        gui1->enable();  
        button->setValue(FALSE);
        
        //prevent starting if clicked right away - solved a bug of button class
        //if ( (ofGetElapsedTimef() - clickTime ) < 1)  {clickTime = ofGetElapsedTimef(); return;}   
    }
    
}



