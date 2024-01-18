/*
 * C code to take (time fields, power field) from file ($1) and integrate to obtain energy
 * mkbane, Jan2024
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define TIMELEN 13
int timeLen = TIMELEN;

int main(int argc, char *argv[]) {
  double energy, width, x, y, meanHeight;  // for each trapezium
  double totalEnergy = 0.0;                 // overall (sum of all trapeziums)

  /*
   * required line format (after header) e.g. as from measure_GPU_dateTime_and_power.sh and preProcessed
   * timestamp power.draw 
   * where timestamp is YYYY/MM/DD hh:mm:ss.ddd
   *
   * ALGORITHM (trapezium rule, where _0 index is LHS and _1 index is RHS of given trapezium)
   * set totalEnergy as zero
   *
   * read first row as t_0, power_0 (ignoring other fields)
   * while (another row)
   *    read next row as t_1, power_1 (ignoring other fields)
   *    estimate energy for this trapezium & accumulate into totalEnergy
   *    set t_0 as t_1 and power_0 as power_1 ready for next step
   */

  if (argc != 2) {
    printf("%s requires name of file as sole parameter\n", argv[0]);
  }
  else {
    // open file, get initial row
    FILE* filePtr;
    filePtr = fopen(argv[1], "r"); // open file, given by sole parameter, as read-only
    if (filePtr == NULL) {
      printf("Cannot open file %s\n", argv[1]);
    }
    else {
      char buffer[40];  // for our case we usually have max of 31 chars per line
      char dateString[11], timeString[timeLen];
      char buf[timeLen-1];
      char startDateString[11];       // save dateString and throw error if changes
      int timeSecs_0, timeNanosecs_0;
      int timeSecs_1, timeNanosecs_1;
      float power_0, power_1;
      /*
      // read initial line (header) and discuard
      fgets(buffer, BUFSIZ-1, filePtr); // read up to EOL
      printf("HEADER\n%s\n", buffer);
      */

      // get initial data and parse 
      fgets(buffer, BUFSIZ-1, filePtr);
      printf("%s\n", buffer);
      int hour, minute;
      float sec;
      char secString[7];
      // reads in ss.ddd, as a string
      sscanf(buffer, "%s %d:%d:%f %f", &startDateString, &hour, &minute, &sec, &power_0);
      printf("date: %s\nhour: %d\nminute: %d\nsecond: %f\n", startDateString, hour, minute, sec);
      /* strncpy(buf, secString, 6); // buf has no trailing comma */
      /* printf("hour: %d\nminute: %d\nsecond: %s\n", hour, minute, buf); */
      /* printf("power: %f\n", power_0); */
      /* float secsSinceMidnight = (float) hour*3600.0 + (float) minute*60.0 + (float) atof(buf); */
      //      printf("secs since midnight = %f\n", secsSinceMidnight);

      exit;

      /*
      sscanf(buffer, "%s %s %f", &startDateString, &timeString, &power_0); 
      printf(" date: %s\n time: '%s'\n power: %f\n", startDateString, timeString, power_0);
      // convert time to seconds since unix began
      strncpy(buf, timeString, timeLen-1);
      printf("'%s'\n", buf);
      timeSecs_0 = convertTimestring(buf); // NB timeString will have trailing comma so pass without
      printf("timeSecs=%f\n",timeSecs_0);
      */

      
      // loop whilst more rows
      int numPairs = 0;
      double deltaTime, meanPower, partialEnergy;
      while (fgets(buffer, BUFSIZ-1, filePtr) != NULL) {
	numPairs++;
	printf("START: timeSecs=%d, timeNanosecs=%d, power=%d microW\n", timeSecs_0, timeNanosecs_0, power_0);
	sscanf(buffer, "%d.%d %d", &timeSecs_1, &timeNanosecs_1, &power_1);
	printf("FINISH: timeSecs=%d, timeNanosecs=%d, power=%d microW\n", timeSecs_1, timeNanosecs_1, power_1);
	deltaTime = (timeSecs_1 - timeSecs_0) + (double) (timeNanosecs_1 - timeNanosecs_0) * 1.0E-09;
	meanPower = 0.5d * (double) (power_1 + power_0);
	partialEnergy = deltaTime * meanPower;
	printf("Delta time=%f, mean power=%f microWatts ==> partial energy=%f microJoules\n", \
	       deltaTime, meanPower, partialEnergy);
	totalEnergy += partialEnergy;
	printf("Total energy so far: %f\n", totalEnergy);
	// now set RHS to be LHS for any next step
	timeSecs_0 = timeSecs_1;
	timeNanosecs_0 = timeNanosecs_1;
	power_0 = power_1;
      } // while
      printf("Read %d pairs of points.\nTotal energy = %f microJoules\n", numPairs, totalEnergy);
    }
  }
}
