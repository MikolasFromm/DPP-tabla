// TODO rename to setup.h and fill in credentials of your wifi
#ifndef SETUP_H
#define SETUP_H

//
//
// Všechny údaje je potřeba začít a zakončit uvozovkami -> "data, co chci napsat"
//
//

// Zde vložte jméno WiFi sítě
#define WIFI_NAME ""

// Zde vložte heslo k vaší WiFi síti
#define WIFI_PASS ""

// Zde vložte váš vygenerovaný API key
#define X-HEADER-TOKEN ""

// Zde vložte vaši zastávku - !! vždy však pouze jeden ze dvou níže uvedených údajů. Druhý nechejte prázdný. !!
// Pole _ZASTAVKA_ pro všechny směry jedné zastávky.
// Pole _ZASTAVKOVY-SLOPEK_ pro jeden konkrétní směr.
// Více na <https://github.com/MeCoolGJK/DPP-tabla>

// Název stanice musí být v URL UTF-8 encoded formátu
#define ZASTAVKA-CELA ""

// ID zastávky 
#define ZASTAVKOVY-SLOUPEK ""


#endif /* SETUP_H */
