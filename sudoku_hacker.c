//
//  sudoku_hacker.c
//  Sudoku Hacker
//
//  Created by Lucka.
//

#include <stdio.h>
#include <string.h>
#define LEN_FILENAME 256
#define LEN_SUDOKU 9

int isIllegal (int sudoku[LEN_SUDOKU][LEN_SUDOKU][10]);
void PrintSudoku (int sudoku[LEN_SUDOKU][LEN_SUDOKU][10]);

int Exclude (int sudoku[LEN_SUDOKU][LEN_SUDOKU][10]);
void SearchOnlySolution (int sudoku[LEN_SUDOKU][LEN_SUDOKU][10], int x, int y);
void Exhaustion (int sudoku[LEN_SUDOKU][LEN_SUDOKU][10], int isFinished[LEN_SUDOKU][LEN_SUDOKU], int begin_x, int begin_y);

int main(int argc, const char * argv[]) {
    /* 读取数独问题文件 */
    char questFileName[LEN_FILENAME];
    FILE *questFile;
    printf("> Please enter the file name of the sudoku quest:\n");
    printf("> ");
    fgets(questFileName, LEN_FILENAME, stdin);
    questFileName[strlen(questFileName) - 1] = '\0';    // 删去fgets()获得的字符串结尾的'\n'
    
    if (strcmp(questFileName, "exit") == 0) return 0;
    questFile = fopen(questFileName, "r");
    
    // 处理打开失败的
    while (!questFile) {
        printf("> Failed to open file.\n");
        printf("> Please enter the file name of the sudoku quest again:\n");
        printf("> ");
        fgets(questFileName, LEN_FILENAME, stdin);
        questFileName[strlen(questFileName) - 1] = '\0';
        if (strcmp(questFileName, "exit") == 0) return 0;
        questFile = fopen(questFileName, "r");
    }
    
    // 读取数独至数组
    printf("> Loading sudoku...\n");
    int sudoku[LEN_SUDOKU][LEN_SUDOKU][10];
    for (int x = 0; x < LEN_SUDOKU; x++) {
        for (int y = 0; y < LEN_SUDOKU; y++) {
            if (!fscanf(questFile, "%d", &sudoku[x][y][0])){
                printf("> Fail to load the sudoku.\n");
                return 0;
            };
        }
    }
    printf("> Successed.\n");
    fclose(questFile);
    
    /* 检查数独 */
    printf("> Checking...\n");
    if (isIllegal(sudoku)) {
        printf("> Illegal sudoku!\n");
        return 0;
    }
    printf("> Successed.\n");
    
    /* 输出数独问题 */
    printf("> Your sudoku quest:\n");
    PrintSudoku(sudoku);
    
    /* 解数独 */
    /* 剔除法 */
    // 确定完成情况
    int cntBlank = 0;
    for (int x = 0; x < LEN_SUDOKU; x++) {
        for (int y = 0; y < LEN_SUDOKU; y++) {
            if (sudoku[x][y][0]) continue;
            cntBlank++;
            
            // 初始化候选数表
            for (int num = 1; num < 10; num++) {
                sudoku[x][y][num] = 1;
            }
        }
    }
    printf("> %d blank(s) founded.\n", cntBlank);
    
    // 剔除
    printf("> Searching the only solutions...\n");
    int cntExcluded;
    do {
        cntExcluded = Exclude(sudoku);
    } while (cntExcluded > 0);
    
    // 检查是否全部完成
    int isAllFinished = 1;
    for (int x = 0; x < LEN_SUDOKU; x++) {
        for (int y = 0; y < LEN_SUDOKU; y++) {
            if (!sudoku[x][y][0]) {
                isAllFinished = 0;
            }
        }
    }
    
    if (!isAllFinished) {
        printf("> Failed to complete.\n");
        printf("> The result now:\n");
        PrintSudoku(sudoku);
        
        /* 穷举法 */
        // 搜索穷举起始位置
        int isBeginFound = 0;
        int begin_x = 0;
        int begin_y = 0;
        
        // 确定完成情况
        int isFinished[LEN_SUDOKU][LEN_SUDOKU] = {0};
        cntBlank = 0;
        for (int x = 0; x < LEN_SUDOKU; x++) {
            for (int y = 0; y < LEN_SUDOKU; y++) {
                if (sudoku[x][y][0]) {
                    isFinished[x][y] = 1;
                    continue;
                }
                cntBlank++;
                sudoku[x][y][0] = 1;
                if (!isBeginFound) {
                    begin_x = x;
                    begin_y = y;
                    isBeginFound = 1;
                }
            }
        }
        sudoku[begin_x][begin_y][0] = 0;
        
        /* 当空大于12个时提示用户将会非常耗时 */
        printf("> %d blank(s) founded.\n", cntBlank);
        if (cntBlank > 12) {
            printf("> Too many blanks, it may spend much time.\n");
            printf("> If you want to continue, enter Y please.\n");
            printf("> ");
            char cmd;
            cmd = getchar();
            while (getchar() != '\n');
            if (cmd != 'Y' && cmd != 'y') return 0;
        }
        
        // 穷举
        printf("> Searching one by one...\n");
        Exhaustion(sudoku, isFinished, begin_x, begin_y);
    }
    
    /* 输出结果 */
    printf("> Successed.\n");
    printf("> Result:\n");
    PrintSudoku(sudoku);
    
    /* 储存结果 */
    char resultFileName[LEN_FILENAME];
    FILE *resultFile;
    printf("> Please enter the name of file to save the result:\n");
    printf("> ");
    fgets(resultFileName, LEN_FILENAME, stdin);
    resultFileName[strlen(resultFileName) - 1] = '\0';    // 删去fgets()获得的字符串结尾的'\n'
    
    if (strcmp(resultFileName, "exit") == 0) {
        return 0;
    }
    resultFile = fopen(resultFileName, "w");
    
    // 保存数独至文件
    printf("> Saving result...\n");
    int cnt = 0;
    for (int x = 0; x < LEN_SUDOKU; x++) {
        for (int y = 0; y < LEN_SUDOKU; y++) {
            fprintf(resultFile, "%d", sudoku[x][y][0]);
            cnt++;
            if (cnt % LEN_SUDOKU == 0) {
                fputc('\n', resultFile);
            } else {
                fputc(' ', resultFile);
            }
        }
    }
    printf("> Saved.\n");
    fclose(resultFile);
    
    return 0;
}

// 检查数独合法性
int isIllegal (int sudoku[LEN_SUDOKU][LEN_SUDOKU][10]) {
    for (int x = 0; x < LEN_SUDOKU; x++) {
        for (int y = 0; y < LEN_SUDOKU; y++) {
            if (sudoku[x][y][0] != 0) {
                // 检查行
                for (int check_y = y + 1; check_y < LEN_SUDOKU; check_y++) {
                    if (sudoku[x][y][0] == sudoku[x][check_y][0]) return 1;
                }
                
                // 检查列
                for (int check_x = x + 1; check_x < LEN_SUDOKU; check_x++) {
                    if (sudoku[x][y][0] == sudoku[check_x][y][0]) return 1;
                }
                
                // 检查宫格
                int block_x = x / 3;
                int block_y = y / 3;
                for (int check_x = block_x * 3; check_x < block_x * 3 + 3; check_x++) {
                    for (int check_y = block_y * 3; check_y < block_y * 3 + 3; check_y++) {
                        if (check_x == x && check_y == y) continue;
                        else if (sudoku[x][y][0] == sudoku[check_x][check_y][0]) return 1;
                    }
                }
            }
        }
    }
    return 0;
}

// 输出数独
void PrintSudoku (int sudoku[LEN_SUDOKU][LEN_SUDOKU][10]) {
    for (int x = 0; x < LEN_SUDOKU; x++) {
        for (int y = 0; y < LEN_SUDOKU; y++) {
            printf("%d", sudoku[x][y][0]);
            if (y < LEN_SUDOKU - 1) {
                putchar(' ');
            }
        }
        putchar('\n');
    }
}

// 剔除法解数独
int Exclude (int sudoku[LEN_SUDOKU][LEN_SUDOKU][10]) {
    int cntExcluded = 0;
    for (int x = 0; x < LEN_SUDOKU; x++) {
        for (int y = 0; y < LEN_SUDOKU; y++) {
            if (sudoku[x][y][0]) continue;
            
            // 检查行和列
            for (int check = 0; check < LEN_SUDOKU; check++) {
                if (sudoku[x][check][0] && sudoku[x][y][sudoku[x][check][0]]) {
                    sudoku[x][y][sudoku[x][check][0]] = 0;
                    cntExcluded++;
                }
                if (sudoku[check][y][0] && sudoku[x][y][sudoku[check][y][0]]) {
                    sudoku[x][y][sudoku[check][y][0]] = 0;
                    cntExcluded++;
                }
            }
            
            // 检查宫格
            int block_x = x / 3;
            int block_y = y / 3;
            for (int check_x = block_x * 3; check_x < block_x * 3 + 3; check_x++) {
                for (int check_y = block_y * 3; check_y < block_y * 3 + 3; check_y++) {
                    if (check_x == x && check_y == y) continue;
                    
                    if (sudoku[check_x][check_y][0] && sudoku[x][y][sudoku[check_x][check_y][0]]) {
                        sudoku[x][y][sudoku[check_x][check_y][0]] = 0;
                        cntExcluded++;
                    }
                }
            }
            
            // 搜索唯一解并填入
            SearchOnlySolution(sudoku, x, y);
            
            // 检查是否有唯一候选数并填入
            int cntLeft = 0;
            int temp = 0;
            for (int num = 1; num < 10; num++) {
                if (sudoku[x][y][num]) {
                    cntLeft++;
                    temp = num;
                }
            }
            if (cntLeft == 1) {
                sudoku[x][y][0] = temp;
                for (int num = 1; num < 10; num++) {
                    sudoku[x][y][num] = 0;  // 安全起见
                }
            }
        }
    }
    return cntExcluded;
}

// 搜索唯一解并填入
void SearchOnlySolution (int sudoku[LEN_SUDOKU][LEN_SUDOKU][10], int x, int y) {
    for (int num = 1; num < 10; num++) {
        if (!sudoku[x][y][num]) continue;
        
        int isOnly = 1;
        
        // 检查行
        for (int check_y = 0; check_y < LEN_SUDOKU; check_y++) {
            if (sudoku[x][check_y][0] || check_y == y) continue;
            
            if (sudoku[x][check_y][num]) {
                isOnly = 0;
                break;
            }
        }
        if (isOnly) {
            sudoku[x][y][0] = num;
            break;
        }
        
        // 检查列
        isOnly = 1;
        for (int check_x = 0; check_x < LEN_SUDOKU; check_x++) {
            if (sudoku[check_x][y][0] || check_x == x) continue;
            
            if (sudoku[check_x][y][num]) {
                isOnly = 0;
                break;
            }
        }
        if (isOnly) {
            sudoku[x][y][0] = num;
            break;
        }
        
        // 检查宫格
        isOnly = 1;
        int block_x = x / 3;
        int block_y = y / 3;
        for (int check_x = block_x * 3; check_x < block_x * 3 + 3; check_x++) {
            for (int check_y = block_y * 3; check_y < block_y * 3 + 3; check_y++) {
                if (sudoku[check_x][check_y][0] || (check_x == x && check_y == y)) continue;
                
                if (sudoku[check_x][check_y][num]) {
                    isOnly = 0;
                    break;
                }
            }
            if (!isOnly) break;
        }
        if (isOnly) {
            sudoku[x][y][0] = num;
            break;
        }
    }
    
}

// 穷举
void Exhaustion (int sudoku[LEN_SUDOKU][LEN_SUDOKU][10], int isFinished[LEN_SUDOKU][LEN_SUDOKU], int begin_x, int begin_y) {
    do {
        // 横着遍历
        sudoku[begin_x][begin_y][0]++;
        
        // 进位
        int isCarry = 0;
        for (int x = begin_x; x < LEN_SUDOKU; x++) {
            for (int y = 0; y < LEN_SUDOKU; y++) {
                if (isFinished[x][y]) continue;
                
                if (isCarry) {
                    do {
                        sudoku[x][y][0]++;
                    } while (!sudoku[x][y][sudoku[x][y][0]] && sudoku[x][y][0] < 10);
                }
                if (sudoku[x][y][0] == 10) {
                    sudoku[x][y][0] = 1;
                    isCarry = 1;
                } else {
                    isCarry = 0;
                }
            }
        }
    } while (isIllegal(sudoku));
}