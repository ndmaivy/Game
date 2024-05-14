# Maria and the mysterious treasure - Game project

    Bài tập môn Lập trình nâng cao - INT2215 22.
    Họ và tên: Nguyễn Diệu Mai Vy.
    Mã sinh viên: 23021755.
## *Hướng dẫn cài đặt game*

1. 
## *Mô tả trò chơi*

### *Giới thiệu*

* "Maria and the mysterious treasure" (gọi tắt là Maria) là một trò chơi lấy ý tưởng từ trò chơi "Giải cứu Mario" cổ điển.
* "Maria" được xây dựng với nhân vật chính Maria là một nữ sinh đang đi tìm "điều mà sinh viên nào cũng mong muốn nhận được". Người chơi phải điều khiển nhân vật này vượt qua 3 màn của game (có 2 chế độ khó/dễ) để đến được đích cuối cùng là chiếc rương chứa bí mật.


### *Cách chơi*

#### 1. Giới thiệu nhân vật
*  **Nhân vật chính** ![Idle](https://github.com/ndmaivy/Maria/assets/160201509/e9941e8a-d5f4-41de-8090-00649de0898b).
* **Quái 1 - Nấm** ![one](https://github.com/ndmaivy/Maria/assets/160201509/83afc98d-77a2-43dd-90d9-39656f838a32).
* **Quái 2 - Ốc sên** ![one](https://github.com/ndmaivy/Maria/assets/160201509/7522c2b9-6ddc-41a4-94e2-f6af90bc6d7f)
* **Quái 3 - Plant** ![one](https://github.com/ndmaivy/Maria/assets/160201509/b2a6e2c5-1ba6-48d2-b5f8-0f50f471aa46)
* **Quái 4 - Trunk** ![one](https://github.com/ndmaivy/Maria/assets/160201509/8a281888-6686-443f-96a5-c8fa4cfc056a)
#### 2. Các chế độ game
* **Chế độ dễ - Easy:**
  * Nhân vật chính có 3 máu
  * Tốc độ bắn của quái ở chế độ bình thường.
* **Chế độ khó - Hard:**
  * Nhân vật chính chỉ có tối đa 1 máu.
  * Tăng tốc độ bắn của quái.
  * Giảm tỉ lệ rớt vật phẩm từ quái.
  * x3 điểm khi ghi nhận highscore.

#### 3. Luật chơi
* Người chơi điều khiển nhân vật sao cho không bị các quái thú chạm vào mình.
* Để tiêu diệt các quái thú, người chơi có các lựa chọn sau:
   * Nhảy lên đầu quái:
     * Nấm: 1 lần nhảy.
     * Ốc sên: 2 lần nhảy.
     * Plant: 1 lần nhảy.
     * Trunk: 1 lần nhảy.
   * Bắn đạn:
     * Nấm: 1 đạn.
     * Ốc sên: không thể giết bằng đạn.
     * Plant: 2 đạn.
     * Trunk: Không thể giết bằng đạn.
   * Ulti (cost: 5 viên đạn): Quét vùng quái nhất trước mặt nhân vật chính và tiêu diệt chúng.
* Mỗi khi tiêu diệt được quái, các vật phẩm sẽ được random rơi ra (hoặc không):
  * Các đồng xu: 
* Lưu ý: Người chơi chỉ được lưu 1 sức mạnh một lúc trong mình.

#### 3. Các phím thao tác
* Người chơi sử dụng bàn phím máy tính để điều khiển nhân vật.

    | Chức năng  | Phím |
    | ------------- |:-------------:| 
    | Di chuyển sang trái    | A    |
    | Di chuyển sang phải    | D    |
    | Di chuyển lên trên     | W    |
    | Bắn đạn                | E    |
    | Ulti                   | F    |
* Các thao tác trò chơi
    * Tạm dừng trò chơi: sử dụng phím P.
    * Bật/Tắt âm thanh: Nhấn nút Mute/Unmute.
    * Menu: Dùng thao tác chuột để chọn Play, Resume, hay Quit.
    * Chọn chế độ chơi: Dùng thao tác chuột chọn chế độ Easy/Hard.
## *Hình ảnh minh họa*

### Start Menu
![mainscr](https://github.com/ndmaivy/Maria/assets/160201509/8e72a584-d009-41ef-ba26-d9ee899ebf51)
### Run
![image](https://github.com/ndmaivy/Maria/assets/160201509/22ff843e-33c0-4764-aaad-50280bbf7291)
### Win game
![cert](https://github.com/ndmaivy/Maria/assets/160201509/9413f982-9fb4-4708-a3e9-586bcb51c5b9)
### More

## *Các kĩ thuật sử dụng*
### SDL
* Sử dụng các kĩ thuật để render hình ảnh, âm thanh.
* Load hình ảnh, âm thanh đơn giản.
* Sử dụng các kĩ thuật để xoay, cắt hình ảnh để giảm số lượng hình ảnh đầu vào.

### C++
* Sử dụng các kiến thức về: Mảng, Vector, Class, Struct, Random,...
* Sử dụng phân chia file để quản lí từng Game Object.

### Kĩ thuật khác
* Tính toán, căn chỉnh vị trí các đối tượng hiển thị, bo góc rắn.
* Tạo menu cùng các chức năng: Play, Resume, Pause, Continue, Mute, Unmute, High Score, Quit.
* Tối giản hàm main dễ dọc, dễ hiểu.

## *Nguồn tham khảo*
* Nguồn hình ảnh, âm thanh: [itch.io](https://itch.io), tự thiết kế, ...
* Thư viện SDL2: [Lazyfoo](https://lazyfoo.net/tutorials/SDL/index.php).

## *Kết luận*
* Sau quá trình làm game, em rút ra được việc phân chia, quản lí các đối tượng hết sức quan trọng. Việc phân chia tốt sẽ giúp giảm thời gian sửa lỗi và dễ quản lí. Học được thêm về lập trình hướng đối tượng thông qua từng GameObject, ... Trong quá trình làm game, em từng không quản lí tốt bộ nhớ sau khi sử dụng con trỏ, dẫn đến việc chức năng restart bị lỗi, tuy nhiên sau khi dọn dẹp, cũng như quản lí lại bộ nhớ để làm chức năng restart giúp em nhận ra thêm về phần quan trọng trong việc quản lí bộ nhớ khi code.
* Hướng phát triển: Trong quá trình phát triển thêm chức năng Power cho rắn, em đã tạo một class Power để chứa các chức năng này. Điều này giúp các phiên bản sau, không chỉ có chức năng thả bom mà còn có thể dễ dàng thêm các chức năng mới và cải thiện các chức năng cũ. 

## *Lời cuối*
*Cảm ơn mọi người đã đọc và đánh giá project đầu tiên của em/mình.*

#### **Mức điểm tự đánh giá: 9 - 10.**

