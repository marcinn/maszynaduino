# maszynaduino

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
##include "maszynaduino.h"

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


## Plan rozwoju

### 1.0

- [x] przełączniki (bezpośrednio do pinów)
- [x] przełączniki (przez multiplekser)
- [x] kontrolki (bezpośrednio do pinów)
- [ ] kontrolki (przez multiplekser)
- [ ] przełączniki wielopozycyjne
- [ ] wskaźniki / mierniki (direct i mux)
- [ ] ułatwienie obsługi wielu multiplekserów (do 4 szt, common data i select)
- [ ] optymalizacje, ewentualna przebudowa i stabilizacja API

### 1.1
- [ ] gotowe rozwiązania dla mierników, manometrów (np. servo)
