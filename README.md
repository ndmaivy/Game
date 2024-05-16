# Maria and the mysterious treasure - Game project

    Bài tập môn Lập trình nâng cao - INT2215 22.
    Họ và tên: Nguyễn Diệu Mai Vy.
    Mã sinh viên: 23021755.
## *Hướng dẫn cài đặt game*

1. Tại phần "Code" ![image](https://github.com/ndmaivy/Maria/assets/160201509/2d43932c-dbb3-4963-acbb-7eebaf111175) chọn "Download ZIP".
2. Sau khi hoàn tất tải về, giải nén file zip và mở file "game.cbp".
3. Ấn F9 (hoặc tổ hợp fn + F9) để bắt đầu chơi game.
## *Mô tả trò chơi*

### *Giới thiệu*

* "Maria and the mysterious treasure" (gọi tắt là Maria) là một trò chơi lấy cảm hứng rồi cải tiến đáng kể từ trò chơi Mario cổ điển.
* "Maria" được xây dựng với nhân vật chính Maria là một nữ sinh đang đi tìm "điều mà sinh viên nào cũng mong muốn nhận được". Người chơi phải điều khiển nhân vật này vượt qua 3 màn của game (có 2 chế độ khó/dễ) để đến được đích cuối cùng là chiếc rương chứa bí mật.


### *Cách chơi*

#### 1. Giới thiệu nhân vật
*  **Nhân vật chính** ![Idle](https://github.com/ndmaivy/Maria/assets/160201509/e9941e8a-d5f4-41de-8090-00649de0898b).
* **Quái 1 - Nấm** ![one](https://github.com/ndmaivy/Maria/assets/160201509/83afc98d-77a2-43dd-90d9-39656f838a32).
* **Quái 2 - Ốc sên** ![one](https://github.com/ndmaivy/Maria/assets/160201509/7522c2b9-6ddc-41a4-94e2-f6af90bc6d7f).
* **Quái 3 - Plant** ![one](https://github.com/ndmaivy/Maria/assets/160201509/b2a6e2c5-1ba6-48d2-b5f8-0f50f471aa46).
* **Quái 4 - Trunk** ![one](https://github.com/ndmaivy/Maria/assets/160201509/8a281888-6686-443f-96a5-c8fa4cfc056a).
#### 2. Các chế độ game
* **Chế độ dễ - Easy:**
  * Nhân vật chính có 3 máu.
  * Tốc độ bắn của quái ở chế độ bình thường.
* **Chế độ khó - Hard:**
  * Nhân vật chính chỉ có tối đa 1 máu.
  * Tăng tốc độ bắn của quái.
  * Giảm tỉ lệ rớt vật phẩm từ quái.
  * x3 điểm khi ghi nhận highscore.

#### 3. Luật chơi
* Người chơi điều khiển nhân vật sao cho không bị các quái thú và đạn chạm vào mình cũng như là không bị rơi xuống vực. Bạn sẽ thua nếu nhân vật chính hết máu.
* Để tiêu diệt các quái thú, người chơi có các lựa chọn sau:
   * Nhảy lên đầu quái:
     * Nấm: 1 lần nhảy.
     * Ốc sên: 2 lần nhảy.
     * Plant: 1 lần nhảy.
     * Trunk: 1 lần nhảy.
   * Bắn đạn:
     * Đạn của nhân vật chính ![9](https://github.com/ndmaivy/Maria/assets/160201509/c924c93a-e11b-4580-b767-da4c4f57d858).
     * Đạn của quái: **Plan** ![10](https://github.com/ndmaivy/Maria/assets/160201509/9e77cbdf-e0fe-408d-bd33-c1bdd26bb8bc), **Trunk** ![11](https://github.com/ndmaivy/Maria/assets/160201509/ab4b0f97-9e3f-42ca-9daa-ea19e942e1a3).
     * Đạn mặc định của nhân vật chính chỉ có 1 sát thương và được nhân đôi nếu trong trạng thái buff. Người chơi nên tính toán cẩn thận lượng đạn để tiêu diệt quái dựa theo bảng máu sau:
     * Nấm: 1 HP.
     * Ốc sên: 10 HP.
     * Plant: 2 HP.
     * Trunk: 5 HP.
   * Ulti (cost: 5 viên đạn): Quét tìm vùng nhiều quái nhất trước mặt nhân vật chính và tiêu diệt chúng.
* Mỗi khi tiêu diệt được quái, các vật phẩm sẽ được rơi ra ngẫu nhiên (hoặc không):
  * Các loại đồng xu: Tăng điểm (10-100-1000 tương ứng với đồng xu bạc-vàng-đỏ).
  * Trái tim ![3](https://github.com/ndmaivy/Maria/assets/160201509/7079d285-a00c-45d2-b24e-0be331b51ebd): Hồi máu.
  * Bình thuốc  ![4](https://github.com/ndmaivy/Maria/assets/160201509/a841946d-1870-4a86-a718-8e09c9d73f5b): Tăng sát thương cho đạn (1->2 dmg).
  * Hộp đạn ![2](https://github.com/ndmaivy/Maria/assets/160201509/2713725a-80bf-4d8d-9e03-34a21b7118fc): Hồi 2 đạn.
* Khi đã vượt qua 3 round, nhân vật sẽ đến trước chiếc rương cuối cùng. Sau khi nhặt được vật phẩm ![12](https://github.com/ndmaivy/Maria/assets/160201509/29b2a931-5dd7-4adb-b5aa-b6407f9408c5) bạn chính thức giành chiến thắng ở game này.
#### 3. Các phím thao tác
* Người chơi sử dụng bàn phím máy tính để điều khiển nhân vật.

    | Chức năng  | Phím |
    | ------------- |:-------------:| 
    | Di chuyển sang trái    | A    |
    | Di chuyển sang phải    | D    |
    | Di chuyển lên trên     | W    |
    | Bắn đạn                | Space|
    | Ulti                   | F    |
* Các thao tác trò chơi
    * Tạm dừng và lưu tiến độ trò chơi: sử dụng phím P.
    * Bật/Tắt âm thanh: Nhấn nút Mute/Unmute.
    * Menu: Dùng thao tác chuột để chọn Play, Resume, hay Quit.
    * Chọn chế độ chơi: Dùng thao tác chuột chọn chế độ Easy/Hard.
## *Hình ảnh minh họa*

### Start Menu
![mainscr](https://github.com/ndmaivy/Maria/assets/160201509/8e72a584-d009-41ef-ba26-d9ee899ebf51)
### Run
![image](https://github.com/ndmaivy/Maria/assets/160201509/85d9db4b-0905-41e5-a2f8-b7e9605865b3)
### Win game
![cert](https://github.com/ndmaivy/Maria/assets/160201509/9413f982-9fb4-4708-a3e9-586bcb51c5b9)
### More
* **Spin**: Nhân vật chính sẽ tự quay tại chỗ khi bạn không điều khiển nhân vật trong một thời gian
![image](https://github.com/ndmaivy/Maria/assets/160201509/84c1f341-c554-4ed9-a11a-140c888c2c5a)

* **[Video demo trò chơi](https://youtu.be/yK7rr_moS8Q)**
## *Các kĩ thuật sử dụng*
### SDL
* Tương tác mượt mà với chuột và bàn phím.
* Sử dụng các kĩ thuật để render hình ảnh, âm thanh.
* Dùng nhiều hình ảnh, nhạc và hiệu ứng âm thanh phong phú và phù hợp.
* Sử dụng các kĩ thuật để xoay, hiện hoạt ảnh một cách đơn giản và dễ dàng.
* Kỹ thuật Scrolling View để di chuyển màn hình theo nhân vật, đem lại trải nghiệm tuyệt vời cho người chơi.

### C++
* Sử dụng các kiến thức về: Vòng lặp, Mảng, Vector, Class, Struct, Random, Sort...

### Kĩ thuật khác
* Tính toán, căn chỉnh vị trí các đối tượng hiển thị, bo góc rắn.
* Tạo menu cùng các nút bấm: Play, Resume, Pause, Continue, Mute, Unmute, High Score, Quit.
* Tối giản hàm main dễ dọc, dễ hiểu.
* Tách code thành nhiều hàm để tránh lặp nhiều đoạn code có cùng ý nghĩa.
* Thuật toán segment tree để xác định nhanh 1 vùng có bao nhiêu quái. Sau đó sử dụng thuật toán tham lam để tìm vùng có nhiều địch nhất ngay trước nhân vật chính. 

## *Nguồn tham khảo*
* Nguồn hình ảnh, âm thanh: [itch.io](https://itch.io), tự thiết kế, ...
* Cách sử dụng (có code mẫu) thư viện SDL2: [Lazyfoo](https://lazyfoo.net/tutorials/SDL/index.php).

## *Kết luận*
* Sau quá trình làm game, em rút ra được việc phân chia, quản lí các đối tượng hết sức quan trọng. Việc phân chia tốt sẽ giúp giảm thời gian sửa lỗi và dễ quản lí. Học được thêm về lập trình hướng đối tượng thông qua từng GameObject, ... Trong quá trình làm game, em từng không quản lí tốt bộ nhớ sau khi sử dụng con trỏ, dẫn đến việc một số tính năng bị lỗi, tuy nhiên sau khi dọn dẹp, cũng như quản lí lại bộ nhớ, đồng bộ lại đồ họa thì em đã giải quyết được vấn đề (tuy nhiên có quá nhiều nhân vật trong cùng 1 map nên một số tình huống diễn ra với tốc độ chậm hơn bình thường). Bên cạnh đó, em không tìm được bản mixer tương thích với tần số máy (dù đã dò tần số) nên nếu game chạy trên máy em sẽ không có âm thanh (buộc phải đóng comment dòng code Mixer_Openaudio để game chạy bình thường), tuy nhiên em đã thử chạy project trên các máy khác đều ổn nên đây là lỗi do máy và em chưa khắc phục được.

## *Lời cuối*
*Cảm ơn mọi người đã đọc và đánh giá project đầu tiên của em/mình.*

#### **Mức điểm tự đánh giá: 9 - 10.**

