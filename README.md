# sudoku-hacker
A sudoku solver for learning C language, without any practical value.

## Description
一个数独求解器，目前使用穷举法因而没有实用性（空格超过6个运算时间就会暴涨）。

编写这个程序的主要目的是学习C语言的二维数组和文件的读写，将来（大概）会加入新的算法使其具有实用性。

## Supported Features

   | 功能
---|---
✓  | 读取用户指定文件中的数独问题
✓  | 检查数独的合法性
✓  | 在空格超过6个时向用户发出警告
✓  | 穷举法解数独
✓  | 将结果保存至用户指定的文件中

其中用的穷举法被<s>中二的</s>我命名为<s>土得掉渣的</s>数组进位法。
