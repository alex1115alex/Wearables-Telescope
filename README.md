# Project Telescope

![image](https://user-images.githubusercontent.com/27019702/166100358-2a8c5e24-3cba-4de2-8207-f2f2cb567834.png)

## What's this?

The "Telescope" is a DIY smartglasses system similar to the Google Glass in form and technology. It currently displays the time and your phone's notifications and can also function as a teleprompter.

## Guide to build

### 3D print and assemble the parts in this repository
- Telescope Computer
- - .12 layer height. Place supports ONLY under the bit with the screw hole that sticks out. See my Cura project file for reference.
- Telescope Glasses
- - .12 layer height. No supports
- Telescope Viewfinder
- - .12 layer height. No supports

Use an M4 screw to screw the pieces together.

![image](https://user-images.githubusercontent.com/27019702/166100073-e835d9a8-9390-4659-8f3d-ce69649822cc.png)

### Assemble the electronics

You may need to trim some of the wires coming from the microdisplay, but otherwise, follow my amerture wiring diagram:

![telescope wiring diagram](https://user-images.githubusercontent.com/27019702/166118811-e94d75ac-100d-4b00-a1df-2a38f8fd822b.png)

The TinyPICO has a few ways you can attach a battery (information here: https://www.tinypico.com/gettingstarted#:~:text=POWER%20FROM%20BATTERY). Personally, I soldered the battery's leads to the pads on the back of the device because I accidentally destroyed the BAT pin.

It should look like this when all's said and done:

![image](https://user-images.githubusercontent.com/27019702/166100152-1be64898-3567-424b-a2f1-9e702ae4f003.png)

Lastly, VERY CAREFULLY slot the whole thing into the "Telescope Computer" module:

![image](https://user-images.githubusercontent.com/27019702/166100176-5dd10037-f704-4ce8-812e-7dfde75a1828.png)

### Complete the viewfinder module

If you haven't done this already, remove the microdisplay from its original casing. You will need to temporarily disconnect the ZIF connector attaching the ribbon cable to the display module to do this. Also, remove the original magnifying glass from the casing. 

Before removing the casing:

![image](https://user-images.githubusercontent.com/27019702/166099372-432517a2-381f-477b-8b4a-06c0e811804b.png)

Slide the magnifying glass into the front of the viewfinder you printed, and then glue the beam splitting cube to the "tongue" using your favorite glass glue (make sure to use the same orientation as seen in the completed photo).

Lastly, slide the microdisplay into the slot in the back.

Complete (this picture is from an older version of the device, but the viewfinder is the same):

![image](https://user-images.githubusercontent.com/27019702/166099536-5d1daca7-eb95-48be-b324-0ac1c810783f.png)

### Install the required libraries to the Arduino IDE & upload the code

Libraries:
* ESP_8_BIT_Composite_Video
* Adafruit_GFX
* ArduinoJSON
* TinyPICO

### Install the companion app (Android only) and open it in

The APK is in the repository. Just open the app and it will try to connect to the Telescope automatically.

## Notable parts list

### 960x540 Microdisplay

A .24" 540p microdisplay that would normally be used as a camera's viewfinder or for cheap FPV drone goggles. It only takes a composite video source which limits us in terms of what computers we can use for this thing, but, as luck would have it, the ESP32 (unofficially) supports composite video output.

I got mine on AliExpress (https://www.aliexpress.com/item/4000958970634.html) but you might also be able to get it directly from the manufacturer if you know Chinese. The model is "FLCOS-SON-FL08".

#### Potential money-saving tip:

Although I haven't found many other options, you don't necessarily need to use the SON-FL08. For example, I found another microdisplay by the same company called the "SON-FL02" which is lower resolution (but can be cheaper). The display library we're using only supports 240p anyway so that shouldn't matter. Doing this would require you to modify the "Telescope Viewfinder" model, of course.

### TinyPICO

This is an ESP32 devboard with a few key features:

* Built-in support for battery usage and charging
* Super power-efficient AND has a very stable voltage output
* Freaking tiny

Get one at https://www.tinypico.com/buy

## NOTE: EXTREMELY IMPORTANT! DO NOT SKIP!

Make sure you buy the original TinyPico, which uses a chip (ESP32-PICO-D4) based on the original ESP32. Why? The original ESP32 has an integrated DAC, which is what we use to output a composite video stream to our microdisplay. 

Do not buy a TinyS2! It is based on the ESP32-S2 and is single-core only.
Do not buy a TinyS3! It is based on the ESP32-S3 and does NOT have an integrated DAC.

### Beam splitting cube prism

This is a cube that lets light through in all directions while also reflecting light 90 degrees from a single direction. It's a little hard to explain.

I used a 10mm\*10mm cube, but you might want to consider getting a 15mm\*15mm cube to make the display easier to see while moving.

Mine: https://www.aliexpress.com/item/2255800715557324.html

#### Potential money-saving tip:

I've heard you can use a triangular beamsplitter instead of a cube. It won't look as nice, but it can also save you ~$10.

Link: https://www.aliexpress.com/item/4000032849513.html

### a LiPo battery of your choice

I used a 1100mah one from Amazon, but feel free to get a smaller one (and optionally: modify the "Telescope Computer" model to securely fit it)

## Devblog

## Update 0

This week was all about getting the basics of the device running along with starting to develop the software.

The first matter of order was to get the display working. Using a photo some dude posted in his review of the microdisplay,
I managed to hook it up to my generic ESP32-Wroom. I loaded up the "ESP_8_BIT Composite Video" library's "Hello World" example,
and the display instantly popped up with a pretty slick hello world screen. Aces.


https://user-images.githubusercontent.com/27019702/161512802-bf7a4b23-00ec-401d-ad0f-1953da85d323.mp4


The next step was starting to modify the example code to fit my needs, starting with a basic notification screen, 
complete with a corresponding "Notification" class to keep the organization and passing of data simple.

![image](https://user-images.githubusercontent.com/27019702/161512779-f7b1ff63-247e-486d-b1aa-417b8715e926.png)


Once that was done, I started exploring one of the most important features - sending notifications from my Android phone to the ESP32!
I'll need an app that will run in the background, capture my notifications, and then send them to the ESP32 via Bluetooth. There are a few Flutter libraries for this, so that'll hopefully be easy when I get around to it. 

On the ESP32 side of things, I started setting up Bluetooth connectivity which seemed super easy at first. 
However, things are never as easy as they seem, and I quickly found that the Bluetooth stops working once I start outputting video...

![image](https://user-images.githubusercontent.com/27019702/161512313-b1ec0475-73b7-41b2-8d49-17912f0cc89b.png)

Queue me spending about 8 hours trying to diagnose the problem. I tried using BLE instead of Classic, investigated using a separate Bluetooth module, and even manually separated the Bluetooth and video output tasks to separate cores with every possible configuration. In a last-ditch effort, I tried using my TinyPICO instead... and it paired with my phone perfectly. Turns out, the generic ESP32 just didn't have enough RAM to open a Bluetooth connection and output video at the same time. Goddamnit.

You would think it would be smooth sailing from there, but it turns out that while the TinyPICO has enough memory for Bluetooth pairing, it doesn't have enough to maintain a connection. Fortunately, I found that I could reduce my memory usage by freeing the memory pertaining to BLE. This fixed the problem. Still, that's not a lot of memory to work with. I think I'll have to investigate freeing other unused modules' memory. Anyway, it works! 


https://user-images.githubusercontent.com/27019702/161512519-5b5b7ee5-1f59-4cb5-8c88-aafe5ebc0273.mp4


I spent the rest of my time this week working on the software, which I'm calling TelescopeOS. To do this more efficiently, I ditched the microdisplay for now and started using a composite to USB capture card which lets me view the ESP32's video output on my computer. If that sounds like an extra step for little reason, you should try developing a GUI on a .24inch screen connected with a delicate ribbon cable that - if broken - can't be replaced by the due date.

![image](https://user-images.githubusercontent.com/27019702/161509228-904db5aa-9c5c-4e7a-8513-d0e286be7ed8.png)

Progress on the software is far from over, however, and now that the capture card is setup I'll be working on it over the next few days. 

Software-specific "What I have now" list:
* Home screen
* * Shows the time and the number of unread notifications
* * Has a function for cycling through those notifications starting from the most recent
* * * Triggered via Bluetooth for now, will have a physical button later
* Groundwork for a basic JSON API over Bluetooth, which right now accepts types:
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
* * time (Finish the logic behind this - right now it just uses a dummy value regardless of what is sent via Bluetooth
* * temperature (temperature is stored as a variable but never used. This needs automatic syncing... mostly works for Android app)

## Update 1


https://user-images.githubusercontent.com/27019702/162689365-c6cba215-2a70-41da-a39b-cae65eb1acbe.mp4


I was able to make a lot of progress on the software this week. It's not exactly everything I wanted to do, but I took care of the difficult problems meaning it should be somewhat easy to add the remaining features sometime down the line. As such, from now on I'll be working on the hardware.

Picking up from last week, I started doing real work on the app. Here were the MVP requirements along with some notes on each: 

### On boot, pair with the ESP32 via Bluetooth

* I set the app up so that it starts scanning for Bluetooth on boot and tries to connect to any device with the name "ESP32test". 

### Automatically reconnect to the ESP32 if the connection is lost

* The pairing function just reruns itself whenever there isn't a current connection. This is a really ugly solution but works provided you don't reload the code in Flutter, which isn't something I'm sure a normal user (me) would be able to reproduce with a release build.
* * On the ESP32 side of things, I found the ESP32-PICO-D4 chip (the one the TinyPICO runs on) isn't able to gracefully accept new connections after one is dropped, or at least there isn't an easy way to do this. As such, I went the "lovably janky" route and just have the ESP32 do a full reset whenever the Bluetooth connection is lost.

* I also developed a message queueing system. The rundown of how this works:
* * When a message is sent (such as a time/weather sync, notification, etc), the connection status is checked.
* * * If the connection is good, the message is sent. Easy.
* * * If the connection isn't good, the message is added to a queue. The queue (currently) stores up to 10 messages, after which the oldest is removed when a new message is added.
* * * * The contents of the queue are sent to the ESP32 the next time it connects

### On boot (and periodically) send the current time + outdoor temperature to the ESP32 

* This is pretty self-explanatory (and simple on my end!), but if you're interested, I merged time and temperature into a single "sync" API call.

* There was a bit of a struggle here in that the ESP32's real-time clock isn't exactly very accurate... especially when you have a constant composite video stream AND a Bluetooth connection messing with the timings and whatnot. To be honest I don't 100% know if those two things are messing with it - I'm not a hardware guy - so I "fixed" this by just syncing every 10 minutes. 
* * I haven't gotten much of a chance to see how this affects my phone's battery life, but I can't imagine it'll be pretty.

### Listen for relevant notifications and send them to the ESP32

* This was *mostly* straightforward, with a couple exceptions:
* * I needed to make a blacklist for app package names because some apps spam notifications. The winner on my shitlist here is "com.samsung.android.app.smartcapture" which sends a new notification every second to remind you that you're recording your screen.

![image](https://user-images.githubusercontent.com/27019702/162689987-94b97060-01d0-45dd-826d-0825fec3e015.png)

* * I encountered a bug in the "notifications" flutter package which caused the app to crash upon receiving a GMail notification. The fix required I do something absolutely disgusting: fork a dependency to edit a single line in a file.

### Theme support

* I made all the colors pull from a "Theme" object which includes a textColor, backgroundColorPrimary, backgroundColorSecondary, and flair. 
* Premade themes: basic light, basic dark, cyberpunk
* I am horrible with matching colors, especially when limited to an 8bit color set that doesn't even display correctly

![image](https://user-images.githubusercontent.com/27019702/162690140-6ae9ee4c-d482-4384-990c-7257de7a5384.png)

## Let's talk hardware

As I said, the next and final week will be all about the hardware. I haven't gotten started on any concrete designs, but it's been a topic on my mind for the last while, so I'll share some thoughts:

* The original plan was a 3Dprinted pair of glasses or sunglasses with a custom arm. 
* * Benefits: Can distribute tech/battery between the arms of the glasses. Beamsplitter built into the lens.
* * Cons: Incompatible with prescription lenses. Will look like a goober.

* The new plan is a clip-on system like with my first smart glasses project, albeit with some sort of system to keep it neatly fixed to my current frames.
* * I'm debating taking the magnifying housing off the microdisplay (you can see what I mean in the first few videos) and making my own custom one. 
* * * Pros: The whole package would be much smaller (especially important when it's next to your eye).
* * * Cons: Would be some extra effort, but more importantly would be extremely risky: I'd need to unclip the ZIF connector on the microdisplay which I've never done before. If I mess up, I can't get another in time for the due date.
* Button
* * For the button to cycle notifications, my job's already simplified by the fact that the ESP32's GPIOs have built-in pull-up resistors.
* * I'm exploring button types:
* * * Touch-sensitive button using a nail (or similar) - would be cool but no tactile feedback
* * * Cherry MX Brown switch - I have some on-hand and the tactile feedback would be hilarious. Might be too bulky though.

## Update 2

I know I wanted to focus exclusively on hardware, but this last week ended up being something of a mix. There were mainly some bugs with the software I needed to fix before being happy with my work, but I also needed to add software support for the physical "next" button, as well as support for displaying a vehicle's speed/RPM data! Such data will need to be pulled from a car's ODBII port via Bluetooth.

That Bluetooth api works like so: {"type":"car","speed":"88","rpm":"5500"}

In terms of hardware, here's what I worked on in no specific order:

### Power/battery switch

McGuckin's didn't have a small enough power switch for me, so I ended up ripping a tiny one out of a AA battery holder I had laying around. The only downside of my current design is that the switch needs to be "ON" in order for the battery to charge, but that's something I can tolerate for now.

It's also worth noting that the display gets messed up whenever we switch from USB -> Battery power, or vice versa. To counteract this, I just wrote some code to monitor whether the USB port is plugged in, and have the ESP32 reset itself whenever the USB state changes. Not a super elegant solution, but neither is anything in this entire repo.

![2022-04-18_07-18](https://user-images.githubusercontent.com/27019702/163813879-b5082a47-ce19-476b-99d5-5b495163218b.png)

### Physical next button

I decided to go with a Cherry MX Brown key switch for this. Nothing much interesting to say here. It's a button. You press it and ba-da-bing ba-da-boom it does a thing.


https://user-images.githubusercontent.com/27019702/163814153-a92c5231-75ce-44cb-83cb-9cef17440e57.mp4


### The shell

I spent a whole lot of time (the past 48 hours) doing the following:

1. Design model of eyepiece/electronics case
2. Test sizing with real hardware
3. Identify issues, and go back to step 1.

I'll save you the time reading about all my hassles with this as I've pretty much summed up the gist above. I'll have the .STL files in this repo if you're interested.

The one other thing I want to say here is that I'm not entirely happy with the current design, and that's almost entirely due to the battery I have (which is very large and rectangular). I didn't want to drop more cash on a better fitting battery so it'll do for now, however, when I pick up this project again over the summer the first thing I'm doing is swapping out the battery with something longer and thinner.

![2022-04-18_07-20](https://user-images.githubusercontent.com/27019702/163814092-32551544-15e1-476d-9454-88733ab4ef7b.png)

### Trimming everything down & getting the hardware into the shell

The shell is *almost* there. Remaining problems:
* Doesn't fit the key switch properly
* * The hole needs minor resizing + relocating
* No place for the power switch
* * It's just crammed in for now
* Doesn't have a top cover
* Display driver doesn't sit securely
* * Minor sizing adjustments needed

This was pretty straightforward but also incredibly obnoxious. To keep print times within 3 hours I needed to keep the walls of the shell at a mere 1mm thick, and as a result, the shells had a tendency to bend and crack and split. When I come back to this project I'll need to do some upgrades to the shell, because the "print/wait3hours/test" iteration cycle really limited me in terms of how much I could accomplish within a week.

![top](https://user-images.githubusercontent.com/27019702/163814189-ac4d822d-6f6f-43ee-8975-133b26bfbff0.jpg)

## Update 3

To finish everything off, this week consisted of a major shell overhaul, adding some buttons to the app, and (FINALLY) horizontally flipping the display.

...I have spent the past 8 hours flipping this display. 

### Some things I've learned, and problems I encountered:

* The Adafruit_GFX library has no built-in way to horizontally flip the output, opting to have that feature added by the display driver's library
* * ESP_8_BIT_Composite_Video (my display library) does NOT have this feature
* Adafruit_GFX draws things like lines/shapes/etc using the drawPixel function
* It ALSO draws text using the drawPixel function*

From reading that, you would think we could just reverse the x coordinates in the drawPixel function, right? WRONG! As it turns out, the drawPixel function is ONLY used for text using the default font + size, which is far too small to be readable. Plus, being stuck on 1 font size would be awful.

If you dig into the Adafruit_GFX library, non-default text is drawn to the screen directly in the drawChar function, which basically means that you get some funky effects when trying to reverse the x coordinates; you'll still get the normal characters, just printed in reverse. As such, I went online and found a horizontally mirrored Sans font, and converted that to a format the Adafruit_GFX library could read. The mirrored font looks fine before flipping the pixels but caused some graphical errors once flipped.

So, I scrapped the flipping idea and went with something even hackier: make a wrapper for the videoOut.print(text) function that reverses the string before calling print, AND handles text wrapping in reverse - manually. In other words, screw screen mirroring - I'll just print everything backward with a reversed font.

## The shell

The new shell is awesome. It's basically the original except made to be as small as possible (form fit to the parts), and built around a 3D model of the original Google Glass frame. It really gets close to the original Google Glass!

It comes in two pieces this time that can be screwed together - the "Computer" and "Glasses". This is a really awesome feature as it will allow me (and you!) to design your own "Glasses"... just use the same connector!

![image](https://user-images.githubusercontent.com/27019702/166119307-99749c45-14c4-44c5-9cc1-54dd899dd635.png)

## Update 4 (the PICO update - ba dum tss)

I was planning on using the Telescope for a different class's project which required some improvements. Here they are in no particular order.

### Theme now defaults to the light theme

Much to my chagrin, the cyberpunk theme just wasn't it.

### Improved text drawing code to reduce weird errors

I fixed an issue where the first character of a multi-line string would print on its own line. I also made everything print left-to-right (finally!) by padding partial lines with space characters.

### JSON error detection

TelescopeOS now makes sure incoming JSON is valid before trying to use it, saving us from crashes. In the event of invalid JSON, the user is alerted using the new "System Popup" screen.

### Improvements + comments on how different screens work

The "next" button now returns the user to the home screen when they're on the Teleprompter, Car, or the System Popup screen.

### Teleprompter screen improvements

I made the Teleprompter screen its own distinct thing, as opposed to just a wrapper for "drawMiddle()" (which now no longer exists).

### Random dumb drink name generation function in the app

I wanted to use the Telescope for another project, so I added a function to the app (which also works as a demo for the Teleprompter function) that generates a dumb fake drink name and sends it to the device. Drink names range from "incredibly dumb" to "incredibly inappropriate". If someone ever uses this, have fun :D
