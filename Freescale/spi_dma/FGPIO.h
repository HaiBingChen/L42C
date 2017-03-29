

/* ----------------------------------------------------------------------------
   -- FGPIO
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup FGPIO_Peripheral FGPIO
 * @{
 */

/** FGPIO - Peripheral register structure */
typedef struct FGPIO_MemMap {
  uint32_t PDOR;                                   /**< Port Data Output Register, offset: 0x0 */
  uint32_t PSOR;                                   /**< Port Set Output Register, offset: 0x4 */
  uint32_t PCOR;                                   /**< Port Clear Output Register, offset: 0x8 */
  uint32_t PTOR;                                   /**< Port Toggle Output Register, offset: 0xC */
  uint32_t PDIR;                                   /**< Port Data Input Register, offset: 0x10 */
  uint32_t PDDR;                                   /**< Port Data Direction Register, offset: 0x14 */
} volatile *FGPIO_MemMapPtr;

/* ----------------------------------------------------------------------------
   -- FGPIO - Register accessor macros
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup FGPIO_Register_Accessor_Macros FGPIO - Register accessor macros
 * @{
 */


/* FGPIO - Register accessors */
#define FGPIO_PDOR_REG(base)                     ((base)->PDOR)
#define FGPIO_PSOR_REG(base)                     ((base)->PSOR)
#define FGPIO_PCOR_REG(base)                     ((base)->PCOR)
#define FGPIO_PTOR_REG(base)                     ((base)->PTOR)
#define FGPIO_PDIR_REG(base)                     ((base)->PDIR)
#define FGPIO_PDDR_REG(base)                     ((base)->PDDR)

/*!
 * @}
 */ /* end of group FGPIO_Register_Accessor_Macros */


/* ----------------------------------------------------------------------------
   -- FGPIO Register Masks
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup FGPIO_Register_Masks FGPIO Register Masks
 * @{
 */

/* PDOR Bit Fields */
#define FGPIO_PDOR_PDO_MASK                      0xFFFFFFFFu
#define FGPIO_PDOR_PDO_SHIFT                     0
#define FGPIO_PDOR_PDO(x)                        (((uint32_t)(((uint32_t)(x))<<FGPIO_PDOR_PDO_SHIFT))&FGPIO_PDOR_PDO_MASK)
/* PSOR Bit Fields */
#define FGPIO_PSOR_PTSO_MASK                     0xFFFFFFFFu
#define FGPIO_PSOR_PTSO_SHIFT                    0
#define FGPIO_PSOR_PTSO(x)                       (((uint32_t)(((uint32_t)(x))<<FGPIO_PSOR_PTSO_SHIFT))&FGPIO_PSOR_PTSO_MASK)
/* PCOR Bit Fields */
#define FGPIO_PCOR_PTCO_MASK                     0xFFFFFFFFu
#define FGPIO_PCOR_PTCO_SHIFT                    0
#define FGPIO_PCOR_PTCO(x)                       (((uint32_t)(((uint32_t)(x))<<FGPIO_PCOR_PTCO_SHIFT))&FGPIO_PCOR_PTCO_MASK)
/* PTOR Bit Fields */
#define FGPIO_PTOR_PTTO_MASK                     0xFFFFFFFFu
#define FGPIO_PTOR_PTTO_SHIFT                    0
#define FGPIO_PTOR_PTTO(x)                       (((uint32_t)(((uint32_t)(x))<<FGPIO_PTOR_PTTO_SHIFT))&FGPIO_PTOR_PTTO_MASK)
/* PDIR Bit Fields */
#define FGPIO_PDIR_PDI_MASK                      0xFFFFFFFFu
#define FGPIO_PDIR_PDI_SHIFT                     0
#define FGPIO_PDIR_PDI(x)                        (((uint32_t)(((uint32_t)(x))<<FGPIO_PDIR_PDI_SHIFT))&FGPIO_PDIR_PDI_MASK)
/* PDDR Bit Fields */
#define FGPIO_PDDR_PDD_MASK                      0xFFFFFFFFu
#define FGPIO_PDDR_PDD_SHIFT                     0
#define FGPIO_PDDR_PDD(x)                        (((uint32_t)(((uint32_t)(x))<<FGPIO_PDDR_PDD_SHIFT))&FGPIO_PDDR_PDD_MASK)

/*!
 * @}
 */ /* end of group FGPIO_Register_Masks */


/* FGPIO - Peripheral instance base addresses */
/** Peripheral FPTA base pointer */
#define FPTA_BASE_PTR                            ((FGPIO_MemMapPtr)0xF80FF000u)
/** Peripheral FPTB base pointer */
#define FPTB_BASE_PTR                            ((FGPIO_MemMapPtr)0xF80FF040u)
/** Peripheral FPTC base pointer */
#define FPTC_BASE_PTR                            ((FGPIO_MemMapPtr)0xF80FF080u)
/** Peripheral FPTD base pointer */
#define FPTD_BASE_PTR                            ((FGPIO_MemMapPtr)0xF80FF0C0u)
/** Peripheral FPTE base pointer */
#define FPTE_BASE_PTR                            ((FGPIO_MemMapPtr)0xF80FF100u)
/** Array initializer of FGPIO peripheral base pointers */
#define FGPIO_BASE_PTRS                          { FPTA_BASE_PTR, FPTB_BASE_PTR, FPTC_BASE_PTR, FPTD_BASE_PTR, FPTE_BASE_PTR }

/* ----------------------------------------------------------------------------
   -- FGPIO - Register accessor macros
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup FGPIO_Register_Accessor_Macros FGPIO - Register accessor macros
 * @{
 */


/* FGPIO - Register instance definitions */
/* FPTA */
#define FGPIOA_PDOR                              FGPIO_PDOR_REG(FPTA_BASE_PTR)
#define FGPIOA_PSOR                              FGPIO_PSOR_REG(FPTA_BASE_PTR)
#define FGPIOA_PCOR                              FGPIO_PCOR_REG(FPTA_BASE_PTR)
#define FGPIOA_PTOR                              FGPIO_PTOR_REG(FPTA_BASE_PTR)
#define FGPIOA_PDIR                              FGPIO_PDIR_REG(FPTA_BASE_PTR)
#define FGPIOA_PDDR                              FGPIO_PDDR_REG(FPTA_BASE_PTR)
/* FPTB */
#define FGPIOB_PDOR                              FGPIO_PDOR_REG(FPTB_BASE_PTR)
#define FGPIOB_PSOR                              FGPIO_PSOR_REG(FPTB_BASE_PTR)
#define FGPIOB_PCOR                              FGPIO_PCOR_REG(FPTB_BASE_PTR)
#define FGPIOB_PTOR                              FGPIO_PTOR_REG(FPTB_BASE_PTR)
#define FGPIOB_PDIR                              FGPIO_PDIR_REG(FPTB_BASE_PTR)
#define FGPIOB_PDDR                              FGPIO_PDDR_REG(FPTB_BASE_PTR)
/* FPTC */
#define FGPIOC_PDOR                              FGPIO_PDOR_REG(FPTC_BASE_PTR)
#define FGPIOC_PSOR                              FGPIO_PSOR_REG(FPTC_BASE_PTR)
#define FGPIOC_PCOR                              FGPIO_PCOR_REG(FPTC_BASE_PTR)
#define FGPIOC_PTOR                              FGPIO_PTOR_REG(FPTC_BASE_PTR)
#define FGPIOC_PDIR                              FGPIO_PDIR_REG(FPTC_BASE_PTR)
#define FGPIOC_PDDR                              FGPIO_PDDR_REG(FPTC_BASE_PTR)
/* FPTD */
#define FGPIOD_PDOR                              FGPIO_PDOR_REG(FPTD_BASE_PTR)
#define FGPIOD_PSOR                              FGPIO_PSOR_REG(FPTD_BASE_PTR)
#define FGPIOD_PCOR                              FGPIO_PCOR_REG(FPTD_BASE_PTR)
#define FGPIOD_PTOR                              FGPIO_PTOR_REG(FPTD_BASE_PTR)
#define FGPIOD_PDIR                              FGPIO_PDIR_REG(FPTD_BASE_PTR)
#define FGPIOD_PDDR                              FGPIO_PDDR_REG(FPTD_BASE_PTR)
/* FPTE */
#define FGPIOE_PDOR                              FGPIO_PDOR_REG(FPTE_BASE_PTR)
#define FGPIOE_PSOR                              FGPIO_PSOR_REG(FPTE_BASE_PTR)
#define FGPIOE_PCOR                              FGPIO_PCOR_REG(FPTE_BASE_PTR)
#define FGPIOE_PTOR                              FGPIO_PTOR_REG(FPTE_BASE_PTR)
#define FGPIOE_PDIR                              FGPIO_PDIR_REG(FPTE_BASE_PTR)
#define FGPIOE_PDDR                              FGPIO_PDDR_REG(FPTE_BASE_PTR)


