# Author: vuaphapthuat410
# Subject: Network Programming (Socket Programming)
# Programming language: C 
# Library use in this program: pthread with some basic library for tcp/ip communication
# Problem:

## Server có các chức năng sau:
- Cho phép người dùng tạo nhóm. Người tạo nhóm có quyền trưởng nhóm
- Mỗi nhóm có một thư mục riêng, chứa các file được chia sẻ trong nhóm đó
- Cho thành viên bất kỳ trong nhóm cũng có thể upload file, tạo thư mục con trong thư mục của nhóm đó
- Chỉ có trưởng nhóm có quyền xóa file, thư mục con

## Client có các chức năng sau:
- Tạo nhóm chia sẻ.
- Xin tham gia một nhóm
- Upload file. 
- Download file.
- Nếu người dùng là trưởng nhóm có thêm các quyền đã mô tả như trên

** Note **
Only account is synchronous, room list is not, so please modify/extend source code for more appropriate result.