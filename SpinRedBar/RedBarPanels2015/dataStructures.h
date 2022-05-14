struct fadeParams {
    uint8_t         num_panels;     // number of panels to do pattern on
    uint16_t        update_delay;   // delay between fade increments
    CRGB            rgb_value;      // rgb value for all LEDs
    uint8_t         step_num;       // the step value for fading
    bool            increment;      // increment or decrement
};
typedef struct fadeParams fadeParams_t;

struct singlePanelRainbowParams {
    uint8_t     num_panels;         // number of panels to do pattern on
    uint8_t     active_panel;       // active panel to do pattern on
    CRGB        active_rgb_value;   // active panel rgb color
    CHSV        active_hsv_value;   // active panel hsv color
    bool        rand;               // do pattern on random or sequential panels
    uint16_t    update_delay;       // time until next update
    bool        increment;          // increment or decrement (direction, only for sequential)
};
typedef struct singlePanelRainbowParams singlePanelRainbowParams_t;

struct sparkleParams {
    uint8_t     num_panels;
    uint8_t     prev_panel;
    uint8_t     active_panel;
    uint16_t    update_delay;
};
typedef struct sparkleParams sparkleParams_t;

struct spiralParams {
    uint8_t     num_panels;
    uint8_t     active_panel;
    uint16_t    update_delay;
    bool        increment;
};
typedef struct spiralParams spiralParams_t;
