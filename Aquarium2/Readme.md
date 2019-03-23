
AQUARIUM : A pale imitation of the incredible module "Cloud Terrarium" for Terminal Tedium

Required externals : Terminal Tedium, Cyclone, iem_tab;

This patch morphs sounds by cycling through a bank. Each bank is 64 wavforms.

|Wav0|Wav1|Wav2|Wav3|...|Wav63|

The patch is reading from min to max: 
Wav0-> Wav2 -> Wav3 ->...

The banks are simply the concatenation of 64 single cycle wavforms from Adventure Kid (many thanks for his work) [sox is you friend for making your own banks]. On the top of that, this patch is based on 7 oscillators which can be detuned like in a supersaw.

The morphing require a gate into the first gate input of the TD. Better send a slow and very regular gate ( i.e. coming from Temps-Utile).


Input a CV pitch into entry corresponding to Pot1
Pitch can be internally quantized or unquantized. Selection is made with button 2. By default pitch is quantized.


Pot2 selects the detune amount between the Oscillators as well as noise (more detune, more noise) ;

Pot3 selects the mix of detuned Osc with main Osc;

Pot4 selects the first table in the bank to be used ( 0->63);

Pot5 selects the last table in the bank  to be used ( 0->63);

Don't worry about which one of Pot 4 or Pot5 is the maximum or minimum, this is taken care of by the patch.

Pot6 sends the the amount of delayed signal in the mix;

Pressed But 3 + turn Pot 6 changes the delay time;

BUTTON 3 changes banks (0->10);

Most of the ideas were gathered elsewhere, thank you to all people who release their work on the internet.
Special thank to Max and all the designers of TD, Temps Utile and o_C
