This is a SuperCollider "patch" is inpired by the AHJ Wave_Swarm module (https://ajhsynth.com/WaveSwarm.html).
At this stage, it still need Xorg to run. I am looking forward to someone explaining to me how to do sclang outside X11.


### Install: 

Copy or move the directory /JMextension into your /home/pi/.local/share/SuperCollider/Extensions/ 

  `cp -r ./JMextension  ~/.local/share/SuperCollider/Extensions/ `

Also, install the OSC client : https://github.com/JMC64/Terminal-Tedium-/tree/master/OSC_receiver


### Usage:
Under X11, open a terminal and first run the OSC_simple_client 

`./OSC_simple_client `

Open a second terminal:

`sclang Swarm_final.scd`

You need to feed at least one input ( left input with a sine or triangle or saw or complex sound, avoid a square wave). 
You might need to attenuate the input level.

Commands : 
* Potentiometer 1 = Soft clipping of the left input
* Potentiometer 2 = Soft clipping of the right input

* Button1 + Potentiometer[1..6] = Folding of left channel
* Button2 + Potentiometer[1..6] = Folding of right channel


Enjoy! 

