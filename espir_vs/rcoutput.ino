static const char* bin2tristate(const char* bin);
static char * dec2binWzerofill(unsigned long Dec, unsigned int bitLength);

void output(unsigned long decimal, unsigned int length, unsigned int delay, unsigned int* raw, unsigned int protocol) {

  if (decimal == 0) {
    strcat(rcbuf, "Unknown encoding.");
  } else {
    const char* b = dec2binWzerofill(decimal, length);
    strcat(rcbuf, "Decimal: ");
    sprintf(rcbuf + strlen(rcbuf), "%lu", decimal);
  
    strcat(rcbuf, " (");
    sprintf(rcbuf + strlen(rcbuf), "%u", length );
    //strcat(rcbuf, "Bit) Binary: ");
    //strcat(rcbuf, b );
    //strcat(rcbuf, " Tri-State: ");
    //strcat(rcbuf, bin2tristate( b) );
    strcat(rcbuf, " PulseLength: ");
    sprintf(rcbuf + strlen(rcbuf), "%u", delay);
    strcat(rcbuf, " microseconds");
    strcat(rcbuf, " Protocol: ");
    sprintf(rcbuf + strlen(rcbuf), "%u", protocol);
  }
  /*
  strcat(rcbuf,  "Raw data: ");
  for (unsigned int i=0; i<= length*2; i++) {
    sprintf(rcbuf + strlen(rcbuf), "%u", raw[i]);
    strcat(rcbuf, ",");
  }
  */
}

static const char* bin2tristate(const char* bin) {
  static char returnValue[50];
  int pos = 0;
  int pos2 = 0;
  while (bin[pos]!='\0' && bin[pos+1]!='\0') {
    if (bin[pos]=='0' && bin[pos+1]=='0') {
      returnValue[pos2] = '0';
    } else if (bin[pos]=='1' && bin[pos+1]=='1') {
      returnValue[pos2] = '1';
    } else if (bin[pos]=='0' && bin[pos+1]=='1') {
      returnValue[pos2] = 'F';
    } else {
      return "not applicable";
    }
    pos = pos+2;
    pos2++;
  }
  returnValue[pos2] = '\0';
  return returnValue;
}

static char * dec2binWzerofill(unsigned long Dec, unsigned int bitLength) {
  static char bin[64]; 
  unsigned int i=0;

  while (Dec > 0) {
    bin[32+i++] = ((Dec & 1) > 0) ? '1' : '0';
    Dec = Dec >> 1;
  }

  for (unsigned int j = 0; j< bitLength; j++) {
    if (j >= bitLength - i) {
      bin[j] = bin[ 31 + i - (j - (bitLength - i)) ];
    } else {
      bin[j] = '0';
    }
  }
  bin[bitLength] = '\0';
  
  return bin;
}


