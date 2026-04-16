#include "IRinput.h"
#include <IRremote.h>
#include "IR.h"

IRrecv irrecv(RECEIVER);
decode_results results;

void irInit() {
  irrecv.enableIRIn();
}

int getIRInput() {
    if (irrecv.decode(&results)) {
      int value = results.value;
      irrecv.resume();
      return value;
    }
    return -1; // no input
}