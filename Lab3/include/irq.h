#define physical_add 0x3F000000

#define IRQ_BASIC_PENDING ((volatile unsigned int *)(physical_add + 0x0000B200))
#define IRQ_PENDING_1 ((volatile unsigned int *)(physical_add + 0x0000B204))
#define IRQ_PENDING_2 ((volatile unsigned int *)(physical_add + 0x0000B208))
#define FIQ_CONTROL ((volatile unsigned int *)(physical_add + 0x0000B20C))
#define ENABLE_IRQS_1 ((volatile unsigned int *)(physical_add + 0x0000B210))
#define ENABLE_IRQS_2 ((volatile unsigned int *)(physical_add + 0x0000B214))
#define ENABLE_BASIC_IRQS ((volatile unsigned int *)(physical_add + 0x0000B218))
#define DISABLE_IRQS_1 ((volatile unsigned int *)(physical_add + 0x0000B21C))
#define DISABLE_IRQS_2 ((volatile unsigned int *)(physical_add + 0x0000B220))
#define DISABLE_BASIC_IRQS ((volatile unsigned int *)(physical_add + 0x0000B224))

#define IRQ_PENDING_1_AUX_INT (1 << 29)

#define CORE0_INT_SRC ((volatile unsigned int *)(0x40000060))
#define CORE0_INT_SRC_GPU (1 << 8)
#define CORE0_INT_SRC_TIMER (1 << 1)
