#include <iostream>
#include <iomanip>
#include "DFT_gosu.h" // DFT_gosu class definition
#include <windows.h>
#include <mmsystem.h>
#include <stdio.h>
#include <stdlib.h>
#include<conio.h>
#include <math.h>
using namespace std;

// default constructor
DFT_gosu::DFT_gosu(int m,int xscale ,double yscale, int* output)
{
	NUMPTS = m;
   y_scale = yscale;
   RECTSNUM = xscale;
   done=0;
   spectrum=output;
   infinity=0;
   short_term_avg_freq=0;
   long_term_avg_freq=0;
   short_term_power=0;
   long_term_power=0;

	//=-=-=-=-=-=-=-=-=-=- setting up the dft: -=-=-=-=-=-=-=-=-=-=-=-=-=-=-//
int i,k;
double arg;
		cosarg=(double**)malloc(NUMPTS*sizeof(double*));
		sinarg=(double**)malloc(NUMPTS*sizeof(double*));

		for(i=0;i<NUMPTS;i++){
			cosarg[i]=(double*)malloc(NUMPTS*sizeof(double));
			sinarg[i]=(double*)malloc(NUMPTS*sizeof(double));
		}

		//pre-calculation of all the exponent arguments that we will need:
		   for (k=0;k<NUMPTS;k++) {

			  arg = -1*2.0 * 3.141592654 * (double)k / (double)NUMPTS;
			  for (i=0;i<NUMPTS;i++) {
				 cosarg[k][i] = cos(i * arg);
				 sinarg[k][i] = sin(i * arg);
			  }
		   }

		      x2 = (double*)malloc(NUMPTS*sizeof(double));
   y2 = (double*)malloc(NUMPTS*sizeof(double));

		   

} // end DFT_gosu constructor

// calculate DFT of the signal x1, and calculate relevant parameters
void DFT_gosu::calc(double *x1)
{

	long i,k;
   int j,mm;
   
   if(done==1) return;
  
   //dft calculation, only for half of the spectrum:
   for (k=0;k<(NUMPTS/2);k++) {
      x2[k] = 0;
      y2[k] = 0;
      for (i=0;i<NUMPTS;i++) {
         x2[k] += (x1[i] * cosarg[k][i] );
         y2[k] += (x1[i] * sinarg[k][i]  );
      }
   }


	power=0;

	//calculate |X(w)| and the signal power
      for (k=0;k<(NUMPTS/2);k++) {
		 x1[k] = x2[k]*x2[k] +y2[k]*y2[k];
		 power=power+x1[k];
         x1[k]=sqrt(x1[k]);	 
	  }

/* only the first half of x1 is good the other is bolshit*/
	  power=2*power/NUMPTS/NUMPTS;

	  mm=NUMPTS/2;

	  for(i=0; i<RECTSNUM; i++){
		  x2[i]=0;
	  }

	  //accumulate ajusent frequencies into bars:
	  for (i=0; i<mm; i++){
		  j = (int)( ((double)i*(double)RECTSNUM)/(double)mm );
		  x2[j]+=x1[i];
	  }

	  //limit to y_scale and normalize the result to range [0,100]:
	  for (i=0; i<RECTSNUM; i++){
		  if (x2[i]>y_scale)
			  x2[i]=y_scale;
		  spectrum[i] = (double)100/y_scale*x2[i];
	  }


	  //calculate current average frequency:
	int end,start;
   avg_freq=0;
   start =0;
   end= RECTSNUM-1;
	while (start<end){
			  if(avg_freq<0){
				  avg_freq+=spectrum[start];
				  start++;
			  }
			  else{
				  avg_freq=avg_freq-spectrum[end];
				  end--;
			  }
	}
	avg_freq=end*((22000/(double)RECTSNUM));
	  



	//calculate long term values:
		short_term_avg_freq=(short_term_avg_freq*19+avg_freq)/(20.0);
		short_term_power=(short_term_power*19+power)/(20.0);
		long_term_avg_freq=(long_term_avg_freq*infinity+avg_freq)/(infinity+1);
		long_term_power=(long_term_power*infinity+power)/(infinity+1);
		infinity=infinity+1;


} // end function calc

double DFT_gosu::getCurrentPower(){
	return power;
}
double DFT_gosu::getShortPower(){
	return short_term_power;
}
double DFT_gosu::getLongPower(){
	return long_term_power;
}
int DFT_gosu::getCurrentAvgFreq(){
	return (int)avg_freq;
}

int DFT_gosu::getShortAvgFreq(){
	return (int)short_term_avg_freq;
}

int DFT_gosu::getLongAvgFreq(){
	return (int)long_term_avg_freq;
}

void DFT_gosu::kill()
{
int i;
	//killing dft:
	done=1;
	for(i=0;i<NUMPTS;i++){
		free(cosarg[i]);
		free(sinarg[i]);
	}
	free(cosarg);
	free(sinarg);
	  free(x2);
	  free(y2);
}

void DFT_gosu::setXscale(int x)
{
	RECTSNUM=x;
}

void DFT_gosu::setYscale(double y)
{
	y_scale=y;
}