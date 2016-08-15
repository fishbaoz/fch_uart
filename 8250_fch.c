/*
 *  Probe for F81216A LPC to 4 UART
 *
 *  Based on drivers/tty/serial/8250_pnp.c, by Russell King, et al
 *
 *  Copyright (C) 2014 Ricardo Ribalda, Qtechnology A/S
 *
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License.
 */
#include <linux/module.h>
#include <linux/pci.h>
#include <linux/pnp.h>
#include <linux/acpi.h>
#include <linux/kernel.h>
#include <linux/serial_core.h>
#include  "8250.h"

//#define DRIVER_NAME "8250_fch"
#define DRIVER_NAME "serial8250"

static int
fch_8250_probe(struct pnp_dev *dev, const struct pnp_device_id *dev_id)
{
	int line;
	struct uart_8250_port uart;

	memset(&uart, 0, sizeof(uart));
	uart.bugs = UART_BUG_TXEN;
	uart.capabilities = UART_CAP_FIFO;
	uart.port.irq = 0xA; //pnp_irq(dev, 0);
	uart.port.mapbase = 0xfedc6000;
	uart.port.membase = ioremap_nocache(/* &dev->dev,  */0xfedc6000, 0x1000);

//	printk(KERN_INFO "membase=%x\n", uart.port.membase);
	uart.port.iotype = UPIO_MEM32;

	uart.port.flags |= /* UPF_SKIP_TEST | */ UPF_BOOT_AUTOCONF /*| UPF_HARD_FLOW*/;// | UPF_SPD_CUST;
	uart.port.regshift = 2;
//	if (pnp_irq_flags(dev, 0) & IORESOURCE_IRQ_SHAREABLE)
	uart.port.flags |= UPF_SHARE_IRQ | UPF_IOREMAP;
	uart.port.uartclk = 48000000;
	uart.port.dev = &dev->dev;

	line = serial8250_register_8250_port(&uart);
	//printk(KERN_INFO "line=%x\n", line);
	if (line < 0)
		return -ENODEV;

	pnp_set_drvdata(dev, (void *)((long)line));
	return 0;
}

static void fch_8250_remove(struct pnp_dev *dev)
{
	long line = (long)pnp_get_drvdata(dev);

	if (line)
		serial8250_unregister_port(line);
}

#if 0 //def CONFIG_PM
static int fch_8250_suspend(struct pnp_dev *dev, pm_message_t state)
{
	long line = (long)pnp_get_drvdata(dev);

	if (!line)
		return -ENODEV;
	serial8250_suspend_port(line - 1);
	return 0;
}

static int fch_8250_resume(struct pnp_dev *dev)
{
	long line = (long)pnp_get_drvdata(dev);

	if (!line)
		return -ENODEV;
	serial8250_resume_port(line - 1);
	return 0;
}
#else
#define fch_8250_suspend NULL
#define fch_8250_resume NULL
#endif /* CONFIG_PM */

static const struct pnp_device_id fch_dev_table[] = {
//	{ "PNP0501"},
//	{ "AMD0020"},
	{ "ANYDEVS"},
	{}
};

MODULE_DEVICE_TABLE(pnp, fch_dev_table);

static struct pnp_driver fch_8250_driver = {
	.name		= DRIVER_NAME,
	.probe		= fch_8250_probe,
	.remove		= fch_8250_remove,
	.suspend	= fch_8250_suspend,
	.resume		= fch_8250_resume,
	.id_table	= fch_dev_table,
};

static int fch_8250_init(void)
{
	//printk(KERN_INFO "fch_8250_init\n");
	return pnp_register_driver(&fch_8250_driver);
}
module_init(fch_8250_init);

static void fch_8250_exit(void)
{
	pnp_unregister_driver(&fch_8250_driver);
}
module_exit(fch_8250_exit);

MODULE_DESCRIPTION("Fch Carrizo module");
MODULE_AUTHOR("Zheng Bao <fishbaozi@gmail.com>");
MODULE_LICENSE("GPL");
