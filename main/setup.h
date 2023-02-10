// TODO rename to setup.h and fill in credentials of your wifi
#ifndef SETUP_H
#define SETUP_H

//
//
// Všechny údaje je potřeba začít a zakončit uvozovkami -> "data, co chci napsat"
//
//

// Zde vložte jméno WiFi sítě
#define WIFI_NAME "I@home_Guest"

// Zde vložte heslo k vaší WiFi síti
#define WIFI_PASS "BeOurGuest"

// Zde vložte váš vygenerovaný API key
#define X_HEADER_TOKEN "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJlbWFpbCI6Im1pa29sYXMuZnJvbW1AZ21haWwuY29tIiwiaWQiOjcxMCwibmFtZSI6bnVsbCwic3VybmFtZSI6bnVsbCwiaWF0IjoxNjYxNDMwODE0LCJleHAiOjExNjYxNDMwODE0LCJpc3MiOiJnb2xlbWlvIiwianRpIjoiZTAzZjBkMWItYmE3OS00OGRiLThiOTItNDA3OTUzMDEyMWRmIn0.it9PdUqBO1YImkBjEHZ29vuKwesEhbYHTu49V8Gh2Dg"

// Zde vložte vaši zastávku - !! vždy však pouze jeden ze dvou níže uvedených údajů. Druhý nechejte prázdný. !!
// Pole _ZASTAVKA_ pro všechny směry jedné zastávky.
// Pole _ZASTAVKOVY-SLOPEK_ pro jeden konkrétní směr.
// Více na <https://github.com/MeCoolGJK/DPP-tabla>

// Název stanice musí být v URL UTF-8 encoded formátu, vložte mezi uvozovky za "names=".
// Pokud chcete použít dvě odlišné zastávky (např. vlakovou stanici a tramvajovou stanici opodál),
// oddělte jednotlivé zastávky pomocí "&"" a před dalším názvem zastávky použijte znovu "names=".
// Příklad: "names=N%C3%A1dra%C5%BE%C3%AD%20Podbaba%0A&names=Praha-Podbaba"
#define ZASTAVKA_CELA ""

// ID zastávky, ID získáte podle testovacího výstupu v GOLEMIO, vložte mezi uvozovky za "ids=".
// Pokud chcete použít dva odlišné sloupky (jako například sloupek autobusu a tramvaje),
// oddělte jednotlivé ID pomocí & a před dalším ID použijte znovu "ids=". Příklad: "ids=U236Z1P&ids=U236Z4P"
#define ZASTAVKOVY_SLOUPEK "ids=U236Z1P&ids=U236Z4P"


#endif /* SETUP_H */
