# DPP-tabla

Odjezdové tabule DPP, pracující s RealTime daty z pražského datového projektu Golemio.

Projekt je stále ve vývoji, přesto jeho základní funckionality jsou již naplněny.

Projekt pro aktuální zobrazení nejbližších odjezdů spojů PID z vybrané stanice/zastávky/nástupiště vznikl jako alternativa pro hledání spojení při odchodu z domu/práce/školy. Po zvolení konkrétní zastávky MHD zobrazují _TABLA_ pět časově nejbližších odjezdů veškerých prostředků, které zastávku obsluhují. Program zohledňuje aktuální zpoždění prostředku, které je k času odjezdu přičteno.

Aktuální data jsou získávána pomocí pražské open-data API banky [GOLEMIO.](https://golemioapi.docs.apiary.io/#)

## INSTALACE

1. Vytvoříme kopii souboru `template_setup.h` pojmenovanou **`setup.h`**, do které budeme následně zapisovat jednotlivé údaje.
2. Vytvoříme vlastní účet a unikátní API klíč zde: https://api.golemio.cz/api-keys/ Nazpět získáme **`X-header-token`**, který je naším API klíčem pro použití v programu. Ten zapíšeme do `setup.h` souboru namísto **`"X-HEADER-TOKEN"`**. (zachovat uvozovky na začátku a konci stringu!!)
3. Zvolíme si vlastní podobu requestu, resp. vlastní zastávku, popřípadě vlastní zastávkový sloupek, pro který chceme data zobrazovat, zde: https://golemioapi.docs.apiary.io/#reference/public-transport/departure-boards/get-departure-board?console=1 V sekci URI parameters si zvolí svou podobu requestu - v našem případě pouze names zastávky, tedy např. takto:

![alt text](https://www.vsechnobu.de/wp-content/uploads/2021/04/Volba-zastavky.png "Ukázka nastavení zastávky")

4. Poté je důležité v poli **`Headers`** vložit svůj nový unikátní API klíč:

![alt text](https://www.vsechnobu.de/wp-content/uploads/2021/04/Vlozeni-api.png "Ukázka nastavení API")

5. Následně vzneste dotaz **`Call resource`**, čímž dostanete přibližně tento výstup:
```
  {
    "arrival_timestamp": {
      "predicted": "2021-04-24T08:54:44.000Z",
      "scheduled": "2021-04-24T08:52:00.000Z"
    },
    "delay": {
      "is_available": true,
      "minutes": 3,
      "seconds": 164
    },
    "departure_timestamp": {
      "predicted": "2021-04-24T08:54:44.000Z",
      "scheduled": "2021-04-24T08:52:00.000Z"
    },
    "route": {
      "short_name": "22",
      "type": "0"
    },
    "stop": {
      "id": "U531Z2P",
      "name": "Pohořelec",
      "platform_code": "B",
      "wheelchair_boarding": 1
    },
    "trip": {
      "headsign": "Vypich",
      "id": "22_672_210402",
      "is_canceled": false,
      "is_wheelchair_accessible": 1
    }
  },
  {
    "arrival_timestamp": {
      "predicted": "2021-04-24T08:56:50.000Z",
      "scheduled": "2021-04-24T08:57:00.000Z"
    },
    "delay": {
      "is_available": true,
      "minutes": 0,
      "seconds": -10
    },
    "departure_timestamp": {
      "predicted": "2021-04-24T08:56:50.000Z",
      "scheduled": "2021-04-24T08:57:00.000Z"
    },
    "route": {
      "short_name": "22",
      "type": "0"
    },
    "stop": {
      "id": "U531Z1P",
      "name": "Pohořelec",
      "platform_code": "A",
      "wheelchair_boarding": 1
    },
    "trip": {
      "headsign": "Nádraží Hostivař",
      "id": "22_635_210402",
      "is_canceled": false,
      "is_wheelchair_accessible": 1
    }
  },
```
V tomto výstupu je vhodné si všimnout, že pro odlišný směr linky (trip - headsign) je i odlišné id zastávky (stop - id), např:
```
    "stop": {
      "id": "U531Z1P",
    "trip": {
      "headsign": "Nádraží Hostivař",
      
    "stop": {
      "id": "U531Z2P",
    "trip": {
      "headsign": "Vypich",
```
Pokud Vás zajímá zastávka ve všech směrech, je vhodné použít v `setup.h` pole _`ZASTAVKA-CELA`_ (vyplnit podle bodu 1), zatímco pokud nás zajímá jediný směr, je vhodné použít pole _`ZASTAVKA-SLOUPEK`_ (vyplnit podle bodu 2). Druhý údaj z výběru však musí zůstat prázdný.

1. Název zastávky musí být v URL UTF-8 encoded formátu. Převod je možné provést zde: https://www.urlencoder.org/ Svou zastávku převádějte z plného znění, s mezerami a diakritikou.
2. Pokud chcete zobrazovat pouze jeden směr, vyberte konkrétní ID z testovacího výpisu na [GOLEMIO - PID DEPARTURES.](https://golemioapi.docs.apiary.io/#reference/public-transport/departure-boards/get-departure-board?console=1)

Součástí repozitáře jsou i knihovny [TFT_eSPI](https://github.com/Bodmer/TFT_eSPI) a [U8g2_for_TFT_eSPI](https://github.com/Bodmer/U8g2_for_TFT_eSPI), které jsou potřeba pro správný chod programu a vypisování textů.

### NASTAVENÍ DISPLEJE

Program, resp. knihovna [TFT_eSPI](https://github.com/Bodmer/TFT_eSPI) je nastavena pro displej ILI9341 (240x135), který je součástí [ESP32 TTGO T4](https://www.laskarduino.cz/lilygo-ttgo-t-4-esp32-2-2-tft-psarm-8m-ip5306--vyvojova-deska/). Pokud chcete program použít pro jiný displej, je potřeba změnit nastavení display-driveru podle návodu na stránkách knihovnt [TFT_eSPI](https://github.com/Bodmer/TFT_eSPI/tree/master/User_Setups).

Nyní stačí soubor uložit a Váš firmware nahrát na ESP.

## FUNKCE

Program má několik základních funckionalit:

1. Zobrazuje 5 spojů, jejich číslo linky společně s směrem/cílovou stanicí.
2. Zobrazuje aktuální odhadovanou dobu do odjezdu spoje, do kterého je zohledněno i aktuální zpoždění.
3. Barevně odlišuje dobu do odjezdu bez zpoždění (zeleně) a dobu odjezdu se zpožděním (červeně) - Pokud je spoj jakkoliv zpožděn, doba do odjezdu bude barevně odlišena, aby bylo jasné, že spoj může svoje zpoždění ještě "krátit".
4. Jakmile je doba do odjezdu kratší než 60s, zobrazí se pouze žlutě "<1min".

## POUŽITÍ

Program je volně dostupný pro každého, přesto autor neručí za žádný spoj, který nebyl dostihnut kvůli chybnému zobrazení.

## TODO

V budoucnu je možné do programu doplnit:

1. Časový odskok, odpovídající docházkové vzdálenosti k zastávce - Nepotřebuji vědět, že autobus jede za 2min, když na zastávku jdu 8min. (Potenciální využití tlačítek)
2. Funkcionalitu tlačítek (vytvoření menu)
3. Night shift - snížit/vypnout jas v nastavených hodinách.
