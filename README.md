# AM2Discord

Apple Music Rich Presence for Discord Application. 
Written in C with the help of AppleScript. 
Only for MacOS right now.

Download the project, the only information that needs to be added in order to work is the client ID, large image name & text, and small image name & text. If you open the app in xcode and go to the ipc.c file the comments will be very clear what needs to be set. You will need to make a Discord App in the discord dev portal to use this. It's not super difficult. Just google it to find out. As for Icons to use either use the ones I provided in this repo or make your own. As for the iTunes logo you can find it on google easily. 

Then go to the product menu then archive, then click distribute app, and then choose copy app, it'll build it into an .app file. Take the file and throw it into your applications folder. Note this does need access to applevents to work.

I may make a precompiled .app version at some point but I have to make a simple way to allow users to add the client id and other information for the app to work. 

Also please ignore my terrible app name and icon. I couldn't really think of anything else and didn't feel like spending any longer on this to come up with a name and design and icon.
