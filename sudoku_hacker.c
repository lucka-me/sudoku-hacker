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

int isIllegal (int sudoku[LEN_SUDOKU][LEN_SUDOKU]);

int main(int argc, const char * argv[]) {
    char questFileName[LEN_FILENAME];
    FILE *questFile;
    
    /* 读取数独问题文件 */
    printf("> Please enter the name of your sudoku quest file:\n");
    printf("> ");
    fgets(questFileName, LEN_FILENAME, stdin);
    questFileName[strlen(questFileName) - 1] = '\0';    // 删去fgets()获得的字符串结尾的'\n'
    
    if (strcmp(questFileName, "exit") == 0) {
        return 0;
    }
    questFile = fopen(questFileName, "r");
    
    // 处理打开失败的
    while (!questFile) {
        printf("> Failed to open file.\n");
        printf("> Please enter the name of your sudoku quest file again:\n");
        printf("> ");
        fgets(questFileName, LEN_FILENAME, stdin);
        questFileName[strlen(questFileName) - 1] = '\0';
        if (strcmp(questFileName, "exit") == 0) {
            return 0;
        }
        questFile = fopen(questFileName, "r");
    }
    
    // 读取数独至数组
    printf("> Loading sudoku...\n");
    int quest[LEN_SUDOKU][LEN_SUDOKU] = {0};
    for (int x = 0; x < LEN_SUDOKU; x++) {
        for (int y = 0; y < LEN_SUDOKU; y++) {
            if (fscanf(questFile, "%d", &quest[x][y]) != 1){
                printf("> Fail to load the sudoku.\n");
                return 0;
            };
        }
    }
    fclose(questFile);
    
    /* 检查数独 */
    printf("> Successed.\n");
    printf("> Checking...\n");
    if (isIllegal(quest)) {
        printf("> Illegal sudoku!\n");
        return 0;
    }
    
    /* 输出数独问题 */
    printf("> Successed.\n");
    printf("> Your sudoku quest:\n");
    for (int x = 0; x < LEN_SUDOKU; x++) {
        for (int y = 0; y < LEN_SUDOKU; y++) {
            printf("%d ", quest[x][y]);
        }
        putchar('\n');
    }
    
    
    /* 解数独 */
    // 复制到结果数组
    int result[LEN_SUDOKU][LEN_SUDOKU] = {1};
    // 确认格子是否可编辑
    int editable[LEN_SUDOKU][LEN_SUDOKU];
    int cnt_editable = 0;
    // 搜索穷举起始位置
    int isBeginFound = 0;
    int begin_x = 0;
    int begin_y = 0;
    
    for (int x = 0; x < LEN_SUDOKU; x++) {
        for (int y = 0; y < LEN_SUDOKU; y++) {
            if (quest[x][y]) {
                result[x][y] = quest[x][y];
                editable[x][y] = 0;   //  0 表示本格已经钦定了
            } else {
                result[x][y] = 1;
                editable[x][y] = 1;
                cnt_editable++;
                // 搜索到穷举起始位置
                if (!isBeginFound) {
                    begin_x = x;
                    begin_y = y;
                    isBeginFound = 1;
                }
            }
        }
    }
    result[begin_x][begin_y] = 0;
    
    /* 当空大于6个时提示用户将会非常耗时 */
    printf("> %d blank(s) founded.\n", cnt_editable);
    if (cnt_editable > 6) {
        printf("> Too many blanks, it may spend much time.\n");
        printf("> If you want to continue, enter Y please.\n");
        printf("> ");
        char cmd;
        cmd = getchar();
        while (getchar() != '\n');
        if (cmd != 'Y' && cmd != 'y') {
            return 0;
        }
    }
    
    /* 穷举 */
    printf("> Calculating...\n");
    int isWrong = 0;
    do {
        // 横着遍历
        result[begin_x][begin_y]++;
        
        // 进位
        int isCarry = 0;
        for (int x = begin_x; x < LEN_SUDOKU; x++) {
            for (int y = 0; y < LEN_SUDOKU; y++) {
                if (editable[x][y]) {
                    if (isCarry) {
                        result[x][y]++;
                    }
                    if (result[x][y] == 10) {
                        result[x][y] = 1;
                        isCarry = 1;
                    } else {
                        isCarry = 0;
                    }
                }
            }
        }
        
        // 检查结果
        isWrong = isIllegal(result);
        
    } while (isWrong);
    
    /* 输出结果 */
    printf("> Successed.\n");
    printf("> Result:\n");
    for (int x = 0; x < LEN_SUDOKU; x++) {
        for (int y = 0; y < LEN_SUDOKU; y++) {
            printf("%d ", result[x][y]);
        }
        putchar('\n');
    }
    
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
            fprintf(resultFile, "%d", result[x][y]);
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

/* 函数区 */
int isIllegal (int sudoku[LEN_SUDOKU][LEN_SUDOKU]) {
    for (int x = 0; x < LEN_SUDOKU; x++) {
        for (int y = 0; y < LEN_SUDOKU; y++) {
            if (sudoku[x][y] != 0) {
                // 检查行
                for (int check_col= y + 1; check_col < LEN_SUDOKU; check_col++) {
                    if (sudoku[x][y] == sudoku[x][check_col]) {
                        return 1;
                    }
                }
                
                // 检查列
                for (int check_row= x + 1; check_row < LEN_SUDOKU; check_row++) {
                    if (sudoku[x][y] == sudoku[check_row][y]) {
                        return 1;
                    }
                }
                
                // 检查宫格
                int block_x = x / 3;
                int block_y = y / 3;
                for (int check_x = block_x * 3; check_x < block_x * 3 + 3; check_x++) {
                    for (int check_y = block_y * 3; check_y < block_y * 3 + 3; check_y++) {
                        if (check_x == x && check_y == y) {
                            continue;
                        } else if (sudoku[x][y] == sudoku[check_x][check_y]) {
                            return 1;
                        }
                    }
                }
            }
        }
    }
    return 0;
}