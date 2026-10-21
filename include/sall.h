#ifndef SALL_H_

#define SALL_H_

typedef enum
{
    HLT, // 0   -- HLT                              -- stops the program
    LDR, // 1   -- LDR register value               -- load a value into a register
    TRR, // 2   -- TRR register1 register2          -- transfer value from register 1 to register 2
    PSH, // 3   -- PSH value                        -- push a value onto the stack
    PSR, // 4   -- PSR register                     -- push the value of a register onto the stack
    POP, // 5   -- POP                              -- pop a value from the stack
    PLR, // 6   -- PLR register                     -- loads the value from the stack into a register and pops it
    SLR, // 7   -- SLR register                     -- loads the value from the stack into a register but doenst pop it
    ADD, // 8   -- ADD register                     -- adds the value of the accumulator onto the value of a register
    SUB, // 9   -- SUB register                     -- subtracts the value of the accumulator from the value of a register
    MUL, // 10  -- MUL register                     -- multiplies the value of the accumulator with the value of a register
    DIV, // 11  -- DIV register                     -- divides the value of a register by the accumulator
    JMP, // 12  -- JMP value                        -- sets the program counter to the value
    INC, // 13  -- INC value                        -- increments the program counter by the value
    IFR, // 14  -- IFR register value p1 p2         -- if register == value increment program counter by p1 else p2
    IRR, // 15  -- IRR register1 register2 p1 p2    -- if register1 == register2 increment program counter by p1 else p2
    INR, // 16  -- INR register value p1 p2         -- if register != value increment program counter by p1 else p2
    INN, // 17  -- INN register1 register2 p1 p2    -- if register1 != register2 increment program counter by p1 else p2
    PRT, // 18  -- PRT register mode                -- print contents of a register to stdout
    NOP, // 19  -- NOP                              -- do nothing
    JMR, // 20  -- JMR register                     -- set program counter to value of register
    ICR, // 21  -- INR register                     -- increment the program counter by the value of a register
    SLP, // 22  -- SLP value                        -- sleep for value milliseconds
    SPR, // 23  -- SPR register                     -- sleep for value of a register seconds
    IKB, // 25  -- IOK register                     -- get 1 character of keyboard input and write it into a register

} InstructionSet;

typedef enum
{
    A,      // 0 -- accumulator, used for math
    X,      // 1 -- general purpose
    Y,      // 2 -- general purpose
    Z,      // 3 -- general purpose
    ER,     // 4 -- error, contains the current error values
    PC,     // 5 -- program counter
    SP,     // 6 -- stack pointer
    IM,     // 7 -- interrupt mask
    REGISTER_SIZE
} Registers;

typedef enum
{
    NOERR,       // 0    -- no errors
    INVALID,     // 1    -- invalid instruction or missing operands, halts the machine
    NOREGISTER,  // 2    -- the register the instruction is trying to access doesnt exist
    STOVERFLOW,  // 3    -- the stack is full and gets overflown
    STUNDERFLOW, // 4    -- the stack is empty and cant be popped
    INTOOB,      // 5    -- the result of an operation is outside of the integer range
    DIVZERO,     // 6    -- divide by 0
    PCOOB,       // 7    -- program counter is out of bounds
    UNDEF,       // 8    -- undefined error
} ErrorCodes;

typedef enum {
    NONE,       // 0    -- enable all interrupts
    KEYBOARD,   // 1    -- disable keyboard interrupts    
    CONSOLE,    // 2    -- disable console output interrupts
    KBCO,       // 3    -- disable keyboard and console interrupts
    RANDOM,     // 4    -- disable random register interrupts
    KBRA,       // 5    -- disable keyboard and random interrupts
    CORA,       // 6    -- disable console and random interrupts
    KBCORA,     // 7    -- disable keyboard, console and random interrupts
} InterruptMask;

typedef enum {
    DECIMAL,    // 0    -- print value as decimal number (base10)
    HEX,        // 1    -- print value as hexadecimal (base16)
    CHAR,       // 2    -- print value as c character 
} PrintMode;

static const struct
{
    InstructionSet instruction;
    const char *str;
} instruction_conversion[] = {
    {HLT, "HLT"},
    {LDR, "LDR"},
    {TRR, "TRR"},
    {PSH, "PSH"},
    {PSR, "PSR"},
    {POP, "POP"},
    {PLR, "PLR"},
    {SLR, "SLR"},
    {ADD, "ADD"},
    {SUB, "SUB"},
    {MUL, "MUL"},
    {DIV, "DIV"},
    {JMP, "JMP"},
    {INC, "INC"},
    {IFR, "IFR"},
    {IRR, "IRR"},
    {INR, "INR"},
    {INN, "INN"},
    {PRT, "PRT"},
    {NOP, "NOP"},
    {JMR, "JMR"},
    {ICR, "ICR"},
    {SLP, "SLP"},
    {SPR, "SPR"},
    {IKB, "IKB"},
};

static const struct
{
    Registers reg;
    const char *str;
} register_conversion[] = {
    {A, "A"},
    {X, "X"},
    {Y, "Y"},
    {Z, "Z"},
    {ER, "ER"},
    {PC, "PC"},
    {SP, "SP"},
    {IM, "IM"},
};

static const struct
{
    ErrorCodes error;
    const char *str;
} error_conversion[] = {
    {NOERR, "NOERR"},
    {INVALID, "INVALID"},
    {NOREGISTER, "NOREGISTER"},
    {STOVERFLOW, "STOVERFLOW"},
    {STUNDERFLOW, "STUNDERFLOW"},
    {INTOOB, "INTOOB"},
    {DIVZERO, "DIVZERO"},
    {PCOOB, "PCOOB"},
    {UNDEF, "UNDEF"}};

static const struct 
{
    InterruptMask mask;
    const char *str;
} interrupt_mask_conversion[] = {
    {NONE, "NONE"},
    {KEYBOARD, "KEYBOARD"},
    {CONSOLE, "CONSOLE"},
    {KBCO, "KBCO"},
    {RANDOM, "RANDOM"},
    {KBRA, "KBRA"},
    {CORA, "CORA"},
    {KBCORA, "KBCORA"}
};

static const struct 
{
    PrintMode mode;
    const char *str;
} print_mode_conversion[] = {
    {DECIMAL, "DECIMAL"},
    {HEX, "HEX"},
    {CHAR, "CHAR"}
};

InstructionSet str2ins(const char *str);
Registers str2reg(const char *str);
ErrorCodes str2err(const char *str);
InterruptMask str2interrupt_mask(const char *str);
PrintMode str2prtint_mode(const char *str);

static const char *file_header = "sall23\n";

#endif