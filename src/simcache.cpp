/*
CS-UY 2214
Jeff Epstein
Starter code for E20 cache assembler
simcache.cpp
*/

#include <cstddef>
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <limits>
#include <iomanip>
#include <regex>
#include "e20.h"
#include "cache.h"

using namespace std;

// Some helpful constant values that we'll be using.
size_t const static NUM_REGS = 8;
size_t const static MEM_SIZE = 1 << 13;
size_t const static REG_SIZE = 1 << 16;

/*
    Prints out the correctly-formatted configuration of a cache.

    @param cache_name The name of the cache. "L1" or "L2"

    @param size The total size of the cache, measured in memory cells.
        Excludes metadata

    @param assoc The associativity of the cache. One of [1,2,4,8,16]

    @param blocksize The blocksize of the cache. One of [1,2,4,8,16,32,64])
*/
void print_cache_config(const string &cache_name, int size, int assoc, int blocksize, int num_lines)
{
    cout << "Cache " << cache_name << " has size " << size << ", associativity " << assoc << ", blocksize " << blocksize << ", lines " << num_lines << endl;
}

void load_machine_code(ifstream &f, unsigned mem[])
{
    regex machine_code_re("^ram\\[(\\d+)\\] = 16'b(\\d+);.*$");
    size_t expectedaddr = 0;
    string line;
    while (getline(f, line))
    {
        smatch sm;
        if (!regex_match(line, sm, machine_code_re))
        {
            cerr << "Can't parse line: " << line << endl;
            exit(1);
        }
        size_t addr = stoi(sm[1], nullptr, 10);
        unsigned instr = stoi(sm[2], nullptr, 2);
        if (addr != expectedaddr)
        {
            cerr << "Memory addresses encountered out of sequence: " << addr << endl;
            exit(1);
        }
        if (addr >= MEM_SIZE)
        {
            cerr << "Program too big for memory" << endl;
            exit(1);
        }
        expectedaddr++;
        mem[addr] = instr;
    }
}

/*
    Prints out a correctly-formatted log entry.

    @param cache_name The name of the cache where the event
        occurred. "L1" or "L2"

    @param status The kind of cache event. "SW", "HIT", or
        "MISS"

    @param pc The program counter of the memory
        access instruction

    @param addr The memory address being accessed.

    @param line The cache line or set number where the data
        is stored.
*/
void print_log_entry(const string &cache_name, const string &status, int pc, int addr, int line)
{
    cout << left << setw(8) << cache_name + " " + status << right << " pc:" << 
    setw(5) << pc << "\taddr:" << setw(5) << addr << "\tline:" << setw(4) << line << endl;
}

/**
 * @brief Set the Reg Zero to always be zero.
 * @param reg $0
 * @return unsigned 0
 */
unsigned setRegZero(unsigned reg)
{
    return (reg != 0) ? 0 : 0;
}

/**
 * @brief The sign_extension fuction extends the 7 bit imm
 * to signed value if imm > 64.
 * @param imm 7 bits
 * @return uint16_t imm
 */
uint16_t sign_extension(unsigned imm)
{
    uint16_t signedEx = ~127;

    return imm | signedEx;
}
/**
 * @brief Sets the Registers Size to be 16 bits.
 * @param regs registers [0:7]
 */
void setRegSize(unsigned regs[])
{

    for (int i = 0; i < 8; i++)
    {
        regs[i] %= REG_SIZE;
    }
}

/**
 * @brief Sets the Memory Size to be 16 bits.
 * @param mem  mermory[0:8191]
 */
void setMemSize(unsigned mem[])
{

    for (int i = 0; i < 8; i++)
    {
        mem[i] %= 1 << 16;
    }
}

/**
 * @brief The isThreeRegInstc checks to see if its a three register instuction.
 * @param opcode add, sub, and, or, slt, jr.
 * @return true  if it is a three register instuction.
 * @return false if not a three register instuction.
 */
bool isThreeRegInstc(unsigned opcode)
{
    bool inst;

    if (opcode == 0b000)
    {
        inst = true;
    }
    else
    {
        inst = false;
    }

    return inst;
}

/**
 * @brief The isTwoRegInstc checks to see if its a two register instuction.
 * @param opcode addi, slti, lw, sw, jeq.
 * @return true if it is a two register instuction.
 * @return false if not a two register instuction.
 */
bool isTwoRegInstc(unsigned opcode)
{
    bool inst;

    if ((opcode == 0b111) || (opcode == 0b100) || (opcode == 0b101) 
    || (opcode == 0b110) || (opcode == 0b001))
    {
        inst = true;
    }
    else
    {
        inst = false;
    }
    return inst;
}

/**
 * @brief The isNORegInstc checks to see if its a no register instuction.
 * @param opcode j, jal.
 * @return true if it is a no register instuction.
 * @return false if not a no register instuction.
 */
bool isNoRegInstc(unsigned opcode)
{
    bool inst;

    if ((opcode == 0b010) || (opcode == 0b011))
    {
        inst = true;
    }
    else
    {
        inst = false;
    }

    return inst;
}

/**
 * @brief The One_cache gets the configution of an architecture that contains one cache.
 * 
 * @param L1assoc  cache association 
 * @param L1size   cache size
 * @param L1blocksize cache blocksize 
 * @param numLinesL1  the number of lines cache contains.
 */
void one_cache(int &L1assoc, int &L1size, int &L1blocksize, int &numLinesL1)
{

    if (L1assoc == 1)
    {
        numLinesL1 = L1size / L1blocksize;
        print_cache_config("L1", L1size, L1assoc, L1blocksize, numLinesL1);
    }

    else
    {
        numLinesL1 = (L1size) / (L1blocksize * L1assoc);
        print_cache_config("L1", L1size, L1assoc, L1blocksize, numLinesL1);
    }
}

/**
 * @brief  The One_cache gets the configution of an architecture that contains two caches.
 * 
 * @param L1assoc      cache 1 association
 * @param L2assoc      cache 2 association
 * @param numLinesL1   the number of lines cache1 contains.
 * @param numLinesL2   the number of lines cache2 contains.
 * @param L1size       cache 1 size
 * @param L2size       cache 2 size
 * @param L1blocksize  blocksize cache 1
 * @param L2blocksize  blocksize cache 2
 */
void two_cache(int &L1assoc, int &L2assoc, int &numLinesL1, int &numLinesL2, int &L1size, int &L2size, int &L1blocksize, int &L2blocksize)
{
    if ((L1assoc == 1) && (L2assoc == 1))
    {
        numLinesL1 = L1size / L1blocksize;
        print_cache_config("L1", L1size, L1assoc, L1blocksize, numLinesL1);

        numLinesL2 = L2size / L2blocksize;
        print_cache_config("L2", L2size, L2assoc, L2blocksize, numLinesL2);
    }
    else if ((L1assoc == 1) && (L2assoc != 1))
    {

        numLinesL1 = L1size / L1blocksize;
        print_cache_config("L1", L1size, L1assoc, L1blocksize, numLinesL1);

        numLinesL2 = (L2size) / (L2blocksize * L2assoc);
        print_cache_config("L2", L2size, L2assoc, L2blocksize, numLinesL2);
    }
    else if ((L1assoc != 1) && (L2assoc == 1))
    {

        numLinesL1 = (L1size) / (L1blocksize * L1assoc);
        print_cache_config("L1", L1size, L1assoc, L1blocksize, numLinesL1);

        numLinesL2 = L2size / L2blocksize;
        print_cache_config("L2", L2size, L2assoc, L2blocksize, numLinesL2);
    }
    else
    {
        numLinesL1 = (L1size) / (L1blocksize * L1assoc);
        print_cache_config("L1", L1size, L1assoc, L1blocksize, numLinesL1);

        numLinesL2 = (L2size) / (L2blocksize * L2assoc);
        print_cache_config("L2", L2size, L2assoc, L2blocksize, numLinesL2);
    }
}

/**
 * @brief load_instr is a function checks if a load instuction will produce a hit or miss.
 * 
 * @param instuction  e20 processor
 * @param l1  cache 1
 * @param l2  cache 2
 * @param isTwoCache  if wether we have 2 caches.
 * @param address memory address 
 */
void load_instr(e20_processor &instuction, cache &l1, cache &l2, bool isTwoCache, int address)
{

    l1.config_Cache(address);  // configure the cache
    l1.cacheType();            // selects the cache type

    print_log_entry("L1", l1.status, instuction.pc, address, l1.line); 

    if ((l1.status == "MISS") && (isTwoCache == true)) // if cache 1 is a miss and we have two caches, check cache 2.
    {
        l2.config_Cache(address);  // configure the cache 2
        l2.cacheType();        // selects the cache 2 type

        print_log_entry("L2", l2.status, instuction.pc, address, l2.line);
    }
}

/**
 * @brief write_instr is a function checks wether to write to the cache.
 * 
 * @param instuction  e20 processor
 * @param l1 cache 1
 * @param l2 cache 2
 * @param isTwoCache if wether we have 2 caches
 * @param address   memory address 
 */
void write_instr(e20_processor &instuction, cache &l1, cache &l2, bool isTwoCache, int address)
{

    l1.config_Cache(address);
    l1.cacheType();

    print_log_entry("L1", "SW", instuction.pc, address, l1.line);

    if (isTwoCache == true)
    {
        l2.config_Cache(address);
        l2.cacheType();

        print_log_entry("L2", "SW", instuction.pc, address, l2.line);
    }
}

/**
 * @brief The e20Sim fuction simulates the e20 processor.
 * 
 * @param instuction e20 processor
 * @param l1  cache 1
 * @param l2  cache 2
 * @param isTwoCache wether if we have two caches.
 */
void e20Sim(e20_processor &instuction, cache &l1, cache &l2, bool isTwoCache)
{

    unsigned opcode, regSrcA, regSrcB, regDst;

    bool halt = false; // condtion on when to end the program.

    while (halt == false)
    {

        setMemSize(instuction.memory); // sets memory to 16 bits.
        setRegSize(instuction.regs);   // sets registers to 16 bits.

        opcode = instuction.memory[instuction.pc & 8191] >> 13; // shifts 13 bits to the right to get 3 bits MSB.

        if (isThreeRegInstc(opcode) == true)
        { // if its a three registers instuction.

            regSrcA = ((instuction.memory[instuction.pc & 8191] & 0b0001110000000000) >> 10);
            regSrcB = ((instuction.memory[instuction.pc & 8191] & 0b0000001110000000) >> 7);
            regDst = ((instuction.memory[instuction.pc & 8191] & 0b0000000001110000) >> 4);
            instuction.imm = (instuction.memory[instuction.pc & 8191] & 0b0000000000001111);

            instuction.getThreeRegInst(regSrcA, regSrcB, regDst);
        }

        else if (isTwoRegInstc(opcode) == true)
        { // if its a two registers instuction.

            regSrcA = ((instuction.memory[instuction.pc & 8191] & 0b0001110000000000) >> 10);
            regSrcB = ((instuction.memory[instuction.pc & 8191] & 0b0000001110000000) >> 7);
            instuction.imm = (instuction.memory[instuction.pc & 8191] & 0b0000000001111111);

            // if imm is outside the singed range 2^6-1 sign extend.
            if (instuction.imm & 64)
            {
                instuction.imm = sign_extension(instuction.imm);
            }
            
         
            if (opcode == 0b100)  // If "LW"
            {
                int address = instuction.imm + instuction.regs[regSrcA];
                load_instr(instuction, l1, l2, isTwoCache, address);
            }
    
            else if (opcode == 0b101)   // If "SW"
            {
                int address = instuction.imm + instuction.regs[regSrcA];
                write_instr(instuction, l1, l2, isTwoCache,  address);
            }

            instuction.getTwoRegInst(opcode, regSrcA, regSrcB);

        }

        else if (isNoRegInstc(opcode) == true)
        { // if its a no registers instuction.

            instuction.imm = (instuction.memory[instuction.pc & 8191] & 0b0001111111111111);

            // halt if its a jump instuction and program counter is equal to imm.
            if ((opcode == 0b010) && ((instuction.pc & 8191) == instuction.imm))
            {
                halt = true;
            }

            instuction.getNoRegInst(opcode);
        }

        else
        { // If given an invalid operation.

            cerr << "Invalid E20 Instuctions." << endl;
            exit(1);
        }

        instuction.regs[0] = setRegZero(instuction.regs[0]); // make $0 immutable.
    }
}

/**
    Main function
    Takes command-line args as documented below
*/
int main(int argc, char *argv[])
{

    /*
        Parse the command-line arguments
    */
    char *filename = nullptr;
    bool do_help = false;
    bool arg_error = false;
    string cache_config;
    for (int i = 1; i < argc; i++)
    {
        string arg(argv[i]);
        if (arg.rfind("-", 0) == 0)
        {
            if (arg == "-h" || arg == "--help")
                do_help = true;
            else if (arg == "--cache")
            {
                i++;
                if (i >= argc)
                    arg_error = true;
                else
                    cache_config = argv[i];
            }
            else
                arg_error = true;
        }
        else
        {
            if (filename == nullptr)
                filename = argv[i];
            else
                arg_error = true;
        }
    }
    /* Display error message if appropriate */
    if (arg_error || do_help || filename == nullptr)
    {
        cerr << "usage " << argv[0] << " [-h] [--cache CACHE] filename" << endl
             << endl;
        cerr << "Simulate E20 cache" << endl
             << endl;
        cerr << "positional arguments:" << endl;
        cerr << "  filename    The file containing machine code, typically with .bin suffix" << endl
             << endl;
        cerr << "optional arguments:" << endl;
        cerr << "  -h, --help  show this help message and exit" << endl;
        cerr << "  --cache CACHE  Cache configuration: size,associativity,blocksize (for one" << endl;
        cerr << "                 cache) or" << endl;
        cerr << "                 size,associativity,blocksize,size,associativity,blocksize" << endl;
        cerr << "                 (for two caches)" << endl;
        return 1;
    }

    ifstream f(filename);
    if (!f.is_open())
    {
        cerr << "Can't open file " << filename << endl;
        return 1;
    }

    e20_processor instuction; // instantiating e20 processor.

    load_machine_code(f, instuction.memory);

    int numLinesL1, L1size, L1assoc, L1blocksize;
    int numLinesL2, L2size, L2assoc, L2blocksize;
    bool isTwoCache;

    /* parse cache config */
    if (cache_config.size() > 0)
    {
        vector<int> parts;
        size_t pos;
        size_t lastpos = 0;
        while ((pos = cache_config.find(",", lastpos)) != string::npos)
        {
            parts.push_back(stoi(cache_config.substr(lastpos, pos)));
            lastpos = pos + 1;
        }
        parts.push_back(stoi(cache_config.substr(lastpos)));
        if (parts.size() == 3)
        {
            L1size = parts[0];
            L1assoc = parts[1];
            L1blocksize = parts[2];
            isTwoCache = false;

            one_cache(L1assoc, L1size, L1blocksize, numLinesL1); //sets cache 1 configrations.
        }
        else if (parts.size() == 6)
        {

            L1size = parts[0];
            L1assoc = parts[1];
            L1blocksize = parts[2];
            L2size = parts[3];
            L2assoc = parts[4];
            L2blocksize = parts[5];
            isTwoCache = true;
             
            //sets cache 1 and 2 configrations.
            two_cache(L1assoc, L2assoc, numLinesL1, numLinesL2, L1size, L2size, L1blocksize, L2blocksize);
        }
        else
        {
            cerr << "Invalid cache config" << endl;
            return 1;
        }
  
        cache l1 (numLinesL1, L1blocksize, L1assoc);  // instantiating Cache 1.
        cache l2 (numLinesL2, L2blocksize, L2assoc);  // instantiating Cache 2.
      
        e20Sim(instuction, l1, l2, isTwoCache);      // Run the e20 processor.

    }

    return 0;
}
// ra0Eequ6ucie6Jei0koh6phishohm9