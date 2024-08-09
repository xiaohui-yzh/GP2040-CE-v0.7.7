/*
 * SPDX-License-Identifier: MIT
 * SPDX-FileCopyrightText: Copyright (c) 2021 Jason Skuby (mytechtoybox.com)
 */

#include <stdint.h>

#include "tusb_config.h"
#include "tusb.h"
#include "class/hid/hid.h"
#include "device/usbd_pvt.h"

#include "gamepad/GamepadDescriptors.h"

#include "usb_driver.h"
#include "net_driver.h"
#include "hid_driver.h"
#include "xinput_driver.h"
#include "ps4_driver.h"
#include "xid_driver/xid_driver.h"
#include "xbone_driver.h"

UsbMode usb_mode = USB_MODE_HID;
InputMode input_mode = INPUT_MODE_XINPUT;
static bool usb_mounted = false;
static bool usb_suspended = false;

InputMode get_input_mode(void)
{
	return input_mode;
}

bool get_usb_mounted(void)
{
	return usb_mounted;
}

bool get_usb_suspended(void)
{
	return usb_suspended;
}

void initialize_driver(InputMode mode)
{
	input_mode = mode;
	if (mode == INPUT_MODE_CONFIG)
		usb_mode = USB_MODE_NET;

	tud_init(TUD_OPT_RHPORT);
}

void receive_report(uint8_t *buffer)
{
#if 1
	if (input_mode == INPUT_MODE_XINPUT)
	{
		receive_xinput_report();
		memcpy(buffer, xinput_out_buffer, XINPUT_OUT_SIZE);
	}
#endif
}

bool send_report(void *report, uint16_t report_size)
{
	static uint8_t previous_report[CFG_TUD_ENDPOINT0_SIZE] = { };

    uint8_t xIndex = 0;
    if (input_mode == INPUT_MODE_XBOXORIGINAL) {
        xIndex = xid_get_index_by_type(0, XID_TYPE_GAMECONTROLLER);
    }

	bool sent = false;

	if (tud_suspended())
		tud_remote_wakeup();

	if (memcmp(previous_report, report, report_size) != 0)
	{
		switch (input_mode)
		{
			case INPUT_MODE_XBOXORIGINAL:
				sent = xid_send_report(xIndex, report, report_size);
				break;
			case INPUT_MODE_XINPUT:
//			    printf("%s:%d\n", __FUNCTION__, __LINE__);
				sent = send_xinput_report(report, report_size);
				break;
			case INPUT_MODE_XBONE:
				sent = send_xbone_report(report, report_size);
				break;
			case INPUT_MODE_KEYBOARD:
				sent = send_keyboard_report(report);
				break;
			default:
				sent = send_hid_report(0, report, report_size);
				break;
		}

		if (sent)
			memcpy(previous_report, report, report_size);
	}

	return sent;
}

// Some input drivers need their own process/update logic
void update_input_driver() {
	switch (input_mode)
	{
		case INPUT_MODE_XBONE:
			xbone_driver_update();
			break;
		default:
			break;
	};
}

/* USB Driver Callback (Required for XInput) */
const usbd_class_driver_t *usbd_app_driver_get_cb(uint8_t *driver_count)
{
#if 0
    static usbd_class_driver_t usbd_class_driver[2];
    *driver_count = 1;

    usbd_class_driver[0] = xinput_driver;
    usbd_class_driver[1] = vendord_driver;

    return usbd_class_driver;
#else

	*driver_count = 1;

	if (usb_mode == USB_MODE_NET)
	{
		return &net_driver;
	}
	else
	{
		switch (input_mode)
		{
			case INPUT_MODE_XINPUT:
			    printf("%s:%d xinput_driver\n", __FUNCTION__, __LINE__);
				return &xinput_driver;

			case INPUT_MODE_PS4:
				return &ps4_driver;

			case INPUT_MODE_XBOXORIGINAL:
				return xid_get_driver();

			case INPUT_MODE_XBONE:
				return &xbone_driver;

			default:
				return &hid_driver;
		}
	}
#endif
}

/* USB HID Callbacks (Required) */

// Invoked when received GET_REPORT control request
// Application must fill buffer report's content and return its length.
// Return zero will cause the stack to STALL request
uint16_t tud_hid_get_report_cb(uint8_t itf, uint8_t report_id, hid_report_type_t report_type, uint8_t *buffer, uint16_t reqlen)
{
	// TODO: Handle the correct report type, if required
	(void)itf;
	printf("%s:%d\n", __FUNCTION__, __LINE__);
#if 1
	uint8_t report_size = 0;
	SwitchReport switch_report;
	HIDReport hid_report;
	KeyboardReport keyboard_report;
	PS4Report ps4_report;
	NeogeoReport neogeo_report;
	MDMiniReport mdmini_report;
	PCEngineReport pcengine_report;
	EgretReport egret_report;
	AstroReport astro_report;
	PSClassicReport psclassic_report;
	switch (input_mode)
	{
		case INPUT_MODE_SWITCH:
			report_size = sizeof(SwitchReport);
			memcpy(buffer, &switch_report, report_size);
			break;
		case INPUT_MODE_NEOGEO:
			report_size = sizeof(NeogeoReport);
			memcpy(buffer, &neogeo_report, report_size);
			break;
		case INPUT_MODE_MDMINI:
			report_size = sizeof(MDMiniReport);
			memcpy(buffer, &mdmini_report, report_size);
			break;
		case INPUT_MODE_PCEMINI:
			report_size = sizeof(PCEngineReport);
			memcpy(buffer, &pcengine_report, report_size);
			break;
		case INPUT_MODE_EGRET:
			report_size = sizeof(EgretReport);
			memcpy(buffer, &egret_report, report_size);
			break;
		case INPUT_MODE_ASTRO:
			report_size = sizeof(AstroReport);
			memcpy(buffer, &astro_report, report_size);
			break;
		case INPUT_MODE_PSCLASSIC:
			report_size = sizeof(PSClassicReport);
			memcpy(buffer, &psclassic_report, report_size);
			break;
		case INPUT_MODE_KEYBOARD:
			report_size = report_id == KEYBOARD_KEY_REPORT_ID ? sizeof(KeyboardReport::keycode) : sizeof(KeyboardReport::multimedia);
			memcpy(buffer, report_id == KEYBOARD_KEY_REPORT_ID ?
				(void*) keyboard_report.keycode : (void*) &keyboard_report.multimedia, report_size);
			break;
		case INPUT_MODE_PS4:
			if ( report_type == HID_REPORT_TYPE_FEATURE ) {
				// Get feature report (for Auth)
				report_size = get_ps4_report(report_id, buffer, reqlen);
			} else {
				report_size = sizeof(PS4Report);
				memcpy(buffer, &ps4_report, report_size);
			}
			break;
		default:
		    printf("%s:%d\n", __FUNCTION__, __LINE__);
			report_size = sizeof(HIDReport);
			memcpy(buffer, &hid_report, report_size);
			break;
	}

	return report_size;
#else
	return 0;
#endif
}

// Invoked when received SET_REPORT control request or
// received data on OUT endpoint ( Report ID = 0, Type = 0 )
void tud_hid_set_report_cb(uint8_t itf, uint8_t report_id, hid_report_type_t report_type, uint8_t const *buffer, uint16_t bufsize)
{
	(void) itf;
#if 1
	switch (input_mode)
	{
		case INPUT_MODE_PS4:
			if ( report_type == HID_REPORT_TYPE_FEATURE ) {
				set_ps4_report(report_id, buffer, bufsize);
			}
			break;
		default:
			break;
	}

	 printf("%s:%d\n", __FUNCTION__, __LINE__);
	// echo back anything we received from host
	tud_hid_report(report_id, buffer, bufsize);
#else
	return;
#endif
}


/* Device callbacks (Optional) */

// Invoked when device is mounted
void tud_mount_cb(void)
{
	usb_mounted = true;
}

// Invoked when device is unmounted
void tud_umount_cb(void)
{
	usb_mounted = false;
}

// Invoked when usb bus is suspended
// remote_wakeup_en : if host allow us  to perform remote wakeup
// Within 7ms, device must draw an average of current less than 2.5 mA from bus
void tud_suspend_cb(bool remote_wakeup_en)
{
	(void)remote_wakeup_en;
	usb_suspended = true;
}

// Invoked when usb bus is resumed
void tud_resume_cb(void)
{
	usb_suspended = false;
}

// Vendor Controlled XFER occured
#if 1
bool tud_vendor_control_xfer_cb(uint8_t rhport, uint8_t stage,
                                tusb_control_request_t const *request)
{
	bool ret = false;
	switch (input_mode)
	{
    case INPUT_MODE_XBOXORIGINAL:
      ret |= xid_get_driver()->control_xfer_cb(rhport, stage, request);
      break;
		case INPUT_MODE_XBONE:
			ret = xbone_vendor_control_xfer_cb(rhport, stage, request);
			break;
		default:
			break;
	}
	return ret;
}
#else

// Microsoft Compatible ID Descriptor
const uint8_t msCompatibleIdDescriptor[] = {
    // Microsoft Compatible ID Header
    0x28, 0x00, 0x00, 0x00, // dwLength (40 bytes)
    0x00, 0x01, // bcdVersion
    0x04, 0x00, // wIndex (Extended Compat ID Descriptor)
    0x01, // bCount (Number of Function Sections)
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // Reserved

    // Function Section
    0x00, // bFirstInterfaceNumber (Interface 0)
    0x01, // Reserved
    'X', 'U', 'S', 'B', '1', '0', 0x00, 0x00, // Compatible ID (XUSB10)
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 // Sub-Compatible ID (NULL)
};

// 自定义控制请求处理
bool tud_vendor_control_xfer_cb(uint8_t rhport, uint8_t stage, tusb_control_request_t const * request)
{
    printf("%s:%d\n", __FUNCTION__, __LINE__);
    if (stage == CONTROL_STAGE_SETUP)
    {
        printf("%s:%d\n", __FUNCTION__, __LINE__);
        // 处理 GET_DESCRIPTOR 请求
        if (request->bmRequestType == 0xC0 && request->bRequest == 0x20)
        {
            printf("%s:%d\n", __FUNCTION__, __LINE__);
            // 检查是否请求 Microsoft OS 1.0 兼容 ID 描述符
            if (request->wIndex == 0x04)
            {
                // 返回兼容 ID 描述符
                printf("%s:%d\n", __FUNCTION__, __LINE__);
                return tud_control_xfer(rhport, request, (void*)msCompatibleIdDescriptor, sizeof(msCompatibleIdDescriptor));
            }
        }
    }

    // 使用默认控制请求处理程序处理其他请求
    return false;
}
#endif
