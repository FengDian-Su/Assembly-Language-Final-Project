1112 組合語言與系統程式 期末作業 SIC Assembler
資工二 4110056007 蘇豐典

想法：
主程式 main 從 line110 開始，先把 optable 讀進來
使用全域變數陣列的資料結構儲存

path1 從 line120 開始，讀取 source file
採用 fgets() 讀行的方式再切割成三塊
如果碰到 comment line 就跳過
從第一行開始取得 locctr 的起始位置 => 轉成 10 進位
從 line168 開始處理 symbol label 的問題
判斷 symbol label 是否重複，如果有就報錯，沒有就存入
然後查找 optable，決定 locctr 要加多少
可以判斷 special case：RESW、RESB、BYTE、WORD 四種
然後在下一次讀行的時候回到 line152，把 dec 轉成 hex 字串
line158 判斷碰到 END 的情況，用全域變數把 final location 存起來
輸出最後一行到 immediate file 並結束 path1

path2 從 line196 開始
在 line203 先把第一行讀進來，把程式名稱、起始位置、大小都算出來
直接輸出到 object file
然後從 line220 開始，採用 fgets() 讀行的方式再切割成四塊
在 operand 存在的情況下查詢 symtab
有找到就用變數 find_operand 存起來 (line247)
沒有找到就要判斷是否可能為 indexed addressing => 找有沒有 '
有的話就用函式 change_x() 改掉 x-bit
如果也不是 indexed addressing => 判斷為 error flag
如果沒有 operand，則 find_operand 預設為 0
line269 開始考量沒有在 optable 找到 mnemonic operation 的情況
如果遇到 BYTE，就分 C 和 X 兩種模式
C 的 operand 要轉 hex ASCII，X 直接存為 final_object_code 就好
如果遇到 WORD，就把 dec 轉成 hex 之後存入
produce object code 的最後一個步驟在 line286 => 把長度不足 6 的補齊

每次產生 object code 就用 temp_file 串接起來，然後 reset final_object_code
接著每十行就換行一次，同時用變數 loc_start 和 loc_end 紀錄每行的起始和終點
每到下一輪的第一行就計算前一次的結果，用全域變數 hex_loc 儲存
每到第十行的時候就輸出上一次的 hex_loc 到 restore 裡面
碰到 END 就算結束了
最後離開讀檔，把 restore 裡面所有的資料全部轉成 uppercase
然後寫完開頭為 E 的最後一行，結束

line347 行補充 symbol table 的輸出，採用 linked list 的資料結構
並且在輸出的同時進行 free
