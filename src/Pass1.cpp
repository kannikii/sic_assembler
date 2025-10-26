#include "../include/assembler.h"

Pass1::Pass1(OPTAB* opt, SYMTAB* sym) 
    : optab(opt), symtab(sym), locctr(0), startAddr(0), programName("") {}

int Pass1::getInstructionLength(const std::string& mnemonic, const std::string& operand) {
    if (!optab->isInstruction(mnemonic)) {
        return 0;
    }
    
    int format = optab->getFormat(mnemonic);
    
    // Format 4 체크 (+ 접두사가 있는 경우)
    if (!operand.empty() && operand[0] == '+') {
        return 4;
    }
    
    return format;
}

int Pass1::getDirectiveLength(const std::string& directive, const std::string& operand) {
    if (directive == "WORD") {
        return 3;
    } else if (directive == "RESW") {
        return 3 * std::stoi(operand);
    } else if (directive == "BYTE") {
        if (operand.size() >= 3 && operand[0] == 'C' && operand[1] == '\'') {
            // C'...'
            size_t start = operand.find('\'');
            size_t end = operand.rfind('\'');
            if (start != std::string::npos && end != std::string::npos && end > start) {
                return end - start - 1;
            }
        } else if (operand.size() >= 3 && operand[0] == 'X' && operand[1] == '\'') {
            // X'...'
            size_t start = operand.find('\'');
            size_t end = operand.rfind('\'');
            if (start != std::string::npos && end != std::string::npos && end > start) {
                return (end - start - 1 + 1) / 2; // 홀수 처리
            }
        }
    } else if (directive == "RESB") {
        return std::stoi(operand);
    }
    return 0;
}

bool Pass1::execute(const std::string& srcFilename) {
    std::ifstream file(srcFilename);
    if (!file.is_open()) {
        std::cerr << "Error: Cannot open source file: " << srcFilename << std::endl;
        return false;
    }
    
    std::string line;
    int lineNum = 0;
    
    while (std::getline(file, line)) {
        lineNum++;
        
        // 빈 줄이나 주석 건너뛰기
        if (line.empty()) continue;
        
        SourceLine parsed = Parser::parseLine(line);
        
        if (parsed.opcode.empty()) continue;
        
        // START 처리
        if (parsed.opcode == "START") {
            programName = parsed.label;
            startAddr = std::stoi(parsed.operand, nullptr, 16);
            locctr = startAddr;
            
            IntermediateLine intLine;
            intLine.location = locctr;
            intLine.label = parsed.label;
            intLine.opcode = parsed.opcode;
            intLine.operand = parsed.operand;
            intLine.objcode = "";
            intLine.hasLocation = true;
            intFile.push_back(intLine);
            continue;
        }
        
        // END 처리
        if (parsed.opcode == "END") {
            IntermediateLine intLine;
            intLine.location = 0;
            intLine.label = parsed.label;
            intLine.opcode = parsed.opcode;
            intLine.operand = parsed.operand;
            intLine.objcode = "";
            intLine.hasLocation = false;
            intFile.push_back(intLine);
            break;
        }
        
        // 현재 위치 저장
        int currentLoc = locctr;
        
        // 라벨이 있으면 SYMTAB에 추가
        if (!parsed.label.empty()) {
            if (!symtab->insert(parsed.label, currentLoc)) {
                std::cerr << "Warning at line " << lineNum 
                          << ": Duplicate symbol " << parsed.label << std::endl;
            }
        }
        
        // 명령어 길이 계산
        int length = 0;
        if (optab->isInstruction(parsed.opcode)) {
            length = getInstructionLength(parsed.opcode, parsed.operand);
        } else {
            length = getDirectiveLength(parsed.opcode, parsed.operand);
        }
        
        // 중간파일에 추가
        IntermediateLine intLine;
        intLine.location = currentLoc;
        intLine.label = parsed.label;
        intLine.opcode = parsed.opcode;
        intLine.operand = parsed.operand;
        intLine.objcode = "";
        intLine.hasLocation = true;
        intFile.push_back(intLine);
        
        // LOCCTR 증가
        locctr += length;
    }
    
    file.close();
    std::cout << "Pass 1 completed: " << lineNum << " lines processed" << std::endl;
    return true;
}

void Pass1::writeIntFile(const std::string& intFilename) {
    std::ofstream file(intFilename);
    if (!file.is_open()) {
        std::cerr << "Error: Cannot write intermediate file" << std::endl;
        return;
    }
    
    for (const auto& line : intFile) {
        if (line.hasLocation) {
            file << "0x" << std::hex << std::uppercase 
                 << std::setw(6) << std::setfill('0') << line.location << "  ";
        } else {
            file << "          ";
        }
        
        file << std::left << std::setfill(' ')
             << std::setw(10) << line.label
             << std::setw(10) << line.opcode
             << std::setw(20) << line.operand
             << line.objcode << std::endl;
    }
    
    file.close();
    std::cout << "Intermediate file written: " << intFilename << std::endl;
}

void Pass1::printIntFile() const {
    std::cout << "\n" << std::string(80, '=') << std::endl;
    std::cout << "INTERMEDIATE FILE (INTFILE)" << std::endl;
    std::cout << std::string(80, '=') << std::endl;
    std::cout << std::left 
              << std::setw(10) << "LOC"
              << std::setw(10) << "LABEL"
              << std::setw(10) << "OPCODE"
              << std::setw(20) << "OPERAND"
              << "OBJCODE" << std::endl;
    std::cout << std::string(80, '-') << std::endl;
    
    for (const auto& line : intFile) {
        if (line.hasLocation) {
            std::cout << "0x" << std::hex << std::uppercase 
                      << std::setw(6) << std::setfill('0') << line.location << "  ";
        } else {
            std::cout << "          ";
        }
        
        std::cout << std::dec << std::left << std::setfill(' ')
                  << std::setw(10) << line.label
                  << std::setw(10) << line.opcode
                  << std::setw(20) << line.operand
                  << line.objcode << std::endl;
    }
    std::cout << std::string(80, '=') << std::endl;
}

int Pass1::getProgramLength() const {
    return locctr - startAddr;
}

int Pass1::getStartAddress() const {
    return startAddr;
}

int Pass1::getFinalLocctr() const {
    return locctr;
}


// ======== [추가] ========
const std::vector<IntermediateLine>& Pass1::getIntFile() const {
    return intFile;
}

std::string Pass1::getProgramName() const {
    return programName;
}