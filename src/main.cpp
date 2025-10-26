// ========== src/main.cpp (수정) ==========
#include "../include/assembler.h"

int main() {
    std::cout << "\n" << std::string(70, '=') << std::endl;
    std::cout << "           SIC/XE ASSEMBLER" << std::endl;
    std::cout << std::string(70, '=') << std::endl;
    
    // ==================================================
    // 1. OPTAB 로드
    // ==================================================
    std::cout << "\n[Step 1] Loading OPTAB..." << std::endl;
    OPTAB optab;
    if (!optab.load("input/optab.txt")) {
        std::cerr << "Failed to load OPTAB. Exiting..." << std::endl;
        return 1;
    }
    
    // ==================================================
    // 2. SYMTAB 생성
    // ==================================================
    std::cout << "\n[Step 2] Initializing SYMTAB..." << std::endl;
    SYMTAB symtab;
    std::cout << "SYMTAB initialized successfully" << std::endl;
    
    // ==================================================
    // 3. Pass 1 실행
    // ==================================================
    std::cout << "\n[Step 3] Running Pass 1..." << std::endl;
    Pass1 pass1(&optab, &symtab);
    
    if (!pass1.execute("input/SRCFILE")) {
        std::cerr << "Pass 1 failed. Exiting..." << std::endl;
        return 1;
    }
    
    // Pass 1 결과 (중간파일) 저장
    pass1.writeIntFile("output/INTFILE");
    // SYMTAB 파일 저장
    symtab.writeToFile("output/SYMTAB.txt");
    std::cout << "Pass 1 output (INTFILE, SYMTAB.txt) saved." << std::endl;

    // 프로그램 정보
    int startAddress = pass1.getStartAddress();
    int programLength = pass1.getProgramLength();
    std::string programName = pass1.getProgramName();
    
    // ==================================================
    // 4. [신규] Pass 2 실행
    // ==================================================
    Pass2 pass2(&optab, &symtab, pass1.getIntFile(), 
                startAddress, programLength, programName);

    if (!pass2.execute()) {
        std::cerr << "Pass 2 failed. Exiting..." << std::endl;
        return 1;
    }

    // Pass 2 결과 (오브젝트 파일) 저장
    pass2.writeObjFile("output/OBJFILE");
    
    // ==================================================
    // 5. [신규] 최종 결과 출력
    // ==================================================
    std::cout << "\n" << std::string(70, '=') << std::endl;
    std::cout << "     ASSEMBLY COMPLETED SUCCESSFULLY" << std::endl;
    std::cout << std::string(70, '=') << std::endl;
    
    // 최종 리스팅 파일 (objcode 포함)
    pass2.printListingFile();

    // 최종 오브젝트 파일
    pass2.printObjFile();

    std::cout << "\n✓ All output files generated successfully!" << std::endl;
    std::cout << "  - output/INTFILE (Pass 1 output)" << std::endl;
    std::cout << "  - output/SYMTAB.txt (Symbol table)" << std::endl;
    std::cout << "  - output/OBJFILE (Pass 2 output)" << std::endl;
    
    return 0;
}