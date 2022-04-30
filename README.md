# Project Telescope

## What's this?

The "Telescope" is a DIY smartglass system similar to the Google Glass in form and technology.

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

## UPDATE #1


https://user-images.githubusercontent.com/27019702/162689365-c6cba215-2a70-41da-a39b-cae65eb1acbe.mp4


I was able to make a lot of progress on the software this week. It's not exactly everything I wanted to do, but I took care of the difficult problems meaning it should be somewhat easy to add the remaining features sometime down the line. As such, from now on I'll be working on the hardware.

Picking up from last week, I started doing real work on the app. Here were the MVP requirements along with some notes on each: 

### On boot, pair with the ESP32 via bluetooth

* I set the app up so that it starts scanning for bluetooth on boot and tries to connect to any device with the name "ESP32test". 

### Automatically reconnect to the ESP32 if connection is lost

* The pairing function just reruns itself whenever there isn't a current connection. This is a really ugly solution, but works provided you don't reload the code in Flutter, which isn't something I'm sure a normal user (me) would be able to reproduce with a release build.
* * On the ESP32 side of things, I found the ESP32-PICO-D4 chip (the one the TinyPICO runs on) isn't able to gracefully accept new connections after one is dropped, or at least there isn't an easy way to do this. As such, I went the "lovably janky" route and just have the ESP32 do a full reset whenever the bluetooth connection is lost.

* I also developed a message queueing system. The rundown of how this works:
* * When a message is sent (such as a time/weather sync, notification, etc), the connection status is checked.
* * * If the connection is good, the message is sent. Easy.
* * * If the connection isn't good, the message is added to a queue. The queue (currently) stores up to 10 messages, after which the oldest is removed when a new message is added.
* * * * The contents of the queue are sent to the ESP32 the next time it connects

### On boot (and periodically) send the current time + outdoor temperature to the ESP32 

* This is pretty self explanatory (and simple on my end!), but if you're interested, I merged time and temperature into a single "sync" api call.

* There was a bit of a struggle here in that the ESP32's real-time clock isn't exactly very accurate... especially when you have a contant composite video stream AND a bluetooth connection messing with the timings and whatnot. To be honest I don't 100% know if those two things are messing with it - I'm not a hardware guy - so I "fixed" this by just syncing every 10 minutes. 
* * I haven't gotten much of a chance to see how this affects my phone's battery life, but I can't imagine it'll be pretty.

### Listen for relevant notifications and send them to the ESP32

* This was *mostly* straightforward, with a couple exceptions:
* * I needed to make a blacklist for app packagenames because some apps spam notifications. The winner on my shitlist here is "com.samsung.android.app.smartcapture" which sends a new notification every second to remind you that you're recording your screen.

![image](https://user-images.githubusercontent.com/27019702/162689987-94b97060-01d0-45dd-826d-0825fec3e015.png)

* * I encountered a bug in the "notifications" flutter package which caused the app to crash upon receiving a GMail notification. The fix required I do something absolutely disgusting: fork a dependency to edit a single line in a file.

### Theme support

* I made all the colors pull from a "Theme" object which includes a textColor, backgroundColorPrimary, backgroundColorSecondary, and flair. 
* Premade themes: basic light, basic dark, cyberpunk
* I am horrible with matching colors especially when limited to an 8bit color set that doesn't even display correctly

![image](https://user-images.githubusercontent.com/27019702/162690140-6ae9ee4c-d482-4384-990c-7257de7a5384.png)

## Let's talk hardware

As I said, the next and final week will be all about the hardware. I haven't gotten started on any concrete designs, but it's been a topic on my mind for the last while, so I'll share some thoughts:

* The original plan was a 3Dprinted pair of glasses, or sunglasses with a custom arm. 
* * Benefits: Can distribute tech/battery between the arms of the glasses. Beamsplitter built into the lens.
* * Cons: Incompatible with prescription lenses. Will look like a goober.

* The new plan is a clip-on system like with my first smartglasses project, albeit with some sort of system to keep it neatly fixed to my current frames.
* * I'm debating taking the magnifying housing off the microdisplay (you can see what I mean in the first few videos) and making my own custom one. 
* * * Pros: The whole package would be much smaller (especially important when it's next to your eye).
* * * Cons: Would be some extra effort, but more importantly would be extremely risky: I'd need to unclip the ZIF connector on the microdisplay which I've never done before. If I mess up, I can't get another in time for the due date.
* Button
* * For the button to cycle notifications, my job's already simplified by the fact that the ESP32's GPIOs have built-in pullup resistors.
* * I'm exploring button types:
* * * Touch sensitive button using a nail (or similar) - would be cool but no tactile feedback
* * * Cherry MX Brown switch - I have some on-hand and the tactile feedback would be hilarious. Might be too bulky though.

## UPDATE #2

I know I wanted to focus exclusively on hardware, but this last week ended up being something of a mix. There were mainly some bugs with the software I needed to fix before being happy with my work, but I also needed to add software support for the physical "next" button, as well as support for displaying a vehicle's speed/RPM data! Such data will need to be pulled from a car's ODBII port via Bluetooth.

That Bluetooth api works like so: {"type":"car","speed":"88","rpm":"5500"}

In terms of hardware, here's what I worked on in no specific order:

### Power/battery switch

McGuckin's didn't have a small enough power switch for me, so I ended up ripping a tiny one out of a AA battery holder I had laying around. The only downside of my current design is that the switch needs to be "ON" in order for the battery to charge, but that's something I can tolerate for now.

It's also worth noting that the display gets messed up whenever we switch from USB -> Battery power, or vice versa. To counteract this, I just wrote some code to monitor weather the USB port is plugged in, and have the ESP32 reset itself whenever the USB state changes. Not a super elegant solution, but neither is anything in this entire repo.

![2022-04-18_07-18](https://user-images.githubusercontent.com/27019702/163813879-b5082a47-ce19-476b-99d5-5b495163218b.png)

### Physical next button

I decided to go with a Cherry MX Brown keyswitch for this. Nothing much interesting to say here. It's a button. You press it and ba-da-bing ba-da-boom it does a thing.


https://user-images.githubusercontent.com/27019702/163814153-a92c5231-75ce-44cb-83cb-9cef17440e57.mp4


### The shell

I spent a whole lot of time (the past 48 hours) doing the following:

1. Design model of eyepiece/electronics case
2. Test sizing with real hardware
3. Identify issues, go back to step 1.

I'll save you the time reading about all my hassles with this as I've pretty much summed up the gist above. I'll have the .STL files in this repo if you're interested.

The one other thing I want to say here is that I'm not entirely happy with the current design, and that's almost entirely due to the battery I have (which is very large and rectangular). I didn't want to drop more cash on a better fitting battery so it'll do for now, however, when I pick up this project again over the summer the first thing I'm doing is swapping out the battery with something longer and thinner.

![2022-04-18_07-20](https://user-images.githubusercontent.com/27019702/163814092-32551544-15e1-476d-9454-88733ab4ef7b.png)

### Trimming everything down & getting the hardware into the shell

The shell is *almost* there. Remaining problems:
* Doesn't fit the keyswitch properly
* * The hole needs minor resizing + relocating
* No place for the power switch
* * It's just crammed in for now
* Doesn't have a top cover
* Display driver doesn't sit securely
* * Minor sizing adjustments needed

This was pretty straightforward but also incredibly obnoxious. In order to keep print times within 3 hours I needed to keep the walls of the shell at a mere 1mm thick, and as a result the shells had a tendency to bend and crack and split. When I come back to this project I'll need to do some upgrades to the shell, because the "print/wait3hours/test" iteration cycle really limited me in terms of how much I could accomplish within a week.

![top](https://user-images.githubusercontent.com/27019702/163814189-ac4d822d-6f6f-43ee-8975-133b26bfbff0.jpg)

## Update #3

To finish everything off, this week consisted of a major shell overhaul, adding some buttons to the app, and (FINALLY) horizontally flipping the display.

...I have spent the past 8 hours flipping this display. 

### Some things I've learned, and problems I encountered:

* The Adafruit_GFX library has no built-in way to horizontally flip the output, opting to have that feature added by the display driver's library
* * ESP_8_BIT_Composite_Video (my display library) does NOT have this feature
* Adafruit_GFX draws things like lines/shapes/etc using the drawPixel function
* It ALSO draws text using the drawPixel function*

From reading that, you would thing we could just reverse the x coordinates in the drawPixel function, right? WRONG! As it turns out, the drawPixel function is ONLY used for text using the default font + size, which is far too small to be readable. Plus, being stuck on 1 font size would be awful.

If you dig into the Adafruit_GFX library, non-default text is drawn to the screen directly in the drawChar function, which basically means that you get some funky effects when trying to reverse the x coordinates; you'll still get the normal characaters, just printed in reverse. As such, I went online and found a horizontally mirrored Sans font, and converted that to a format the Adafruit_GFX library could read. The mirrored font looks fine before flipping the pixels, but caused some graphical errors once flipped.

So, I scrapped the flipping idea and went with something even more hacky: make a wrapper for the videoOut.print(text) function that reverses the string before calling print, AND handles text wrapping in reverse - manually. In other words, screw screen mirroring - I'll just print everything backwards with a reversed font.

## The shell

The new shell is awesome 90% of the time. It's basically the original except made to be as small as possible (form fit to the parts), and built around a 3D model of the original Google Glass frame. Unfortunately, the piece that holds the glasses on your nose (I'll call it the "nose stick") broke off in class which prompted me to make some adjustments afterwards. First and obviously - reenforcing the nose sticks - which was stupidly easy. 

The other problem was the device being all one piece. In the original new shell, if anything broke, the entire thing (besides the viewfinder) would need to be reprinted. Now, the viewfinder, computer module, and glasses are three separate parts held together by the M4 screw. Awesome! 
