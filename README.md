# Imagine:
### Presentations where the speaker is free to walk around the stage, use their hands to freely emphasize their speech, and bring passionate, emphatic body language to their work.

## Introducing Air-Pen
### Air Pen is a new take on the computer mouse - **A pen-style pointing device that works in midair.**
- Moving your cursor, clicking, and inputting keystrokes made possible from anywhere in the room.  
- The small form factor and long battery life of the Air Pen make it easy and unobtrusive for presenters to carry the device with them while working.
  
![Screenshot 2023-08-29 at 1 43 28 PM](https://github.com/Trevin-Small/Air-Pen/assets/47619990/f2cd6f70-c243-40bf-9244-d1a370475ce8)


# Say goodbye to dull presentations.
- Presenters tethered to their computers may as well play an automatic slideshow. It's boring!

# Say hello to dynamic, interactive presentations!
- Emphatic body language, hand expressing, and freedom of motion are possible with Air Pen!

## How does it work?
Air Pen is a device based on Espressif's ESP32-S3 SoC that utilizes the built-in BLE (Bluetooth Low Energy) to communicate with your computer/laptop/iPad/etc. and act as a Mouse input device.

Embedded in the front end of the Air Pen is a camera with a special long-pass filter that allows only IR light in the 850nm range to enter the image sensor.  

Some clever image processing and up-sampling allows for fast, accurate tracking of light sources.  

With Air Pen's companion IR lighthouse placed near your presentation screen, the Pen is able to track its location anywhere in the room, and therefore convert your movements into mouse movements!

## Product Timeline
### July 22nd - First Working Prototype
- Built with an ESP32 Mini, an old Wiimote, a portable charger, a ruler and scotch tape!

### August 15th - Second Working Prototype
- Built with an ESP32-S3 SoC and an OV2460 Image Sensor
- Custom image processing algorithms and a higher FOV image sensor increased the speed and usability of the device.

### August 27th - Third Prototype
- Integrated Rechargable battery allows for freedom of motion anywhere in the room
- Improved motion-filtering makes cursor control easier
- Working distance up to 15 feet

### September ??th - Fourth Prototype
- Integrated Long-Pass 850nm IR filter will remove the noise of ambient light improving tracking accuracy
- Anti-reflective lens coating should reduce "double-vision" - false detections of IR light from internal reflection
