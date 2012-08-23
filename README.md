beatStation
===========

beatStation - an openFrameworks tapping recorder interface 

The app has an user management system and a tapping recording system.

USER MANAGEMENT
- recorded in the data/users.xml folder
- each new user is required to enter the initials, then a new ID is generated based on those initials(ex: aa1 - the first user with the aa initials), plus an unique index id for each user. 
- an existing user can log-in using the ID generated based on initials

TAP RECORDING
- once logged an user can listen to a song with PLAY, move to the next song with NEXT
- tappings are recorded in a vector for each song
- an user can always quit pressing QUIT button
- tappings are saved in an xml named with the user's ID each time an user hits quit or he finishes the tapping
- instructions are displayed with the INSTRUCTIONS button
- volume can be adjusted with the VOL slider
- the percentage played from a song can be seen in the rotary slider POS

