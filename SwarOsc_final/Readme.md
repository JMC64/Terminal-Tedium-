This is SuperCollider "patch" is inpired by the AHJ Wave_Swarm module (https://ajhsynth.com/WaveSwarm.html).
At this stage, it still need Xorg to run. I am looking forward to someone explaining to me how to do sclang outside X11.


In order to use it: 
Copy or move the directory /JMextension into your /home/pi/.local/share/SuperCollider/Extensions/ 

  `cp -r ./JMextension  ~/.local/share/SuperCollider/Extensions/ `

Under X11, open a terminal 

`sclang Swarm_final.scd`

You need to feed at least one input ( left input with a sine or triangle or saw or complex sound, avoid a square wave). 
You might need to attenuate this input.

Enjoy! 

