#ifndef _WIFI_RESTART_H_
#define _WIFI_RESTART_H_

#define LOG_FILE "/etc/wifi_monitor"
#define MODULE_PATH "/lib/modules/4.14.22+"
#define MODULE_NAME "mwlwifi"
#define PCIE_RESET_PATH "/sys/class/leds/pcie-reset/brightness"
#define PCIE_REMOVE_PATH "/sys/devices/platform/soc/d0070000.pcie/pci0000:00/0000:00:00.0/remove"
#define PCIE_RESCAN_PATH "/sys/devices/platform/soc/d0070000.pcie/pci0000:00/pci_bus/0000:00/rescan"
#define MRVL_OUI        0x005043

struct nl80211_global {
	struct nl_sock *nl_sock;
	struct nl_sock *nl_event;
	struct nl_cb *nl_cb;
	int nl80211_id;
	int ready;
};

enum DRIVER_STATE {
	DRIVER_UNLOADED,
	DRIVER_LOADING,
	DRIVER_LOADED,
	DRIVER_CMD_TIMEOUT,
};

enum SERVICE_STATE {
	SERVICE_UNKNOWN,
	SERVICE_UP,
	SERVICE_DOWN
};

enum mwl_vendor_events {
	MWL_VENDOR_EVENT_DRIVER_READY,
	MWL_VENDOR_EVENT_DRIVER_START_REMOVE,
	MWL_VENDOR_EVENT_CMD_TIMEOUT,

	__MWL_VENDOR_EVENT_AFTER_LAST,
	NUM_MWL_VENDOR_EVENT = __MWL_VENDOR_EVENT_AFTER_LAST,
	MWL_VENDOR_EVENT_MAX = __MWL_VENDOR_EVENT_AFTER_LAST - 1
};

#endif
