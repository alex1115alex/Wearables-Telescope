# Project Telescope

## Notable parts list

1. 960x540 Microdisplay

This thing is pretty nifty. It's a .24" 540p microdisplay that was originally intended to be used in cheap budget FPV goggles for flying drones. Outside of being AWESOME, it has one major downside: it only takes a composite video source. Yup. The analouge kind. That unforunately really limits us in terms of what computers we can use for this thing, but, as luck would have it, the ESP32 (unofficially) supports composite video output. Awesome!

2. TinyPICO

This is an ESP32 derivative with a few key features that we'll need for this project:

* It has built-in support for battery usage and charging
* It's super power efficient
* It's freaking tiny

3. Beamsplitting cube prism

This is a cube that reflects light at 90 degrees. I'm not a physics major so I don't really know how it works past that, but the main point is that we're using it as the viewfinder. 

## Progress...

This week was all about getting the basics of the device running along with starting to develop the software.

First thing was first - get the display working. Using a photo some dude posted in his review of the microdisplay,
I managed to hook it up to my generic ESP32-Wroom. I loaded up the "ESP_8_BIT Composite Video" library's "Hello World" example,
and the display instantly popped up with a pretty slick hello world screen. Aces.


https://user-images.githubusercontent.com/27019702/161512802-bf7a4b23-00ec-401d-ad0f-1953da85d323.mp4


Next step was starting to modify the example code to fit my needs, starting with a basic notification screen, 
complete with a corresponding "Notification" class to keep the organization and passing of data simple.

![image](https://user-images.githubusercontent.com/27019702/161512779-f7b1ff63-247e-486d-b1aa-417b8715e926.png)


Once that was done, I started exploring one of the most important features - sending notifications from my Android phone to the ESP32!
I'll need an app that will run in the background, capture my notifications, and then send them to ESP32 via Bluetooth. Speaking of which, there are some relevant Flutter libraries for this, so that'll hopefully be easy when I get around to it. 

On the ESP32 side of things, I started setting up Bluetooth connectivity, which seemed super easy at first. 
However, things are never as easy as they seem, and I quickly found that the Bluetooth stops working once I start outputting video....

![image](https://user-images.githubusercontent.com/27019702/161512313-b1ec0475-73b7-41b2-8d49-17912f0cc89b.png)

Queue me spending about 8 hours trying to diagnose the problem. I tried using BLE instead of Classic, investigated using a separate bluetooth module, and even manually separated the bluetooth and video output tasks to separate cores with every possible configuration. In a last-ditch effort, I tried using my TinyPICO instead... and it paired with my phone perfectly. Turns out, the generic ESP32 just didn't have enough RAM to pair with bluetooth. Goddamnit.

You would think it would be smooth sailing from there, but it turns out that while the TinyPICO has enough memory for bluetooth pairing, it doesn't have enough to maintain a connection. Fortunately, I found that I could reduce my memory usage by freeing the memory pertaining to BLE. This fixed the problem. Still though, that's not a lot of memory to work with. I think I'll have to investigate freeing other unused module's memory. Anyway, it works! 


https://user-images.githubusercontent.com/27019702/161512519-5b5b7ee5-1f59-4cb5-8c88-aafe5ebc0273.mp4


I spent the rest of my time this week working on the software, which I'm calling TelescopeOS for now... very original. In order to do this more efficiently, I ditched the microdisplay for now and started using a composite to USB capture card which lets me view the ESP32's video output on my computer. If that sounds like an extra step for little reason, you should try developing a GUI on a .24inch screen connected with a delicate ribbon cable that can't be replaced by April 29th.

![image](https://user-images.githubusercontent.com/27019702/161509228-904db5aa-9c5c-4e7a-8513-d0e286be7ed8.png)

Progress on the software is far from over, however, and now that the capture card is set up I'll be working on it the next few days. 

Software-specific "What I have now" list:
* Home screen
* * Shows the time and the number of unread notifications
* * Has a function for cycling through those notifications starting from the most recent
* * * Triggered via bluetooth for now, will have a physical button later
* Groundwork for a basic json API over bluetooth, which right now accepts types:
* * "notif" (notification)
* * "time" (for time synchronization w/ phone)
* * "temperature" (will be used to display the weather)
* * "next" (used to cycle notifications & includes option to dismiss).



https://user-images.githubusercontent.com/27019702/161516270-00c9a139-ab8d-40a9-a35c-806e2149c768.mp4



As for a SOFTWARE-SPECIFIC TODO list, here's how things are looking:
* Theme support (with basic light, basic dark, and cyberpunk)
* "Apps"
* * At minimum I need a teleprompter app
* Home screen
* * Display the next direction from Google Maps notifications
* Android app (have done some research + minor work)
* Finish the following API calls
* * time (Finish the logic behind this - right now it just uses a dummy value regardless of what is sent via bluetooth
* * temperature (temperature is stored as a variable but never used. This needs automatic syncing... mostly work for Android app)
