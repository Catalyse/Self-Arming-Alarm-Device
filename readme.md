# Self Arming Alarm Device
- This is the code for an arduino based device that functions as a self arming alarm system.-

## Details
Group 6
Paul Brown, Taylor May
VP Nguyen
CSCI 3511
December 7, 2016

Proximity Alarm

Introduction

	Our group decided to build a proximity alarm as our embedded systems project. It’s a practical device that could be used in day-to-day operations. It can be used to guard a high value item like jewelry, museum artifacts or larger items like a car. If someone is too close to said object, the alarm can be set to go off or warn the perpetrator away from the object. This can also be used to notify security in the event that the threshold is violated via bluetooth. If the object is picked up or moved in any way, an alarm would sound, which would help protect it.

System Overview

	We used an Arduino UNO microcontroller along with an accelerometer shield and four ultrasonic range sensors to create the embedded system. Our system detects any changes of the physical location or orientation of the microcontroller using the accelerometer and also detects any movement in range of the ultrasonic sensors. Using Bluetooth technology, we convey the signal back to a laptop with alert information. We also use lights and a tone module to show the current mode on the device, and allow it to be changed with an on board button.

System Design
Physical Design
	The system design relies on the Arduino UNO board to manage and run the device. All the devices connected to the system are powered by 5V out and the GND all wired in parallel. While running the device is powered by a 9V battery. An accelerometer shield is used in place of an accelerometer module to save pin space on the board.  The reason for this is because so many pins are required to run the system.  Of the 14 digital pins available, each ultrasonic sensor requires one pin for its echo function, and one pin for its trigger function for a total of 8 pins for the ultrasonic sensors. Of the remaining 6, 2 are used for the two LEDs in the system, 2 are used for the bluetooth module, and one for the button, leaving 1, which is PIN 4. PIN 4 cannot be used in this system because of the shield. PIN 4 is generally for most shields the reset pin, and it going HIGH will reset the shield.  The last pin used is A2, which is used for the TONE module. Originally the TONE module had a digital pin, but was moved to analog when we realized PIN 4 needed to be freed up. Luckily the tone library works on any type of pin.  With all of these devices connected the system has a well rounded set of sensors to detect the world around it.
	Software Design
	The software designed for this embedded system is designed to be as smart as possible with the small amount of space the board offers.  The software sports 4 modes: idle, arming, armed, and alarmed. Each mode has a separate set of checks and systems that it runs.
	IDLE: Idle mode flashes the green light and listens for input on the onboard button to change the mode.
	ARMING: Arming mode flashes green and red alternating lights, and starts scanning the environment around it. When in arming mode the device scans for ten cycles, using an algorithm to calculate the rolling average to find the most accurate environment variables around it, and watching for outliers. If an outlying value is found during arming outside of the tolerance thresholds, the system will fail to arm, report why it failed to arm over bluetooth, then restart the arming process. This will continue to occur until either it successfully arms, or the onboard button is clicked to send it back to idle mode.
	ARMED: When armed the red LED will be solid, and the system scans the environment comparing the current values to those of the scan. If they values fall outside of the tolerance the alarm will go off.
	ALARMED: When alarmed the device flashes red, and sets off a tone from the buzzer until the onboard button is clicked disarming the device.

Challenges

- The HC-05 range-finder sensors have a significantly slower echo response time when they are out of range, slowing down the entire system. 
 - Due to the limited coverage of each sensor and the size of the embedded system, full 360 degree coverage could not be obtained. 
- We ran out of digital pins on the board so analog pins were used.
- Memory management is an issue; as the system continues to run, more memory is used which slows down the arm time and sensors’ ability to accurately detect movement in the area. Since so much data needs to be kept to arm and run the system, there is very little available for runtime memory.

Evaluation

Although the system does not provide 360 degree coverage, the proximity alarm system works as designed. All four ultrasonic sensors detect movement when armed in their respective zones. The accelerometer can detect movement of the system, including vibrations created near it, such as the surface it’s on being moved. 


Group Website

http://csci.mayd.co 
The demo video is also included in group website.
