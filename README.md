# lcd-billboard
The Billboard (LCD AD Scheduler)

Github repo:
https://github.com/NoJi-nx/lcd-billboard

Wokwi link:
https://wokwi.com/projects/458899997912236033


## 1. Översikt

Detta är ett projekt för ett digitalt reklamskyltsystem med en ATmega328P och en 16x2 LCD-skärm. Det visar annonser från flera kunder med olika krav och prioriteringar. Systemet stöder flera visningslägen (statisk, rullande, blinkande) och använder viktat slumpmässigt urval. Det säkerställer också att samma kund aldrig visas två gånger i rad och inkluderar tidsregeln för specifika annonser.

Implementeringen är skriven i ren C utan Arduino-bibliotek, med hjälp av  registermanipulation och en anpassad LCD-drivrutin.

## 2. Systemarkitektur

Systemet är uppdelat i flera  moduler Var och en ansvarar för en specifik del av funktionaliteten:

### 2.1 LCD-drivrutin (lågnivå)
Hanterar all kommunikation med LCD-skärmen med hjälp av direkt registermanipulation. Den implementerar ett anpassat 4-bitarsgränssnitt, inklusive funktioner för att skicka kommandon, skriva data, styra markören och rensa displayen.

### 2.2 Displaymotor
Ansvarar för att visa text på LCD-skärmen. Den inkluderar tre visningslägen:

* Statisk
* Rullning
* Blinkande

Varje läge implementeras som en återvinnsfunktion och fungerar oberoende av schema logiken.
<br>

### 2.3 Datamodell (kunder och annonser)
Systemet använder strukturerade datatyper (struct) för att representera kunder och annonser. 
Varje kund innehåller:
* Ett namn
* En vikt (baserat på betalning)
* En lista med annonser

<br>

Varje annons innehåller:
* Text
* Visningsläge
* Valfri regellogik
<br>

Denna design gör systemet skalbart och enkelt att utöka.

### 2.4 Schemaläggare (Scheduler)
Styr systemets exekveringsflöde. Den:
* Väljer en kund
* Visar kundens namn
* Väljer en giltig annons
* Kör annonsen under en fast tidslucka


Schemaläggaren körs kontinuerligt i huvudloopen.

### 2.5 Slumpmässigt urval (Random Selection)
Implementerar en  slumpmässig algoritm för att välja kunder baserat på deras betalning. Säkerställer vissa villkor:

* Högre betalande kunder visas oftare
* Samma kund aldrig väljs två gånger i rad

## 3. Maskinvaruinstallation

* Systemet använder en Arduino Uno (**ATmega328P**) ansluten till en **16x2 LCD** via ett kopplingsdäck. 
* LCD-skärmen drivs i **4-bitarsläge** för att minska antalet nödvändiga datapinnar.

* Kontrollpinnar (**RS**, **E**) och datapinnar (**D4–D7**) är anslutna till digitala **I/O-pinnar**. 

*  **RW** är kopplad till **GND**, och strömpinnar är anslutna till **5V** och **GND**. 
*  Kontrast och bakgrundsbelysning styrs med hjälp av **motstånd**(**resistor**).
*  

### 3.1 Pin-mappning

LCD-skärmen är ansluten till ATmega328P med PORTD-stift i 4-bitarsläge. Varje LCD-signal mappas till en specifik mikrokontrollerstift MCU-pin enligt följande:

- RS → PD2
- E → PD3
- D4 → PD4
- D5 → PD5
- D6 → PD6
- D7 → PD7

**Beskrivning**
* RS (Register Select): Bestämmer om data eller kommandon skickas till LCD-skärmen
* E (Enable): Trigger LCD-skärmen för att läsa inkommande data
* D4–D7: Datalinjer som används för 4-bitarskommunikation
Alla pins är konfigurerade som utgångar med hjälp av DDRD-registret.

### 3.2 LCD Kommunikation

LCD-skärmen drivs i 4-bitarsläge för att minska antalet nödvändiga datapinnar. Varje byte delas upp i två 4-bitarsdelar (nibbles), där den höga nibblen skickas först, följt av den låga nibblen.

Efter att data placerats på pinnarna (D4–D7) slås E-pinnen (Aktiveringspin) för att låsa data i LCD-skärmen. RS-pinnen avgör om data är ett kommando eller displaytecken.

Metoden tillåter effektiv kommunikation med färre I/O-pinnar.

### 3.3 Visningen (Display Engine)

Visningsmotorn hanterar hur text ges på LCD-skärmen och stöder tre lägen: 
- Statisk - visar fast text under en given duration
- Rullning - flyttar lång text horisontellt för att passa skärmen
- Blinkning - växlar textens synlighet med ett fast intervall

Varje läge skapas som en användbar funktion. Schemaläggaren styr hur länge varje läge körs, vilket säkerställer tydlig seperation mellan logiken på visning och tid.


### 3.4 Schemaläggningssystem

Systemet använder en 20/21-sekunders schemaläggare där varje kund får en fast visningstid. Loopen kör koninuerrligt, väljer en kund, visar deras annons och går vidare till nästa annons.

Kundval hanteras med hjälp av viktad slumpmässig logik samtidigt som ingen uprepning säkerställs genom att spåra den tidigare visade kund.

Schemakäggaren styr timing och val. Visningsfunnktionerna hanterar rendering. Dessa skapar en ansvarsfördelning

### 3.5  Viktat slumpmässigt urval
Systemet använder viktad slumpmässig algoritm där varje kund tilldelas en vikt baserat på deras betalning. Ett tal genereras inom det totala viktintervallet och kunder väljs basera på kumulativa vikter. 

Det säkerställer att kunder med högre betalning völjs oftare över tid, vilket beräknar fördelningen.
Systemet också hindrar att samma kund väljs två gånger i rad genom att jämföra med den tidigare valda kunden.

### 3.6 Tidsbaserad regel (Peter)

Petters annonser följer en tidsregel där olika annonser visas beroende på  jämna eller udda minuter. Eftersom ingen realtidsklocka används implementeras en räknarbaserad metod för att justera beteende över tid.

## 4. Limitationer

Systemet använder blockerande dröjningar, vilket innebär att multitasking inte är möjligt.
<br>
Tiden simuleras med hjälp av en räknare istället för en riktig klocka, minskar noggrannheten. 
<br>
LCD-skärmen stöder inte svenska tecken som å, ä, ö. Det kräver translitterering. 
<br>
Dessutom är all data hårdkodad, vilket begränsar flexibiliteten, och det slumpmässiga urvalet använder ett fast seed värde.