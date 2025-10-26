// ========== src/main.cpp ==========
#include "../include/assembler.h"

int main() {
    std::cout << "\n" << std::string(70, '=') << std::endl;
    std::cout << "       SIC/XE ASSEMBLER - PASS 1" << std::endl;
    std::cout << std::string(70, '=') << std::endl;
    
    // OPTAB 로드
    std::cout << "\n[Step 1] Loading OPTAB..." << std::endl;
    OPTAB optab;
    if (!optab.load("input/optab.txt")) {
        std::cerr << "Failed to load OPTAB. Exiting..." << std::endl;
        return 1;
    }
    
    // SYMTAB 생성
    std::cout << "\n[Step 2] Initializing SYMTAB..." << std::endl;
    SYMTAB symtab;
    std::cout << "SYMTAB initialized successfully" << std::endl;
    
    // Pass 1 실행
    std::cout << "\n[Step 3] Running Pass 1..." << std::endl;
    Pass1 pass1(&optab, &symtab);
    
    if (!pass1.execute("input/SRCFILE")) {
        std::cerr << "Pass 1 failed. Exiting..." << std::endl;
        return 1;
    }
    
    // 결과 출력
    std::cout << "\n" << std::string(70, '=') << std::endl;
    std::cout << "       PASS 1 COMPLETED SUCCESSFULLY" << std::endl;
    std::cout << std::string(70, '=') << std::endl;
    
    // SYMTAB 출력 (메모리 상에 존재)
    symtab.print();
    
    // SYMTAB 파일 저장 (선택사항 - 보고서용)
    symtab.writeToFile("output/SYMTAB.txt");
    std::cout << "\nSYMTAB written to: output/SYMTAB.txt (for report)" << std::endl;
    
    // 중간파일 출력
    pass1.printIntFile();
    
    // 중간파일 저장
    pass1.writeIntFile("output/INTFILE");
    
    // 프로그램 정보
    std::cout << "\n" << std::string(70, '=') << std::endl;
    std::cout << "       PROGRAM INFORMATION" << std::endl;
    std::cout << std::string(70, '=') << std::endl;
    std::cout << "Start Address:  0x" << std::hex << std::uppercase 
              << std::setw(6) << std::setfill('0') 
              << pass1.getStartAddress() << std::endl;
    std::cout << "Final LOCCTR:   0x" << std::hex << std::uppercase 
              << std::setw(6) << std::setfill('0') 
              << pass1.getFinalLocctr() << std::endl;
    std::cout << "Program Length: 0x" << std::hex << std::uppercase 
              << std::setw(4) << std::setfill('0') 
              << pass1.getProgramLength() << " (" 
              << std::dec << pass1.getProgramLength() << " bytes)" << std::endl;
    std::cout << std::string(70, '=') << std::endl;
    
    std::cout << "\n✓ All output files generated successfully!" << std::endl;
    std::cout << "  - output/INTFILE (Pass 1 output)" << std::endl;
    std::cout << "  - output/SYMTAB.txt (Symbol table)" << std::endl;
    
    return 0;
}