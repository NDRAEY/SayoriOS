// Fuck-Its-Ass driver for Intel E1000

#include "drv/pci.h"
#include "io/ports.h"
#include "common.h"

#define E1000_VEN 0x8086
#define E1000_DEV 0x100E

uint8_t e1000_bus = 0, e1000_slot = 0, e1000_func = 0;

void e1000_init() {
	pci_find_device(E1000_VEN, E1000_DEV, &e1000_bus, &e1000_slot, &e1000_func);

	uint16_t id = pci_get_device(e1000_bus, e1000_slot, e1000_func);

	if(id == PCI_VENDOR_NO_DEVICE) {
		qemu_err("No Intel E1000");
		return;
	} else {
		qemu_ok("Found Intel E1000!");
	}
}
