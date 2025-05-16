#ifndef IR_DEBUG_MACROSES_H
#define IR_DEBUG_MACROSES_H

#define IR_DEBUG    // FIXME delete this!

#ifdef IR_DEBUG

#define ON_IR_DEBUG(...) __VA_ARGS__

#define IR_VERIFY(ir, error_exit_val) do                                                                                                    \
{                                                                                                                                           \
    /*IrDump(ir);*/                                                                                                                         \
                                                                                                                                            \
    int verify_code = IrVerify(ir);                                                                                                         \
                                                                                                                                            \
    if (verify_code != IR_OK)                                                                                                               \
    {                                                                                                                                       \
        const char *const errors = GetIrErrors(verify_code);                                                                                \
        lassert(errors, "errors == NULL");                                                                                                  \
        log(ERROR, "IR_VERIFY failed! Errors:\n\t %s\n", errors);                                                                           \
        return error_exit_val;                                                                                                              \
    }                                                                                                                                       \
                                                                                                                                            \
} while(0)


#define IR_DUMP(ir_ptr_) do                             \
{                                                       \
    log(DUMP, "IR dump");                               \
    IrDump(ir_ptr_);                                        \
} while (0)


#else
#define ON_IR_DEBUG(...)
#define IR_VERIFY(ir, error_exit_val)
#define IR_DUMP(ir_ptr_)
#endif

#endif /*IR_DEBUG_MACROSES_H*/