# Patch- Notes

## Part 4

#### 4.2 Helper funktion för välja kund + kundslot implementation

```C
//helper funktion, få första annonsen för en kund
Ad* get_first_ad_for_customer(Customer* customer) {
    return &customer->ads[0];
}

//funkion, visar annons för en kund baserat på kundslot
void run_customer_slot(uint8_t customer_index) {
    Ad* ad = get_first_ad_for_customer(&customers[customer_index]);
    show_ad(ad)
}
```

#### 4.1 Variabler för växling + helper delay funktion

```C
//hålla koll på tiden för att växla annonser
int8_t current_customer_index = 0;
int8_t last_customer_index = -1;


//main funktionen
int main(void) {
    //setup
    lcd_pins_init();
    lcd_init();

    //text som visar på LCD
    lcd_print("Billboard");
    lcd_set_cursor(0,1);
    lcd_print("Online");

    //delay innan annonser börjar visas
    void wait_slot_20s() {
        delay_ms_safe(20000);
    }
```


## Part 3

#### 3.4 Helper funktion för visningsläge
```C
//visar annons baserat på dess visning
void show_ad(const Ad* ad) {
    switch (ad->mode) {
        case MODE_STATIC:
            show_static(ad->text, 2000);
            break;
        case MODE_SCROLL:
            show_scroll(ad->text, 200);
            break;
        case MODE_BLINK:
            show_blink(ad->text, 5000, 500);
            break;
    }
}
```

#### 3.3 Skapar kunder & annonser + räkna ut antalen och testar data

```C
//exempeldata för annonser
Ad harry_ads[] = {
    {"Köp bil hos Harry", MODE_SCROLL, RULE_NONE},
    {"En god bilaffär (för Harry!)", MODE_STATIC, RULE_NONE},
    {"Hederlige Harrys Bilar", MODE_BLINK, RULE_NONE}

};

Ad grandma_ads[] = {
    {"Köp paj hos Farmor Anka", MODE_SCROLL, RULE_NONE},
    {"Skynda innan Mårten ätit alla pajer", MODE_STATIC, RULE_NONE}
};

Ad petter_ads[] = {
    {"Låt Petter bygga åt dig", MODE_SCROLL, RULE_EVEN_MINUTE},
    {"Bygga svart? Ring Petter", MODE_STATIC, RULE_ODD_MINUTE}

};

Ad goofy_ads[] = {
    {"Mysterier? Ring Långben", MODE_STATIC, RULE_NONE},
    {"Långben fixar biffen", MODE_STATIC, RULE_NONE}

};

Ad house_ads[] = {
    {"Synas här? IOT:s Reklambyrå", MODE_STATIC, RULE_NONE}
};

//skapar kunder och deras annonser
Customer customers [] = {
    {"Harrys bilar", 5000, harry_ads, 3},
    {"Farmor Anka", 3000, grandma_ads, 2},
    {"Petter Svartbyggen", 1500, petter_ads, 2},
    {"Långbens detektivbyrå", 4000, goofy_ads, 2},
    {"Reklambyrå", 1000, house_ads, 1}
};

//räknar antal kunder
const uint8_t customer_count = sizeof(customers) / sizeof(customers[0]);

```

```C
/loop som visar olika annonser vid olika lägen
    while (1) {
       show_scroll(customers[0].ads[0].text, 2000);
       show_scroll(customers[1].ads[0].text, 2000);
       show_static(customers[2].ads[0].text, 2000);


    }
```

#### 3.2 3.2 Struktur för kunder och annonser

```C
//annons struktur, hantera annonsdata
typedef struct {
    const char* text;
    DisplayedMode mode;
    RuleType rule;
} Ad;

//kund struktur, hantera kunddata och deras annonser
typedef struct {
    const char* name;
    uint16_t weight;
    Ad* ads;
    uint8_t ad_count;
} Customer;

```

#### 3.1 Enum för visning & regel

```C
//display läge enum, hantera olika visningslägen
typedef enum {
    MODE_STATIC,
    MODE_SCROLL,
    MODE_BLINK
} DisplayMode;

//regel enum, växla mellan olika lägen
typedef enum {
    RULE_NONE,
    RULE_EVEN_MINUTE,
    RULE_ODD_MINUTE
} RuleType;
```

## Part 2

#### 2.5 Testar + mixar display funktioner

```C
  //loop som visar olika annonser vid olika lägen
    while (1) {
       show_static("Static Demo", 2000);
       show_scroll("This is scrolling text on LCD", 250);
       show_blink("Blink Demo", 3000, 400)

    }
```

#### 2.4 Implementerar blink läge
```C
//visar blinkande text, implementera blinkande läge
void show_blink(const char* text, uint16_t total_duration_ms, uint16_t blink_interval_ms) {
    uint16_t elapsed = 0;

    while (elapsed < total_duration_ms) {
        lcd_clear();
        lcd_set_cursor(0,0);
        lcd_print(text);
        delay_ms_safe(blink_interval_ms);
        elapsed += blink_interval_ms;

        if (elapsed >= total_duration_ms) break;

        lcd_clear();
        delay_ms_safe(blink_interval_ms);
        elapsed += blink_interval_ms;
    }
}
```

Testar blinkande läge
```C
//loop som visar olika annonser vid olika lägen
    while (1) {
       show_blink("Harry Cars", 3000, 500); //blinkande
    }
```


#### 2.3 Implementera scroll-läge + helper finktion

```C
//helper funktion för att räkna längden på en string
uint8_t string_length(const char* str) {
    uint8_t len = 0;
    while (str[len] != '\0') {
        len++;
    }
    return len;
}

//visar scrollande text, implementera scrollande läge
void show_scroll(const char* text, uint16_t step_delay_ms) {
    uint8_t len = string_length(text);

    if (len <= 16) {
        show_static(text, 2000);
        return;
    }

    for (uint8_t start = 0; start <= len - 16; start++) {
        lcd_clear();
        lcd_set_cursor(0,0);

        for (uint8_t i = 0; i < 16; i++) {
            lcd_data(text[start + i]);
        }

        delay_ms_safe(step_delay_ms);
    }
}
```

Testar scroll lägen
```C
 //loop som visar olika annonser vid olika lägen
    while (1) {
       show_scroll("Buy a car from Harry", 300); //scrollande
    }

```

#### 2.2 Säker delay funktion + Testa statisk läge

```C
//säker delay funktion
void delay_ms_safe(uint16_t ms) {
    while (ms--) {
        _delay_ms(1);
    }
}


//visar statisk text, implementera statisk läge
void show_static(const char* text, uint16_t duration_ms){
    lcd_clear();
    lcd_set_cursor(0,0);
    lcd_print(text);
    delay_ms_safe(duration_ms); //använder säkra funktionen
}


```

Testar Statisk läge

```C
    //loop som visar olika annonser
    while (1) {
       show_static("Static Test", 2000);
       show_static("It Works!", 2000);
    }
``` 


#### 2.1 Display funktioner + statisk läge

```C
//display funktioner
void show_static(const char* text, uint16_t duration_ms);
void show_scroll(const char* text, uint16_t step_delay_ms);
void show_blink(const char* text, uint16_t total_duration_ms, uint16_t blink_interval_ms);


//visar statisk text, implementera statisk läge
void show_static(const char* text, uint16_t duration_ms){
    lcd_clear();
    lcd_set_cursor(0,0);
    lcd_print(text);
    _delay_ms(duration_ms);
}


```
#### 1.1 Setup

```C
#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>

// mappar LCD pins till MCU pins
#define LCD_RS PD2
#define LCD_E PD3
#define LCD_D4 PD4
#define LCD_D5 PD5
#define LCD_D6 PD6
#define LCD_D7 PD7


//sätter LCD pins som outputs
void lcd_pins_init() {
    DDRD |= (1 << LCD_RS) |
            (1 << LCD_E) |
            (1 << LCD_D4) |
            (1 << LCD_D5) |
            (1 << LCD_D6) |
            (1 << LCD_D7) ;


}

//tillsätterr E pin
void lcd_enable_pulse() {
    PORTD |= (1 << LCD_E);
    _delay_us(1);
    PORTD &= ~(1 << LCD_E);
    _delay_us(100);
}


void lcd_send_nibble(uint8_t data){
    if (data & 0x01) PORTD |= (1 << LCD_D4);
    else PORTD &= ~(1 << LCD_D4);

    if (data & 0x02) PORTD |= (1 << LCD_D5);
    else PORTD &= ~(1 << LCD_D5);

    if (data & 0x04) PORTD |= (1 << LCD_D6);
    else PORTD &= ~(1 << LCD_D6);

    if (data & 0x08) PORTD |= (1 << LCD_D7);
    else PORTD &= ~(1 << LCD_D7);

    lcd_enable_pulse();
}

void lcd_send_byte(uint8_t data, uint8_t rs){
    if (rs)
        PORTD |= (1 << LCD_RS); //Data

        else
        PORTD &= ~(1 << LCD_RS); //Command

        lcd_send_nibble(data >> 4);
        lcd_send_nibble(data & 0x0f); 

        _delay_us(50);

}

void lcd_command(uint8_t cmd) {
    lcd_send_byte(cmd, 0);
}

void lcd_data(uint8_t data) {
    lcd_send_byte(data, 1);
}

//lcd initiliaziton
void lcd_init() {
    _delay_ms(50); 

    lcd_send_nibble(0x03); //funktion 
    _delay_ms(5);
    lcd_send_nibble(0x03); //funktion 
    _delay_us(150);
    lcd_send_nibble(0x03); //funktion
    _delay_us(150);
    lcd_send_nibble(0x02); //skickar 4 bit mode
    _delay_us(150);

    lcd_command(0x28); // 4-bit, 2 line, 5x8 dots
    lcd_command(0x0C); 
    lcd_command(0x06); 
    lcd_command(0x01); 
    _delay_ms(2);
}

//printar string
void lcd_print(const char* str) {
    while (*str) {
        lcd_data(*str++);
    }
}

//sättter cursor position
void lcd_set_cursor(uint8_t col, uint8_t row) {
    uint8_t address = (row == 0) ? 0x00 : 0x40;
    lcd_command(0x80 | (address + col));
}

//visar skärmen på LCD
void lcd_clear() {
    lcd_command(0x01);
    _delay_ms(2);
}

int main(void) {
    //setup
    lcd_pins_init();
    lcd_init();

    //text som visar på LCD
    lcd_print("Billboard");
    lcd_set_cursor(0,1);
    lcd_print("Online");
    _delay_ms(2000);


    //loop som visar olika annonser
    while (1) {
        lcd_clear();
        lcd_print("Ad 1");
        _delay_ms(2000);

        lcd_clear();
        lcd_print("Ad 2");
        _delay_ms(2000);
    }
}

```