#pragma once

#include <cstdint>
#include <iostream>


/**
 * @brief An e20 processor object is represented by the e20_processor class. It has a 16-bit program counter, 
 * seven read and write (R/W) registers, and register $0 is an exclusively read (R) register with all 
 * registers being 16 bits, as well as a memory array with 8191 instructions, each instruction comprising 16 bits.
 */
class e20_processor
{

// All data members are public.
public:
  size_t const static NUM_REGS = 8;   
  size_t const static MEM_SIZE = 1 << 13;  //8191
  uint16_t pc, imm;                        //unsniged 16 bits program counter and immediate value.
  unsigned *memory, *regs;                 // memory array and register array.


  /**
   * @brief Constructer of e20 object would intilize the program counter,
   *  8191 memory cells, immediate , and all 8 registers.
   */
  e20_processor(): memory(new unsigned[MEM_SIZE]), regs(new unsigned[NUM_REGS]){
    
    pc = 0;  // intilizing program counter to zero.
    imm = 0; // intilizing immediate value to zero.
    
    // All 8191 memory cells are intilized to zero.
    for(int i=0; i < MEM_SIZE; i++){
      memory[i] =0; 
    }
    
     // All regs are intilized to zero.
    for(int j=0; j<NUM_REGS; j++){
      regs[j]=0; 
    }

  }
  
  /*
  The following 3 not inline member fuctions are large so for better perfurmance it is better to
  define them outside the e20 class.
  */
  void getThreeRegInst(unsigned regSrcA, unsigned regSrcB, unsigned regDst);

  void getTwoRegInst(unsigned opcode, unsigned regSrcA, unsigned regSrcB);

  void getNoRegInst(unsigned opcode);

  
  /**
   * @brief Adds the value of registers $regSrcA and $regSrcB, storing the sum in $regDst. 
   * Symbolically: R[regDst] <- R[regSrcA] + R[regSrcB].
   * 
   * @param regSrcA src regA [10:12].
   * @param regSrcB src regB [7:9].
   * @param regDst  Dst regC [4:6]. 
   */
  void add(unsigned regSrcA, unsigned regSrcB, unsigned regDst){
    regs[regDst] = regs[regSrcA] + regs[regSrcB];
    pc += 1; //increment program counter.
  }

  /**
   * @brief Subtracts the value of register $regSrcB from $regSrcA, storing the difference in $regDst. 
   * Symbolically: R[regDst] <- R[regSrcA] - R[regSrcB].
   * 
   * @param regSrcA src regA [10:12].
   * @param regSrcB src regB [7:9].
   * @param regDst  Dst regC [4:6]. 
   */
  void sub(unsigned regSrcA, unsigned regSrcB, unsigned regDst){
    regs[regDst] = regs[regSrcA] - regs[regSrcB];
    pc += 1; //increment program counter.
  }

  /**
   * @brief Calculates the bitwise AND of the value of registers $regSrcA and $regSrcB, storing the result in $regDst.
   * Symbolically: R[regDst] <- R[regSrcA] & R[regSrcB].
   * 
   * @param regSrcA src regA [10:12].
   * @param regSrcB src regB [7:9].
   * @param regDst  Dst regC [4:6]. 
   */
  void And(unsigned regSrcA, unsigned regSrcB, unsigned regDst){
    regs[regDst] = regs[regSrcA] & regs[regSrcB];
    pc += 1; //increment program counter.
  }

  /**
   * @brief Calculates the bitwise OR of the value of registers $regSrcA and 
   * $regSrcB, storing the result in $regDst.
   * 
   * @param regSrcA src regA [10:12].
   * @param regSrcB src regB [7:9].
   * @param regDst  Dst regC [4:6]. 
   */
  void Or(unsigned regSrcA, unsigned regSrcB, unsigned regDst){
    regs[regDst] = regs[regSrcA] | regs[regSrcB];
    pc += 1; //increment program counter.
  }
  
  /**
   * @brief Compares the value of $regSrcA with $regSrcB, setting $regDst to 1 if $regSrcA is less than $regSrcB,
   * and to 0 otherwise. The comparison performed is unsigned. 
   * 
   * @param regSrcA src regA [10:12].
   * @param regSrcB src regB [7:9].
   * @param regDst  Dst regC [4:6]. 
   */
  void slt(unsigned regSrcA, unsigned regSrcB, unsigned regDst){
    regs[regDst] = (regs[regSrcA] < regs[regSrcB]) ? 1 : 0;
    pc += 1; //increment program counter.
  }
  
  /**
   * @brief Jumps unconditionally to the memory address in $reg.The jump destination 
   * is expressed as an absolute address. All 16 bits of the value of $reg are stored into the program counter.
   * 
   * @param regSrcA src regA [10:12].
   */
  void jr(unsigned regSrcA){
    pc = regs[regSrcA]; // jump to register, with a register containing a 16-bit destination address.
  }

  /**
   * @brief Compares the value of $regSrc with sign-extended imm, setting $regDst to 1 
   * if $regSrc is less than imm, and to 0 otherwise. The comparison performed is unsigned.
   * 
   * @param regSrcA src regA [10:12].
   * @param regDst  Dst regC [7:9].
   * @param imm     sign-extended immdatidate value[6:0].
   */
  void slti(unsigned regSrcA, unsigned regDst, unsigned imm)
  {
    regs[regDst] = (regs[regSrcA] < imm) ? 1 : 0;
    pc += 1; //increment program counter.
  }

  /**
   * @brief Calculates a memory pointer by summing the signed number imm and the value $regAddr, 
   * and loads thevalue from that address, storing it in $regDst.
   * 
   * @param regSrcA src regA [10:12].
   * @param regDst  Dst regC [7:9].
   * @param imm     sign-extended immdatidate value[6:0].
   */
  void lw(unsigned regSrcA, unsigned regDst, unsigned imm)
  {
    regs[regDst] = memory[(regs[regSrcA] + imm) & 8191];
    pc += 1; //increment program counter.
  }

  /**
   * @brief Calculates a memory pointer by summing the signed number imm and the value $regAddr, 
   * and stores the value in $regSrc to that memory address.
   * 
   * @param regSrcA  src regA [10:12].
   * @param regSrcB  src regB [7:9].
   * @param imm      sign-extended immdatidate value[6:0].
   */
  void sw(unsigned regSrcA, unsigned regSrcB, unsigned imm)
  {
    memory[(regs[regSrcA] + imm) & 8191] = regs[regSrcB];
    pc += 1; //increment program counter.
  }

  /**
   * @brief Compares the value of $regA with $regB. If the values are equal, jumps to the memory address
   *  identified by the address imm, which is encoded as the signed number rel_imm.
   * 
   * @param regSrcA  src regA [10:12].
   * @param regSrcB  src regB [7:9].
   * @param imm      sign-extended immdatidate value[0:6].
   */
  void jeq(unsigned regSrcA, unsigned regSrcB, unsigned imm)
  {
    pc = (regs[regSrcA] == regs[regSrcB]) ? pc + 1 + imm : pc + 1;
  }

  /**
   * @brief Adds the value of register $regSrc and the signed number imm, storing the sum in $regDst.
   * 
   * @param regSrcA  src regA [10:12].
   * @param regSrcB  src regB [7:9].
   * @param imm      sign-extended immdatidate value[0:6].
   */
  void addi(unsigned regSrcA, unsigned regDst, unsigned imm)
  {
    regs[regDst] = regs[regSrcA] + imm;
    pc += 1;  //increment program counter.
  }

  /**
   * @brief Jumps unconditionally to the memory address 13 bits immdatidate.
   * 
   * @param imm immdatidate value [0:13].
   */
  void j(unsigned imm)
  {
    pc = imm & 8191; // most significant 3 bits of the program counter will be set to zero.
  }

  /**
   * @brief Stores the memory address of the next instruction in sequence in register $7, then jumps 
   * unconditionally to the memory address immdatidate.
   * 
   * @param imm immdatidate value [0:13].
   */
  void jal(unsigned imm)
  {
    regs[7] = pc + 1; 
    pc = imm & 8191;    // most significant 3 bits of the program counter will be set to zero.
  }
   
   /**
    * @brief The deconstuctor of e20 processor deletes the data of memeory and registers 
    * when out of scope from the heap.
    */
   ~e20_processor(){
    delete[] memory;
    delete[] regs;
  }

};


/**
 * @brief The getThreeRegInst not inline member fuction selects the three regeister instuction
 * based the four LSB imm.
 * Instuctions: add, sub, and, or, slt, jr.
 * @param regSrcA src regA [10:12].
 * @param regSrcB src regB [7:9].
 * @param regDst  Dst regC [4:6]. 
 */
inline void e20_processor::getThreeRegInst(unsigned regSrcA, unsigned regSrcB, unsigned regDst) {
   
     if (imm == 0b0000){
      add(regSrcA, regSrcB, regDst);
    }

    else if (imm == 0b0001){
      sub(regSrcA, regSrcB, regDst);
    }

    else if (imm == 0b0010){
      And(regSrcA, regSrcB, regDst);
    }

    else if (imm == 0b0011){
      Or(regSrcA, regSrcB, regDst);
    }

    else if (imm == 0b0100){
      slt(regSrcA, regSrcB, regDst);
    }

    else if (imm == 0b1000) {
      jr(regSrcA);
    }

  }

  /**
   * @brief The getTwoRegInst not inline member fuction selects the two regeister instuction
   * based on the opcode. 
   * @param opcode   addi, slti, lw, sw, jeq.
   * @param regSrcA  src regA [10:12]. 
   * @param regSrcB  Dst regC [4:6]. 
   */
  inline void e20_processor::getTwoRegInst(unsigned opcode, unsigned regSrcA, unsigned regSrcB){

    if (opcode == 0b111){
     
    slti(regSrcA, regSrcB, imm);

    }
    else if (opcode == 0b100){
    
    lw(regSrcA, regSrcB, imm);

    }

    else if (opcode == 0b101){
    
    sw(regSrcA, regSrcB, imm);

    }

    else if (opcode == 0b110){

    jeq(regSrcA, regSrcB, imm);

    }

    else if (opcode == 0b001) {

    addi(regSrcA, regSrcB, imm);

    }

  }


  /**
  * @brief The getNoRegInst not inline member fuction selects the no regeister instuction
  * based on the opcode. 
  * @param opcode j, jal.
  */
   inline void e20_processor::getNoRegInst(unsigned opcode){

    if (opcode == 0b010){
      j(imm);
    }

    else if (opcode == 0b011){
      jal(imm);
    }

  }
