# maszynaduino
[![experimental](http://badges.github.io/stability-badges/dist/experimental.svg)](http://github.com/badges/stability-badges)

Maszynaduino jest biblioteką dla Arduino (Uno, Mega, etc) ułatwiającą oprogramowanie
pulpitów budowanych na potrzeby symulatora MaSzyna (aka EU07).

Biblioteka posiada obiektowy interfejs i ułatwia korzystanie z analogowych multiplekserów
umożliwiających obsługę większej ilości kontrolek, przełączników i mierników, 
niż fizycznie posiadają mikrokontrolery.

## Możliwości

* obiektowy interfejs
* szeregowa konsola do diagnostyki
* wsparcie dla analogowych multiplekserów (typu 74HC4067)
* obsługa kontrolek i przełączników 
* prostota użycia i czytelność
* kompatybilność z MaSzyną wysyłającą preambułę w komunikacji UART (cztery bajty 0xEF na początkach ramek wejść i wyjść)

## Pierwsze kroki

Użycie biblioteki należy rozpocząć od załadowania plików nagłówkowych:

```cpp
#include "maszynaduino.h"
```

W dalszej części (przed `setup()` i `loop()`) trzeba zadeklarować kontrolki, przełączniki i sam pulpit.
Przykładowo przełącznik baterii podłączony do portu 2 można zadeklarować następująco:

```cpp
#define PIN_BATERIA 2

Switch *bateria = new PinSwitch(PIN_BATERIA, 0, 2)
```

Parametry konstruktora `PinSwitch` są następujące:
- nr pinu
- nr bajtu ramki wyjścia (tu: 0, liczony bez preambuły)
- nr bitu do gaszenia / zapalania (tu: 2)


Do pinu 3 podłączymy kontrolkę lampki czuwaka:

```cpp
#define PIN_LAMPKA_CZUWAK 3

Indicator *lampkaCzuwaka = new PinIndicator(PIN_LAMPKA_CZUWAK, 4, 6);
```

Parametry konstuktora `PinIndicator` są następujące:
- nr pinu
- nr bajtu ramki wejścia (tu: 4, liczony bez preambuły)
- nr bitu określającego stan kontrolki (tu: 6)


W następnym kroku należy utworzyć pulpit określając szeregowy port do komunikacji:

```cpp
Console *sm42 = new Console(&Serial);
```

W funkcji `setup()` należy dodać do pulpitu utworzone przełącznik i kontrolkę, oraz zainicjować pulpit.


```cpp
void setup() {
    sm42->addSwitch(bateria);           // dodanie przełącznika baterii
    sm42->addIndicator(lampkaCzuwaka);  // dodanie kontrolki czuwaka
    sm42->setup();                      // zainicjowanie pulpitu
}
```

W funkcji `loop()` należy dodać aktualizację pulpitu oraz transmisję danych:

```cpp
void loop() {
    sm42->update();            // aktualizacja pulpitu
    sm42->transmit();          // komunikacja Pulpit <-> PC
}
```

Aby przykład zadziałał w MaSzynie, w pliku `eu07_input-uart.ini`, musi pojawić się adekwatny wpis:
```
2 toggle batteryenable batterydisable
```

### Kompletny przykład:

```cpp
#include "maszynaduino.h"

#define PIN_BATERIA 2
#define PIN_LAMPKA_CZUWAK 3

Switch *bateria = new PinSwitch(PIN_BATERIA, 0, 2);
Indicator *lampkaCzuwaka = new PinIndicator(PIN_LAMPKA_CZUWAK, 4, 6);

Console *sm42 = new Console(&Serial);


void setup() {
  sm42->addSwitch(bateria);
  sm42->addIndicator(lampkaCzuwaka);
  sm42->setup();
}

void loop() {
  sm42->update();
  sm42->transmit();
}
```

### Schemat połączeń

Do przełączników nie trzeba podłączać rezystorów ściągających, ponieważ Maszynaduino domyślnie konfiguruje wejścia w trybie `INPUT_PULLUP` (z rezystorami podciągającymi wbudowanymi w Arduino). W razie potrzeby dowolny przełącznik można skonfigurować bez użycia rezystora podciągającego - biblioteka zadba o prawidłową interpretację stanu wejścia.   

![maszynaduino-example-01](https://user-images.githubusercontent.com/139032/117209484-ae22ac00-adf6-11eb-8b7a-1a5a4f64b4e7.png)

### Transmisja danych

W tej chwili `Console` konfiguruje port szeregowy na 57600 bodów (bez możliwości zmiany), i taką prędkość należy ustawić w `eu07.ini` w MaSzynie.

## Elementy interfejsu programistycznego

**Uwaga. Do wydania wersji 1.0 API biblioteki może się zmieniać bez zachowania kompatybilności.**

Klasa           | Opis
--------------- | -------------------
`PinSwitch`     | przełącznik podłączony bezpośrednio do pinu mikrokontrolera
`MuxSwitch`     | przełącznik podłączony przez multiplekser
`PinIndicator`  | kontrolka podłączona bezpośrednio do pinu mikrokontrolera
`Console`       | obiekt opisu i obsługi pulpitu (może być zdefiniowanych wiele pulpitów)
`Mux`           | obsługa pojedynczego multipleksera
`ConsoleDebug`  | konsola debugowania podłączana do portu szeregowego


Struktura       | Opis
--------------- | --------------------
`InputFrame`    | struktura opisująca ramkę wejśc z symulatora (PC)
`OutputFrame`   | struktura opisująca ramkę wyjść do symulatora (PC)

## Użycie multiplekserów

**Uwaga. Interfejs obsługi multiplekserów może się zmienić przed wydaniem wersji 1.0**

Aby zwiększyć liczbę I/O należy zastosować multiplekser (jeden lub więcej).
Biblioteka testowana jest o popularny układ `74HC4067`, 16-to kanałowy analogowy multiplekser.

W celu opisania multipleksera wpiętego w układ należy utworzyć instancję klasy `Mux`
z opisem użytych pinów, a przy tworzeniu instancji przełączników lub kontrolek użyć klas
dedykowanych dla multipleksera.

Zakładając, że mamy jeden multiplekser wpięty do pinów:
- 2: pin "enable" multipleksera
- 3: pin "data" multipleksera
- 4, 5, 6, 7: piny sterujące "S0-S3" multipleksera,
w kodzie programu należy zdefiniować multiplekser następująco:

```cpp

// Mux(int pinEnable, int pinData, int pinS0, int pinS1, int pinS2, int pinS3);

Mux* mux1 = new Mux(2, 3, 4, 5, 6, 7);
```

a przełącznik baterii z przykładu należałoy zdefiniować następująco:

```cpp

// MuxSwitch(Mux *mux, int channel, int nrBajtuRamki, int nrBitu);

Switch *bateria = new MuxSwitch(mux1, MUX_CH1, 0, 2);
```

Ponieważ multiplekser zajął pin 3, to lampkę czuwaka (z przykładu) należałoby podłączyć do innego pinu (np. 8).
To wszystkie zmiany, które należy wprowadzić w celu przeniesienia przełącznika na multiplekser.


## Konsola debugowania

Maszynaduino posiada prostą konsolę szeregową do ułatwienia debugowania.
Konsolę można podłączyć pod dowolny port szeregowy wpierany przez Arduino.

![maszynaduino_consoledebug](https://user-images.githubusercontent.com/139032/117201451-f2a94a00-adec-11eb-9887-1b0b499b6e34.png)

```Cpp
// Pulpit będzie komunikował się z PC podstawowym portem (`Serial`)
Console *sm42 = new Console(&Serial);  

// Konsola debugowania będzie używała portu `Serial1`
ConsoleDebug *debug = new ConsoleDebug(sm42, &Serial1);

void setup() {
    sm42->setup();
    debug->setup();
}

void loop() {
    sm42->update();
    sm42->transmit();

    // wysłanie debug output na konsolę debugowania
    debug->send();
}
```

Aby wykorzystać dodatkowe porty szeregowe do debugowania, konieczna będzie płytka obsługująca większą liczbę portów szeregowych (typu Arduino Mega, Arduino Due) oraz adapter USB.

![usb-ttl-serial-adapter](https://user-images.githubusercontent.com/139032/117203173-00f86580-adef-11eb-90cd-3f6f00fb1971.jpg)


Konsolę debugowania można też uruchomić także na urządzeniach z jednym portem szeregowym (typu Arduino Uno).
W tej sytuacji nie powino się transmitować danych z/do PC (za pomocą `Console::transmit()`), bo zaburzy to dzialanie konsoli.
Transmisję danych warto wtedy uruchamiać warunkowo. Przykład:

```cpp

#define DEBUG

#ifdef DEBUG
ConsoleDebug *debug = new ConsoleDebug(sm42, &Serial);
#endif

void setup() {
  // ...
#ifdef DEBUG
    debug->setup();
#endif
  // ...
}

void loop() {
  // ...
#ifdef DEBUG
    debug->send();
#else
    sm42->update();
    sm42->transmit();
#endif
  // ...
}
```

### Transmisja

Ponieważ konsola debugowania wysyła polecenia terminala do "czyszczenia" ekranu, zalecane jest korzystanie z Putty lub Screena zamiast Serial Monitora wbudowanego w Arduino IDE. Przykładowo:

```bash
screen /dev/ttyACM0 57600
```

**Transmisja konsoli debugowania wymaga ustawienia prędkości na 57600 bodów**. W przyszłości będzie możliwa zmiana.

## Plan rozwoju

### 1.0

- [x] przełączniki (bezpośrednio do pinów)
- [x] przełączniki (przez multiplekser)
- [x] kontrolki (bezpośrednio do pinów)
- [ ] kontrolki (przez multiplekser)
- [ ] przełączniki wielopozycyjne
- [ ] wskaźniki / mierniki (direct i mux)
- [ ] ułatwienie obsługi wielu multiplekserów (do 4 szt, common data i select)
- [ ] optymalizacje, użycie przerwań, ewentualna przebudowa i stabilizacja API

### 1.1
- [ ] gotowe rozwiązania dla mierników, manometrów (np. servo)


## Sugestie i pytania

Pytania i sugestie proszę wpisywać za pomocą https://github.com/marcinn/maszynaduino/issues

## FAQ

**Czy można korzystać z biblioteki za darmo?**

Tak.

**Czy mogę budować swoje produkty w oparciu o nią?**

Tak, ale z zachowaniem zasad licencji GPL v3.0 (w skrócie - powstały produkt także musi być tak samo licencjonowany i dystrybuowany).

**Biblioteka nie posiada bardzo potrzebnej funkcji.**

Należy opisać problem, sugestię lub zapotrzebowanie za pomocą https://github.com/marcinn/maszynaduino/issues.
