# Algorithm: Intersection Manager
## A 4 way Intersection Manager That Minimizes the Average Waiting Time of Each Car at a crossroad, Implementation in C++

## Description
- 使用之程式語言：**< C++ >**
- 使用之編譯器：**< GNU g++ >**
- 各檔案說明：
	* manager.cpp 		: 純文字主程式
	* a.out 			: manager.cpp compile過後的執行檔
	* Readme.md		: This File
	* result.pdf	: result presentation之投影片

- 編譯方式說明：        	
    * 主程式：manager.cpp
	 	- 請在主程式的目錄下，鍵入g++ manager.cpp -o manager -O3 指令，即可完成編譯，
	 	- 在主程式的目錄下會產生一個名為 manager 的執行檔

- 執行、使用方式說明：
    * 主程式：
    * 編譯完成後，在檔案目錄下會產生一個 manager 的執行檔
	   	- 執行檔的命令格式為：
	   	- ./manager <input file name> <output file name>
	   	- 例如：要對 input1.txt 做運算，並輸出成output1.txt
	   	- 則在命令提示下鍵入：
	   	- ./manager input1.txt output1.txt

- 執行結果說明（說明執行結果的觀看方法，及解釋各項數據等）：
   	* 主程式：
		- (1)在執行完後輸出的四行數據，則為經過運算後，所安排的N,E,S,W出車方式及其車輛行駛方向。
		- (2)利用./checker <input file> <output file> 查看Average waiting rounds，
		- ex:
			//(input file=> case10)
			//(運算後的output file =>case10_out.txt)
			./manager case10 case10_out.txt
			./checker case10 case10_out.txt
			=>顯示結果
			 Total car number: 3012
			 Total rounds spent: 1058932
			 Average waiting rounds: 351.571