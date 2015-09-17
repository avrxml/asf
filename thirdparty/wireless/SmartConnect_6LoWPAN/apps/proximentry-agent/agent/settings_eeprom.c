#include <asf.h>
#include <string.h>
#include "settings_eeprom.h"
#include "conf_agent_app.h"
#include "misc.h"
#include "eeprom.h"


#define EEPROM_SETTING_OFFSET 0

struct eeprom_settings{
	uint8_t valid;            //indicates whether eeprom data is valid
	uint8_t led0_state;
	uint32_t sync_interval;
};

int configure_eeprom(void)
{
    /* Setup EEPROM emulator service */
    enum status_code error_code = eeprom_emulator_init();
    if (error_code == STATUS_ERR_NO_MEMORY) {
        LOG("ERROR: eeprom: No section has been set in the device's fuses.");
	    panic();
    }
    else if (error_code != STATUS_OK) {
		LOG("ERROR eeprom: Erase the emulated EEPROM memory (assume it is unformatted or * irrecoverably corrupt).");
        eeprom_emulator_erase_memory();
        eeprom_emulator_init();
		return 1;
    }

	struct eeprom_emulator_parameters parameters;
	eeprom_emulator_get_parameters(&parameters);

	return 0;
}

/**
 * @brief Storing the settings to the eeprom
 *
 * @param[in] pointer to settings
 *
 */
int store_settings_to_eeprom(agent_settings_t *s)
{
	int ret = 0;

	struct eeprom_settings es;
	es.valid = 1;
	es.led0_state = s->led0_state;
	es.sync_interval = s->sync_interval;
	ret =  eeprom_emulator_write_buffer(EEPROM_SETTING_OFFSET, (const uint8_t *const)&es,sizeof(struct eeprom_settings));
	//TODO: commit function should be called prior to a system reset or shutdown,
	//      not every save settings request
	eeprom_emulator_commit_page_buffer();

	return ret;
}

/**
 * @brief Filling the struct settings with value from eeprom
 *
 * @param[in] pointer to the settings in the ram to fills
 */
int load_settings_from_eeprom(agent_settings_t * const s)
{
	struct eeprom_settings es;

	eeprom_emulator_read_buffer(EEPROM_SETTING_OFFSET,(uint8_t *const)&es,sizeof(struct eeprom_settings));

	if (es.valid != 1)
	{
		LOG("WARNING: eeprom settings are not valid - will be overwritten by default values"EOL);
		es.led0_state = DEF_LED0_STATE;
		es.sync_interval = DEF_SYNC_INTERVAL;
		es.valid = 1;
		eeprom_emulator_write_buffer(EEPROM_SETTING_OFFSET, (const uint8_t *const)&es,sizeof(struct eeprom_settings));
		//TODO: commit function should be called prior to a system reset or shutdown,
		//      not every save settings request
		eeprom_emulator_commit_page_buffer();
	}

	s->led0_state = es.led0_state;
	s->sync_interval = es.sync_interval;
	return 0;
}

/**
 * @brief Dump settings from eeprom
 *
 * @return
 */
void dump_settings_form_eeprom(void)
{
	struct eeprom_settings es;
	eeprom_emulator_read_buffer(EEPROM_SETTING_OFFSET, (uint8_t *const)&es,sizeof(struct eeprom_settings));
	LOG("Eeprom settings: "EOL);
	LOG("   %s:\t\t%d"EOL,KEY_LED0 ,es.led0_state );
 	LOG("   %s:\t%d"EOL,KEY_SYNC_INTERVAL  ,(int)es.sync_interval);

}
