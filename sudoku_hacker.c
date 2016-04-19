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
void printf_sudoku (int sudoku[LEN_SUDOKU][LEN_SUDOKU][10]);
int sudoku_exclude (int sudoku[LEN_SUDOKU][LEN_SUDOKU][10], int isFinished[LEN_SUDOKU][LEN_SUDOKU]);

int main(int argc, const char * argv[]) {
    char questFileName[LEN_FILENAME];
    FILE *questFile;
    
    /* 读取数独问题文件 */
    printf("> Please enter the name of your sudoku quest file:\n");
    printf("> ");
    fgets(questFileName, LEN_FILENAME, stdin);
    questFileName[strlen(questFileName) - 1] = '\0';    // 删去fgets()获得的字符串结尾的'\n'
    
    if (strcmp(questFileName, "exit") == 0) return 0;
    questFile = fopen(questFileName, "r");
    
    // 处理打开失败的
    while (!questFile) {
        printf("> Failed to open file.\n");
        printf("> Please enter the name of your sudoku quest file again:\n");
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
            if (fscanf(questFile, "%d", &sudoku[x][y][0]) != 1){
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
    printf_sudoku(sudoku);
    
    /* 解数独 */
    /* 剔除法 */
    // 确定完成情况
    int cnt_blank = 0;
    int isFinished[LEN_SUDOKU][LEN_SUDOKU];
    for (int x = 0; x < LEN_SUDOKU; x++) {
        for (int y = 0; y < LEN_SUDOKU; y++) {
            if (sudoku[x][y][0]) {
                isFinished[x][y] = 1;
                continue;
            }
            isFinished[x][y] = 0;
            cnt_blank++;
            for (int list = 1; list < 10; list++) {
                sudoku[x][y][list] = 1;
            }
        }
    }
    printf("> %d blank(s) founded.\n", cnt_blank);
    
    // 剔除
    printf("> Searching the only solutions...\n");
    int cnt_exclude;
    do {
        cnt_exclude = sudoku_exclude(sudoku, isFinished);
    } while (cnt_exclude > 0);
    
    // 检查是否全部完成
    int isAllFinished = 1;
    for (int x = 0; x < LEN_SUDOKU; x++) {
        for (int y = 0; y < LEN_SUDOKU; y++) {
            if (!isFinished[x][y]) {
                isAllFinished = 0;
            }
        }
    }
    
    if (!isAllFinished) {
        printf("> Failed to complete.\n");
        printf("> The result now:\n");
        printf_sudoku(sudoku);
        
        /* 穷举法 */
        // 搜索穷举起始位置
        int isBeginFound = 0;
        int begin_x = 0;
        int begin_y = 0;
        // 确定完成情况
        cnt_blank = 0;
        for (int x = 0; x < LEN_SUDOKU; x++) {
            for (int y = 0; y < LEN_SUDOKU; y++) {
                if (isFinished[x][y]) continue;
                sudoku[x][y][0] = 1;
                cnt_blank++;
                if (!isBeginFound) {
                    begin_x = x;
                    begin_y = y;
                    isBeginFound = 1;
                }
            }
        }
        sudoku[begin_x][begin_y][0] = 0;
        
        /* 当空大于12个时提示用户将会非常耗时 */
        printf("> %d blank(s) founded.\n", cnt_blank);
        if (cnt_blank > 12) {
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
    
    /* 输出结果 */
    printf("> Successed.\n");
    printf("> Result:\n");
    printf_sudoku(sudoku);
    
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
                // 检查行和列
                for (int check_col = y + 1; check_col < LEN_SUDOKU; check_col++) {
                    if (sudoku[x][y][0] == sudoku[x][check_col][0]) return 1;
                }
                
                // 检查列
                for (int check_row = x + 1; check_row < LEN_SUDOKU; check_row++) {
                    if (sudoku[x][y][0] == sudoku[check_row][y][0]) return 1;
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
void printf_sudoku (int sudoku[LEN_SUDOKU][LEN_SUDOKU][10]) {
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
int sudoku_exclude (int sudoku[LEN_SUDOKU][LEN_SUDOKU][10], int isFinished[LEN_SUDOKU][LEN_SUDOKU]) {
    int cnt_exclude = 0;
    for (int x = 0; x < LEN_SUDOKU; x++) {
        for (int y = 0; y < LEN_SUDOKU; y++) {
            if (isFinished[x][y]) continue;
            
            // 检查行和列
            for (int check = 0; check < LEN_SUDOKU; check++) {
                if (sudoku[x][check][0] && sudoku[x][y][sudoku[x][check][0]]) {
                    sudoku[x][y][sudoku[x][check][0]] = 0;
                    cnt_exclude++;
                }
                if (sudoku[check][y][0] && sudoku[x][y][sudoku[check][y][0]]) {
                    sudoku[x][y][sudoku[check][y][0]] = 0;
                    cnt_exclude++;
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
                        cnt_exclude++;
                    }
                }
            }
            
            // 检查是否有唯一候选数
            int cnt_left = 0;
            int temp_result = 0;
            for (int check_list = 1; check_list < 10; check_list++) {
                if (sudoku[x][y][check_list]) {
                    cnt_left++;
                    temp_result = check_list;
                }
            }
            if (cnt_left == 1) {
                sudoku[x][y][0] = temp_result;
                isFinished[x][y] = 1;
            }
        }
    }
    return cnt_exclude;
}