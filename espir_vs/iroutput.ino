//+=============================================================================
// Display IR code
//
void  ircode (decode_results *results)
{
  // Panasonic has an Address
  /*
  if (results->decode_type == PANASONIC) {
    //Serial.print(results->address, HEX);
    sprintf(irbuf + strlen(irbuf), "%llx\n", results->address);
    strcat(irbuf, ":");
  }*/

  // Print Code
  //Serial.print(results->value, HEX);
  sprintf(irbuf + strlen(irbuf), "0x%x\n", results->value);
}


void encoding(decode_results *results)
{
  switch (results->decode_type) {
  default:
  case UNKNOWN:      strcat(irbuf, "UNKNOWN");       break;
  case NEC:          strcat(irbuf, "NEC");           break;
  case SONY:         strcat(irbuf, "SONY");          break;
  case RC5:          strcat(irbuf, "RC5");           break;
  case RC6:          strcat(irbuf, "RC6");           break;
  case DISH:         strcat(irbuf, "DISH");          break;
  case SHARP:        strcat(irbuf, "SHARP");         break;
  case JVC:          strcat(irbuf, "JVC");           break;
  case SANYO:        strcat(irbuf, "SANYO");         break;
  case MITSUBISHI:   strcat(irbuf, "MITSUBISHI");    break;
  case SAMSUNG:      strcat(irbuf, "SAMSUNG");       break;
  case LG:           strcat(irbuf, "LG");            break;
  case WHYNTER:      strcat(irbuf, "WHYNTER");       break;
  case AIWA_RC_T501: strcat(irbuf, "AIWA_RC_T501");  break;
  case PANASONIC:    strcat(irbuf, "PANASONIC");     break;
  }
}

//+=============================================================================
// Dump out the decode_results structure.
//
void  dumpInfo(decode_results *results)
{
  if (results->overflow) {
    strcat(irbuf, "IR code too long. Edit IRremoteInt.h and increase RAWBUF\n");
    return;
  }

  // Show Encoding standard
  strcat(irbuf, "Encoding  : ");
  encoding(results);
  strcat(irbuf, "\n");

  // Show Code & length
  strcat(irbuf, "Code      : ");
  ircode(results);
  strcat(irbuf, " (");
  sprintf(irbuf + strlen(irbuf), "%d", results->bits);
  strcat(irbuf, " bits)\n");
}

//***** END IR Functions
