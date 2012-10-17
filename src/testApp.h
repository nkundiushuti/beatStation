#pragma once

#include "ofMain.h"
#include "ofxMidi.h"
#include "ofxTextSuite.h"
#include "ofxUI.h"
#include "ofxDirList.h"
#include "ofxNetwork.h"
#include "threadedObject.h"
#include "Usert.h"
#include <sstream>
#include <vector>
using namespace std;



class testApp : public ofBaseApp, public ofxMidiListener {
	
public:
	
	void setup();
	void update();
	void draw();
	void exit();
	
	void keyPressed(int key);
	void keyReleased(int key);
	
	void mouseMoved(int x, int y );
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased();
    
    
    //SETTINGS
    string passToExit;
    int minTaps;
    int fps;
    bool verbose;
    bool fullscreen;
    int midiPort;
    int midiChannel;
    int midiNote;
    int noPlays;
    bool launchScript;
    string scriptDirectory;
    string appToLaunchPath;    
    bool tapWithSpace;
    bool tapWithTab;
    bool canQuit;
    int itemDimGUI;
    bool isClient;
    int tcpPort;
    string ipServer;
    string instrGUI1, instrGUI11, instrGUI2;
    ofxTextBlock instructions1, instructions11, instructions2, results, scoreTable1, scoreTable2, scoreTable3, scoreTable4;    
    bool toggleInstructions1, toggleInstructions11, toggleInstructions2,toggleInstructions3, toggleResults, toggleScore;
    stringstream text;   
    
    void loadXmlSettings(string fileName);
	
    
    //MIDI
    int tempTime;
	void newMidiMessage(ofxMidiMessage& eventArgs);		
	ofxMidiIn midiIn;
	ofxMidiMessage midiMessage;
    
    
    //SOUNDS
    ofxDirList   DIR;    
    ofSoundPlayer  beats;
    int numSounds;
    string* songNames;
    int played;
    bool play;
    
    
    //GUI    
    ofxUICanvas *gui1,*gui2,*gui3, *gui4, *gui5;
    ofImage* img; 
    
    void guiEvent1(ofxUIEventArgs &e);
    void guiEvent2(ofxUIEventArgs &e);
    void guiEvent3(ofxUIEventArgs &e); 
    void guiEvent4(ofxUIEventArgs &e); 
    void guiEvent5(ofxUIEventArgs &e); 
    void loadTapping(int stage);
    
    
    //USER
    Usert usert;  
    vector<int>uIDs;
    vector<string>uNames;
    vector<string>uFullNames;
    vector<string>uDate;
    //vector<int>uMF;
    //vector<int>uAge;
    //vector<int>uYears;
    //vector<int>uFam;
    string tempName;
    string tempFullName;
    string tempDate;
    //int tempAge;
    //int tempMF;
    //int tempYears;
    //int tempFam;
    bool newUser;
    bool fromStart;
    int nrUsers;
    int maxID;
    
    void saveXmlUser(string fileName);        
    void loadXmlUser(string fileName);
    void saveXmlTap(string fileName);        
    void loadXmlTap(string fileName);  
    
    int checkUser(string initials);
    int getUserID(string uName);
    void array_union(int* arr, string* sarr, int n);
    void randomOrder(int uid);  
    
    
    //MATLAB
    threadedObject	matlabScript;
    bool askResults;
    void callScript();  
    void loadXmlResults();
    
    
    //TCP CLIENT/SERVER
    ofxTCPClient tcpClient;
    ofxTCPServer tcpServer;    
    bool weConnected, sendToServer;
    int deltaTime, connectTime;	
    
    //UTILS
    string password;
    bool is_number(string s);
    
    
    //HIGHSCORES
    struct scores{
        int uid;
        string name;
        float score;
        int noTapped;
    };    
    struct MyClassComp
    {
        explicit MyClassComp(int i) { n = i;}
        inline bool operator()(const scores & m) const { return m.uid == n; }
        int n;
    };    
    vector<scores> highscores; 
    void saveXmlScores(string fileName);        
    void loadXmlScores(string fileName);
    void getScoreTable();
    int findName(int idToFind);    
    void insertScore (int u, string i, float s, int n);    
    static bool compareByLength(const scores &a, const scores &b)
    {
        if (a.score == b.score)
            return (a.noTapped > b.noTapped);
        else return a.score > b.score;   
    }
     
};
