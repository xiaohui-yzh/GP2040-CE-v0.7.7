/*
 * SPDX-License-Identifier: MIT
 * SPDX-FileCopyrightText: Copyright (c) 2021 Jason Skuby (mytechtoybox.com)
 * SPDX-FileCopyrightText: Copyright (c) 2019 Ha Thach (tinyusb.org)
 */

#include <wchar.h>
#include "tusb.h"
#include "usb_driver.h"
#include "gamepad/GamepadDescriptors.h"
#include "webserver_descriptors.h"

#if 1
//--------------------------------------------------------------------+
// String Descriptors
//--------------------------------------------------------------------+

// array of pointer to string descriptors
char const *string_desc_arr_test[] =
{
  (const char[]) { 0x09, 0x04 },  // 0: is supported language is English (0x0409)
  "ATK",                          // 1: Manufacturer
  "ATK Device GamePad",           // 2: Product
  "atk123",                       // 3: Serials will use unique ID if possible
  "ATK GamePad",                  // 4: Interface 1 String
  "ATK GamePadVendor",            // 5: Interface 2 String
};

static uint16_t _desc_str_test[32 + 1];

// Invoked when received GET STRING DESCRIPTOR request
// Application return pointer to descriptor, whose contents must exist long enough for transfer to complete
uint16_t const *tud_descriptor_string_cb(uint8_t index, uint16_t langid) {
  (void) langid;
  size_t chr_count;

  switch ( index ) {
    case STRID_LANGID:
      memcpy(&_desc_str_test[1], string_desc_arr_test[0], 2);
      chr_count = 1;
      break;

    //case STRID_SERIAL:
    //  chr_count = board_usb_get_serial(_desc_str_test + 1, 32);
    //  break;

    default:
      // Note: the 0xEE index string is a Microsoft OS 1.0 Descriptors.
      // https://docs.microsoft.com/en-us/windows-hardware/drivers/usbcon/microsoft-defined-usb-descriptors

      if ( !(index < sizeof(string_desc_arr_test) / sizeof(string_desc_arr_test[0])) ) return NULL;

      const char *str = string_desc_arr_test[index];

      // Cap at max char
      chr_count = strlen(str);
      size_t const max_count = sizeof(_desc_str_test) / sizeof(_desc_str_test[0]) - 1; // -1 for string type
      if ( chr_count > max_count ) chr_count = max_count;

      // Convert ASCII string into UTF-16
      for ( size_t i = 0; i < chr_count; i++ ) {
          _desc_str_test[1 + i] = str[i];
      }
      break;
  }

  // first byte is length (including header), second byte is string type
  _desc_str_test[0] = (uint16_t) ((TUSB_DESC_STRING << 8) | (2 * chr_count + 2));

  return _desc_str_test;
}
#else
// Invoked when received GET STRING DESCRIPTOR request
// Application return pointer to descriptor, whose contents must exist long enough for transfer to complete
uint16_t const *tud_descriptor_string_cb(uint8_t index, uint16_t langid)
{
	(void)langid;

	if (get_input_mode() == INPUT_MODE_CONFIG)
	{
		return web_tud_descriptor_string_cb(index, langid);
	}
	else
	{
		uint16_t size = 0;
		return getStringDescriptor(&size, get_input_mode(), index);
	}
}
#endif

#if 1
//--------------------------------------------------------------------+
// Device Descriptors
//--------------------------------------------------------------------+
tusb_desc_device_t const desc_device_test =
{
    .bLength            = sizeof(tusb_desc_device_t),
    .bDescriptorType    = TUSB_DESC_DEVICE,
    .bcdUSB             = 0x0200,
    .bDeviceClass       = 0x00,
    .bDeviceSubClass    = 0x00,
    .bDeviceProtocol    = 0x00,
    .bMaxPacketSize0    = CFG_TUD_ENDPOINT0_SIZE,

    .idVendor           = 0x045E, // idVendor 0x045E 0xCafe 0x1532 0x373B
    .idProduct          = 0x028E, // idProduct 0x028E USB_PID 0x1013 0x1013
    .bcdDevice          = 0x0114,

    .iManufacturer      = 0x01,
    .iProduct           = 0x02,
    .iSerialNumber      = 0x03,

    .bNumConfigurations = 0x01
};

// Invoked when received GET DEVICE DESCRIPTOR
// Application return pointer to descriptor
uint8_t const * tud_descriptor_device_cb(void)
{
  return (uint8_t const *) &desc_device_test;
}
#else
// Invoked when received GET DEVICE DESCRIPTOR
// Application return pointer to descriptor
uint8_t const *tud_descriptor_device_cb()
{
	switch (get_input_mode())
	{
		case INPUT_MODE_CONFIG:
			return  web_tud_descriptor_device_cb();

		case INPUT_MODE_XINPUT:
			return xinput_device_descriptor;

		case INPUT_MODE_XBONE:
			return xbone_device_descriptor;

		case INPUT_MODE_PS4:
			return ps4_device_descriptor;

		case INPUT_MODE_SWITCH:
			return switch_device_descriptor;

		case INPUT_MODE_KEYBOARD:
			return keyboard_device_descriptor;

		case INPUT_MODE_NEOGEO:
			return neogeo_device_descriptor;

		case INPUT_MODE_MDMINI:
			return mdmini_device_descriptor;

		case INPUT_MODE_PCEMINI:
			return pcengine_device_descriptor;

		case INPUT_MODE_EGRET:
			return egret_device_descriptor;

		case INPUT_MODE_ASTRO:
			return astro_device_descriptor;

		case INPUT_MODE_PSCLASSIC:
			return psclassic_device_descriptor;

		case INPUT_MODE_XBOXORIGINAL:
			return xboxoriginal_device_descriptor;

		default:
			return hid_device_descriptor;
	}
}
#endif

#if 1
//--------------------------------------------------------------------+
// HID Report Descriptor
//--------------------------------------------------------------------+

uint8_t const desc_hid_report1[] =
{
    TUD_HID_REPORT_DESC_GAMEPAD(),
};

uint8_t const desc_hid_report2[] =
{
    //TUD_HID_REPORT_DESC_KEYBOARD()
    TUD_HID_REPORT_DESC_GENERIC_INOUT(32)
};
//--------------------------------------------------------------------+
// HID Report Descriptor
//--------------------------------------------------------------------+

// Invoked when received GET HID REPORT DESCRIPTOR
// Application return pointer to descriptor
// Descriptor contents must exist long enough for transfer to complete
uint8_t const * tud_hid_descriptor_report_cb(uint8_t itf)
{
#if 1
    printf("%s:%d itf=%d\n", __FUNCTION__, __LINE__, itf);

    return desc_hid_report2;
#else
  if (itf == 0)
  {
    return desc_hid_report1;
//    return hid_report_descriptor;
  }
  else if (itf == 1)
  {
    //return keyboard_report_descriptor;
      return desc_hid_report2;
  }

  return NULL;
#endif
}
#else
// Invoked when received GET HID REPORT DESCRIPTOR
// Application return pointer to descriptor
// Descriptor contents must exist long enough for transfer to complete
uint8_t const *tud_hid_descriptor_report_cb(uint8_t itf)
{
	(void) itf;
	printf("%s:%d itf=%d\n", __FUNCTION__, __LINE__, itf);
	switch (get_input_mode())
	{
		case INPUT_MODE_SWITCH:
			return switch_report_descriptor;

		case INPUT_MODE_PS4:
			return ps4_report_descriptor;

		case INPUT_MODE_KEYBOARD:
			return keyboard_report_descriptor;

		case INPUT_MODE_NEOGEO:
			return neogeo_report_descriptor;

		case INPUT_MODE_MDMINI:
			return mdmini_report_descriptor;

		case INPUT_MODE_PCEMINI:
			return pcengine_report_descriptor;

		case INPUT_MODE_EGRET:
			return egret_report_descriptor;

		case INPUT_MODE_ASTRO:
			return astro_report_descriptor;

		case INPUT_MODE_PSCLASSIC:
			return psclassic_report_descriptor;

		default:
			return hid_report_descriptor;
	}
}
#endif

#if 1

//--------------------------------------------------------------------+
// Configuration Descriptor
//--------------------------------------------------------------------+

//enum
//{
//  ITF_NUM_HID1_TEST,
//  ITF_NUM_HID2_TEST,
//  ITF_NUM_TOTAL_TEST
//};

//#define  CONFIG_TOTAL_LEN  (TUD_CONFIG_DESC_LEN + /*TUD_HID_DESC_LEN +*/ (9 + 16 + 7 + 7) + TUD_VENDOR_DESC_LEN)
#define  CONFIG_TOTAL_LEN  (TUD_CONFIG_DESC_LEN + /*TUD_HID_DESC_LEN +*/ (9 + 16 + 7 + 7) + TUD_HID_INOUT_DESC_LEN)
//#define  CONFIG_TOTAL_LEN  (TUD_CONFIG_DESC_LEN + TUD_HID_DESC_LEN + TUD_HID_DESC_LEN)

#define EPNUM_HID1          0x81
#define EPNUM_HID2          0x82
#define EPNUM_VENDOR_OUT    0x02
#define EPNUM_VENDOR_IN     EPNUM_HID2

uint8_t const desc_configuration[] =
{
#if 1
    0x09,        // bLength
    0x02,        // bDescriptorType (Configuration)
    //0x30, 0x00,  // wTotalLength 48
    U16_TO_U8S_LE(CONFIG_TOTAL_LEN),
    0x02,        // bNumInterfaces 1
    0x01,        // bConfigurationValue
    0x00,        // iConfiguration (String Index)
    0x80,        // bmAttributes
    0xFA,        // bMaxPower 500mA
#else
    // Config number, interface count, string index, total length, attribute, power in mA
    TUD_CONFIG_DESCRIPTOR(1, 2, 0, CONFIG_TOTAL_LEN, TUSB_DESC_CONFIG_ATT_REMOTE_WAKEUP, 100),
#endif

#if 1
    0x09,        // bLength
    0x04,        // bDescriptorType (Interface)
    0x00,        // bInterfaceNumber 0
    0x00,        // bAlternateSetting
    0x02,        // bNumEndpoints 2
    0xFF,        // bInterfaceClass
    0x5D,        // bInterfaceSubClass
    0x01,        // bInterfaceProtocol
    0x04,        // iInterface (String Index)
#if 1
    0x10,        // bLength
    0x21,        // bDescriptorType (HID)
    0x10, 0x01,  // bcdHID 1.10
    0x01,        // bCountryCode
    0x24,        // bNumDescriptors
    0x81,        // bDescriptorType[0] (Unknown 0x81)
    0x14, 0x03,  // wDescriptorLength[0] 788
    0x00,        // bDescriptorType[1] (Unknown 0x00)
    0x03, 0x13,  // wDescriptorLength[1] 4867
    0x01,        // bDescriptorType[2] (Unknown 0x02)
    0x00, 0x03,  // wDescriptorLength[2] 768
    0x00,        // bDescriptorType[3] (Unknown 0x00)
#else
    0x11, 0x21, 0x10, 0x01, 0x01, 0x25, 0x81, 0x14, 0x03, 0x03, 0x03, 0x04, 0x13, 0x02, 0x08, 0x03, 0x03,
#endif

#if 1
    0x07,        // bLength
    0x05,        // bDescriptorType (Endpoint)
    0x81,        // bEndpointAddress (IN/D2H)
    0x03,        // bmAttributes (Interrupt)
    0x20, 0x00,  // wMaxPacketSize 32
    0x01,        // bInterval 1 (unit depends on device speed)
#else
    0x07, 0x05, 0x81, 0x03, 0x40, 0x00, 0x01,
#endif
#if 1
    0x07,        // bLength
    0x05,        // bDescriptorType (Endpoint)
    0x01,        // bEndpointAddress (OUT/H2D)
    0x03,        // bmAttributes (Interrupt)
    0x20, 0x00,  // wMaxPacketSize 32
    0x08,        // bInterval 8 (unit depends on device speed)
#else
    0x07, 0x05, 0x02, 0x03, 0x40, 0x00, 0x01,
#endif

#else
  // Interface number, string index, protocol, report descriptor len, EP In address, size & polling interval
  TUD_HID_DESCRIPTOR(0x00, 4, HID_ITF_PROTOCOL_NONE, sizeof(desc_hid_report1), EPNUM_HID1, CFG_TUD_HID_EP_BUFSIZE, 10),
#endif
  //TUD_HID_DESCRIPTOR(0x01, 5, HID_ITF_PROTOCOL_NONE, sizeof(desc_hid_report2), EPNUM_HID2, CFG_TUD_HID_EP_BUFSIZE, 10)
  TUD_HID_INOUT_DESCRIPTOR(0x01, 5, HID_ITF_PROTOCOL_NONE, sizeof(desc_hid_report2), EPNUM_VENDOR_OUT, EPNUM_VENDOR_IN, CFG_TUD_HID_EP_BUFSIZE, 10)
  //TUD_VENDOR_DESCRIPTOR(0x01, 5, EPNUM_VENDOR_OUT, EPNUM_VENDOR_IN, CFG_TUD_HID_EP_BUFSIZE),
};

// Invoked when received GET CONFIGURATION DESCRIPTOR
// Application return pointer to descriptor
// Descriptor contents must exist long enough for transfer to complete
uint8_t const * tud_descriptor_configuration_cb(uint8_t index)
{
  (void) index; // for multiple configurations
  return desc_configuration;
}

#else
// Invoked when received GET CONFIGURATION DESCRIPTOR
// Application return pointer to descriptor
// Descriptor contents must exist long enough for transfer to complete
uint8_t const *tud_descriptor_configuration_cb(uint8_t index)
{
	switch (get_input_mode())
	{
		case INPUT_MODE_CONFIG:
			return web_tud_descriptor_configuration_cb(index);

		case INPUT_MODE_XINPUT:
			return xinput_configuration_descriptor;

		case INPUT_MODE_XBONE:
			return xbone_configuration_descriptor_cb(index);

		case INPUT_MODE_PS4:
			return ps4_configuration_descriptor;

		case INPUT_MODE_SWITCH:
			return switch_configuration_descriptor;

		case INPUT_MODE_KEYBOARD:
			return keyboard_configuration_descriptor;

		case INPUT_MODE_NEOGEO:
			return neogeo_configuration_descriptor;

		case INPUT_MODE_MDMINI:
			return mdmini_configuration_descriptor;

		case INPUT_MODE_PCEMINI:
			return pcengine_configuration_descriptor;

		case INPUT_MODE_EGRET:
			return egret_configuration_descriptor;

		case INPUT_MODE_ASTRO:
			return astro_configuration_descriptor;

		case INPUT_MODE_PSCLASSIC:
			return psclassic_configuration_descriptor;

		case INPUT_MODE_XBOXORIGINAL:
			return xboxoriginal_configuration_descriptor;

		default:
			return hid_configuration_descriptor;
	}
}
#endif

uint8_t const* tud_descriptor_device_qualifier_cb(void) {
	switch (get_input_mode())
	{
		case INPUT_MODE_XBONE:
			return xbone_device_qualifier;
		default:
			return nullptr;
	}
}
