JmWrapper2 {
	*ar {
// in :  input signal 
// shift: amplitude of shifting the signal ( higth of the step 0 <shift < 0.5)
// smooth_amount : see SmoothClippingS parameter amount 
// lfo_freq : frequency of the folding

		arg in, shift = 0.1, smooth_amount =0 , lfo_freq = 0.1;
		var out=0, sig_in, sig, sig2, sig1, lfo, t1, t2;

		lfo = LFTri.kr(lfo_freq , mul: 0.5)  ;
		//sig_in =In.ar(in,1) ;
		sig_in=in;
		sig_in = SmoothClipS.ar(sig_in,-1, 1,smooth_amount);
		sig_in = sig_in * 0.5 ;

		t1 = InRange.ar(sig_in,-0.5,lfo);
		sig1 = t1 * sig_in;
		sig1 = sig1  + shift;
		sig1 = t1*sig1;
	
		t2 = 1 - t1;
		sig2 = t2 * sig_in;
		sig = (sig1 + sig2)*2 * 0.8  - shift ;
	
		^sig;	

	}
}
