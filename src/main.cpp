#include "app/app.h"

#ifdef ARDUINO
#include <Arduino.h>

void setup() {
    app::init();
}

void loop() {
    app::update();
}

#else

int main() {
    app::init();
    for (int i = 0; i < 100; ++i) {
        app::update();
    }
    return 0;
}

#endif
