#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>

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

//display funktion deklarationer
void show_static(const char* text, uint16_t duration_ms);
void show_scroll(const char* text, uint16_t step_delay_ms);
void show_blink(const char* text, uint16_t total_duration_ms, uint16_t blink_interval_ms);



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

//annons struktur, hantera annonsdata
typedef struct {
    const char* text;
    DisplayMode mode;
    RuleType rule;
} Ad;

//kund struktur, hantera kunddata och deras annonser
typedef struct {
    const char* name;
    uint16_t weight;
    Ad* ads;
    uint8_t ad_count;
} Customer;

//exempeldata för annonser
Ad harry_ads[] = {
    {"Kop bil hos Harry", MODE_SCROLL, RULE_NONE},
    {"En god bilaffar (for Harry!)", MODE_STATIC, RULE_NONE},
    {"Hederlige Harrys Bilar", MODE_BLINK, RULE_NONE}

};

Ad grandma_ads[] = {
    {"Kop paj hos Farmor Anka", MODE_SCROLL, RULE_NONE},
    {"Skynda innan Marten atit alla pajer", MODE_STATIC, RULE_NONE}
};

Ad petter_ads[] = {
    {"Lat Petter bygga at dig", MODE_SCROLL, RULE_EVEN_MINUTE},
    {"Bygga svart? Ring Petter", MODE_STATIC, RULE_ODD_MINUTE}

};

Ad goofy_ads[] = {
    {"Mysterier? Ring Langben", MODE_STATIC, RULE_NONE},
    {"Langben fixar biffen", MODE_STATIC, RULE_NONE}

};

Ad commercial_ads[] = {
    {"Synas har? IOT:s Reklambyra", MODE_STATIC, RULE_NONE}
};

//exempeldata för kunder, varje kund har en vikt och lista av annonser
Customer customers [] = {
    {"Harrys bilar", 5, harry_ads, 3},
    {"Farmor Anka", 3, grandma_ads, 2},
    {"Petter Svartbyggen", 2, petter_ads, 2},
    {"Langbens detektivbyra", 4, goofy_ads, 2},
    {"Reklambyra", 1, commercial_ads, 1}
};

//räknar antal kunder
const uint8_t customer_count = sizeof(customers) / sizeof(customers[0]);

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

//kontrollerar om annonsen matchar regler
uint8_t ad_rule_matches(const Ad* ad, uint8_t is_even_minute) {
    if (ad->rule == RULE_NONE) return 1;
    if (ad->rule == RULE_EVEN_MINUTE && is_even_minute) return 1;
    if (ad->rule == RULE_ODD_MINUTE && !is_even_minute) return 1;
    return 0; 
}

//hålla koll på tiden för att växla annonser
int8_t current_customer_index = 0;
int8_t last_customer_index = -1;

//vänta i 20 sekunder innan nästa annons visas
void wait_slot_20s() {
    delay_ms_safe(5000);
}

// få första annonsen för en kund
Ad* get_first_ad_for_customer(Customer* customer) {
    return &customer->ads[0];
}


/*// växla mellan kunder baserat på tid
uint8_t get_next_customer_index(uint8_t current_index) {
    return (current_index + 1) % customer_count;
}
*/

uint8_t is_even_minute_for_slot() {
    static uint16_t slot_counter = 0;
    uint8_t is_even = ((slot_counter / 3) % 2 == 0);
    slot_counter++;
    return is_even;
}

uint8_t count_valid_Ads(Customer * customer, uint8_t is_even_minute) {
    uint8_t count = 0;

    for (uint8_t i = 0;i < customer->ad_count; i++) {
        if (ad_rule_matches(&customer->ads[i], is_even_minute)) {
            count++;
        }
    }

    return count;
}

Ad* pick_ad_for_customer(Customer * customer, uint8_t is_even_minute) {
    uint8_t valid_count = count_valid_Ads(customer, is_even_minute);

    if (valid_count == 0) {
        return &customer->ads[0]; // ingen giltig annons
    }

    uint8_t target = rand() % valid_count;
    uint8_t current = 0;

    for (uint8_t i = 0; i < customer->ad_count; i++) {
        if (ad_rule_matches(&customer->ads[i], is_even_minute)) {
            if (current == target) {
                return &customer->ads[i];
            }
            current++;
        }
    }

    return &customer->ads[0]; // fallback
}

//visar annons för en kund baserat på kundslot
void run_customer_slot(uint8_t customer_index) {
    //visa kundnamn
    lcd_clear();
    lcd_set_cursor(0, 0);
    lcd_print(customers[customer_index].name);
    delay_ms_safe(3000);
    
    uint8_t is_even_minute = is_even_minute_for_slot();
    //visar annonsen för kunden
    Ad* ad = pick_ad_for_customer(&customers[customer_index], is_even_minute);
    
    show_ad(ad);
    wait_slot_20s();
}


//räknar total vikt av alla kunder
uint8_t get_total_weight() {
    uint8_t total = 0;
    for (uint8_t i = 0; i < customer_count; i++) {
        total += customers[i].weight;
    }
    return total;
}
//plockar kund baserat på viktad randomisering
uint8_t pick_weighted_customer() {
    uint8_t total_weight = get_total_weight();
    uint8_t r = rand() % total_weight;

    uint8_t cumulative = 0;

    for (uint8_t i = 0; i < customer_count; i++) {
        cumulative += customers[i].weight;
        if (r < cumulative) {
            return i;
        }
    }
    return 0; // fallback
}

//ingen repetition av kund
uint8_t pick_weighted_customer_no_repeat() {
    uint8_t picked;

    do{
        picked = pick_weighted_customer();
    } while (picked == last_customer_index);
    return picked;
}


//löggning av scheduler steg, plockar kund och visar annons
void run_scheduler_step() {
    current_customer_index = pick_weighted_customer_no_repeat();
    run_customer_slot(current_customer_index);
    last_customer_index = current_customer_index;
    
}


//main funktionen
int main(void) {
    //setup
    lcd_pins_init();
    lcd_init();
    srand(42); // seed för viktad randomisering

    //intro text som visar på LCD
    lcd_print("Billboard");
    lcd_set_cursor(0,1);
    lcd_print("Online");
    delay_ms_safe(2000);


    //loop som visar kunder och annonser i schemaläggning
    while (1) {
      run_scheduler_step();
    }
}