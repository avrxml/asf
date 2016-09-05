/**************************************************************************//**
  \file event_handler.c
 
  \brief Includes implementation for event_handler
 
  Copyright (c) 2016, Atmel Corporation. All rights reserved.
  Released under NDA
  Licensed under Atmel's Limited License Agreement.
 
 
  THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
  WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
  EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
  ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
  OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
  HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
  STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
  ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
  POSSIBILITY OF SUCH DAMAGE.
 
  Atmel Corporation: http://www.atmel.com
 
******************************************************************************/

#include <string.h>
#include "platform.h"
#include "event_handler.h"

#define PLATFORM_EVENT_POOL_DEPTH 10

struct platform_event {
	struct platform_event* next;
	uint16_t event_type;
	uint16_t data_len;
	void* data;
};

static struct platform_event platform_event_pool[PLATFORM_EVENT_POOL_DEPTH];

static struct platform_event *platform_event_free_list = NULL;
static struct platform_event* platform_event_pending_list = NULL;
void platform_event_free(struct platform_event* event);
//struct str_watched_event watched_event;

void platform_event_free(struct platform_event* event)
{
	event->next = platform_event_free_list;
	platform_event_free_list = event;
}

void platform_event_post(uint16_t event_type, void * data, uint16_t data_len)
{
	// get a free event object
	struct platform_event* evt = platform_event_free_list;
	if(evt != NULL)
	{
		platform_event_free_list = evt->next;
		
		evt->next = NULL;
		evt->data = data;
		evt->data_len = data_len;
		evt->event_type = event_type;

		if(platform_event_pending_list == NULL)
		{
			platform_event_pending_list = evt;
		}
		else
		{
			struct platform_event* cur = platform_event_pending_list;
			while(cur->next != NULL)
			{
				cur = cur->next;
			}
			cur->next = evt;
		}
		
		//platform_event_signal();
	}
	
}

plf_drv_status platform_event_get(uint16_t* event_type, uint8_t* data, uint16_t *data_len)
{
	struct platform_event* event = NULL;
	plf_drv_status status= STATUS_SUCCESS;

	// block till an event is posted
	while(platform_event_pending_list == NULL )
	{	
		status = platform_event_wait(0);
	}
	
	event = platform_event_pending_list;
	platform_event_pending_list = platform_event_pending_list->next;
	*event_type = event->event_type;
	*data_len = ((event->data_len<=*data_len)?event->data_len:*data_len);
	//*data = event->data;
	memcpy(data,event->data,*data_len);

	platform_event_free(event);

	return status;
	
}

void platform_event_init()
{
	uint32_t i;
	platform_event_free_list = NULL;
	platform_event_pending_list = NULL;
	//memset(&watched_event,0,sizeof(struct str_watched_event));
	for(i = 0; i < PLATFORM_EVENT_POOL_DEPTH; i++)
	{
		platform_event_free(&platform_event_pool[i]);
	}
}
