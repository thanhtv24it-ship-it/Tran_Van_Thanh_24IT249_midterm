# Dự án giữa kỳ – Triển khai ls(1)

**Sinh viên:** [Họ và tên]  
**MSSV:** [StudentID]  
**Kho GitHub:** `https://github.com/[username]/Name_studentID_midterm`  
*(Thay thế bằng đường dẫn thực tế sau khi đẩy lên GitHub)*

## 1. Mô tả dự án

Chương trình là một phiên bản đơn giản hóa của lệnh `ls(1)` trên hệ thống UNIX/NetBSD, được phát triển từ đầu dựa trên trang hướng dẫn (man page) được cung cấp. Chương trình hỗ trợ hầu hết các tùy chọn được liệt kê trong tài liệu và tuân thủ hành vi cơ bản của `ls`.

## 2. Cấu trúc dự án

```
ls_project/
├── Makefile
├── README.md               # Báo cáo này
├── .gitignore
├── include/
│   ├── options.h           # Định nghĩa cấu trúc options và hàm parse
│   ├── util.h              # Các hàm tiện ích (path, mode, humanize, ...)
│   ├── print.h             # Hàm in kết quả
│   └── ls.h                # Logic liệt kê thư mục / sắp xếp
└── src/
    ├── main.c              # Điểm vào chương trình, xử lý đối số
    ├── options.c           # Phân tích tùy chọn dòng lệnh (getopt)
    ├── util.c              # Triển khai các hàm tiện ích
    ├── print.c             # Định dạng và in entry (short / long)
    └── ls.c                # Thu thập entry, sắp xếp, đệ quy (-R)
```

Mã nguồn được tổ chức thành nhiều module rõ ràng, mỗi module có file tiêu đề tương ứng.

## 3. Các tính năng đã triển khai

| Tùy chọn | Mô tả | Trạng thái |
|----------|-------|------------|
| (mặc định) | Liệt kê nội dung thư mục hiện tại, sắp xếp theo tên | ✅ |
| `-a` | Hiển thị tất cả (bao gồm file ẩn `.` và `..`) | ✅ |
| `-A` | Hiển thị tất cả trừ `.` và `..` | ✅ |
| `-l` | Định dạng dài (mode, links, owner, group, size, time, name) | ✅ |
| `-n` | Giống `-l` nhưng hiển thị UID/GID dạng số | ✅ |
| `-i` | In số inode | ✅ |
| `-s` | Hiển thị số block (512-byte mặc định) | ✅ |
| `-h` | Kích thước dạng dễ đọc (K, M, G...) | ✅ |
| `-k` | Block tính theo KB | ✅ |
| `-F` | Thêm ký tự chỉ thị (`/`, `*`, `@`, `\|`, `=`) | ✅ |
| `-d` | Xử lý thư mục như file thường | ✅ |
| `-R` | Liệt kê đệ quy | ✅ |
| `-r` | Đảo ngược thứ tự sắp xếp | ✅ |
| `-t` | Sắp xếp theo thời gian (mtime mặc định) | ✅ |
| `-S` | Sắp xếp theo kích thước (lớn trước) | ✅ |
| `-c` | Dùng ctime thay vì mtime | ✅ |
| `-u` | Dùng atime thay vì mtime | ✅ |
| `-f` | Không sắp xếp (và bật `-a`) | ✅ |
| `-q` | Thay ký tự không in được bằng `?` | ✅ |
| `-w` | In thô ký tự không in được | ✅ |

### Chi tiết định dạng dài (`-l`)

- Mode string đầy đủ (loại file + quyền + setuid/setgid/sticky).
- Số liên kết, tên owner/group (hoặc số với `-n`).
- Kích thước (hoặc major,minor cho device).
- Thời gian theo kiểu `ls` cổ điển (tháng ngày giờ:phút hoặc năm).
- Hiển thị `-> target` cho symbolic link.
- Dòng `total N` trước nội dung thư mục.

### Xử lý ngoại lệ

- Báo lỗi rõ ràng khi không truy cập được file/thư mục (không crash).
- Bỏ qua entry lỗi và tiếp tục.
- Kiểm tra bộ nhớ khi `realloc`.
- Không segfault với đường dẫn dài hoặc tên file chứa.

## 4. Cách biên dịch và chạy

### Biên dịch

```bash
make          # tạo binary 'ls'
make clean    # xóa file object và binary
make test     # biên dịch + chạy một số kiểm thử nhanh
```

### Chạy thử

```bash
./ls                  # liệt kê thư mục hiện tại
./ls -l               # định dạng dài
./ls -la              # tất cả file + dài
./ls -lis             # inode + block + dài
./ls -lh              # kích thước dễ đọc
./ls -F               # chỉ thị loại file
./ls -R src           # đệ quy
./ls -d . include     # không vào thư mục
./ls -lt              # sắp xếp theo thời gian
./ls -lS              # sắp xếp theo kích thước
./ls -r               # đảo ngược
./ls Makefile src/    # nhiều đối số
```

### Biến môi trường hỗ trợ

- `BLOCKSIZE` : ảnh hưởng đến đơn vị block khi dùng `-s` (nếu không có `-h`/`-k`).
- `TZ` : ảnh hưởng đến việc hiển thị thời gian (thông qua `localtime`).

## 5. Hướng dẫn đẩy lên GitHub

1. Tạo repository mới trên GitHub với tên: `Name_studentID_midterm`
2. Trong thư mục dự án:

```bash
git init
git add .
git commit -m "Initial commit: simplified ls(1) implementation"
git branch -M main
git remote add origin https://github.com/<username>/Name_studentID_midterm.git
git push -u origin main
```

3. Đảm bảo `.gitignore` đã loại trừ binary và file `.o`.
4. Dán URL repository vào phần đầu báo cáo này và nộp qua hệ thống học trực tuyến.

## 6. Ghi chú kỹ thuật

- Sử dụng `lstat` để không follow symlink (phù hợp với hành vi `ls`).
- Sắp xếp bằng `qsort` với comparator phụ thuộc tùy chọn.
- Đệ quy `-R` được thực hiện sau khi in nội dung thư mục hiện tại.
- Khi có nhiều đối số, file không phải thư mục được in trước, sau đó đến thư mục (đúng theo man page).
- Chương trình được viết theo chuẩn C11, biên dịch sạch với `-Wall -Wextra -pedantic`.

## 7. Kết luận

Dự án đã hoàn thành các yêu cầu chính: module hóa, hỗ trợ đầy đủ các tùy chọn trong tài liệu tham khảo, xử lý lỗi ổn định, có Makefile và báo cáo. Có thể mở rộng thêm màu sắc, cột căn chỉnh đẹp hơn, hoặc hỗ trợ đầy đủ BLOCKSIZE phức tạp hơn trong tương lai.
