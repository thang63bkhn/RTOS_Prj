# RTOS_Prj
Lập trình bộ lập lịch và hệ điều hành RTOS với STM32
  - Môi trường lập trình: Keil C, STM32CubeMX
  - Phần cứng: Kit STM32F407, LCD1602, DS3231
  - Chức năng: Bao gồm 6 task có mức ưu tiên như trong hình. Chương trình thực hiện chức năng đọc giá trị thời gian được khởi tạo trên DS3231 và giá trị thân nhiệt từ cảm biến hồng ngoại MLX90614 để hiển thị trên màn hình LCD và trên màn hình máy tính bằng UART. Các task sử dụng chung tài nguyên thông qua Queue, cơ chế Mutex và Semaphore để đồng bộ chia sẻ tài nguyên giữa các task.
  - Link video demo: https://drive.google.com/file/d/19gnGz3bnG-SL6ABLGNvu7nGCCOdq2IIk/view?usp=sharing
