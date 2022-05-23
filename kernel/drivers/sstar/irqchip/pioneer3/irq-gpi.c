/*
* irq-gpi.c- Sigmastar
*
* Copyright (c) [2019~2020] SigmaStar Technology.
*
*
* This software is licensed under the terms of the GNU General Public
* License version 2, as published by the Free Software Foundation, and
* may be copied, distributed, and modified under those terms.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License version 2 for more details.
*
*/
#include <linux/init.h>
#include <linux/io.h>
#include <linux/irqchip/chained_irq.h>
#include <linux/irqdomain.h>
#include <linux/of.h>
#include <linux/syscore_ops.h>
#include <asm/mach/irq.h>
#include <linux/irqchip/arm-gic.h>
#include <linux/irqchip.h>
#include <linux/irqdesc.h>

#include <dt-bindings/interrupt-controller/arm-gic.h>

#include "irqs.h"
#include "registers.h"

#include "_ms_private.h"
#include "ms_platform.h"
#include "ms_types.h"

#include <linux/of.h>
#include <linux/of_irq.h>
#include <linux/interrupt.h>
#ifdef CONFIG_PM_SLEEP
/**
 * struct ss_gpi_irq_priv - private gpi interrupt data
 * @polarity:   fiq polarity
 */
struct ss_gpi_irq_priv {
    U16     polarity[(GPI_FIQ_NUM+15)>>4];
};

static struct ss_gpi_irq_priv gpi_irq_priv;
#endif
static void ss_gpi_irq_ack(struct irq_data *d)
{
    U16 gpi_irq;

    if(!d)
    {
        dump_stack();
        return;
    }
    gpi_irq = d->hwirq;
    pr_debug("[%s] hw:%d\n", __FUNCTION__, gpi_irq);

    if( gpi_irq >= 0 && gpi_irq < GPI_FIQ_END )
    {
        SETREG16( (BASE_REG_GPI_INT_PA + REG_ID_0A + (gpi_irq/16)*4 ) , (1 << (gpi_irq%16)) );
		INREG16(BASE_REG_MAILBOX_PA);//read a register make ensure the previous write command was compeleted
    }
    else
    {
        pr_err("[%s] Unknown hwirq %lu\n", __func__, d->hwirq);
        return;
    }
}

static void ss_gpi_irq_mask(struct irq_data *d)
{
    U16 gpi_irq;

    gpi_irq = d->hwirq;
    pr_debug("[%s] hw:%d \n",__FUNCTION__, gpi_irq);

    if( gpi_irq >= 0 && gpi_irq < GPI_FIQ_END )
    {
        SETREG16( (BASE_REG_GPI_INT_PA + REG_ID_00 + (gpi_irq/16)*4 ) , (1 << (gpi_irq%16)) );
    }
    else
    {
        pr_err("[%s] Unknown hwirq %lu\n", __func__, d->hwirq);
        return;
    }
}

static void ss_gpi_irq_unmask(struct irq_data *d)
{
    U16 gpi_irq;

    gpi_irq = d->hwirq;
    pr_debug("[%s] hw:%d \n",__FUNCTION__, gpi_irq);

    if( gpi_irq >= 0 && gpi_irq < GPI_FIQ_END )
    {
        CLRREG16( (BASE_REG_GPI_INT_PA + REG_ID_00 + (gpi_irq/16)*4 ) , (1 << (gpi_irq%16)) );
        OUTREGMSK16(BASE_REG_INTRCTL_PA + REG_ID_57, 0x0, 0x0100); //unmask parent
    }
    else
    {
        pr_err("[%s] Unknown hwirq %lu\n", __func__, d->hwirq);
        return;
    }
}


static int ss_gpi_irq_set_type(struct irq_data *d, unsigned int type)
{
    U16 gpi_irq;
    pr_debug("%s %d type:0x%08x\n", __FUNCTION__, __LINE__, type);

    gpi_irq = d->hwirq;

    if( gpi_irq >= 0 && gpi_irq < GPI_FIQ_END )
    {

        if( (gpi_irq == INT_GPI_FIQ_PAD_KEY0 || gpi_irq == INT_GPI_FIQ_PAD_KEY5) &&
                type != IRQ_TYPE_EDGE_BOTH )
        {
            OUTREGMSK16(BASE_REG_GPI_INT_PA + REG_ID_22, 0x0, 0x3);
        }
        switch(type)
        {
            case IRQ_TYPE_EDGE_FALLING:
                SETREG16( (BASE_REG_GPI_INT_PA + REG_ID_10 + (gpi_irq/16)*4 ) , (1 << (gpi_irq%16)) );
                break;
            case IRQ_TYPE_EDGE_RISING:
                CLRREG16( (BASE_REG_GPI_INT_PA + REG_ID_10 + (gpi_irq/16)*4 ) , (1 << (gpi_irq%16)) );
                break;
            case IRQ_TYPE_EDGE_BOTH:

                if( (gpi_irq == INT_GPI_FIQ_PAD_KEY0 || gpi_irq == INT_GPI_FIQ_PAD_KEY5) &&
                            type == IRQ_TYPE_EDGE_BOTH )
                {
                        CLRREG16( (BASE_REG_GPI_INT_PA + REG_ID_10 + (gpi_irq/16)*4 ) , (1 << (gpi_irq%16)) );
                        // Enable rise/fall int
                        if(gpi_irq == INT_GPI_FIQ_PAD_KEY0)
                            SETREG16(BASE_REG_GPI_INT_PA + REG_ID_22, 0x1);
                        else
                            SETREG16(BASE_REG_GPI_INT_PA + REG_ID_22, 0x2);
                }
                else
                {
                    pr_err("[%s] both-edge trigger doesn't supported @ irq(%u)\n", __func__, gpi_irq);
                    return -EINVAL;
                }
                break;
            default:
                return -EINVAL;

        }
        // prevent the very 1st unexpected trigger right after irq_request()
        // http://mantis.sigmastar.com.tw/view.php?id=1688845
        ss_gpi_irq_ack(d);
    }
    else
    {
        pr_err("[%s] Unknown hwirq %lu\n", __func__, d->hwirq);
        return -EINVAL;
    }

    return 0;
}

#ifdef CONFIG_PM_SLEEP
static int ss_gpi_irq_set_wake(struct irq_data *d, unsigned int enable)
{
    U16 gpi_irq;

    gpi_irq = d->hwirq;
    pr_err("[%s] hw:%d enable? %d \n",__FUNCTION__, gpi_irq, enable);

    if (enable){
        ss_gpi_irq_unmask(d);
    }
    else
    {
        ss_gpi_irq_mask(d);
    }
	return 0;
}

#endif

struct irq_chip ss_gpi_intc_irqchip = {
    .name = "MS_GPI_INTC",
    .irq_ack = ss_gpi_irq_ack,
    .irq_eoi = ss_gpi_irq_ack,
    .irq_mask = ss_gpi_irq_mask,
    .irq_unmask = ss_gpi_irq_unmask,
    .irq_set_type = ss_gpi_irq_set_type,
#ifdef CONFIG_PM_SLEEP
    .irq_enable = ss_gpi_irq_unmask,
    .irq_disable = ss_gpi_irq_mask,
    .irq_set_wake = ss_gpi_irq_set_wake,
#endif
};
EXPORT_SYMBOL(ss_gpi_intc_irqchip);


static void ss_handle_cascade_gpi(struct irq_desc *desc)
{
    unsigned int cascade_irq = 0xFFFFFFFF, i, j;
    unsigned int virq=0xFFFFFFFF;
    struct irq_chip *chip = irq_desc_get_chip(desc);
    struct irq_domain *domain = irq_desc_get_handler_data(desc);
    unsigned int final_status;

    if(!domain)
    {
        printk("[%s] err %d \n", __FUNCTION__, __LINE__);
        goto exit;
    }

    for (j=0; j<=GPI_FIQ_NUM/16; j++)
    {
        final_status = INREG16(BASE_REG_GPI_INT_PA + REG_ID_30 + j*4);
        for(i=0; i<16 && final_status!=0; i++)
        {
            if(0 !=(final_status & (1<<i)))
            {
                cascade_irq = i + j*16;
                goto handle_int;
            }
        }
    }

    if(0xFFFFFFFF==cascade_irq)
    {
        pr_err("[%s:%d] error final_status:%d 0x%04X virq:%d\n", __FUNCTION__, __LINE__, cascade_irq, final_status, virq);
#if 0   // FIXME: goodix touch panel frequently run into this case...
        panic("[%s] error %d \n", __FUNCTION__, __LINE__);
#endif
        chained_irq_exit(chip, desc);
        goto exit;
    }

handle_int:
    virq = irq_find_mapping(domain, cascade_irq);
    if(!virq)
    {
        printk("[%s] err %d cascade_irq:%d\n", __FUNCTION__, __LINE__, cascade_irq);
        goto exit;
    }
    pr_debug("%s %d final_status:%d 0x%04X virq:%d\n", __FUNCTION__, __LINE__, cascade_irq, final_status, virq);
    chained_irq_enter(chip, desc);
    generic_handle_irq(virq);

exit:
    chained_irq_exit(chip, desc);
}

static int ss_gpi_intc_domain_translate(struct irq_domain *domain, struct irq_fwspec *fwspec,
                    unsigned long *hwirq, unsigned int *type)
{
    if (is_of_node(fwspec->fwnode)) {
        if (fwspec->param_count != 1)
            return -EINVAL;
        *hwirq = fwspec->param[0];
        return 0;
    }

    return -EINVAL;
}

static int ss_gpi_intc_domain_alloc(struct irq_domain *domain, unsigned int virq,
                                     unsigned int nr_irqs, void *data)
{
    struct irq_fwspec *fwspec = data;
    struct irq_fwspec parent_fwspec;
    irq_hw_number_t hwirq;
    unsigned int i;

    if (fwspec->param_count != 1)
        return -EINVAL;

    hwirq = fwspec->param[0];

    for (i = 0; i < nr_irqs; i++)
    {
        irq_domain_set_info(domain, virq + i, hwirq + i , &ss_gpi_intc_irqchip, NULL, handle_edge_irq, NULL, NULL);
        pr_err("[%s] hw:%d -> v:%d\n", __FUNCTION__, (unsigned int)hwirq+i, virq+i);
    }

    parent_fwspec = *fwspec;

    return 0;
}

static void ss_gpi_intc_domain_free(struct irq_domain *domain, unsigned int virq, unsigned int nr_irqs)
{
    unsigned int i;

    for (i = 0; i < nr_irqs; i++) {
        struct irq_data *d = irq_domain_get_irq_data(domain, virq + i);
        irq_domain_reset_irq_data(d);
    }
}


struct irq_domain_ops ss_gpi_intc_domain_ops = {
    .translate  = ss_gpi_intc_domain_translate,
    .alloc      = ss_gpi_intc_domain_alloc,
    .free       = ss_gpi_intc_domain_free,
};


#ifdef CONFIG_PM_SLEEP
static int ss_gpi_intc_suspend(void)
{
    unsigned int i, num;

    num = (GPI_FIQ_NUM + 15) >> 4;
    for (i = 0; i < num; i++) {
        gpi_irq_priv.polarity[i] = INREG16(BASE_REG_GPI_INT_PA + REG_ID_18 + (i << 2));
    }
//    //interrupt-parent
//    gpi_irq_priv.irqGpiStatus = INREG16(BASE_REG_GPI_INT_PA + REG_ID_57)&0xFEFF;
//    printk("ss_gpi_intc_suspend %d \n\n", gpi_irq_priv.irqGpiStatus);
    return 0;
}

static void ss_gpi_intc_resume(void)
{
    unsigned int i, num;

    num = (GPI_FIQ_NUM + 15) >> 4;
    for (i = 0; i < num; i++) {
        OUTREG16(BASE_REG_GPI_INT_PA + REG_ID_18 + (i << 2), gpi_irq_priv.polarity[i]);
    }
}

struct syscore_ops ss_gpi_intc_syscore_ops = {
    .suspend = ss_gpi_intc_suspend,
    .resume = ss_gpi_intc_resume,
};
#endif

struct irq_domain *ss_gpi_irq_domain;

static int __init ss_init_gpi_intc(struct device_node *np, struct device_node *interrupt_parent)
{
    struct irq_domain *parent_domain;
    int irq=0;
    if (!interrupt_parent)
    {
        pr_err("%s: %s no parent\n", __func__, np->name);
        return -ENODEV;
    }

    pr_err("%s: np->name=%s, parent=%s\n", __func__, np->name, interrupt_parent->name);

    parent_domain = irq_find_host(interrupt_parent);
    if (!parent_domain)
    {
        pr_err("%s: %s unable to obtain parent domain\n", __func__, np->name);
        return -ENXIO;
    }

    ss_gpi_irq_domain = irq_domain_add_hierarchy(parent_domain, 0,
                    PMSLEEP_FIQ_NR,
                    np, &ss_gpi_intc_domain_ops, NULL);

    if (!ss_gpi_irq_domain)
    {
        pr_err("%s: %s allocat domain fail\n", __func__, np->name);
        return -ENOMEM;
    }

    irq = irq_of_parse_and_map(np, 0);
    if (!irq)
    {
        pr_err("Get irq err from DTS\n");
        return -EPROBE_DEFER;
    }

    irq_set_chained_handler_and_data(irq, ss_handle_cascade_gpi, ss_gpi_irq_domain);

    return 0;
}

IRQCHIP_DECLARE(ss_gpi_intc, "sstar,gpi-intc", ss_init_gpi_intc);
