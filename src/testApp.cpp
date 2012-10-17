#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup() {
    
    //LOAD SETTINGS
    loadXmlSettings("settings.xml"); 
    
    //APP SETTINGS
    ofSetFrameRate(fps);
    //ofSetLogLevel(OF_LOG_VERBOSE);
    ofSetEscapeQuitsApp(FALSE);
    if (fullscreen) ofSetFullscreen(1);
    
    
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
    usert.setFullName("");
    usert.setDate("");
    usert.setID(-1);  
    usert.currentSound = -1;
    fromStart = FALSE;
    askResults = FALSE;
    
    
    //LOAD SOUNDS
    //DIR.setVerbose(false);
    numSounds = DIR.listDir("sounds");
    songNames = new string[numSounds];
    for(int i = 0; i < numSounds; i++){
        songNames[i] = DIR.getPath(i);
    }  
    if (numSounds<2) {cout << "NO SOUNDS IN THE SOUND DIRECTORY!" << endl; exitApp();}
    play = FALSE;
    tempTime = 0; 
    played = 0;
    
    
    //LOAD HIGHSCORES
    loadXmlScores("data/scores.xml"); 
    
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
    toggleInstructions11 = TRUE;
    toggleInstructions2 = FALSE;
    toggleInstructions3 = FALSE;
    toggleResults = FALSE;
    toggleScore = FALSE;
    ofBuffer buffer = ofBufferFromFile("instructions.txt"); // reading into the buffer
    if (buffer.size()>0) instrGUI2 = buffer.getText();
    else instrGUI2 = "";
    buffer = ofBufferFromFile("description.txt"); // reading into the buffer
    if (buffer.size()>0) instrGUI11 = buffer.getText();
    else instrGUI11 = "";
    instrGUI1 = "If you are new user enter your name and click START. <> If you are a returning user please click EXISTING and enter your ID, then click START.";
    instructions1.init("GUI/newmediafett.ttf", 12);
    instructions1.setText(instrGUI1);
    instructions1.wrapTextX(length);     
    instructions1.setColor(240, 240, 240, 180);
    instructions11.init("GUI/newmediafett.ttf", 12);
    instructions11.setText(instrGUI11);
    instructions11.wrapTextX(length);     
    instructions11.setColor(240, 240, 240, 180);
    instructions2.init("GUI/newmediafett.ttf", 12);
    instructions2.setText(instrGUI2);
    instructions2.wrapTextX(length);     
    instructions2.setColor(240, 240, 240, 180);    
    text.clear();text.str("");
    text << "Your tapping score is 0. <> You have tapped 0 songs. <> You can improve your score by resuming tapping at a later time. <>";  
    //needs to be replaced with the actual results
    if (launchScript) 
    {     
        results.init("GUI/newmediafett.ttf", 12);
        results.setText(text.str());
        results.wrapTextX(ofGetHeight()-OFX_UI_GLOBAL_WIDGET_SPACING);     
        results.setColor(240, 240, 240, 180);
    } 
    getScoreTable();
    
    
    //GUI USER
    gui1 = new ofxUICanvas(ofGetWidth()-length,ofGetHeight()/2,ofGetWidth(),ofGetHeight());		//ofxUICanvas(float x, float y, float width, float height)    
    gui1->addWidgetDown(new ofxUILabel("BEAT STATION", OFX_UI_FONT_LARGE));
    ofxUISpacer* spaceri = new ofxUISpacer(1.1*length, 2, "SPACER");
    gui1->addWidgetDown(spaceri);
    
    vector<string> vnames; vnames.push_back("NEW"); vnames.push_back("EXISTING"); 
    ofxUIRadio *radio = (ofxUIRadio *) gui1->addWidgetDown(new ofxUIRadio("USER", vnames, OFX_UI_ORIENTATION_VERTICAL, dim, dim )); 
    radio->activateToggle("NEW"); 
    newUser = TRUE;    
    
    gui1->addSpacer(1.1*length, 2); 
    gui1->addWidgetDown(new ofxUIButton("START",false, dim*2, dim*2, OFX_UI_FONT_MEDIUM)); 
    ofxUIButton* scorest = new ofxUIButton("SCORES",false, dim*2, dim*2, OFX_UI_FONT_MEDIUM);
    gui1->addWidgetRight(scorest);
    if (!launchScript) scorest->setVisible(FALSE);
    ofAddListener(gui1->newGUIEvent, this, &testApp::guiEvent1); 
    
    gui1->addWidgetEastOf(new ofxUILabel("NAME", OFX_UI_FONT_MEDIUM),"USER");
    gui1->addWidgetSouthOf(new ofxUITextInput("ID", "", (length-xInit)/2),"NAME");
    ofxUILabel *errors = (ofxUILabel*) new ofxUILabel("ERRORS", OFX_UI_FONT_SMALL);
    errors->setVisible(FALSE);
    gui1->addWidgetSouthOf(errors,"ID");  
    //gui1->centerWidgets();
    gui1->setDrawBack(false);
    //gui1->disable();     
    
    /*
    //use this too if you wanna use quiz
    //GUI QUIZ
    gui2 = new ofxUICanvas(ofGetWidth()-length,ofGetHeight()/2,ofGetWidth(),ofGetHeight());
    gui2->disable();      
    gui2->addWidgetDown(new ofxUILabel("QUESTIONS", OFX_UI_FONT_LARGE)); 
    gui2->addSpacer(1.3*length, 2); 
    
    gui2->addWidgetDown(new ofxUILabel("AGE", OFX_UI_FONT_MEDIUM));
    gui2->addWidgetSouthOf(new ofxUITextInput("AGEN", "", (length-xInit)/10),"AGE");
    vector<string> vs; vs.push_back("MALE"); vs.push_back("FEMALE");     
    ofxUIRadio *radios = (ofxUIRadio *) gui2->addWidgetSouthOf(new ofxUIRadio("GENDER", vs, OFX_UI_ORIENTATION_HORIZONTAL, dim, dim ),"AGEN");
    radios->activateToggle("MALE"); 
    gui2->addWidgetDown(new ofxUILabel("YEARS OF MUSICAL TRAINING", OFX_UI_FONT_MEDIUM));
    gui2->addWidgetDown(new ofxUITextInput("YEARS", "", (length-xInit)/10));
    vector<string> vy; vy.push_back("LOW"); vy.push_back("MEDIUM"); vy.push_back("HIGH");  
    ofxUIRadio *radioy = (ofxUIRadio *) gui2->addWidgetDown(new ofxUIRadio("FAMILIARITY WITH TURKISH MUSIC", vy, OFX_UI_ORIENTATION_HORIZONTAL, dim, dim ));
    radioy->activateToggle("LOW"); 
    gui2->setDrawBack(false);
    gui2->addWidgetDown(new ofxUIButton("NEXT",false, dim*2, dim*2, OFX_UI_FONT_MEDIUM));
    ofxUILabel *errors1 = (ofxUILabel*) new ofxUILabel("ERRORS", OFX_UI_FONT_MEDIUM);
    errors1->setVisible(FALSE);
    gui2->addWidgetEastOf(errors1,"NEXT");   
    ofAddListener(gui2->newGUIEvent, this, &testApp::guiEvent2); 
    */
    
    //GUI PRE-TEST
    gui3 = new ofxUICanvas(ofGetWidth()-length,ofGetHeight()/2,ofGetWidth(),ofGetHeight());
    gui3->disable();      
    gui3->addWidgetDown(new ofxUILabel("INSTRUCTIONS", OFX_UI_FONT_LARGE)); 
    gui3->addSpacer(1.1*length, 2); 
    ofColor black(0, 0, 0);
    ofxUILabel* id1 = new ofxUILabel("ID", OFX_UI_FONT_MEDIUM);    
    id1->setColorFill(black);
    gui3->addWidgetDown(id1);
    gui3->setDrawBack(false);
    ofxUISpacer* spacer = new ofxUISpacer(1.1*length, instructions2.getHeight(), "SPACER");
    spacer->setVisible(FALSE);
    gui3->addWidgetDown(spacer);
    gui3->addWidgetDown(new ofxUIButton("START",false, dim*2, dim*2, OFX_UI_FONT_MEDIUM));
    ofxUISpacer* sp = new ofxUISpacer(0.5*length, 2, "SPACER1");
    gui3->addWidgetEastOf(sp, "SPACER");
    img = new ofImage(); 
    img->loadImage("screen.png"); 
    ofxUIImage* guimg = new ofxUIImage(2*itemDimGUI + ofGetHeight()-length, 0, img->width, img->height, img, "SNAPSHOT OF THE TAPPING INTERFACE", TRUE);
    gui3->addWidgetEastOf(guimg, "SPACER");
    sp->setVisible(FALSE);
    ofAddListener(gui3->newGUIEvent, this, &testApp::guiEvent3); 
    
    
    //GUI TEST
    gui4 = new ofxUICanvas(ofGetWidth()-length,ofGetHeight()/2,ofGetWidth(),ofGetHeight());
    gui4->disable();    
    gui4->addWidgetDown(new ofxUILabel("BEAT STATION", OFX_UI_FONT_LARGE));    
    gui4->addSpacer(1.1*length, 2); 
    
    ofxUIButton* play = new ofxUIButton("PLAYF",false, dim*2, dim*2);
    play->setLabelVisible(FALSE);
    gui4->addWidgetDown(play);    
    ofxUIButton* next = new ofxUIButton("NEXTF",false, dim*2, dim*2);
    next->setLabelVisible(FALSE);
    gui4->addWidgetDown(next);
    
    gui4->addSpacer(1.1*length, 2);  
    
    gui4->addWidgetDown(new ofxUIButton("INSTRUCTIONS",false, dim*2, dim*2));
    //ofxUILabel* tap = new ofxUILabel("TAP USING SPACE BAR", OFX_UI_FONT_MEDIUM);   
    //tap->setColorFill(black);
    //gui4->addWidgetDown(tap);
    ofxUILabel* id2 = new ofxUILabel("ID", OFX_UI_FONT_MEDIUM);    
    id2->setColorFill(black);
    gui4->addWidgetDown(id2); 
    ofxUIButton* quitButton = new ofxUIButton("QUIT",false, dim*2, dim*2);
    gui4->addWidgetEastOf(quitButton,"INSTRUCTIONS"); 
    if (canQuit) quitButton->setVisible(TRUE);
    else quitButton->setVisible(FALSE);
    
    ofxUILabel* playl = new ofxUILabel("PLAY AGAIN", OFX_UI_FONT_SMALL);
    gui4->addWidgetEastOf(playl,"PLAYF");
    gui4->addWidgetEastOf(new ofxUILabel("NEXT SONG", OFX_UI_FONT_SMALL),"NEXTF");
    gui4->addWidgetEastOf(new ofxUILabel("SONG ID", OFX_UI_FONT_MEDIUM),"PLAY AGAIN");
    gui4->addWidgetSouthOf(new ofxUIRotarySlider(dim*3, 0, 100, 0.0, "%"),"SONG ID");
    gui4->addWidgetEastOf(new ofxUISlider("VOL", 0.0, 99.0, 75.0, dim, dim * 3),"%");  
    playl->setLabel("PLAY");
    ofxUILabel *errors1 = (ofxUILabel*) new ofxUILabel("PLEASE WAIT...", OFX_UI_FONT_MEDIUM);
    errors1->setVisible(FALSE);
    gui4->addWidgetEastOf(errors1,"QUIT");
    ofAddListener(gui4->newGUIEvent, this, &testApp::guiEvent4); 
    //gui4->centerWidgets();
    gui4->setDrawBack(false);
    
    
    //GUI RESULTS
    gui5 = new ofxUICanvas(ofGetWidth()-length,ofGetHeight()/2,ofGetWidth(),ofGetHeight());
    gui5->disable(); 
    gui5->addWidgetDown(new ofxUILabel("THANK YOU!", OFX_UI_FONT_LARGE)); 
    gui5->addSpacer(1.1*length, 2); 
    ofxUILabel* id3 = new ofxUILabel("ID", OFX_UI_FONT_MEDIUM);    
    id3->setColorFill(black);
    gui5->addWidgetDown(id3);
    ofxUISpacer* spacer2 = new ofxUISpacer(length, results.getHeight(), "SPACER2");
    gui5->addWidgetDown(spacer2);    
    gui5->addWidgetDown(new ofxUIButton("FINISH",false, dim*2, dim*2)); 
    ofxUILabel *errors2 = (ofxUILabel*) new ofxUILabel("PLEASE WAIT...", OFX_UI_FONT_MEDIUM);
    errors2->setVisible(FALSE);
    gui5->addWidgetEastOf(errors2,"FINISH");
    spacer2->setVisible(FALSE);
    ofAddListener(gui5->newGUIEvent, this, &testApp::guiEvent5);     
    gui5->setDrawBack(false);  
       
    
    
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
    
    if (ofIsStringInString(password, passToExit)) exitApp();
    
    if (play)
    {
        //ofSleepMillis(1000);
        beats.play();
        play = FALSE;
    }
    
    //WHILE SOUND PLAYING, GET MIDI TAPPING, UPDATE ROTARY SLIDER
    if (beats.getIsPlaying())
    {
        ////TO BE TESTED
        if (midiMessage.status == MIDI_CONTROL_CHANGE)
        { 
            if ((midiChannel == 0) && (midiNote == 0)) 
                usert.sounds[usert.currentSound].time1.push_back(beats.getPositionMS()); 
            else tempTime = beats.getPositionMS();
        }
        //this can happen on a future call
        if ((midiChannel != 0) && (midiNote != 0)) 
            if ((tempTime != 0) && (midiMessage.pitch!=0))
            {
                if (midiMessage.pitch==midiNote)
                {
                    usert.sounds[usert.currentSound].time1.push_back(tempTime); 
                    //if (verbose) cout << " " << tempTime;
                }
                tempTime = 0;           
            }
        
        
        //update playing widget
        ofxUIRotarySlider *rotary = (ofxUIRotarySlider *) gui4->getWidget("%");
        rotary->setValue(beats.getPosition()*100);
        //cout << rotary->getValue();
    }
    else
    {
        if (played==1) 
        {
            //update the play button label
            ofxUILabel* playl = (ofxUILabel *) gui4->getWidget("PLAY AGAIN");
            playl->setLabel("PLAY AGAIN");
        }
        if (played>0) 
        {
            //update playing widget
            ofxUIRotarySlider *rotary = (ofxUIRotarySlider *) gui4->getWidget("%");
            if (rotary->getValue()>0) rotary->setValue(100);
        }
    }
    
    
    //SET VOLUME
    ofxUISlider *volume = (ofxUISlider *) gui4->getWidget("VOL");
    beats.setVolume(volume->getValue());
    
    
    
    if (launchScript) 
    {        
               
        
        if (askResults)
        {
            ofxUILabel *errors1 = (ofxUILabel*) gui4->getWidget("PLEASE WAIT...");
            ofxUILabel *errors2 = (ofxUILabel*) gui5->getWidget("PLEASE WAIT...");
            ofxUIButton *finish = (ofxUIButton*) gui5->getWidget("FINISH");
            
            errors1->setVisible(TRUE);
            errors2->setVisible(TRUE);
            
            if (!matlabScript.isRunning()) 
            {
                //ofSleepMillis(1 * 1000);  
                finish->setVisible(TRUE);
                errors1->setVisible(FALSE);
                errors2->setVisible(FALSE);
                loadXmlResults();
                saveXmlScores("data/scores.xml");
                toggleResults = TRUE;
                askResults = FALSE;
            } 
        }
    }
    
    
    //USER MANAGEMENT, TCP CLIENT/SERVER
    int code=-1;
    vector<string> tokens;
    string msgRx="",msgTx="";
    bool safeToStart = FALSE;
    ofxUILabel *errors1 = (ofxUILabel*) gui1->getWidget("ERRORS");
    //use this too if you wanna use quiz
    //ofxUILabel *errors2 = (ofxUILabel*) gui2->getWidget("ERRORS");
    
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
                tokens = ofSplitString(msgRx.c_str(),"<&>");
                if (tokens.size()>1)
                {
                    code=-1;
                    int newU = ofToInt(tokens[0]);
                    string userName = tokens[1];
                    string userFullName = tokens[2];
                    string userDate = tokens[3];
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
                        text << maxID << "<&>" << userName << code << "<&>";                        
                        tcpServer.send(i, text.str());
                        if(verbose) cout << " sent back to client " << text.str() << endl;
                        
                        //add new user
                        uIDs.push_back(maxID);
                        text.clear();text.str("");
                        text  << userName << code ;
                        uNames.push_back(text.str());
                        uFullNames.push_back(userFullName);
                        uDate.push_back(userDate);
                        //use this too if you wanna use quiz
                        //uAge.push_back(ofToInt(tokens[2]));
                        //uMF.push_back(ofToInt(tokens[3]));
                        //uYears.push_back(ofToInt(tokens[4]));
                        //uFam.push_back(ofToInt(tokens[5]));
                    }
                    else
                    {
                        //get the max id with the existing user with same initials
                        code = getUserID(userName);
                        
                        //send back the data
                        text.clear();text.str("");
                        text << code << "<&>";                        
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
                    //use this instead if you wanna use quiz
                    //text << newUser << "<&>" << tempName << "<&>" << tempAge << "<&>" << tempMF << "<&>" << tempYears << "<&>" << tempFam << "<&>";
                    text << newUser << "<&>" << tempName << "<&>" << tempFullName << "<&>" << tempDate << "<&>";
                    
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
                        /* //use this instead if you wanna use quiz
                        if (newUser)
                        {
                            errors2->setLabel("CONNECTING...");
                            errors2->setVisible(TRUE);                           
                        }
                        else
                        {
                            errors1->setLabel("CONNECTING...");
                            errors1->setVisible(TRUE);
                        }
                         */
                        errors1->setLabel("CONNECTING...");
                        errors1->setVisible(TRUE);
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
                    //use this instead if you wanna use quiz
                    //if (newUser) errors2->setVisible(FALSE); 
                    //else errors1->setVisible(FALSE);
                    errors1->setVisible(FALSE);
                }
            }
            
            //ok we finally received our message back from the server
            if (msgRx.length()>0) 
            {
                //parse the message
                if (newUser)
                {
                    tokens = ofSplitString(msgRx.c_str(),"<&>"); 
                    code = ofToInt(tokens[0]); 
                    tempName = tokens[1]; 
                }
                else
                {
                    tokens = ofSplitString(msgRx.c_str(),"<&>");
                    code = ofToInt(tokens[0]);   
                }
                safeToStart = TRUE;
                tokens.clear();
            }
            else
            {
                /* //use this instead if you wanna use quiz
                if (newUser)
                {
                    errors2->setLabel("AUTHENTICATING...");
                    errors2->setVisible(TRUE);
                }
                else
                {
                    errors1->setLabel("AUTHENTICATING...");
                    errors1->setVisible(TRUE);
                }
                 */
                errors1->setLabel("AUTHENTICATING...");
                errors1->setVisible(TRUE);
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
                uFullNames.push_back(tempFullName);
                uDate.push_back(tempDate);
                //use this too if you wanna use quiz
                //uAge.push_back(tempAge);
                //uMF.push_back(tempMF);
                //uYears.push_back(tempYears);
                //uFam.push_back(tempFam);
                
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
                errors1->setLabel("ERROR! THE USER WITH THIS ID DOESN'T EXIST IN THE DATABASE");
                errors1->setVisible(TRUE);
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
            //just to be safe, we save all the user list
            if (!isClient) saveXmlUser("data/users.xml");          
                                    
            if (newUser) //LOAD INSTRUCTIONS
            {
                //init transcription
                usert.initTranscription(numSounds);  
                usert.currentSound = 0;
                randomOrder(usert.getID());               
                
                //LOAD THE INSTRUCTIONS GUI
                text.clear();text.str("");
                text << "PLEASE REMEMBER YOUR USER ID: " << usert.getName() << " ";
                ofxUILabel *userL = (ofxUILabel *) gui3->getWidget("ID");
                userL->setLabel(text.str());
                ofxUILabel *userL1 = (ofxUILabel *) gui5->getWidget("ID");
                userL1->setLabel(text.str());
                //toggleInstructions1 = FALSE;
                toggleInstructions2 = TRUE;
                
                //use this instead if you wanna use quiz
                //gui2->disable();
                gui1->disable();
                gui3->enable();
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
    ofBackground(150, 150, 150, 190); 
    ofPushStyle(); 
	//ofEnableBlendMode(OF_BLENDMODE_ALPHA); 
    
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
    if (toggleInstructions11)
    {
        //draw instructions
        //instructions.drawJustified(0, 0, instructions.getWidth());
        ofxUISpacer *label = (ofxUISpacer *) gui1->getWidget("SPACER");
        ofxUIRectangle *rect = (ofxUIRectangle *) label->getRect();  
        instructions11.drawLeft(2*itemDimGUI + rect->getX() + rect->getWidth() ,rect->getY());
    }
    
    /*
    if (toggleScore)
    {
        //draw score table
        ofxUIButton *button = (ofxUIButton *) gui1->getWidget("START");
        ofxUIRectangle *rect = (ofxUIRectangle *) button->getRect();          
        //scoreTable.drawJustified(rect->getX(), rect->getY() + rect->getHeight() + instructions1.getHeight() + button->getPadding(), 2.2*scoreTable.getWidth());
        scoreTable1.drawLeft(rect->getX(), rect->getY() + rect->getHeight() + instructions1.getHeight() + button->getPadding());
        //scoreTable2.drawCenter(2*itemDimGUI +rect->getX()+scoreTable1.getWidth(), rect->getY() + rect->getHeight() + instructions1.getHeight() + button->getPadding());
        scoreTable3.drawCenter(2*itemDimGUI +rect->getX()+scoreTable1.getWidth(), rect->getY() + rect->getHeight() + instructions1.getHeight() + button->getPadding());
        scoreTable4.drawCenter(5*itemDimGUI +rect->getX()+scoreTable1.getWidth()+scoreTable3.getWidth(), rect->getY() + rect->getHeight() + instructions1.getHeight() + button->getPadding());
        
    }
    */
    
    if (toggleInstructions2)
    {
        //draw instructions
        //instructions.drawJustified(0, 0, instructions.getWidth());
        ofxUIButton *button = (ofxUIButton *) gui3->getWidget("ID");
        ofxUIRectangle *rect = (ofxUIRectangle *) button->getRect(); 
        instructions2.drawLeft(rect->getX(), rect->getY() + rect->getHeight() + button->getPadding());
    }
    if (toggleInstructions3)
    {
        //draw instructions
        //instructions.drawJustified(0, 0, instructions.getWidth());
        ofxUIButton *button = (ofxUIButton *) gui4->getWidget("ID");
        ofxUIRectangle *rect = (ofxUIRectangle *) button->getRect(); 
        instructions2.drawLeft(rect->getX(), rect->getY() + rect->getHeight() + button->getPadding());
    }
    if ((launchScript) && (toggleResults))
    {
        //draw instructions
        //instructions.drawJustified(0, 0, instructions.getWidth());
        ofxUILabel *label = (ofxUILabel *) gui5->getWidget("ID");
        ofxUIRectangle *rect = (ofxUIRectangle *) label->getRect(); 
        results.drawLeft(rect->getX(), rect->getY() + rect->getHeight() + label->getPadding());
    } 
    
    
}

//--------------------------------------------------------------
void testApp::exit() {
    if ((gui4->isEnabled()) && (usert.currentSound>-1) && (usert.sounds[usert.currentSound].time1.size()>1))
    {
        //save the tapping to xml
        text.clear();text.str("");
        text << "data/" << usert.getName() << ".xml";
        saveXmlTap(text.str());
    }
    
    saveXmlScores("data/scores.xml"); 
    
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
    //use this if you plan to use quiz
    //delete gui2;
    delete gui3;
    delete gui4;
	delete gui5;
    delete img;
    
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
    
    password += key;
    
    switch (key) {
        case ' ':
            //add time stamp to the transcription vector
            if ((tapWithSpace) && (beats.getIsPlaying()))  
                usert.sounds[usert.currentSound].time1.push_back( beats.getPositionMS() ); 
            break;
        case 9:
            //add time stamp to the transcription vector
            if ((tapWithTab) && (beats.getIsPlaying()))  
                usert.sounds[usert.currentSound].time2.push_back( beats.getPositionMS() ); 
            break;
        case 1:
            //OF_EXIT_APP(0); 
            //cout << "1 pressed " << endl;
            break; 
        case 26:
            //OF_EXIT_APP(0); 
            //cout << "q pressed " << endl;
            break; 
        case 113:
            //OF_EXIT_APP(0); 
            //cout << "z pressed " << endl;
            break;        
        case OF_KEY_ESC:
            //cout << "esc pressed " << endl;
            //OF_EXIT_APP(0); 
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
        string name = ofToLower(uName->getTextString()); 
        uName->setTextString("");
        tempName = "";
        
        //remove trailing and leading spaces
        name.erase(name.begin(), std::find_if(name.begin(), name.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
        name.erase(std::find_if(name.rbegin(), name.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), name.end());
        
        //check log-in errors
        if (newUser) 
        {
            //check semantic errors
            if (name.length() < 2)
            {
                errors->setLabel("ERROR! PLEASE ENTER YOUR NAME AGAIN");
                errors->setVisible(TRUE);
            }
            else
            {    
                vector<string> names = ofSplitString(name, " ");
                text.clear();text.str("");
                if (names.size()>1) text << names[0].substr(0,1) << names[1].substr(0,1);
                else text << names[0].substr(0,2);
                tempName = text.str();
                tempFullName = name;
                tempDate = ofGetTimestampString();
                
                //fromStart = TRUE;
                //if(isClient) sendToServer = TRUE;  
                toggleInstructions1 = FALSE;
                toggleInstructions11 = FALSE;
                toggleScore = FALSE;
                
                //comment these three lines if you wanna use quiz
                fromStart = TRUE;
                if(isClient) sendToServer = TRUE;
                
                //use this instead if you wanna use quiz
                //gui2->enable();
                gui1->disable();
                gui3->enable();
                
            }    
            
        }
        else //existing user
        {    
            //check semantic errors
            if (name.length() < 3)
            {
                errors->setLabel("ERROR! PLEASE ENTER YOUR ID AGAIN");
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
    
    //SCORE TABLE
	if ((wname == "SCORES") && (button->getValue()==1))	
    {  
        getScoreTable();
        toggleScore = !toggleScore; 
    }
    
    //CHANGE NEW/EXISTING
    else if(wkind == OFX_UI_WIDGET_TOGGLE)
    {
        ofxUIToggle *toggle = (ofxUIToggle *) e.widget; 
        ofxUILabel *label = (ofxUILabel *) gui1->getWidget("NAME");
        if ((wname == "NEW") && (toggle->getValue() == 1)) {
            newUser = TRUE;
            label->setLabel("NAME");
        }
        else if ((wname == "EXISTING") && (toggle->getValue() == 1)) {
            newUser = FALSE;
            label->setLabel("ID");
        }          
    }
    
}

//use this too if you wanna use quiz
/*
void testApp::guiEvent2(ofxUIEventArgs &e)
{
    int wkind = e.widget->getKind(); 
    string wname = e.widget->getName();
    ofxUIButton *button = (ofxUIButton *) e.widget;    
    
	if ((wname == "NEXT") && (button->getValue()==1))	
    {  
        ofxUILabel *errors = (ofxUILabel*) gui2->getWidget("ERRORS");
        bool nextStep = TRUE;
        //GATHER USER DATA AND CHECK IT
        ofxUITextInput *uAge = (ofxUITextInput *) gui2->getWidget("AGEN");    
        ofxUITextInput *uYears = (ofxUITextInput *) gui2->getWidget("YEARS");
        tempAge = 0;
        tempMF = 0;
        tempYears = 0;
        tempFam = 0;
        
        if ((uYears->getTextString().length() < 1) || (!is_number(uYears->getTextString())))
        {
            errors->setLabel("ERROR! PLEASE ENTER THE YEARS OF MUSICAL TRAINING AGAIN");
            errors->setVisible(TRUE);
            nextStep = FALSE;
            uYears->setTextString("");
        }
        else tempYears = ofToInt(uYears->getTextString());
        
        if  ((uAge->getTextString().length() < 1) || (!is_number(uAge->getTextString())))
        {
            errors->setLabel("ERROR! PLEASE ENTER YOUR AGE AGAIN");
            errors->setVisible(TRUE);
            nextStep = FALSE;
            uAge->setTextString("");
        }
        else tempAge = ofToInt(uAge->getTextString()); 
        
        if (nextStep)
        {
            ofxUIRadio *radios = (ofxUIRadio *) gui2->getWidget("GENDER"); 
            ofxUIRadio *radiof = (ofxUIRadio *) gui2->getWidget("FAMILIARITY WITH TURKISH MUSIC");            
            ofxUIToggle * toggles = radios->getActive();
            ofxUIToggle * togglef = radiof->getActive();
            tempMF = toggles->getValue();
            tempFam = togglef->getValue();
            uAge->setTextString("");
            uYears->setTextString("");
            
            //save the data or send it to server
            fromStart = TRUE;
            if(isClient) sendToServer = TRUE;            
        }
        
    }
}
*/

void testApp::guiEvent3(ofxUIEventArgs &e)
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


void testApp::guiEvent4(ofxUIEventArgs &e)
{  
    ofxUIButton *button = (ofxUIButton *) e.widget;
    
    //play a song
	if ((e.widget->getName() == "PLAYF") && (button->getValue()==1))	
    {         
        if (!beats.isLoaded()) { cout << "sound failed to load" << endl; }
        else if ((!beats.getIsPlaying()) && (played<noPlays)) 
        {
            //update playing widget
            ofxUIRotarySlider *rotary = (ofxUIRotarySlider *) gui4->getWidget("%");
            rotary->setValue(0.0);
            
            //clear transcription vector for the current sound
            tempTime = 0;
            usert.sounds[usert.currentSound].time1.clear();
            usert.sounds[usert.currentSound].time2.clear();
            usert.sounds[usert.currentSound].tryNo = played+1;  
            //beats.play();    
            play = TRUE;
        }
        if (!beats.getIsPlaying())  played++;
    }   
    
    //next song
    if ((e.widget->getName() == "NEXTF") && (button->getValue()==1))	
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
                //ofResetElapsedTimeCounter();
                ofxUILabel *errors1 = (ofxUILabel*) gui4->getWidget("PLEASE WAIT...");
                ofxUILabel *errors2 = (ofxUILabel*) gui5->getWidget("PLEASE WAIT...");
                ofxUIButton *finish = (ofxUIButton*) gui5->getWidget("FINISH");
                finish->setVisible(FALSE);
                errors1->setVisible(TRUE);
                errors2->setVisible(TRUE);
                callScript();
                askResults = TRUE;
            }
            
            //load gui
            toggleInstructions1 = FALSE;
            toggleScore = FALSE;
            toggleInstructions11 = FALSE;
            toggleInstructions3 = FALSE;
            gui4->disable();
            gui5->enable();
            button->setValue(FALSE);
            
            //update the play button label
            ofxUILabel* playl = (ofxUILabel *) gui4->getWidget("PLAY AGAIN");
            playl->setLabel("PLAY");
            played = 0;
        }
        else if (played>0) //move to the next sound if the current sound has been played
        {      
            //save the tapping to xml
            text.clear();text.str("");
            text << "data/" << usert.getName() << ".xml";
            saveXmlTap(text.str()); 
            
            usert.currentSound++;
            
            if (usert.currentSound > 0) {toggleInstructions1 = FALSE;toggleInstructions11 = FALSE;toggleScore = FALSE;}
            
            //load the next sound     
            beats.loadSound(songNames[usert.sounds[usert.currentSound].songID]);            
            beats.setMultiPlay(false);
            played = 0;
            
            //change song number label
            char numstr[21]; // enough to hold all numbers up to 64-bits
            sprintf(numstr, "%d", usert.currentSound+1);
            string label1 = "SONG ID: ", label2 = label1 + numstr;
            ofxUILabel *songN = (ofxUILabel *) gui4->getWidget("SONG ID");
            songN->setLabel(label2);
            
            //update playing widget at zero
            ofxUIRotarySlider *rotary = (ofxUIRotarySlider *) gui4->getWidget("%");
            rotary->setValue(0.0);
            
            //update the play button label
            ofxUILabel* playl = (ofxUILabel *) gui4->getWidget("PLAY AGAIN");
            playl->setLabel("PLAY");            
            
        }        
    }
    
    //quit tapping to the log-in
	if ((e.widget->getName() == "QUIT") && (button->getValue()==1))	
    { 
        //if (beats.getIsPlaying()) {return;} //everything is inactive when sound is playing
        //else 
        //{
            if (usert.currentSound > -1)
            {
                if (usert.sounds[usert.currentSound].time1.size()<1)
                {
                    usert.currentSound--;                      
                }
                else
                {
                    //save the tapping to xml
                    text.clear();text.str("");
                    text << "data/" << usert.getName() << ".xml";
                    saveXmlTap(text.str());
                }
            }
            
            //stop the audio
            if (beats.getIsPlaying()) beats.stop();
            
            //update the play button label
            ofxUILabel* playl = (ofxUILabel *) gui4->getWidget("PLAY AGAIN");
            playl->setLabel("PLAY");
        
            if (usert.currentSound + 1 >= minTaps) //we move to the results
            {
                //compute and load results
                if (launchScript) 
                {
                    //ofResetElapsedTimeCounter();
                    ofxUILabel *errors1 = (ofxUILabel*) gui4->getWidget("PLEASE WAIT...");
                    ofxUILabel *errors2 = (ofxUILabel*) gui5->getWidget("PLEASE WAIT...");
                    ofxUIButton *finish = (ofxUIButton*) gui5->getWidget("FINISH");
                    finish->setVisible(FALSE);
                    errors1->setVisible(TRUE);
                    errors2->setVisible(TRUE);
                    callScript();
                    askResults = TRUE;
                }
                
                //load gui
                toggleInstructions1 = FALSE;
                toggleInstructions11 = FALSE;
                toggleInstructions3 = FALSE;
                toggleScore = FALSE;
                gui4->disable();
                gui5->enable();
                button->setValue(FALSE);
                played = 0;
            }
            else //we move the beggining
            {
                //load the gui1     
                toggleInstructions1 = TRUE;
                toggleInstructions11 = TRUE;
                toggleInstructions3 = FALSE;
                button->setValue(FALSE);
                ofxUIRadio *radio = (ofxUIRadio *) gui1->getWidget("USER");
                radio->activateToggle("NEW"); 
                newUser = TRUE;
                ofxUILabel *label = (ofxUILabel *) gui1->getWidget("NAME");
                label->setLabel("NAME");
                gui4->disable();
                gui1->enable();
                
                
                //clear the data
                usert.setName("");
                usert.setID(-1);
                usert.currentSound = -1;
                usert.deleteTranscription();
                played = 0;
                
            }
            
            
           
            //if (matlabScript.isThreadRunning()) matlabScript.stop();
        //}
    }
    
    if ((e.widget->getName() == "INSTRUCTIONS") && (button->getValue()==1))	
    {     
        //show/hide the instructions
        toggleInstructions3 = !toggleInstructions3;        
    }
    
}


void testApp::guiEvent5(ofxUIEventArgs &e)
{    
    ofxUIButton *button = (ofxUIButton *) e.widget;    
    
    //finish
    if ((e.widget->getName() == "FINISH") && (button->getValue()==1))	
    { 
        matlabScript.stop();
        
        //we move go the beggining
        //clear the data
        usert.setName("");
        usert.setFullName("");
        usert.setDate("");
        usert.setID(-1);
        usert.currentSound = -1;
        usert.deleteTranscription();
        played = 0;
        
        //syncronize data
        //loadXmlUser("data/users.xml");              
        
        //load the gui1     
        toggleInstructions1 = TRUE;
        toggleInstructions11 = TRUE;
        toggleInstructions3 = FALSE;
        button->setValue(FALSE);
        ofxUIRadio *radio = (ofxUIRadio *) gui1->getWidget("USER");
        radio->activateToggle("NEW"); 
        newUser = TRUE;
        ofxUILabel *label = (ofxUILabel *) gui1->getWidget("NAME");
        label->setLabel("NAME");
        toggleResults = FALSE;
        gui5->disable();
        gui1->enable();  
    }
    
}

void testApp::loadTapping(int stage)
{
    //if the user tapped everything before we just present him the results
    if (usert.currentSound >= numSounds)
    {   
        //compute and load results
        if (launchScript) 
        {
            askResults = TRUE;
        }
        
        //ADD WIDGETS
        //user
        text.clear();text.str("");
        text << "PLEASE REMEMBER YOUR USER ID: " << usert.getName() << " ";
        ofxUILabel *userL1 = (ofxUILabel *) gui5->getWidget("ID");
        userL1->setLabel(text.str());
        
        //load gui
        gui1->disable();
        gui5->enable();
        toggleInstructions1 = FALSE;
        toggleInstructions11 = FALSE;
        toggleInstructions2 = FALSE;
        toggleInstructions3 = FALSE;
        toggleScore = FALSE;
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
        text << "PLEASE REMEMBER YOUR USER ID: " << usert.getName() << " ";
        ofxUILabel *userL = (ofxUILabel *) gui4->getWidget("ID");
        userL->setLabel(text.str());
        ofxUILabel *userL1 = (ofxUILabel *) gui5->getWidget("ID");
        userL1->setLabel(text.str());
        
        //song nr
        text.clear();text.str("");
        text << "SONG ID: " << usert.currentSound+1 << " ";
        ofxUILabel *songN = (ofxUILabel *) gui4->getWidget("SONG ID");
        songN->setLabel(text.str());              
        
        ofxUILabel *errors = (ofxUILabel*) gui1->getWidget("ERRORS");
        errors->setVisible(FALSE);            
        
        //ofxUIButton *button = (ofxUIButton *) e.widget;
        //button->setValue(FALSE);
        ofxUIRotarySlider *rotary = (ofxUIRotarySlider *) gui4->getWidget("%");
        rotary->setValue(0.0);        
        ofxUISlider *volume = (ofxUISlider *) gui4->getWidget("VOL");
        volume->setValue(75);
        
        //load the gui
        if (stage==0)   gui1->disable();    
        else    gui3->disable();    
        toggleInstructions1 = FALSE;
        toggleInstructions11 = FALSE;
        toggleInstructions2 = FALSE;
        toggleInstructions3 = FALSE;
        toggleScore = FALSE;
        gui4->enable(); 
        
        played = 0;
    }
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
            passToExit =  xmlSet.getValue("passToExit", "exit");
            if (xmlSet.getValue("verbose", 1)) verbose = TRUE;
            else verbose = FALSE;
            if (xmlSet.getValue("fullscreen", 1)) fullscreen = TRUE;
            else fullscreen = FALSE;
            fps = xmlSet.getValue("fps", 1000);
            midiPort = xmlSet.getValue("midiPort", 0);
            midiChannel = xmlSet.getValue("midiChannel", 10);
            midiNote = xmlSet.getValue("midiNote", 46); 
            noPlays = xmlSet.getValue("noPlays", 2);
            minTaps = xmlSet.getValue("minTaps", 1);
            if (xmlSet.getValue("tapWithTab", 1)) tapWithTab = TRUE;
            else tapWithTab = FALSE;
            if (xmlSet.getValue("tapWithSpace", 1)) tapWithSpace = TRUE;
            else tapWithSpace = FALSE;
            if (xmlSet.getValue("canQuit", 1)) canQuit = TRUE;
            else canQuit = FALSE;
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
            fullscreen = TRUE;
            fps = 1000;
            midiPort = 0;
            midiChannel = 10;
            midiNote = 46;
            noPlays = 2;
            minTaps = 1;
            tapWithSpace = TRUE;
            tapWithTab = TRUE;
            canQuit = TRUE;
            itemDimGUI = 24;
            appToLaunchPath = "";
            launchScript = FALSE;
            scriptDirectory = "";
            isClient = FALSE;
            tcpPort = 9001;
            ipServer = "127.0.0.1";
            passToExit = "exit";
        }
    }
    else {
        verbose = FALSE;
        fullscreen = TRUE;
        fps = 1000;
        midiPort = 0;
        midiChannel = 10;
        midiNote = 46;
        noPlays = 2;
        minTaps = 1;
        tapWithSpace = TRUE;
        tapWithTab = TRUE;
        canQuit = TRUE;
        itemDimGUI = 24;
        appToLaunchPath = "";
        launchScript = FALSE;
        scriptDirectory = "";
        isClient = FALSE;
        tcpPort = 9001;
        ipServer = "127.0.0.1";
        passToExit = "exit";
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
    
    xmlUser.setValue("records", nrUsers);
    xmlUser.setValue("maxID", maxID);
    
    //add all the existing users
    for(int i = 0; i < nrUsers; i++){
        //each position tag represents one user
        xmlUser.addTag( "user" );
        xmlUser.pushTag( "user" , i);
        //set the values 
        xmlUser.setValue("ID", uIDs[i]);
        xmlUser.setValue("name", uNames[i]);
        xmlUser.setValue("fullname", uFullNames[i]);
        xmlUser.setValue("date", uDate[i]);
        //use this too if you wanna use quiz
        //xmlUser.setValue("age", uAge[i]);
        //xmlUser.setValue("gender", uMF[i]);
        //xmlUser.setValue("experience", uYears[i]);
        //xmlUser.setValue("familiarity", uFam[i]);
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
                    uFullNames.push_back(xmlUser.getValue("fullname", ""));
                    uDate.push_back(xmlUser.getValue("date", ""));
                    //use this too if you wanna use quiz
                    //uAge.push_back(xmlUser.getValue("age", 0));
                    //uMF.push_back(xmlUser.getValue("gender", 0));
                    //uYears.push_back(xmlUser.getValue("experience", 0));
                    //uFam.push_back(xmlUser.getValue("familiarity", 0));
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
        xmlTap.addAttribute("song","tryNo", usert.sounds[i].tryNo,i);
        xmlTap.pushTag( "song" , i);
        
        //set the values 
        xmlTap.setValue("fileName",songNames[usert.sounds[i].songID]);
        
        //write the time stamps in a string
        std::vector<int>::const_iterator it;
        text.clear();text.str("");
        //copy(usert.sounds[i].time.begin(), usert.sounds[i].time.end(), ostreambuf_iterator<int>(ss, " "));
        for( it = usert.sounds[i].time1.begin(); it != usert.sounds[i].time1.end(); ++it )
        {
            text << *it << " ";
        }
        xmlTap.setValue("transcription", text.str().c_str());
        text.clear();text.str("");
        for( it = usert.sounds[i].time2.begin(); it != usert.sounds[i].time2.end(); ++it )
        {
            text << *it << " ";
        }
        xmlTap.setValue("transcription2", text.str().c_str());
        
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
            if (usert.getID() != xmlTap.getAttribute("tapping", "userID", 0, 0)) usert.currentSound=0;
            else 
            {
                xmlTap.pushTag("tapping");
                usert.currentSound = xmlTap.getNumTags("song");
                //usert.currentSound=xmlTap.getAttribute("tapping", "currentSound", 0); //or xmlTap.getNumTags("song")
                
                string transcript="";
                int t;
                for(int i = 0; i < xmlTap.getNumTags("song"); i++){     
                    usert.sounds[i].songID = xmlTap.getAttribute("song", "songID", i,  i);   
                    usert.sounds[i].tryNo = xmlTap.getAttribute("song", "tryNo", 1, i);  
                    //xmlTap.getValue("filename", "");
                    
                    xmlTap.pushTag("song", i);
                    //load transcription
                    transcript = xmlTap.getValue("transcription", "");      
                    text.clear();text.str("");
                    text << transcript;
                    while (text >> t)                        
                        usert.sounds[i].time1.push_back(t);
                    
                    transcript = xmlTap.getValue("transcription2", "");      
                    text.clear();text.str("");
                    text << transcript;
                    while (text >> t)                        
                        usert.sounds[i].time2.push_back(t);
                    
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
    
    //WAIT FOR MATLAB SCRIPT TO END
    matlabScript.waitForThread(); 
    
    //ofSleepMillis(2 * 1000);
    float score=0.0;
    int numTapped = 0;
    
    if (usert.currentSound >= numSounds) numTapped = numSounds;
    else numTapped = usert.currentSound + 1;
    text.clear();text.str("");
    text <<  "data/" << usert.getName() << "out.xml";
    //parse xml
    
    ofxXmlSettings xmlTap;    
    
    if(xmlTap.loadFile(text.str()))
    {        
        if (xmlTap.tagExists("results"))
        {  
            xmlTap.pushTag("results");
            //load results
            score = xmlTap.getValue("OverallMean", 0.0);            
            xmlTap.popTag();  
        }
        
    }
    
    insertScore(usert.getID(), usert.getName(), score, numTapped);
    std::sort(highscores.begin(), highscores.end(), compareByLength);
    
    text.clear();text.str("");
    text << "Your tapping score is " << score << ". <> You have tapped " << numTapped << " songs.";    
    text << " <> Your position in the highscore table is " << findName(usert.getID())+1 << "." ;
    if (numTapped < numSounds) text << " <> You can improve your score by resuming tapping at a later time.";
    else text << " <> You tapped all the songs.";
    //needs to be replaced with the actual results
    results.setText(text.str());
    results.wrapTextX(ofGetHeight()-OFX_UI_GLOBAL_WIDGET_SPACING);     
    results.setColor(240, 240, 240, 180);
    
}

//-------UTILS

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

bool testApp::is_number(string s)
{
    std::string::const_iterator it = s.begin();
    while (it != s.end() && std::isdigit(*it)) ++it;
    return !s.empty() && it == s.end();
}


//--------------HIGHSCORES
int testApp::findName(int idToFind)
{
    std::vector<scores>::iterator iter = std::find_if(highscores.begin(), highscores.end(), MyClassComp(idToFind));
    size_t index = std::distance(highscores.begin(), iter);
    if(index == highscores.size())
    {
        return -1;
    }
    else return index;
}

void testApp::insertScore (int u, string i, float s, int n)
{ 
    int exists = findName(u);
    if (exists > -1)
    {
        highscores[exists].uid = u;
        highscores[exists].name = i;
        highscores[exists].score = s;
        highscores[exists].noTapped = n;
    }
    else
    {
        scores uscore;
        uscore.uid = u;
        uscore.name = i;
        uscore.score = s;
        uscore.noTapped = n;
        highscores.push_back(uscore);
    }
}  

void testApp::saveXmlScores(string fileName)
{
    //save user to the xml file
    ofxXmlSettings xmlUser;
    xmlUser.addTag( "scores" );
    xmlUser.pushTag( "scores" );
    
    //add all the existing users
    for(int i = 0; i < highscores.size(); i++){
        //each position tag represents one user
        xmlUser.addTag( "user" );
        xmlUser.pushTag( "user" , i);
        //set the values 
        xmlUser.setValue("id", highscores[i].uid);
        xmlUser.setValue("name", highscores[i].name);
        xmlUser.setValue("score", highscores[i].score);
        xmlUser.setValue("noTapped", highscores[i].noTapped);
        xmlUser.popTag();//pop position        
    } 
    
    xmlUser.popTag();
    xmlUser.saveFile( fileName );
    
}


void testApp::loadXmlScores(string fileName)
{
    ofxXmlSettings xmlUser;
    
    if(xmlUser.loadFile(fileName))
    {
        
        if (xmlUser.tagExists("scores"))
        {
            xmlUser.pushTag("scores");
            
            if (xmlUser.getNumTags("user")>0) 
            {
                //get records from the xml
                for(int i = 0; i < xmlUser.getNumTags("user"); i++){
                    xmlUser.pushTag("user", i);            
                    insertScore(xmlUser.getValue("id",0), xmlUser.getValue("name", ""), xmlUser.getValue("score",0.0), xmlUser.getValue("noTapped", 0));
                    xmlUser.popTag();                        
                }      
                std::sort(highscores.begin(), highscores.end(), compareByLength);
            }
            
            xmlUser.popTag(); //pop position
        }
    }
    
}

void testApp::getScoreTable()
{
    vector<string> tid, tname, tscore , ntoTapped;
    tid.push_back(" <> <> HIGHSCORES <> <> NO."); tname.push_back(" <> <> <> <> ID"); tscore.push_back(" <> <> <> <> SCORE"); ntoTapped.push_back(" <> <> <> <> TAPPED");
    
    int max;
    if (highscores.size() > 10) max = 10;
    else max = highscores.size();
    for(int i = 0; i < highscores.size(); i++){
        tid.push_back(ofToString(i+1));
        tname.push_back(highscores[i].name); 
        tscore.push_back(ofToString(floor(highscores[i].score*100)/100)); 
        ntoTapped.push_back(ofToString(highscores[i].noTapped));
    }
    
    scoreTable1.init("GUI/newmediafett.ttf", 12);
    scoreTable1.setText(ofJoinString(tid, " <> "));
    scoreTable1.wrapTextX(6*itemDimGUI);     
    scoreTable1.setColor(240, 240, 240, 180); 
    
    scoreTable2.init("GUI/newmediafett.ttf", 12);
    scoreTable2.setText(ofJoinString(tname, " <> "));
    scoreTable2.wrapTextX(15*itemDimGUI);     
    scoreTable2.setColor(240, 240, 240, 180);
    
    scoreTable3.init("GUI/newmediafett.ttf", 12);
    scoreTable3.setText(ofJoinString(tscore, " <> "));
    scoreTable3.wrapTextX(15*itemDimGUI);     
    scoreTable3.setColor(240, 240, 240, 180);
    
    scoreTable4.init("GUI/newmediafett.ttf", 12);
    scoreTable4.setText(ofJoinString(ntoTapped, " <> "));
    scoreTable4.wrapTextX(6*itemDimGUI);     
    scoreTable4.setColor(240, 240, 240, 180);
    
}
