s.boot;

(
s.doWhenBooted{
	(
		Routine({
thisProcess.openUDPPort(9000);
OSCFunc.trace(false);


//
~b1_status = 0;
~b2_status = 0;
~sumBut = 0;

// All button OFF
~busP1B1 = Bus.control(s,1);
~busP2B1 = Bus.control(s,1);
~busP3B1 = Bus.control(s,1);
~busP4B1 = Bus.control(s,1);
~busP5B1 = Bus.control(s,1);
~busP6B1 = Bus.control(s,1);

~busP1B1.set(0);
~busP1B2.set(0);
~busP1B3.set(0);
~busP1B4.set(0);
~busP1B5.set(0);
~busP1B6.set(0);



// All button OFF
~busP1B2 = Bus.control(s,1);
~busP2B2 = Bus.control(s,1);
~busP3B2 = Bus.control(s,1);
~busP4B2 = Bus.control(s,1);
~busP5B2 = Bus.control(s,1);
~busP6B2 = Bus.control(s,1);

// Control the clipping of signal
~busP1 = Bus.control(s,1);
~busP2 = Bus.control(s,1);


OSCdef.new(\but1,{
	arg msg;
	var p=  0 ;
	p =msg[1];
//	p.postln;
	~b1_status = p;
	~sumBut = ~b1_status + ~b2_status;
},'/button1');


OSCdef.new(\but2,{
	arg msg;
	var p=  0  ;
	p =msg[1];
	~b2_status = p;
	~sumBut = ~b1_status + ~b2_status;
},'/button2');


OSCdef.new(\pot1,{
	arg msg;
	var p=  0  ;
	p =msg[1];
	if (~sumBut == 0    ) { ~busP1.set(p)}   ;
	if (~b1_status == 1 ) {~busP1B1.set(p); }   ;
	if (~b2_status == 1 ) {~busP1B2.set(p); }   ;
},'/pot1');


OSCdef.new(\pot2,{
	arg msg;
	var p=  0  ;
	p =msg[1];
	if (~sumBut == 0    ) { ~busP2.set(p); }   ;
	if (~b1_status == 1 ) {~busP2B1.set(p); }   ;
	if (~b2_status == 1 ) {~busP2B2.set(p); }   ;
},'/pot2');


OSCdef.new(\pot3,{
	arg msg;
	var p=  0  ;
	p =msg[1];
	if (~b1_status == 1 ) { ~busP3B1.set(p); }   ;
	if (~b2_status == 1 ) { ~busP3B2.set(p); }   ;
},'/pot3');


OSCdef.new(\pot4,{
	arg msg;
	var p=  0;
	p =msg[1];
	if (~b1_status == 1 ) { ~busP4B1.set(p); }   ;
	if (~b2_status == 1 ) { ~busP4B2.set(p); }   ;
},'/pot4');


OSCdef.new(\pot5,{
	arg msg;
	var p=  0 ;
	p =msg[1];
	if (~b1_status == 1 ) { ~busP5B1.set(p); }   ;
	if (~b2_status == 1 ) { ~busP5B2.set(p); }   ;
},'/pot5');


OSCdef.new(\pot6,{
	arg msg;
	var p=  0  ;
	p =msg[1];
	if (~b1_status ==1  ) { ~busP6B1.set(p); }   ;
	if (~b2_status == 1 ) { ~busP6B2.set(p); }   ;
},'/pot6');




1.wait;

SynthDef.new(\wraper_1,{
	arg shift1 = 0.1, shift2 = 0.1, shift3 = 0.1,
	shift4 = 0.1, shift5 = 0.1, shift6 = 0.1,
	smooth_amount=0.2   ;
	var sig, sig1, sig2, sig3, sig4, sig5, sig6;

	sig = SoundIn.ar(0);
	sig1 = JmWrapper2.ar(sig,shift1,smooth_amount, 0.16 );
	sig2 = JmWrapper2.ar(sig,shift2,smooth_amount, 0.25 );
	sig3 = JmWrapper2.ar(sig,shift3,smooth_amount, 0.38 );
	sig4 = JmWrapper2.ar(sig,shift4,smooth_amount, 0.55 );
	sig5 = JmWrapper2.ar(sig,shift5,smooth_amount, 0.801 );
	sig6 = JmWrapper2.ar(sig,shift6,smooth_amount, 1.2 );
	sig1 = (sig1+ sig2 + sig3+ sig4 + sig5 +sig6 ) * 0.2;
	Out.ar(0,sig1 );}
).add;

//SystemClock.sched(2,{"First synth ready!".postln});
1.wait;
SynthDef.new(\wraper_2,{
	arg shift1 = 0.1, shift2 = 0.1, shift3 = 0.1,
	shift4 = 0.1, shift5 = 0.1, shift6 = 0.1,
	smooth_amount=0.2   ;
	var sig, sig1, sig2, sig3, sig4, sig5, sig6;

	sig = SoundIn.ar(1);
	sig1 = JmWrapper2.ar(sig,shift1,smooth_amount, 0.14 );
	sig2 = JmWrapper2.ar(sig,shift2,smooth_amount, 0.2 );
	sig3 = JmWrapper2.ar(sig,shift3,smooth_amount, 0.3 );
	sig4 = JmWrapper2.ar(sig,shift4,smooth_amount, 0.45 );
	sig5 = JmWrapper2.ar(sig,shift5,smooth_amount, 0.65 );
	sig6 = JmWrapper2.ar(sig,shift6,smooth_amount, 1.0 );
	sig1 = (sig1+ sig2 + sig3+ sig4 + sig5 +sig6 ) * 0.2;
	Out.ar(1,sig1 );}
).add;


1.wait;


//
//         |--> Synth[Wrapper1]--> OutL
//  In1 -->|
//         |--> Synth[Wrapper2]--> OutR
//
//
//  In1 -->|--> Synth[Wrapper1]--> OutL
//  In2 -->|--> Synth[Wrapper2]--> OutR
//
// Patching OutL to In2  makes a serie of the 2 folders
//
//s.plotTree;
SystemClock.sched(2,{"Second synth Ready".postln});

w = Synth(\wraper_1);
w.map(\shift1,~busP1B1);
w.map(\shift2,~busP2B1);
w.map(\shift3,~busP3B1);
w.map(\shift4,~busP4B1);
w.map(\shift5,~busP5B1);
w.map(\shift6,~busP6B1);
w.map(\smooth_amount,~busP1);

//SystemClock.sched(2,{"Synth1 lauched".postln});
1.wait;
x =  Synth(\wraper_2);
x.map(\shift1,~busP1B2);
x.map(\shift2,~busP2B2);
x.map(\shift3,~busP3B2);
x.map(\shift4,~busP4B2);
x.map(\shift5,~busP5B2);
x.map(\shift6,~busP6B2);
x.map(\smooth_amount,~busP2);


		}).play;
)};)


//s.scope

//~b1_status;

//~b2_status = 0;
//~sumBut

//s.plotTree;
//s.newBufferAllocators;

