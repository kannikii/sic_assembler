#ifndef ASSEMBLER_H
#define ASSEMBLER_H

#include <string>
#include <map>
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <algorithm>

// ==================== OPTAB ====================
struct InstructionInfo {
    std::string opcode;
    int format;  // 1, 2, 3/4
};

class OPTAB {
private:
    std::map<std::string, InstructionInfo> table;
    
    // 자동으로 형식 결정
    int determineFormat(const std::string& mnemonic);

public:
    OPTAB();
    bool load(const std::string& filename);
    bool isInstruction(const std::string& mnemonic) const;
    std::string getOpcode(const std::string& mnemonic) const;
    int getFormat(const std::string& mnemonic) const;
    void printTable() const;
};

// ==================== SYMTAB ====================
class SYMTAB {
private:
    std::map<std::string, int> table;

public:
    SYMTAB();
    bool insert(const std::string& symbol, int address);
    int lookup(const std::string& symbol) const;
    bool exists(const std::string& symbol) const;
    void print() const;
    void writeToFile(const std::string& filename) const;
};

// ==================== Parser ====================
struct SourceLine {
    std::string label;
    std::string opcode;
    std::string operand;
};

class Parser {
public:
    static SourceLine parseLine(const std::string& line);
    static std::string trim(const std::string& str);
    static bool startsWithWhitespace(const std::string& line);
};

// ==================== Pass1 ====================
struct IntermediateLine {
    int location;
    std::string label;
    std::string opcode;
    std::string operand;
    std::string objcode;
    bool hasLocation;
};

class Pass1 {
private:
    OPTAB* optab;
    SYMTAB* symtab;
    std::vector<IntermediateLine> intFile;
    int locctr;
    int startAddr;
    std::string programName;
    
    int getInstructionLength(const std::string& mnemonic, const std::string& operand);
    int getDirectiveLength(const std::string& directive, const std::string& operand);

public:
    Pass1(OPTAB* opt, SYMTAB* sym);
    bool execute(const std::string& srcFilename);
    void writeIntFile(const std::string& intFilename);
    void printIntFile() const;
    int getProgramLength() const;
    int getStartAddress() const;
    int getFinalLocctr() const;
};

#endif