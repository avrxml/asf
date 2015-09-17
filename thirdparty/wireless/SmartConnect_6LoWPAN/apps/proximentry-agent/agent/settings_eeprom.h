#include "settings.h"

int configure_eeprom(void);
void dump_settings_form_eeprom(void);
int store_settings_to_eeprom(agent_settings_t * const s);
int load_settings_from_eeprom(agent_settings_t *const s);
