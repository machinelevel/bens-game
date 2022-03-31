
/*
 */


/***********************************************************************\
	use the following routines to adjust joystick sensitivity, or just
	about any other curve that is linear and should not be.
	args are as follows: (note: sign is always preserved)
	 
	 inval: the value
	 power: the power of the curve
	 scale: post-power scale-up
	 clamp: magnitude will never exceed this.
\***********************************************************************/
   
float umGeneralCurveClamp(float inval, long power, float scale, float clamp)
{
	float outval;
	char	neg;
	
	if (inval > 0) neg = 0;
	else {
		neg = 1;
		inval = -inval;
	}
	
	outval = inval;
	while (power > 1) {
		outval *= inval;
		power--;
	}
	
	outval *= scale;
	if (outval > clamp) outval = clamp;
	if (neg) return(-outval);
	else return(outval);
}




float umRampDownCalc(float RampParam, float DeltaTime)
{
	float RampDownT;

	/* General formula is (1.0-(1.0/RampParam)) */

	/* RampParam 20 =  (x0.95) at 33.3fps */
	/* RampParam 10 =  (x0.90) at 33.3fps */
	/* RampParam  5 =  (x0.80) at 33.3fps */
	/* RampParam  4 =  (x0.75) at 33.3fps */
	/* RampParam  2 =  (x0.50) at 33.3fps */
	
	RampDownT=DeltaTime/0.03f; /* Assume that we'd get 0.9f if we were at 33.3fps */
	RampDownT= 1.0f-((RampDownT)/(RampDownT+RampParam));  /* 1 - (1/10) = 0.9 */ /* 1-(1/5) = 0.8 */
	return(RampDownT);
}
