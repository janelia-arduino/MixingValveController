#include <MixingValveController.h>


MixingValveController dev;

void setup()
{
  dev.setup();
  dev.startServer();
}

void loop()
{
  dev.update();
}
