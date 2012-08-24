#ifndef _THREADED_OBJECT
#define _THREADED_OBJECT

#include "ofMain.h"

// this is not a very exciting example yet
// but ofThread provides the basis for ofNetwork and other
// operations that require threading.
//
// please be careful - threading problems are notoriously hard
// to debug and working with threads can be quite difficult


class threadedObject : public ofThread{

	public:


    int count;      // threaded fucntions that share data need to use lock (mutex)
	string path;    // and unlock in order to write to that data
	                // otherwise it's possible to get crashes.
	                //
	                // also no opengl specific stuff will work in a thread...
	                // threads can't create textures, or draw stuff on the screen
	                // since opengl is single thread safe

		//--------------------------
		threadedObject(){
           path ="";
           count = 0;
		}

		void start(){
            startThread(true, false);   // blocking, verbose
        }

        void stop(){            
            stopThread();
            count = 0;
        }

		//--------------------------
		void threadedFunction(){

			while( isThreadRunning() != 0 ){
                if (count==0)
				if( lock() ){
                    count++;
                    system(path.c_str());
					cout << " THREAD START " << count << endl;
					unlock();
					ofSleepMillis(1 * 1000);
                    //stop();
				}
			}
		}
		

};

#endif
