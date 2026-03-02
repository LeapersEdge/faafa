// *************************************************
//
// DEPENDENCIES:
// - C11
// - raylib 5.5 (GLFW backend)
//
// DOCUMENTATION:
// - http://devernay.free.fr/hacks/chip8/C8TECH10.HTM
//
// *************************************************

#include "raylib.h"
#include <math.h>
#include <stdio.h>
#include <stdint.h>

// table of contents:
// STRUCTS/GLOBALS
// FUNCTION_DECLARATIONS
// MAIN
// FUNCTION_DEFINITIONS

// =======================================================================================
//
// STRUCTS/GLOBALS
//
// =======================================================================================

#define PROGRAM_START 0x200
#define PROGRAM_START_ETI_660 0x600

typedef struct
{
    struct
    {
        struct // registers
        {
            union // general purpose registers
            {
                struct {
                    uint8_t reg0, reg1, reg2, reg3;
                    uint8_t reg4, reg5, reg6, reg7;
                    uint8_t reg8, reg9, regA, regB;
                    uint8_t regC, regD, regE, regF;
                };
                uint8_t reg[16];
            };

            uint16_t regI; // index register
            
            uint8_t regST; // special purpose register (sound timers)
            uint8_t regDT; // special purpose register (delay timers)

            uint16_t regPC; // program counter register
            uint8_t regSP; // stack pointer register

            uint16_t stack[256];
        };
    }
    cpu;

    uint8_t ram[4096];
    bool display[32][64];
    
    bool wait_for_vblank;

    struct
    {
        struct
        {
            union
            {
                struct
                {
                    bool key0, key1, key2, key3;
                    bool key4, key5, key6, key7;
                    bool key8, key9, keyA, keyB;
                    bool keyC, keyD, keyE, keyF;
                };
                bool key[16];
            };
        } 
        old;
        
        struct
        {
            union
            {
                struct
                {
                    bool key0, key1, key2, key3;
                    bool key4, key5, key6, key7;
                    bool key8, key9, keyA, keyB;
                    bool keyC, keyD, keyE, keyF;
                };
                bool key[16];
            };
        }
        new;
    }
    keyboard;
}
C8_Context;

typedef struct  // keyboard bindings for CHIP-8
{
    union
    {
        struct
        {
            KeyboardKey key0, key1, key2, key3;
            KeyboardKey key4, key5, key6, key7;
            KeyboardKey key8, key9, keyA, keyB;
            KeyboardKey keyC, keyD, keyE, keyF;
        };
        KeyboardKey key[16];
    };
}
C8_Input;

uint8_t font_0[5] = {0xF0, 0x90, 0x90, 0x90, 0xF0};
uint8_t font_1[5] = {0x20, 0x60, 0x20, 0x20, 0x70};
uint8_t font_2[5] = {0xF0, 0x10, 0xF0, 0x80, 0xF0};
uint8_t font_3[5] = {0xF0, 0x10, 0xF0, 0x10, 0xF0};
uint8_t font_4[5] = {0x90, 0x90, 0xF0, 0x10, 0x10};
uint8_t font_5[5] = {0xF0, 0x80, 0xF0, 0x10, 0xF0};
uint8_t font_6[5] = {0xF0, 0x80, 0xF0, 0x90, 0xF0};
uint8_t font_7[5] = {0xF0, 0x10, 0x20, 0x40, 0x40};
uint8_t font_8[5] = {0xF0, 0x90, 0xF0, 0x90, 0xF0};
uint8_t font_9[5] = {0xF0, 0x90, 0xF0, 0x10, 0xF0};
uint8_t font_A[5] = {0xF0, 0x90, 0xF0, 0x90, 0x90};
uint8_t font_B[5] = {0xE0, 0x90, 0xE0, 0x90, 0xE0};
uint8_t font_C[5] = {0xF0, 0x80, 0x80, 0x80, 0xF0};
uint8_t font_D[5] = {0xE0, 0x90, 0x90, 0x90, 0xE0};
uint8_t font_E[5] = {0xF0, 0x80, 0xF0, 0x80, 0xF0};
uint8_t font_F[5] = {0xF0, 0x80, 0xF0, 0x80, 0x80};

// =======================================================================================
//
// FUNCTION_DECLARATIONS
//
// =======================================================================================

C8_Context Load_C8_Context(const char* program_filepath);
bool C8_Decode_And_Execute_Instruction(uint16_t instruction, C8_Context* ctx);

C8_Input Load_C8_Input_Default();

bool Inst_CLS(C8_Context* ctx);                                 // 00E0
bool Inst_RET(C8_Context* ctx);                                 // 00EE
bool Inst_SYS_addr(C8_Context* ctx, uint16_t instruction);      // 0nnn
bool Inst_JP_addr(C8_Context* ctx, uint16_t instruction);       // 1nnn
bool Inst_CALL_addr(C8_Context* ctx, uint16_t instruction);     // 2nnn
bool Inst_SE_reg_byte(C8_Context* ctx, uint16_t instruction);   // 3xkk
bool Inst_SNE_reg_byte(C8_Context* ctx, uint16_t instruction);  // 4xkk
bool Inst_SE_reg_reg(C8_Context* ctx, uint16_t instruction);    // 5xy0
bool Inst_LD_reg_byte(C8_Context* ctx, uint16_t instruction);   // 6xkk
bool Inst_ADD_reg_byte(C8_Context* ctx, uint16_t instruction);  // 7xkk
bool Inst_LD_reg_reg(C8_Context* ctx, uint16_t instruction);    // 8xy0
bool Inst_OR_reg_reg(C8_Context* ctx, uint16_t instruction);    // 8xy1
bool Inst_AND_reg_reg(C8_Context* ctx, uint16_t instruction);   // 8xy2
bool Inst_XOR_reg_reg(C8_Context* ctx, uint16_t instruction);   // 8xy3
bool Inst_ADD_reg_reg(C8_Context* ctx, uint16_t instruction);   // 8xy4
bool Inst_SUB_reg_reg(C8_Context* ctx, uint16_t instruction);   // 8xy5
bool Inst_SHR_reg_reg(C8_Context* ctx, uint16_t instruction);   // 8xy6
bool Inst_SUBN_reg_reg(C8_Context* ctx, uint16_t instruction);  // 8xy7
bool Inst_SHL_reg_reg(C8_Context* ctx, uint16_t instruction);   // 8xyE
bool Inst_SNE_reg_reg(C8_Context* ctx, uint16_t instruction);   // 9xy0
bool Inst_LD_I_addr(C8_Context* ctx, uint16_t instruction);     // Annn
bool Inst_JP_V0_addr(C8_Context* ctx, uint16_t instruction);    // Bnnn
bool Inst_RND_reg_byte(C8_Context* ctx, uint16_t instruction);  // Cxkk
bool Inst_DRW_reg_reg_nibble(C8_Context* ctx, uint16_t instruction); // Dxyn
bool Inst_SKP_reg(C8_Context* ctx, uint16_t instruction);       // Ex9E
bool Inst_SKNP_reg(C8_Context* ctx, uint16_t instruction);      // ExA1
bool Inst_LD_reg_DT(C8_Context* ctx, uint16_t instruction);     // Fx07
bool Inst_LD_reg_K(C8_Context* ctx, uint16_t instruction);      // Fx0A
bool Inst_LD_DT_reg(C8_Context* ctx, uint16_t instruction);     // Fx15
bool Inst_LD_ST_reg(C8_Context* ctx, uint16_t instruction);     // Fx18
bool Inst_ADD_I_reg(C8_Context* ctx, uint16_t instruction);     // Fx1E
bool Inst_LD_F_reg(C8_Context* ctx, uint16_t instruction);      // Fx29
bool Inst_LD_B_reg(C8_Context* ctx, uint16_t instruction);      // Fx33
bool Inst_LD_Iptr_reg(C8_Context* ctx, uint16_t instruction);   // Fx55
bool Inst_LD_reg_Iptr(C8_Context* ctx, uint16_t instruction);   // Fx65

// =======================================================================================
//
// MAIN
//
// =======================================================================================

int main()
{
    InitWindow(640, 320, "CHIP-8");
    SetTargetFPS(60);
    InitAudioDevice();

    int sample_rate = 44100;
    float frequency = 523.25f;
    int sample_count = sample_rate * 0.13f;
    float* samples = (float*)MemAlloc(sizeof(float)*sample_count);
    for (int i = 0; i < sample_count; i++) 
        samples[i] = sinf(2.0f * PI * frequency * i / sample_rate);
    Wave wave = {
        .frameCount = sample_count,
        .sampleRate = sample_rate,
        .sampleSize = 32,
        .channels = 1,
        .data = samples
    };

    Sound beep = LoadSoundFromWave(wave);

    C8_Context ctx = Load_C8_Context(ASSETS "CH8_test_roms/5-quirks.ch8");
    C8_Input input_bindings = Load_C8_Input_Default();

    while (!WindowShouldClose())
    {
        // -----------------------------------
        // UPDATE

        if (ctx.cpu.regDT)
            ctx.cpu.regDT--;
        if (ctx.cpu.regST)
        {
            PlaySound(beep);
            ctx.cpu.regST--;
        }
        ctx.wait_for_vblank = false;

        // fetch-decode-execute loop
        for (int i = 0; i < 2000/60; i++)
        {
            // fetch
            uint8_t instruction_top    = ctx.ram[ctx.cpu.regPC];
            uint8_t instruction_bottom = ctx.ram[ctx.cpu.regPC + 1];
            uint16_t instruction = (instruction_top << 8) | instruction_bottom;
            //printf("%x: %x\n", ctx.cpu.regPC, instruction);
            ctx.cpu.regPC += 2;

            // decode and execute
            bool success = C8_Decode_And_Execute_Instruction(instruction, &ctx);
            if (!success)
                ctx.cpu.regPC -= 2;

            if (ctx.wait_for_vblank)
                break;
        }

        // update inputs
        for (int i = 0; i < 16; i++)
            ctx.keyboard.old.key[i] = ctx.keyboard.new.key[i];
        for (int i = 0; i < 16; i++)
            ctx.keyboard.new.key[i] = IsKeyDown(input_bindings.key[i]);

        // -----------------------------------
        // RENDER

        BeginDrawing();
        ClearBackground(WHITE);
        for (uint16_t y = 0; y < 32; y++)
        {
            for (uint16_t x = 0; x < 64; x++)
            {
                static const int rect_size = 10;
                if (ctx.display[y][x])
                    DrawRectangle(x * rect_size, y * rect_size, rect_size, rect_size, ORANGE);
                else
                    DrawRectangle(x * rect_size, y * rect_size, rect_size, rect_size, BLACK);
                
            }
        }
        EndDrawing();
    }
   
    UnloadSound(beep);
    MemFree(samples);
    CloseAudioDevice();
    CloseWindow();
}

// =======================================================================================
//
// FUNCTION_DEFINITIONS
//
// =======================================================================================

C8_Context Load_C8_Context(const char* program_filepath)
{
    C8_Context ctx = {0};

    // ucitati fontset u C8_Context
    for (int i = 0; i < 5; i++)
    {
        ctx.ram[i + 0*5] = font_0[i];
        ctx.ram[i + 1*5] = font_1[i];
        ctx.ram[i + 2*5] = font_2[i];
        ctx.ram[i + 3*5] = font_3[i];
        ctx.ram[i + 4*5] = font_4[i];
        ctx.ram[i + 5*5] = font_5[i];
        ctx.ram[i + 6*5] = font_6[i];
        ctx.ram[i + 7*5] = font_7[i];
        ctx.ram[i + 8*5] = font_8[i];
        ctx.ram[i + 9*5] = font_9[i];
        ctx.ram[i + 10*5] = font_A[i];
        ctx.ram[i + 11*5] = font_B[i];
        ctx.ram[i + 12*5] = font_C[i];
        ctx.ram[i + 13*5] = font_D[i];
        ctx.ram[i + 14*5] = font_E[i];
        ctx.ram[i + 15*5] = font_F[i];
    }

    // ucitati program u ctx.ram
    FILE* fileptr;
    long filelen;
    fileptr = fopen(program_filepath, "rb");
    fseek(fileptr, 0, SEEK_END);
    filelen = ftell(fileptr);
    rewind(fileptr);
    fread(ctx.ram + PROGRAM_START, 1, filelen, fileptr);
    fclose(fileptr);

    ctx.cpu.regPC = PROGRAM_START;

    return ctx;
}

C8_Input Load_C8_Input_Default()
{
    C8_Input c8_input;

    c8_input.key0 = KEY_ZERO;
    c8_input.key1 = KEY_ONE;
    c8_input.key2 = KEY_TWO;
    c8_input.key3 = KEY_THREE;
    c8_input.key4 = KEY_FOUR;
    c8_input.key5 = KEY_FIVE;
    c8_input.key6 = KEY_SIX;
    c8_input.key7 = KEY_SEVEN;
    c8_input.key8 = KEY_EIGHT;
    c8_input.key9 = KEY_NINE;
    c8_input.keyA = KEY_A;
    c8_input.keyB = KEY_B;
    c8_input.keyC = KEY_C;
    c8_input.keyD = KEY_D;
    c8_input.keyE = KEY_E;
    c8_input.keyF = KEY_F;

    return c8_input;
}

bool C8_Decode_And_Execute_Instruction(uint16_t instruction, C8_Context* ctx)
{
    const uint8_t arg1 = (instruction & 0xF000) >> 3*4;
    const uint8_t arg4 = (instruction & 0x000F) >> 0*4;

    // 0NNN series
    if      (instruction == 0x00E0)           return Inst_CLS(ctx);
    else if (instruction == 0x00EE)           return Inst_RET(ctx);
    else if (arg1 == 0x0)                     return Inst_SYS_addr(ctx, instruction);

    // 1NNN - 7NNN
    else if (arg1 == 0x1)                     return Inst_JP_addr(ctx, instruction);
    else if (arg1 == 0x2)                     return Inst_CALL_addr(ctx, instruction);
    else if (arg1 == 0x3)                     return Inst_SE_reg_byte(ctx, instruction);
    else if (arg1 == 0x4)                     return Inst_SNE_reg_byte(ctx, instruction);
    else if (arg1 == 0x5 && arg4 == 0x0)      return Inst_SE_reg_reg(ctx, instruction);
    else if (arg1 == 0x6)                     return Inst_LD_reg_byte(ctx, instruction);
    else if (arg1 == 0x7)                     return Inst_ADD_reg_byte(ctx, instruction);

    // 8XYN Arithmetic series
    else if (arg1 == 0x8) 
    {
        if      (arg4 == 0x0)                 return Inst_LD_reg_reg(ctx, instruction);
        else if (arg4 == 0x1)                 return Inst_OR_reg_reg(ctx, instruction);
        else if (arg4 == 0x2)                 return Inst_AND_reg_reg(ctx, instruction);
        else if (arg4 == 0x3)                 return Inst_XOR_reg_reg(ctx, instruction);
        else if (arg4 == 0x4)                 return Inst_ADD_reg_reg(ctx, instruction);
        else if (arg4 == 0x5)                 return Inst_SUB_reg_reg(ctx, instruction);
        else if (arg4 == 0x6)                 return Inst_SHR_reg_reg(ctx, instruction);
        else if (arg4 == 0x7)                 return Inst_SUBN_reg_reg(ctx, instruction);
        else if (arg4 == 0xE)                 return Inst_SHL_reg_reg(ctx, instruction);
    }

    // 9NNN - DNNN
    else if (arg1 == 0x9 && arg4 == 0x0)      return Inst_SNE_reg_reg(ctx, instruction);
    else if (arg1 == 0xA)                     return Inst_LD_I_addr(ctx, instruction);
    else if (arg1 == 0xB)                     return Inst_JP_V0_addr(ctx, instruction);
    else if (arg1 == 0xC)                     return Inst_RND_reg_byte(ctx, instruction);
    else if (arg1 == 0xD)                     return Inst_DRW_reg_reg_nibble(ctx, instruction);

    // EXNN Keyboard series
    else if (arg1 == 0xE) 
    {
        uint8_t kk = (instruction & 0x00FF);
        if      (kk == 0x9E)                  return Inst_SKP_reg(ctx, instruction);
        else if (kk == 0xA1)                  return Inst_SKNP_reg(ctx, instruction);
    }

    // FXNN Misc series
    else if (arg1 == 0xF) 
    {
        uint8_t kk = (instruction & 0x00FF);
        if      (kk == 0x07)                  return Inst_LD_reg_DT(ctx, instruction);
        else if (kk == 0x0A)                  return Inst_LD_reg_K(ctx, instruction);
        else if (kk == 0x15)                  return Inst_LD_DT_reg(ctx, instruction);
        else if (kk == 0x18)                  return Inst_LD_ST_reg(ctx, instruction);
        else if (kk == 0x1E)                  return Inst_ADD_I_reg(ctx, instruction);
        else if (kk == 0x29)                  return Inst_LD_F_reg(ctx, instruction);
        else if (kk == 0x33)                  return Inst_LD_B_reg(ctx, instruction);
        else if (kk == 0x55)                  return Inst_LD_Iptr_reg(ctx, instruction);
        else if (kk == 0x65)                  return Inst_LD_reg_Iptr(ctx, instruction);
    }

    return false;
}

bool Inst_CLS(C8_Context* ctx) 
{
    const uint8_t display_size_y = 32;
    const uint8_t display_size_x = 64;
    
    for (uint8_t y = 0; y < display_size_y; y++)
        for (uint8_t x = 0; x < display_size_x; x++)
            ctx->display[y][x] = 0;

    return true;
}
 
bool Inst_RET(C8_Context* ctx)
{
    ctx->cpu.regPC = ctx->cpu.stack[ctx->cpu.regSP];
    ctx->cpu.regSP--;

    return true;
}
 
bool Inst_SYS_addr(C8_Context* ctx, uint16_t instruction)
{
    const uint16_t nnn = (instruction & 0x0FFF);
    ctx->cpu.regPC = nnn;

    return true;
}
 
bool Inst_JP_addr(C8_Context* ctx, uint16_t instruction)
{
    const uint16_t nnn = (instruction & 0x0FFF);
    ctx->cpu.regPC = nnn;

    return true;
}
 
bool Inst_CALL_addr(C8_Context* ctx, uint16_t instruction)
{
    const uint16_t nnn = (instruction & 0x0FFF);

    ctx->cpu.regSP++;
    ctx->cpu.stack[ctx->cpu.regSP] = ctx->cpu.regPC;
    ctx->cpu.regPC = nnn;

    return true;
}
 
bool Inst_SE_reg_byte(C8_Context* ctx, uint16_t instruction)
{
    const uint8_t Vx = (instruction & 0x0F00) >> 8;
    const uint8_t kk = (instruction & 0x00FF);

    if (ctx->cpu.reg[Vx] == kk)
        ctx->cpu.regPC += 2;

    return true;
}
 
bool Inst_SNE_reg_byte(C8_Context* ctx, uint16_t instruction)
{
    const uint8_t Vx = (instruction & 0x0F00) >> 8;
    const uint8_t kk = (instruction & 0x00FF);

    if (ctx->cpu.reg[Vx] != kk)
        ctx->cpu.regPC += 2;

    return true;
}
 
bool Inst_SE_reg_reg(C8_Context* ctx, uint16_t instruction)
{
    const uint8_t Vx = (instruction & 0x0F00) >> 8;
    const uint8_t Vy = (instruction & 0x00F0) >> 4;

    if (ctx->cpu.reg[Vx] == ctx->cpu.reg[Vy])
        ctx->cpu.regPC += 2;

    return true;
}
 
bool Inst_LD_reg_byte(C8_Context* ctx, uint16_t instruction)
{
    const uint8_t Vx = (instruction & 0x0F00) >> 8;
    const uint8_t kk = (instruction & 0x00FF);

    ctx->cpu.reg[Vx] = kk;

    return true;
}
 
bool Inst_ADD_reg_byte(C8_Context* ctx, uint16_t instruction)
{
    const uint8_t Vx = (instruction & 0x0F00) >> 8;
    const uint8_t kk = (instruction & 0x00FF);

    ctx->cpu.reg[Vx] += kk;

    return true;
}
 
bool Inst_LD_reg_reg(C8_Context* ctx, uint16_t instruction)
{
    const uint8_t Vx = (instruction & 0x0F00) >> 8;
    const uint8_t Vy = (instruction & 0x00F0) >> 4;

    ctx->cpu.reg[Vx] = ctx->cpu.reg[Vy];

    return true;
}
 
bool Inst_OR_reg_reg(C8_Context* ctx, uint16_t instruction)
{
    const uint8_t Vx = (instruction & 0x0F00) >> 8;
    const uint8_t Vy = (instruction & 0x00F0) >> 4;

    ctx->cpu.reg[Vx] = ctx->cpu.reg[Vx] | ctx->cpu.reg[Vy];
    ctx->cpu.regF = 0;

    return true;
}
 
bool Inst_AND_reg_reg(C8_Context* ctx, uint16_t instruction)
{
    const uint8_t Vx = (instruction & 0x0F00) >> 8;
    const uint8_t Vy = (instruction & 0x00F0) >> 4;

    ctx->cpu.reg[Vx] = ctx->cpu.reg[Vx] & ctx->cpu.reg[Vy];
    ctx->cpu.regF = 0;

    return true;
}
 
bool Inst_XOR_reg_reg(C8_Context* ctx, uint16_t instruction)
{
    const uint8_t Vx = (instruction & 0x0F00) >> 8;
    const uint8_t Vy = (instruction & 0x00F0) >> 4;

    ctx->cpu.reg[Vx] = ctx->cpu.reg[Vx] ^ ctx->cpu.reg[Vy];
    ctx->cpu.regF = 0;

    return true;
}
 
bool Inst_ADD_reg_reg(C8_Context* ctx, uint16_t instruction)
{
    const uint8_t Vx = (instruction & 0x0F00) >> 8;
    const uint8_t Vy = (instruction & 0x00F0) >> 4;

    const uint16_t result = ctx->cpu.reg[Vx] + ctx->cpu.reg[Vy];
    ctx->cpu.reg[Vx] = (result & 0x00FF);
    ctx->cpu.regF = (result & 0x0100) >> 8;

    return true;
}
 
bool Inst_SUB_reg_reg(C8_Context* ctx, uint16_t instruction)
{
    const uint8_t Vx = (instruction & 0x0F00) >> 8;
    const uint8_t Vy = (instruction & 0x00F0) >> 4;

    const uint8_t result = ctx->cpu.reg[Vx] - ctx->cpu.reg[Vy];
    const uint8_t flag = ctx->cpu.reg[Vx] >= ctx->cpu.reg[Vy];

    ctx->cpu.reg[Vx] = result;
    ctx->cpu.regF = flag;

    return true;
}
 
bool Inst_SHR_reg_reg(C8_Context* ctx, uint16_t instruction)
{
    const uint8_t Vx = (instruction & 0x0F00) >> 8;
    const uint8_t Vy = (instruction & 0x00F0) >> 4;
    
    ctx->cpu.reg[Vx] = ctx->cpu.reg[Vy];

    const uint8_t flag = ctx->cpu.reg[Vx] & 1;
    const uint8_t result = ctx->cpu.reg[Vx] / 2;

    ctx->cpu.reg[Vx] = result;
    ctx->cpu.regF = flag;

    return true;
}
 
bool Inst_SUBN_reg_reg(C8_Context* ctx, uint16_t instruction)
{
    const uint8_t Vx = (instruction & 0x0F00) >> 8;
    const uint8_t Vy = (instruction & 0x00F0) >> 4;
    
    ctx->cpu.reg[Vx] = ctx->cpu.reg[Vy] - ctx->cpu.reg[Vx];
    ctx->cpu.regF = ctx->cpu.reg[Vy] > ctx->cpu.reg[Vx];

    return true;
}
 
bool Inst_SHL_reg_reg(C8_Context* ctx, uint16_t instruction)
{
    const uint8_t Vx = (instruction & 0x0F00) >> 8;
    const uint8_t Vy = (instruction & 0x00F0) >> 4;

    ctx->cpu.reg[Vx] = ctx->cpu.reg[Vy];

    const uint8_t flag = (ctx->cpu.reg[Vx] >> 7) & 1;
    const uint8_t result = ctx->cpu.reg[Vx] * 2;

    ctx->cpu.reg[Vx] = result;
    ctx->cpu.regF = flag;

    return true;
}
 
bool Inst_SNE_reg_reg(C8_Context* ctx, uint16_t instruction)
{
    const uint8_t Vx = (instruction & 0x0F00) >> 8;
    const uint8_t Vy = (instruction & 0x00F0) >> 4;

    if (ctx->cpu.reg[Vx] != ctx->cpu.reg[Vy])
        ctx->cpu.regPC += 2;

    return true;
}
 
bool Inst_LD_I_addr(C8_Context* ctx, uint16_t instruction)
{
    const uint16_t nnn = (instruction & 0x0FFF);
    ctx->cpu.regI = nnn;

    return true;
}
 
bool Inst_JP_V0_addr(C8_Context* ctx, uint16_t instruction)
{
    const uint16_t nnn = (instruction & 0x0FFF);
    ctx->cpu.regPC = ctx->cpu.reg0 + nnn;

    return true;
}
 
bool Inst_RND_reg_byte(C8_Context* ctx, uint16_t instruction)
{
    const uint8_t Vx = (instruction & 0x0F00) >> 8;
    const uint8_t kk = (instruction & 0x00FF);

    const uint8_t rand = GetRandomValue(0, 255);
    ctx->cpu.reg[Vx] = rand & kk;

    return true;
}
 
bool Inst_DRW_reg_reg_nibble(C8_Context* ctx, uint16_t instruction)
{
    ctx->wait_for_vblank = true;

    const uint8_t display_size_y = 32;
    const uint8_t display_size_x = 64;
    
    const uint8_t Vx = (instruction & 0x0F00) >> 8;
    const uint8_t Vy = (instruction & 0x00F0) >> 4;
    const uint8_t nimble = (instruction & 0x000F);
    const uint16_t loc = ctx->cpu.regI;
   
    uint8_t x = ctx->cpu.reg[Vx] % display_size_x;
    uint8_t y = ctx->cpu.reg[Vy] % display_size_y;
    ctx->cpu.regF = 0;

    for (uint8_t i = 0; i < nimble; i++)
    {
        uint8_t byte_line = ctx->ram[loc + i];
        x = ctx->cpu.reg[Vx] % display_size_x;
        for (int8_t i_bit = 7; i_bit >= 0; i_bit--)
        {
            uint8_t bit = (byte_line >> i_bit) & 0b00000001;
            ctx->display[y][x] ^= bit;
            if (bit && ctx->display[y][x] == 0)
                ctx->cpu.regF = 1;

            x++;
            if (x >= display_size_x)
                break;
        }
            
        y++;
        if (y >= display_size_y)
            break;
    }

    return true;
}
 
bool Inst_SKP_reg(C8_Context* ctx, uint16_t instruction)
{
    const uint8_t Vx = (instruction & 0x0F00) >> 8;

    const uint8_t value = ctx->cpu.reg[Vx];

    if (ctx->keyboard.new.key[value])
        ctx->cpu.regPC += 2;

    return true;
}
 
bool Inst_SKNP_reg(C8_Context* ctx, uint16_t instruction)
{
    const uint8_t Vx = (instruction & 0x0F00) >> 8;

    const uint8_t value = ctx->cpu.reg[Vx];

    if (!ctx->keyboard.new.key[value])
        ctx->cpu.regPC += 2;

    return true;
}
 
bool Inst_LD_reg_DT(C8_Context* ctx, uint16_t instruction)
{
    const uint8_t Vx = (instruction & 0x0F00) >> 8;

    ctx->cpu.reg[Vx] = ctx->cpu.regDT;

    return true;
}
 
bool Inst_LD_reg_K(C8_Context* ctx, uint16_t instruction)
{
    for (uint8_t i = 0; i < 16; i++)
    {
        if (!ctx->keyboard.new.key[i] && ctx->keyboard.old.key[i])
        {
            uint8_t Vx = (instruction & 0x0F00) >> 8;

            ctx->cpu.reg[Vx] = i;

            return true;
        }
    }
    
    return false;
}
 
bool Inst_LD_DT_reg(C8_Context* ctx, uint16_t instruction)
{
    const uint8_t Vx = (instruction & 0x0F00) >> 8;

    ctx->cpu.regDT = ctx->cpu.reg[Vx];

    return true;
}
 
bool Inst_LD_ST_reg(C8_Context* ctx, uint16_t instruction)
{
    const uint8_t Vx = (instruction & 0x0F00) >> 8;

    ctx->cpu.regST = ctx->cpu.reg[Vx];

    return true;
}
 
bool Inst_ADD_I_reg(C8_Context* ctx, uint16_t instruction)
{
    const uint8_t Vx = (instruction & 0x0F00) >> 8;
    
    ctx->cpu.regI = ctx->cpu.regI + ctx->cpu.reg[Vx];

    return true;
}
 
bool Inst_LD_F_reg(C8_Context* ctx, uint16_t instruction)
{
    const uint8_t Vx = (instruction & 0x0F00) >> 8;
    
    const uint16_t loc = ctx->cpu.reg[Vx] * 5 /*each num is 5 bytes long for display representation*/;
    ctx->cpu.regI = loc;

    return true;
}
 
bool Inst_LD_B_reg(C8_Context* ctx, uint16_t instruction)
{
    const uint8_t Vx = (instruction & 0x0F00) >> 8;

    const uint8_t value = ctx->cpu.reg[Vx];
    const uint8_t value100 = value / 100;
    const uint8_t value10  = (value / 10) % 10;
    const uint8_t value1   = value % 10;

    const uint16_t loc = ctx->cpu.regI;
    ctx->ram[loc+0] = value100;
    ctx->ram[loc+1] = value10;
    ctx->ram[loc+2] = value1;

    return true;
}
 
bool Inst_LD_Iptr_reg(C8_Context* ctx, uint16_t instruction)
{
    const uint8_t Vx = (instruction & 0x0F00) >> 8;
   
    for (uint8_t i = 0; i <= Vx; i++)
        ctx->ram[ctx->cpu.regI++] = ctx->cpu.reg[i];

    return true;    
}
 
bool Inst_LD_reg_Iptr(C8_Context* ctx, uint16_t instruction)
{
    const uint8_t Vx = (instruction & 0x0F00) >> 8;
   
    for (uint8_t i = 0; i <= Vx; i++)
        ctx->cpu.reg[i] = ctx->ram[ctx->cpu.regI++];

    return true;    
}
 
