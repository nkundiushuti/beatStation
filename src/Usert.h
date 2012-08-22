//
//  Usert.h
//  soundPlayerExample
//
//  Created by Marius on 8/13/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef soundPlayerExample_Usert_h
#define soundPlayerExample_Usert_h



#endif

class Transcription {
public:
    
    Transcription(){};
    
    int	songID;
    vector<int> time;
    
    
};

class Usert {
private:
    string name; //initials
    int ID; //unique id - index   
        
public:
    
    Usert(){};
    
    ~Usert() { if ((currentSound > 0) || (ID > -1)) delete[] sounds; }
        
    void setName (string uname){ name = uname; }
    string getName (){ return name; }
    void setID (int uID){ ID = uID; }
    int getID (){ return ID;}
    void initTranscription (int n){ sounds = new Transcription[n]; } 
    void deleteTranscription (){ delete[] sounds; }
  
    int currentSound;  
    Transcription* sounds;    

    
    
};
