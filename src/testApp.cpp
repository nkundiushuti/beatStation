#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup() {
    ofSetFrameRate(1000);
	//ofSetLogLevel(OF_LOG_VERBOSE);
    ofSetEscapeQuitsApp(FALSE);
    ofSetFullscreen(1);  
    
    //LOAD SETTINGS
    loadXmlSettings("settings.xml");    
  
        
    //LOAD USERS 
    nrUsers = 0;
    maxID = 0;
    if (isClient) 
    {
        if (verbose) cout << " BEAT STATION CLIENT " << endl;
    }
    else
    {    
        if (verbose) cout << " BEAT STATION SERVER " << endl;
        loadXmlUser("data/users.xml"); 
    }
    //usert.initTranscription(numSounds);
    usert.setName("");
    usert.setID(-1);  
    usert.currentSound = -1;
    fromStart = FALSE;
        
    
    //LOAD SOUNDS
    //DIR.setVerbose(false);
    numSounds = DIR.listDir("sounds");
    songNames = new string[numSounds];
    for(int i = 0; i < numSounds; i++){
        songNames[i] = DIR.getPath(i);
    }  
    if (numSounds<2) {cout << "NO SOUNDS IN THE SOUND DIRECTORY!" << endl; exitApp();}
    
    
    ///////MIDI	INTERFACE
	midiIn.openPort(midiPort);
	//midiIn.openPort("IAC Pure Data In");	// by name
	midiIn.ignoreTypes(false, false, false);
    midiIn.addListener((ofxMidiListener *)this);    
        
    
    /////////// INITIALIZE GUI SETTINGS
    float dim = itemDimGUI; 
    float xInit = OFX_UI_GLOBAL_WIDGET_SPACING; 
    float length = ofGetHeight()-xInit;
    vector<string> names; 
    
    
    //LOAD INSTRUCTIONS
    toggleInstructions1 = TRUE;
    toggleInstructions2 = FALSE;
    toggleInstructions3 = FALSE;
    toggleResults = FALSE;
    ofBuffer buffer = ofBufferFromFile("instructions.txt"); // reading into the buffer
    if (buffer.size()>0) instrGUI2 = buffer.getText();
    else instrGUI2 = "";
    instrGUI1 = "If you are a new user, enter your initials then click START.\n If you are already registered click on EXISTING and enter your ID, then click START.";
    instructions1.init("GUI/NewMedia Fett.ttf", 12);
    instructions1.setText(instrGUI1);
    instructions1.wrapTextX(length);     
    instructions1.setColor(240, 240, 240, 180);
    instructions2.init("GUI/NewMedia Fett.ttf", 12);
    instructions2.setText(instrGUI2);
    instructions2.wrapTextX(length);     
    instructions2.setColor(240, 240, 240, 180);
     
    
    //GUI USER
    gui1 = new ofxUICanvas(ofGetWidth()-length,ofGetHeight()/2,ofGetWidth(),ofGetHeight());		//ofxUICanvas(float x, float y, float width, float height)    
    gui1->addWidgetDown(new ofxUILabel("BEAT STATION", OFX_UI_FONT_LARGE));
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
    

    //GUI PRE-TEST
    gui2 = new ofxUICanvas(ofGetWidth()-length,ofGetHeight()/2,ofGetWidth(),ofGetHeight());
    gui2->disable();      
    gui2->addWidgetDown(new ofxUILabel("INSTRUCTIONS", OFX_UI_FONT_LARGE)); 
    gui2->addSpacer(length-xInit, 2); 
    gui2->addWidgetDown(new ofxUILabel("ID", OFX_UI_FONT_MEDIUM));
    gui2->setDrawBack(false);
    ofxUISpacer* spacer = new ofxUISpacer(ofGetWidth()-length, instructions2.getHeight(), "SPACER");
    spacer->setVisible(FALSE);
    gui2->addWidgetDown(spacer);
    gui2->addWidgetDown(new ofxUIButton("START",false, dim*2, dim*2, OFX_UI_FONT_MEDIUM));
    ofAddListener(gui2->newGUIEvent, this, &testApp::guiEvent2); 
    
    
    //GUI TEST
    gui3 = new ofxUICanvas(ofGetWidth()-length,ofGetHeight()/2,ofGetWidth(),ofGetHeight());
    gui3->disable();      
    gui3->addWidgetDown(new ofxUILabel("ID", OFX_UI_FONT_LARGE)); 
    gui3->addSpacer(length-xInit, 2); 
    
    gui3->addWidgetDown(new ofxUIButton("PLAY",false, dim*2, dim*2));
    gui3->addWidgetDown(new ofxUIButton("NEXT",false, dim*2, dim*2));
    
    gui3->addSpacer(length-xInit, 2);  
    
    gui3->addWidgetDown(new ofxUIButton("INSTRUCTIONS",false, dim*2, dim*2));
    //gui3->addSpacer(length-xInit, 2);  
    gui3->addWidgetEastOf(new ofxUIButton("QUIT",false, dim*2, dim*2),"INSTRUCTIONS");    
    
    gui3->addWidgetEastOf(new ofxUILabel("SONG ID", OFX_UI_FONT_MEDIUM),"PLAY");
    gui3->addWidgetSouthOf(new ofxUIRotarySlider(dim*3, 0, 100, 0.0, "POS"),"SONG ID");
    gui3->addWidgetEastOf(new ofxUISlider("VOL", 0.0, 99.0, 75.0, dim, dim * 3),"POS");   
    
    ofAddListener(gui3->newGUIEvent, this, &testApp::guiEvent3); 
    //gui3->centerWidgets();
    gui3->setDrawBack(false);
    
    
    //GUI RESULTS
    gui4 = new ofxUICanvas(ofGetWidth()-length,ofGetHeight()/2,ofGetWidth(),ofGetHeight());
    gui4->disable(); 
    gui4->addWidgetDown(new ofxUILabel("THANK YOU!", OFX_UI_FONT_LARGE)); 
    gui4->addSpacer(length-xInit, 2); 
    
    gui4->addWidgetDown(new ofxUIButton("FINISH",false, dim*2, dim*2)); 
    ofAddListener(gui4->newGUIEvent, this, &testApp::guiEvent4);     
    gui4->setDrawBack(false);  
    if (launchScript) 
    {
        gui4->addWidgetDown(new ofxUILabel("RESULTS", OFX_UI_FONT_MEDIUM));        
        results.init("GUI/NewMedia Fett.ttf", 12);
    }    
   
    
    //TCP CLIENT SERVER   
    if (isClient) 
    {
        //are we connected to the server - if this fails we
        //will check every few seconds to see if the server exists
        weConnected = tcpClient.setup(ipServer, tcpPort);
        
        sendToServer = FALSE;
        connectTime = 0;
        deltaTime = 0;
        
    }
    else
    {
        //setup the server to listen on specific port
        tcpServer.setup(tcpPort);        
    }
     
    
}


//--------------------------------------------------------------
void testApp::update() {    //cout << ofGetElapsedTimeMillis() << " ";
    
    //WHILE SOUND PLAYING, GET MIDI TAPPING, UPDATE ROTARY SLIDER
    if (beats.getIsPlaying())
    {
        if (midiMessage.status == MIDI_CONTROL_CHANGE)
        {        
            //add time stamp to the transcription vector
            usert.sounds[usert.currentSound].time.push_back( beats.getPositionMS() ); 
        }
    
        //update playing widget
        ofxUIRotarySlider *rotary = (ofxUIRotarySlider *) gui3->getWidget("POS");
        //if (beats.getPosition() > 0.99) rotary->setValue(100);
        //else 
            rotary->setValue(beats.getPosition()*100);
    }
    
    
    //SET VOLUME
    ofxUISlider *volume = (ofxUISlider *) gui3->getWidget("VOL");
    beats.setVolume(volume->getValue());
    
    
    //WAIT FOR MATLAB SCRIPT TO END
    if (launchScript) matlabScript.waitForThread();
    
    
    //USER MANAGEMENT, TCP CLIENT/SERVER
    int code=-1;
    vector<string> tokens;
    string msgRx="",msgTx="";
    bool safeToStart = FALSE;
    ofxUILabel *errors = (ofxUILabel*) gui1->getWidget("ERRORS");
    
    //IMPORTANT! we have to be careful with this because it will happen every frame like in a loop
    
    //if SERVER, first we add any users from the clients
    //SERVER LISTENING FOR NEW USERS EVERY FRAME
    if ((!isClient) && (tcpServer.getLastID() >0) )
    {
        //for each connected client lets get the data being sent 
        for(int i = 0; i < tcpServer.getLastID(); i++)
        {   
            //receive a message
            msgRx = "";
            if( !tcpServer.isClientConnected(i) )continue;
            msgRx = tcpServer.receive(i);
            
            if (msgRx.length() > 0) 
            {
                //parse the message
                tokens = ofSplitString(msgRx.c_str(),";");
                if (tokens.size()>1)
                {
                    code=-1;
                    int newU = ofToInt(tokens[0]);
                    string userName = tokens[1];
                    msgTx = "";
                    if(verbose) {cout << " received message from client " << i << " with ip " << tcpServer.getClientIP(i) << endl;cout << " message: " << msgRx << endl;}
                    //add the user to the database or check if it exists
                    if (newU==1)
                    {
                        //get the max id with the existing user with same initials
                        code = checkUser(userName);
                        
                        //increment maxID and nrUsers
                        maxID++;
                        nrUsers++;
                        
                        //send back the data                        
                        text.clear();text.str("");
                        text << maxID << ";" << userName << code << ";";                        
                        tcpServer.send(i, text.str());
                        if(verbose) cout << " sent back to client " << text.str() << endl;
                        
                        //add new user
                        uIDs.push_back(maxID);
                        text.clear();text.str("");
                        text  << userName << code ;
                        uNames.push_back(text.str());
                        
                        
                    }
                    else
                    {
                        //get the max id with the existing user with same initials
                        code = getUserID(userName);
                        
                        //send back the data
                        text.clear();text.str("");
                        text << code << ";";                        
                        tcpServer.send(i, text.str());
                        if(verbose) cout << " sent back to client " << text.str() << endl;
                    }   
                }
                //clear the tokens vector so we can receive a new message
                tokens.clear();
            }
        }  
    }

    //then we add the new local user
    if (fromStart)
    {
        code = -1;
        
        if (isClient) //CLIENT
        {
            msgRx="";
            
            //send the data to server and get the message
            if (sendToServer)
            {
                //we are connected - lets send our text and check what we get back
                if(weConnected){
                    if(verbose) cout << " connected to server " << endl;
                    
                    //what we need to send
                    text.clear();text.str("");
                    text << newUser << ";" << tempName << ";";
                    
                    if(tcpClient.send(text.str()))
                    {
                        //ok we manage to send it so we can exit the loop
                        sendToServer = FALSE;
                        if(verbose) cout << " sent message to server " << endl;
                        
                        //if data has been sent lets receive the response
                        string str = tcpClient.receive();
                        if( str.length() > 0 ){
                            msgRx = str;
                        }
                    }else if(!tcpClient.isConnected()){
                        weConnected = false;
                    }
                }else{
                    //if we are not connected lets try and reconnect every 5 seconds
                    deltaTime = ofGetElapsedTimeMillis() - connectTime;
                    if( deltaTime > 5000 ){
                        if(verbose) cout << " attempting to reconnect to server " << endl;
                        weConnected = tcpClient.setup(ipServer, tcpPort);
                        connectTime = ofGetElapsedTimeMillis();
                        errors->setLabel("CONNECTING...");
                        errors->setVisible(TRUE);
                    }        
                }
            } 
            else
            {
                //if data has been sent lets receive the response
                string str = tcpClient.receive();
                if( str.length() > 0 ){
                    if(verbose) cout << " received back message " << str << endl;
                    msgRx = str;
                    errors->setVisible(FALSE);
                }
            }
            
            //ok we finally received our message back from the server
            if (msgRx.length()>0) 
            {
                //parse the message
                if (newUser)
                {
                    tokens = ofSplitString(msgRx.c_str(),";"); 
                    code = ofToInt(tokens[0]); 
                    tempName = tokens[1]; 
                }
                else
                {
                    tokens = ofSplitString(msgRx.c_str(),";");
                    code = ofToInt(tokens[0]);   
                }
                safeToStart = TRUE;
                tokens.clear();
            }
            else
            {
                errors->setLabel("AUTHENTICATING...");
                errors->setVisible(TRUE);
            }
        }
        else //SERVER
        {
            if (newUser)
            {
                //get the max id with the existing user with same initials
                code = checkUser(tempName);
                
                //assign an ID to the new user, the maximum ID + 1 
                maxID++;
                nrUsers++;
                uIDs.push_back(maxID);
                text.clear();text.str("");
                text << tempName << code;
                uNames.push_back(text.str());
                
                //initialize the current user
                code = maxID;
                tempName = text.str(); 
                                
                //save all the existing users!
                saveXmlUser("data/users.xml");
            }
            //get the max id with the existing user with same initials
            else code = getUserID(tempName);     
            
            safeToStart = TRUE;
        }
        
        //if we already have an user set, let's make everything ready 
        if (safeToStart)
        {
            //now we check what we've got
            if (code == -1) 
            {
                errors->setLabel("ERROR! THE USER WITH THIS ID DOESN'T EXIST IN THE DATABASE");
                errors->setVisible(TRUE);
                safeToStart = FALSE;
                fromStart = FALSE;
            }
            else 
            {
                usert.setID(code);
                usert.setName(tempName); 
                safeToStart = TRUE;
            }
        }
                
        //if we already have an user set, let's make everything ready 
        if (safeToStart)
        {
            if (newUser) //LOAD INSTRUCTIONS
            {
                //init transcription
                usert.initTranscription(numSounds);  
                usert.currentSound = 0;
                randomOrder(usert.getID());               
                
                //LOAD THE INSTRUCTIONS GUI
                text.clear();text.str("");
                text << "TAPPER ID: " << usert.getName() << " ";
                ofxUILabel *userL = (ofxUILabel *) gui2->getWidget("ID");
                userL->setLabel(text.str());
                toggleInstructions1 = FALSE;
                toggleInstructions2 = TRUE;
                gui1->disable();
                gui2->enable();
            }
            else //LOAD TAPPING 
            {
                //load transcription
                usert.initTranscription(numSounds);  
                text.clear();text.str("");
                text << "data/" << tempName << ".xml";
                loadXmlTap(text.str());                   
                randomOrder(usert.getID());
                
                //LOAD TAPPING INTERFACE
                loadTapping(0);
            }
            
            fromStart = FALSE;
        }

    }
       
        
}

//--------------------------------------------------------------
void testApp::draw() {
	ofSetColor(0);
    
    // update the sound playing system:
	ofSoundUpdate();	
    
    if (toggleInstructions1)
    {
        //draw instructions
        //instructions.drawJustified(0, 0, instructions.getWidth());
        ofxUIButton *button = (ofxUIButton *) gui1->getWidget("START");
        ofxUIRectangle *rect = (ofxUIRectangle *) button->getRect();  
        instructions1.drawLeft(rect->getX(), rect->getY() + rect->getHeight() + button->getPadding());
    }
    if (toggleInstructions2)
    {
        //draw instructions
        //instructions.drawJustified(0, 0, instructions.getWidth());
        ofxUIButton *button = (ofxUIButton *) gui2->getWidget("ID");
        ofxUIRectangle *rect = (ofxUIRectangle *) button->getRect(); 
        instructions2.drawLeft(rect->getX(), rect->getY() + rect->getHeight() + button->getPadding());
    }
    if (toggleInstructions3)
    {
        //draw instructions
        //instructions.drawJustified(0, 0, instructions.getWidth());
        ofxUIButton *button = (ofxUIButton *) gui3->getWidget("INSTRUCTIONS");
        ofxUIRectangle *rect = (ofxUIRectangle *) button->getRect(); 
        instructions2.drawLeft(rect->getX(), rect->getY() + rect->getHeight() + button->getPadding());
    }
    if ((launchScript) && (toggleResults))
    {
        //draw instructions
        //instructions.drawJustified(0, 0, instructions.getWidth());
        ofxUILabel *label = (ofxUILabel *) gui4->getWidget("RESULTS");
        ofxUIRectangle *rect = (ofxUIRectangle *) label->getRect(); 
        results.drawLeft(rect->getX(), rect->getY() + rect->getHeight() + label->getPadding());
    } 

    
}

//--------------------------------------------------------------
void testApp::exit() {
    
    if (isClient) tcpClient.close();
    else if (tcpServer.close()) 
    {
        tcpServer.unlock();
        tcpServer.stopThread();
        //save all the existing users!
        saveXmlUser("data/users.xml");
    }
    
    matlabScript.stop();
    
    delete gui1;
    delete gui3;
	delete gui4;
    
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
             
            //load settings
            if (xmlSet.getValue("verbose", 1)) verbose = TRUE;
            else verbose = FALSE;
            midiPort = xmlSet.getValue("midiPort", 0);
            midiChannel = xmlSet.getValue("midiChannel", 10);
            midiNote = xmlSet.getValue("midiNote", 46); 
            noPlays = xmlSet.getValue("noPlays", 2);
            if (xmlSet.getValue("tapWithSpace", 1)) tapWithSpace = TRUE;
            else tapWithSpace = FALSE;
            itemDimGUI = xmlSet.getValue("itemDimGUI", 24);
            if (xmlSet.getValue("launchScript", 1)) launchScript = TRUE;
            else launchScript = FALSE;
            scriptDirectory =  xmlSet.getValue("scriptDirectory", "");
            appToLaunchPath =  xmlSet.getValue("appToLaunchPath", "");
            if (xmlSet.getValue("isClient", 1)) isClient = TRUE;
            else isClient = FALSE;
            tcpPort = xmlSet.getValue("tcpPort", 9001);
            ipServer = xmlSet.getValue("ipServer", "127.0.0.1");;
                    
            xmlSet.popTag();  
        }
        else {
            verbose = FALSE;
            midiPort = 0;
            midiChannel = 10;
            midiNote = 46;
            noPlays = 2;
            tapWithSpace = TRUE;
            itemDimGUI = 24;
            appToLaunchPath = "";
            launchScript = FALSE;
            scriptDirectory = "";
            isClient = FALSE;
            tcpPort = 9001;
            ipServer = "127.0.0.1";
        }
    }
    else {
        verbose = FALSE;
        midiPort = 0;
        midiChannel = 10;
        midiNote = 46;
        noPlays = 2;
        tapWithSpace = TRUE;
        itemDimGUI = 24;
        appToLaunchPath = "";
        launchScript = FALSE;
        scriptDirectory = "";
        isClient = FALSE;
        tcpPort = 9001;
        ipServer = "127.0.0.1";
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
    for(int i = 0; i < nrUsers; i++){
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
            
            if (nrUsers<1) //when entering the program we have to add all users stored
            {
                //get records from the xml
                nrUsers = xmlUser.getNumTags("user"); 
                maxID=-1;
                for(int i = 0; i < nrUsers; i++){
                    xmlUser.pushTag("user", i);            
                    uIDs.push_back(xmlUser.getValue("ID", 0));
                    uNames.push_back(xmlUser.getValue("name", "")); 
                    xmlUser.popTag();                        
                    if (maxID<uIDs[i]) maxID=uIDs[i]; 
                }      
            }
            
            xmlUser.popTag(); //pop position
        }
        else
        {
            //first time, xml is empty
            maxID = -1;
            nrUsers = 0;
        }
    }
    else{
        //first time, xml doesn't exist
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
        ofxUILabel *errors = (ofxUILabel*) gui1->getWidget("ERRORS");
        
        //GATHER USER DATA AND CHECK IT
        ofxUITextInput *uName = (ofxUITextInput *) gui1->getWidget("ID");        
        string name = uName->getTextString(); 
        uName->setTextString("");
        tempName = "";
       
        //check log-in errors
        if (newUser) 
        {
            //check semantic errors
            if ( (name.length() != 2) || (name == "  ") )
            {
                errors->setLabel("ERROR! PLEASE INTRODUCE YOUR INITIALS AGAIN");
                errors->setVisible(TRUE);
            }
            else
            {    
                tempName = name;
                fromStart = TRUE;
                if(isClient) sendToServer = TRUE;
            }    
                        
        }
        else //existing user
        {    
            //check semantic errors
            if ( (name.length() < 3) || (name.substr(0,1) == " ") )
            {
                errors->setLabel("ERROR! PLEASE INTRODUCE YOUR ID AGAIN");
                errors->setVisible(TRUE);
            }
            else
            {
                tempName = name;
                fromStart = TRUE;
                if(isClient) sendToServer = TRUE;
            }
        }
    }  
    
    //CHANGE NEW/EXISTING
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
    int wkind = e.widget->getKind(); 
    string wname = e.widget->getName();
    ofxUIButton *button = (ofxUIButton *) e.widget;    
    
    //START TEST
	if ((wname == "START") && (button->getValue()==1))	
    {  
        //LOAD TAPPING INTERFACE
        loadTapping(1);        
    }
}


void testApp::guiEvent3(ofxUIEventArgs &e)
{  
    ofxUIButton *button = (ofxUIButton *) e.widget;
    
    //play a song
	if ((e.widget->getName() == "PLAY") && (button->getValue()==1))	
    {         
        if (!beats.isLoaded()) { cout << "sound failed to load" << endl; }
        else if ((!beats.getIsPlaying()) && (played<(noPlays+1))) 
        {
            //update playing widget
            ofxUIRotarySlider *rotary = (ofxUIRotarySlider *) gui3->getWidget("POS");
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
            
            //compute and load results
            if (launchScript) 
            {
                callScript();
                loadXmlResults();
                toggleResults = TRUE;
            }
            
            //load gui
            toggleInstructions1 = FALSE;
            gui3->disable();
            gui4->enable();
            button->setValue(FALSE);
        }
        else if (played>0) //move to the next sound if the current sound has been played
        {       
            usert.currentSound++;
            
            if (usert.currentSound > 0) toggleInstructions1 = FALSE;
            
            //load the next sound     
            beats.loadSound(songNames[usert.sounds[usert.currentSound].songID]);            
            beats.setMultiPlay(false);
            played = 0;
            
            //change song number label
            char numstr[21]; // enough to hold all numbers up to 64-bits
            sprintf(numstr, "%d", usert.currentSound+1);
            string label1 = "SONG ID: ", label2 = label1 + numstr;
            ofxUILabel *songN = (ofxUILabel *) gui3->getWidget("SONG ID");
            songN->setLabel(label2);
            
            //update playing widget at zero
            ofxUIRotarySlider *rotary = (ofxUIRotarySlider *) gui3->getWidget("POS");
            rotary->setValue(0.0);
            
        }        
    }
    
    //quit tapping to the log-in
	if ((e.widget->getName() == "QUIT") && (button->getValue()==1))	
    { 
        if (beats.getIsPlaying()) beats.stop();
        
        if (usert.currentSound > 0)
        {
            usert.currentSound--;  //comment this if you want to save the current song
            //save the tapping to xml
            text.clear();text.str("");
            text << "data/" << usert.getName() << ".xml";
            saveXmlTap(text.str());
        }
        
        //we move go the beggining
        //clear the data
        usert.setName("");
        usert.setID(-1);
        usert.currentSound = -1;
        usert.deleteTranscription();
        
        //load the gui1     
        toggleInstructions1 = TRUE;
        toggleInstructions3 = FALSE;
        button->setValue(FALSE);
        ofxUIRadio *radio = (ofxUIRadio *) gui1->getWidget("USER");
        radio->activateToggle("NEW"); 
        newUser = TRUE;
        ofxUILabel *label = (ofxUILabel *) gui1->getWidget("INITIALS");
        label->setLabel("INITIALS");
        gui3->disable();
        gui1->enable();  
        
        if (matlabScript.isThreadRunning()) matlabScript.stop();
    }
    
    if ((e.widget->getName() == "INSTRUCTIONS") && (button->getValue()==1))	
    {     
        //show/hide the instructions
        toggleInstructions3 = !toggleInstructions3;        
    }
    
}


void testApp::guiEvent4(ofxUIEventArgs &e)
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
        toggleInstructions1 = TRUE;
        toggleInstructions3 = FALSE;
        button->setValue(FALSE);
        ofxUIRadio *radio = (ofxUIRadio *) gui1->getWidget("USER");
        radio->activateToggle("NEW"); 
        newUser = TRUE;
        ofxUILabel *label = (ofxUILabel *) gui1->getWidget("INITIALS");
        label->setLabel("INITIALS");
        gui4->disable();
        gui1->enable();  
    }
    
}

void testApp::loadTapping(int stage)
{
    //if the user tapped everything before we just present him the results
    if (usert.currentSound >= numSounds)
    {           
        //load gui
        gui1->disable();
        gui4->enable();
        toggleInstructions1 = FALSE;
        toggleInstructions2 = FALSE;
        toggleInstructions3 = FALSE;
    }
    else 
    {
        //LOAD THE FIRST SOUND
        beats.loadSound(songNames[usert.sounds[usert.currentSound].songID]);
        beats.setMultiPlay(false); 
        beats.setVolume(75);
        
        //ADD WIDGETS
        //user
        text.clear();text.str("");
        text << "TAPPER ID: " << usert.getName() << " ";
        ofxUILabel *userL = (ofxUILabel *) gui3->getWidget("ID");
        userL->setLabel(text.str());
        
        //song nr
        text.clear();text.str("");
        text << "SONG ID: " << usert.currentSound+1 << " ";
        ofxUILabel *songN = (ofxUILabel *) gui3->getWidget("SONG ID");
        songN->setLabel(text.str());
        
        //LOAD THE TAPPING GUI
        toggleInstructions1 = FALSE;
        
        ofxUILabel *errors = (ofxUILabel*) gui1->getWidget("ERRORS");
        errors->setVisible(FALSE);            

        //ofxUIButton *button = (ofxUIButton *) e.widget;
        //button->setValue(FALSE);
        ofxUIRotarySlider *rotary = (ofxUIRotarySlider *) gui3->getWidget("POS");
        rotary->setValue(0.0);        
        ofxUISlider *volume = (ofxUISlider *) gui3->getWidget("VOL");
        volume->setValue(75);
            
        //load the gui
        if (stage==0)   gui1->disable();    
        else    gui2->disable();    
        toggleInstructions1 = FALSE;
        toggleInstructions2 = FALSE;
        toggleInstructions3 = FALSE;
        gui3->enable(); 
    }
}


//---------MATLAB

//run matlab script
void testApp::callScript()
{
    if (!matlabScript.isThreadRunning())
    {
        string command=appToLaunchPath;
        command.replace(command.find("path"), 4, ofFilePath::getAbsolutePath(scriptDirectory));
        text.clear();text.str("");
        text << ofFilePath::getAbsolutePath("data") << "/" << usert.getName() << ".xml";
        command.replace(command.find("xmlin"), 5, text.str());
        text.clear();text.str("");
        text << ofFilePath::getAbsolutePath("data") << "/" << usert.getName() << "out.xml";
        command.replace(command.find("xmlout"), 6, text.str());
        matlabScript.path = command;
        matlabScript.start();
    }
}


//load the results from the xml
void testApp::loadXmlResults()
{
    text.clear();text.str("");
    text <<  "data/" << usert.getName() << "out.xml";
    //parse xml
    
    //needs to be replaced with the actual results
    results.setText(text.str());
    results.wrapTextX(ofGetHeight()-OFX_UI_GLOBAL_WIDGET_SPACING);     
    results.setColor(240, 240, 240, 180);

}


