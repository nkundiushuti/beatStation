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
    int tryNo;
    vector<int> time1;
    vector<int> time2;
    
};

class Usert {
private:
    string name; //initials
    int ID; //unique id - index  
    string fullName;
    string date;
        
public:
    
    Usert(){};
     
    ~Usert() { if ((currentSound > 0) || (ID > -1)) delete[] sounds; }
        
    void setName (string uname){ name = uname; }
    string getName (){ return name; }
    void setFullName (string ufname){ fullName = ufname; }
    string getFullName (){ return fullName; }
    void setDate (string udate){ date = udate; }
    string getDate (){ return date; }
    void setID (int uID){ ID = uID; }
    int getID (){ return ID;}
    void initTranscription (int n){ sounds = new Transcription[n]; } 
    void deleteTranscription (){ delete[] sounds; }
  
    int currentSound;  
    Transcription* sounds;    

    
    
};
