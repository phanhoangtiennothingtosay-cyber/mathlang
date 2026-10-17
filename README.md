MathLang — Ngôn ngữ lập trình toán học

Tài liệu cú pháp và hướng dẫn sử dụng từ A đến Z

Dự án: MathLang
Ngôn ngữ triển khai: C++20
Thư viện số học: Boost.Multiprecision
Phần mở rộng mã nguồn: ".mth"
Mục tiêu: Xây dựng một ngôn ngữ lập trình lấy toán học làm trung tâm, có cú pháp dễ đọc, dễ nhập trên điện thoại và có ngữ nghĩa được định nghĩa rõ ràng.

---

Phần I — Triết lý thiết kế

1. MathLang là gì?

MathLang là ngôn ngữ lập trình được thiết kế để biểu diễn và tính toán các biểu thức toán học bằng cú pháp ngắn gọn, dễ đọc.

Thay vì buộc người dùng phải nhập các ký hiệu Unicode phức tạp, MathLang ưu tiên những tên tiếng Anh thông dụng khi một ký hiệu khó nhập hoặc khó đọc.

Ví dụ:

Pi()
SinDeg(30)
Sqrt(25)
Log(100, 10)

Những biểu thức này thể hiện rõ ý nghĩa ngay cả khi người dùng không có bàn phím toán học chuyên dụng.

Tuy nhiên, MathLang không loại bỏ mọi ký hiệu. Những phép toán có ký hiệu phổ biến và dễ nhập vẫn giữ nguyên.

2 + 3
8 - 5
4 * 6
20 / 4
17 % 5
2 ^ 10
5!

2. Những nguyên tắc cốt lõi

1. Dễ nhập: Hạn chế phụ thuộc vào các ký hiệu Unicode khó tìm trên bàn phím.
2. Dễ đọc: Tên hàm mô tả trực tiếp phép toán.
3. Nhất quán: Một quy tắc cú pháp không nên thay đổi tùy theo hàm.
4. Có kiểm tra: Lỗi cú pháp và lỗi toán học cần được phát hiện rõ ràng.
5. Độ chính xác cao: Sử dụng kiểu số thập phân độ chính xác cao khi có thể.
6. Tương thích điện thoại: Mã nguồn phải dễ viết trong Termux, trình soạn thảo văn bản và bàn phím ảo.
7. Có khả năng mở rộng: Cú pháp biểu thức phải có thể phát triển thành ngôn ngữ lập trình hoàn chỉnh.

3. Quy ước trạng thái tính năng

Tài liệu này sử dụng ba nhãn:

- Đã có trong mã nguồn: Có cấu trúc hoặc xử lý tương ứng trong phiên bản mã nguồn được kiểm tra.
- Cần xác nhận khi build: Có mã xử lý nhưng chưa được kiểm thử trên thiết bị hoặc chưa xác nhận tích hợp đầy đủ.
- Đặc tả đề xuất: Quy tắc thiết kế hướng tới phiên bản hoàn chỉnh; không đồng nghĩa tính năng đã được triển khai.

Không được coi một ví dụ trong tài liệu là bằng chứng rằng chương trình hiện tại chắc chắn thực thi được ví dụ đó.

---

Phần II — Cấu trúc cơ bản

4. Tệp MathLang

Tệp nguồn sử dụng phần mở rộng:

example.mth

Ví dụ tên tệp:

calculator.mth
geometry.mth
statistics.mth
physics.mth

Một tệp nguồn có thể chứa một biểu thức toán học hoặc, theo đặc tả chương trình nhiều câu lệnh, nhiều câu lệnh được phân tách bằng dấu chấm phẩy.

Ví dụ biểu thức đơn:

2 + 3 * 4

Ví dụ chương trình nhiều câu lệnh theo đặc tả đề xuất:

radius = 5;
area = Pi() * radius ^ 2;
Print(area)

Lưu ý: Bộ phân tích cú pháp trong mã nguồn có cấu trúc hỗ trợ câu lệnh gán, "Print(...)" và dấu chấm phẩy. Tuy nhiên, bộ thực thi hiện tại cần được tích hợp đầy đủ để chạy cả chương trình nhiều câu lệnh. Ví dụ này là hình thức cú pháp mục tiêu, không phải cam kết rằng mọi bản build hiện tại đã hỗ trợ toàn bộ chương trình.

5. Khoảng trắng

Có thể sử dụng dấu cách và xuống dòng để làm mã nguồn dễ đọc.

2+3*4

Tương đương về ý nghĩa toán học với:

2 + 3 * 4

Không nên xóa khoảng trắng một cách máy móc bên trong những cấu trúc mà ngôn ngữ quy định có ý nghĩa riêng.

6. Phân biệt chữ hoa và chữ thường

Quy tắc đề xuất: Tên hàm dựng sẵn không phân biệt chữ hoa và chữ thường.

Ví dụ:

Pi()
PI()
pi()

Cả ba cách viết đều chỉ cùng một hằng số.

Tương tự:

Sqrt(25)
SQRT(25)
sqrt(25)

Các tên biến nên được chuẩn hóa theo một quy tắc riêng trước khi đưa vào phiên bản chính thức. Không nên mặc định rằng biến "Radius" và "radius" là cùng một biến nếu chưa có quy định rõ.

---

Phần III — Số và hằng số

7. Số nguyên

Số nguyên được viết bằng các chữ số thập phân.

0
1
42
1000
-25

Dấu âm được xử lý như toán tử một ngôi, không phải một phần độc lập của số nguyên trong cấu trúc cú pháp.

8. Số thập phân

Sử dụng dấu chấm làm dấu phân cách phần nguyên và phần thập phân.

3.14
0.5
12.75
.25

Không sử dụng dấu phẩy để viết số thập phân. Dấu phẩy được dành cho việc phân tách đối số trong lời gọi hàm.

Ví dụ:

Log(100, 10)

Ở đây, "100" và "10" là hai đối số riêng biệt.

9. Ký pháp khoa học

Ký pháp khoa học cho phép viết số rất lớn hoặc rất nhỏ.

1e3
2.5e4
6.02e23
1e-6

Ý nghĩa:

- "1e3" bằng (1 \times 10^3).
- "2.5e4" bằng (2.5 \times 10^4).
- "1e-6" bằng (1 \times 10^{-6}).

Bộ lexer hiện tại có nhận diện ký pháp khoa học khi phần số mũ có chữ số hợp lệ.

10. Hằng số toán học

Các hằng số được biểu diễn bằng lời gọi hàm không đối số để tránh nhầm lẫn với biến thông thường.

10.1. Pi

Pi()

[
\pi \approx 3.141592653589793238462643383279502884197169399375105820974944
]

Mã nguồn hiện có khai báo giá trị Pi với 60 chữ số sau dấu thập phân. Kiểu số sử dụng có độ chính xác danh nghĩa cao hơn, nhưng điều đó không tự tạo thêm chữ số đúng cho một hằng số chỉ được cung cấp 60 chữ số.

10.2. Tau

Tau()

[
\tau = 2\pi
]

10.3. Euler's number

E()

[
e \approx 2.71828182845904523536
]

10.4. Golden ratio

GoldenRatio()

[
\varphi = \frac{1+\sqrt{5}}{2}
]

Tên "GoldenRatio()" mô tả rõ ý nghĩa hơn việc bắt người dùng tìm ký tự Hy Lạp phi trên bàn phím.

Quy tắc thiết kế: Không nên dùng một tên hằng số trần như "e" nếu nó có thể gây mơ hồ với ký pháp khoa học. Lời gọi "E()" là cách viết rõ ràng hơn trong tài liệu cú pháp chính thức.

---

Phần IV — Toán tử

11. Cộng

2 + 3

Kết quả toán học:

[
2+3=5
]

12. Trừ

10 - 4

Kết quả:

[
10-4=6
]

13. Nhân

4 * 5

Kết quả:

[
4 \times 5 = 20
]

Dùng dấu "*" thay cho ký hiệu nhân Unicode để dễ nhập trên bàn phím thông thường.

14. Chia

20 / 4

Kết quả:

[
20/4=5
]

Phép chia cho số không phải trả về lỗi, không được âm thầm tạo ra một kết quả số thông thường.

1 / 0

Kết quả mong muốn: lỗi chia cho số không.

15. Chia lấy dư

17 % 5

Kết quả:

[
17 \bmod 5 = 2
]

Theo đặc tả, "%" là toán tử modulo. Quy tắc dấu của kết quả với số âm phải được định nghĩa rõ và kiểm thử nhất quán.

16. Lũy thừa

2 ^ 10

Kết quả:

[
2^{10}=1024
]

Có thể dùng số mũ âm:

2 ^ -3

Kết quả:

[
2^{-3}=\frac{1}{8}
]

Quy tắc đề xuất là lũy thừa kết hợp từ phải sang trái.

2 ^ 3 ^ 2

Được hiểu là:

[
2^{(3^2)}=512
]

Không phải:

[
(2^3)^2=64
]

17. Giai thừa

5!

Kết quả:

[
5!=120
]

Giai thừa thông thường được định nghĩa cho số nguyên không âm.

0!

Kết quả:

[
0!=1
]

Một đầu vào âm hoặc không nguyên phải tạo ra lỗi miền toán học.

Ngoài toán tử "!", có thể dùng hàm tên:

Fact(5)
Factorial(5)

18. Dấu ngoặc

Dùng ngoặc tròn để kiểm soát thứ tự tính toán.

(2 + 3) * 4

Kết quả:

[
(2+3)\times4=20
]

So sánh với:

2 + 3 * 4

Kết quả:

[
2+(3\times4)=14
]

---

Phần V — Thứ tự ưu tiên

19. Bảng thứ tự ưu tiên đề xuất

Từ cao xuống thấp:

Mức| Cấu trúc| Ví dụ
1| Ngoặc và lời gọi hàm| "Sqrt(25)"
2| Giai thừa| "5!"
3| Lũy thừa| "2 ^ 3"
4| Dấu một ngôi| "-5"
5| Nhân, chia, modulo| "4 * 5", "7 % 2"
6| Cộng, trừ| "2 + 3"
7| So sánh| "3 < 5"

Việc xác định chính xác quan hệ giữa dấu một ngôi và lũy thừa là bắt buộc. Ví dụ "-2 ^ 2" có thể được hiểu là "-(2 ^ 2)" hoặc "(-2) ^ 2" tùy quy tắc ngôn ngữ. MathLang cần chọn một cách hiểu duy nhất và có bài kiểm thử cho trường hợp này.

20. Ví dụ kết hợp

2 + 3 * 4 ^ 2

Theo thứ tự ưu tiên thông thường:

1. (4^2=16)
2. (3\times16=48)
3. (2+48=50)

Kết quả là "50".

---

Phần VI — Hàm toán học

21. Quy tắc gọi hàm

Cấu trúc tổng quát:

FunctionName(argument1, argument2)

Các quy tắc:

- Tên hàm phải hợp lệ.
- Đối số được đặt trong ngoặc tròn.
- Dấu phẩy phân cách các đối số.
- Số lượng đối số phải đúng với định nghĩa hàm.
- Hàm không nhận đối số vẫn cần ngoặc tròn theo quy ước của MathLang.

Ví dụ:

Pi()
Sqrt(25)
Pow(2, 10)
Clamp(12, 0, 10)

22. Hàm trị tuyệt đối

Abs(-12)

[
|-12|=12
]

23. Căn bậc hai

Sqrt(81)

Kết quả: "9".

Đầu vào âm không thuộc miền số thực của căn bậc hai thông thường.

24. Căn bậc ba

Cbrt(27)

Kết quả: "3".

Ví dụ với số âm:

Cbrt(-8)

Kết quả toán học mong muốn: "-2".

25. Căn bậc bất kỳ

Cú pháp đề xuất:

Root(27, 3)

Ý nghĩa: căn bậc ba của 27.

Root(16, 4)

Ý nghĩa: căn bậc bốn của 16.

Với căn bậc chẵn của số âm, MathLang cần báo lỗi trong chế độ số thực.

26. Hàm mũ tự nhiên

Exp(1)

[
\operatorname{Exp}(x)=e^x
]

Do đó:

[
\operatorname{Exp}(1)=e
]

27. Logarithm tự nhiên

Ln(E())

Kết quả toán học: "1".

Hàm "Ln(x)" là logarithm cơ số (e).

28. Logarithm cơ số tùy chọn

Log(100, 10)

Kết quả: "2".

Quy ước:

Log(value, base)

Với logarithm cơ số 10, có thể dùng:

Log10(1000)

Kết quả: "3".

Với logarithm cơ số 2:

Log2(8)

Kết quả: "3".

29. Lượng giác theo radian

MathLang phân biệt rõ góc radian và góc độ.

Sin(Pi() / 2)

Kết quả toán học: "1".

Cos(0)

Kết quả: "1".

Các hàm:

- "Sin(x)"
- "Cos(x)"
- "Tan(x)"
- "Asin(x)"
- "Acos(x)"
- "Atan(x)"

Theo quy ước, các hàm lượng giác thuận nhận góc theo radian và các hàm nghịch trả về góc theo radian.

30. Lượng giác theo độ

Để tránh nhầm lẫn với radian, MathLang sử dụng hậu tố "Deg".

SinDeg(30)
CosDeg(60)
TanDeg(45)

Kết quả mong muốn:

- "SinDeg(30)" bằng "0.5".
- "CosDeg(60)" bằng "0.5".
- "TanDeg(45)" bằng "1".

Các hàm nghịch:

AsinDeg(0.5)
AcosDeg(0.5)
AtanDeg(1)

Chúng trả về góc theo độ.

Tên viết tắt như "SinD(x)" có thể được hỗ trợ làm alias, nhưng tài liệu chính thức nên ưu tiên "SinDeg(x)" vì dễ hiểu hơn.

31. Hàm hyperbolic

Các hàm:

Sinh(x)
Cosh(x)
Tanh(x)
Asinh(x)
Acosh(x)
Atanh(x)

Chúng khác với hàm lượng giác thông thường và không nên bị nhầm lẫn với "Sin", "Cos", "Tan".

32. Làm tròn

Floor(3.8)
Ceil(3.2)
Round(3.6)
Trunc(3.9)

Ý nghĩa:

- "Floor(x)": làm tròn xuống số nguyên gần nhất.
- "Ceil(x)": làm tròn lên số nguyên gần nhất.
- "Round(x)": làm tròn theo quy tắc được định nghĩa bởi thư viện số học.
- "Trunc(x)": bỏ phần thập phân, hướng về số không.

Cần kiểm thử cả số âm để tránh hiểu nhầm "Floor(-2.3)" với "Trunc(-2.3)".

33. Dấu của số

Sign(-8)
Sign(0)
Sign(8)

Kết quả lần lượt là "-1", "0", "1".

34. Giá trị nhỏ nhất và lớn nhất

Min(3, 9)
Max(3, 9)

Kết quả lần lượt là "3" và "9".

35. Hàm lũy thừa

Pow(2, 10)

Kết quả: "1024".

"Pow(a, b)" và toán tử "a ^ b" nên có ngữ nghĩa toán học thống nhất.

36. Độ dài cạnh huyền

Hypot(3, 4)

Kết quả: "5".

Hàm này tính:

[
\sqrt{x^2+y^2}
]

37. Hàm hai đối số Atan2

Atan2(y, x)

Hàm trả về góc xác định hướng của điểm ((x,y)), theo radian. Thứ tự đối số cần được giữ nhất quán giữa tài liệu, semantic và runtime.

38. Giới hạn giá trị

Clamp(12, 0, 10)

Kết quả: "10".

Cấu trúc:

Clamp(value, lower, upper)

Giá trị kết quả không nhỏ hơn "lower" và không lớn hơn "upper". Nếu "lower > upper", chương trình cần báo lỗi.

39. Chuyển đổi góc

Deg(Pi())
Rad(180)

Quy ước:

- "Deg(x)": chuyển radian sang độ.
- "Rad(x)": chuyển độ sang radian.

---

Phần VII — So sánh

40. Toán tử so sánh

Toán tử| Ý nghĩa
"=="| Bằng
"!="| Không bằng
"<"| Nhỏ hơn
">"| Lớn hơn
"<="| Nhỏ hơn hoặc bằng
">="| Lớn hơn hoặc bằng

Ví dụ:

3 < 5
10 == 10
7 != 2
4 >= 4

Kết quả của một phép so sánh là giá trị Boolean, không phải một số tùy ý.

Các biểu thức so sánh trong runtime cần được thống nhất với semantic analyzer về kiểu kết quả.

---

Phần VIII — Biến và chương trình nhiều câu lệnh

41. Khai báo và gán biến

Đặc tả cú pháp đề xuất:

radius = 5

Một biến lưu giá trị để có thể dùng lại trong các biểu thức sau.

radius = 5;
area = Pi() * radius ^ 2;
Print(area)

Trong ví dụ này:

1. "radius" nhận giá trị 5.
2. "area" được tính từ bán kính.
3. "Print(area)" hiển thị kết quả.

Đây là cú pháp mục tiêu. Cần tích hợp runtime có môi trường biến và trình thực thi chương trình để thực hiện trọn vẹn.

42. Quy tắc biến

Đặc tả nên quy định:

- Tên biến bắt đầu bằng chữ cái hoặc dấu gạch dưới.
- Những ký tự tiếp theo có thể gồm chữ cái, chữ số và dấu gạch dưới.
- Không cho phép tên biến trùng với từ khóa dành riêng nếu điều đó gây mơ hồ.
- Việc dùng biến chưa được gán phải sinh lỗi dễ hiểu.
- Quy tắc phân biệt chữ hoa/chữ thường phải được thống nhất.
- Việc gán giá trị cần có ngữ nghĩa rõ ràng.

Ví dụ tên biến:

radius
circle_area
value2
_total

43. In kết quả

Cú pháp mục tiêu:

Print(2 + 3)

Kết quả:

5

Ví dụ nhiều câu lệnh:

x = 10;
y = 20;
Print(x + y)

Kết quả mong muốn:

30

Trạng thái: Bộ parser hiện có cấu trúc cho câu lệnh "Print(...)", nhưng cần xác nhận và hoàn thiện runtime chương trình trước khi xem đây là tính năng sử dụng ổn định.

44. Dấu chấm phẩy

Theo đặc tả chương trình nhiều câu lệnh, dấu ";" dùng để phân tách các câu lệnh.

a = 2;
b = 3;
Print(a + b)

Quy tắc xuống dòng và việc có bắt buộc dấu ";" ở cuối chương trình hay không cần được quy định thống nhất.

---

Phần IX — Quy tắc cú pháp hình thức

45. Ngữ pháp biểu thức giản lược

Đây là bản ngữ pháp tham khảo cho biểu thức toán học:

expression     := comparison

comparison     := additive
                  (comparison_operator additive)*

additive       := multiplicative
                  (("+" | "-") multiplicative)*

multiplicative := unary
                  (("*" | "/" | "%") unary)*

unary          := ("+" | "-") unary
                  | power

power          := postfix
                  ("^" unary)?

postfix        := primary ("!")*

primary        := number
                  | identifier
                  | function_call
                  | "(" expression ")"

function_call  := identifier
                  "(" [arguments] ")"

arguments      := expression
                  ("," expression)*

Đây là bản rút gọn. Quy tắc nhân ngầm, dấu so sánh và thứ tự ưu tiên cuối cùng cần được giữ đồng bộ với parser thực tế.

46. Nhân ngầm

Nhân ngầm là cách viết phép nhân mà không có dấu "*".

Ví dụ toán học quen thuộc:

[
2x
]

Tuy nhiên, việc hỗ trợ nhân ngầm dễ gây mơ hồ giữa biến và lời gọi hàm.

Ví dụ:

2x
2 * x
2 * Sin(x)

Quy tắc đề xuất: Cho phép nhân ngầm trong những trường hợp parser có thể phân biệt chắc chắn; ưu tiên "*" khi biểu thức có thể gây mơ hồ.

---

Phần X — Lỗi và xử lý lỗi

47. Lỗi cú pháp

Ví dụ:

2 + * 3

Đây là biểu thức không hợp lệ. Parser cần chỉ ra vị trí lỗi và lý do dễ hiểu.

48. Lỗi số học

Ví dụ:

1 / 0
Sqrt(-1)
Log(0)

Các biểu thức này nằm ngoài miền hợp lệ của những phép toán tương ứng trong chế độ số thực.

MathLang cần báo lỗi cụ thể thay vì trả về một giá trị giả như "0".

49. Lỗi hàm

Ví dụ:

Sqrt(1, 2)

Hàm "Sqrt" chỉ nhận một đối số. Semantic analyzer nên phát hiện lỗi số lượng đối số trước khi runtime tính toán.

50. Lỗi tên hàm

Ví dụ:

UnknownFunction(5)

Nếu tên hàm không nằm trong danh sách hàm hợp lệ và không phải hàm do người dùng định nghĩa, chương trình cần báo lỗi tên hàm không tồn tại.

51. Thông điệp lỗi tốt

Một thông báo lỗi tốt nên có:

- Loại lỗi.
- Vị trí trong tệp nguồn.
- Biểu thức hoặc tên hàm liên quan.
- Nguyên nhân.
- Gợi ý sửa nếu có thể.

Ví dụ định dạng mục tiêu:

Semantic Error at position 8:
Function 'sqrt' expects 1 argument, but received 2.

Định dạng này là đề xuất cho hệ thống thông báo lỗi hoàn chỉnh.

---

Phần XI — Độ chính xác số học

52. Kiểu số

Mã nguồn hiện sử dụng:

boost::multiprecision::cpp_dec_float_100

Đây là kiểu số dấu phẩy động thập phân độ chính xác cao, với độ chính xác danh nghĩa khoảng 100 chữ số thập phân.

Nó không giống "double" của C++ vốn thường chỉ cung cấp khoảng 15–17 chữ số có nghĩa.

53. Số chính xác cao không có nghĩa mọi kết quả đều chính xác tuyệt đối

Ví dụ:

Pi()
1 / 3
Sqrt(2)

Các phép tính được thực hiện bằng số học độ chính xác cao, nhưng những giá trị như (\pi), (1/3) và (\sqrt{2}) có biểu diễn thập phân vô hạn.

Việc hiển thị 60 chữ số sau dấu thập phân không bảo đảm 60 chữ số đó đều đúng trong mọi phép tính. Độ chính xác còn phụ thuộc vào đầu vào, thuật toán, sai số làm tròn và cách chuyển đổi kết quả.

54. Hiển thị kết quả

Đặc tả nên cho phép:

- Hiển thị mặc định gọn gàng.
- Chọn số chữ số hiển thị.
- Chọn ký pháp khoa học.
- Giữ độ chính xác nội bộ cao hơn số chữ số hiển thị.
- Không làm tròn dữ liệu nội bộ chỉ vì người dùng muốn xem ít chữ số hơn.

---

Phần XII — Chạy MathLang

55. Build bằng CMake trên Termux

Môi trường Android sử dụng Termux có thể build dự án C++ bằng Clang và CMake.

Các lệnh tổng quát:

pkg update
pkg install clang cmake make boost

Đi đến thư mục dự án:

cd ~/mathlang

Cấu hình:

cmake -S . -B build

Build:

cmake --build build -j2

Nếu build thất bại, không nên tiếp tục chạy chương trình cũ và cho rằng phiên bản mới đã được cập nhật.

56. Chạy tệp nguồn

Cách chạy theo giao diện dòng lệnh hiện được thiết kế là:

./build/mathlang example.mth

Tệp ".mth" phải chứa biểu thức mà phiên bản runtime hiện tại hỗ trợ.

Ví dụ kiểm thử biểu thức đơn:

printf '2 + 3 * 4' > test.mth
./build/mathlang test.mth

Kết quả mong đợi:

14

Ví dụ kiểm thử Pi:

printf 'Pi()' > test.mth
./build/mathlang test.mth

Chỉ nên đánh dấu những ví dụ đã thực sự chạy qua build và test là “đã xác minh”.

---

Phần XIII — Chiến lược kiểm thử

57. Kiểm thử phép toán cơ bản

Đầu vào| Kết quả mong đợi
"2 + 3"| "5"
"10 - 4"| "6"
"4 * 5"| "20"
"20 / 4"| "5"
"17 % 5"| "2"
"2 ^ 10"| "1024"
"5!"| "120"

58. Kiểm thử hàm

Đầu vào| Kết quả mong đợi
"Pi()"| Xấp xỉ (\pi)
"Sqrt(81)"| "9"
"Cbrt(-8)"| "-2"
"Log10(1000)"| "3"
"SinDeg(30)"| "0.5"
"CosDeg(60)"| "0.5"
"Pow(2, 10)"| "1024"
"Root(27, 3)"| "3"

59. Kiểm thử lỗi

Các trường hợp cần có trong bộ test:

1 / 0
Sqrt(-1)
Log(0)
Sqrt(1, 2)
UnknownFunction(1)

Mục tiêu là xác nhận mỗi trường hợp tạo ra lỗi đúng loại, không làm chương trình crash và không trả về kết quả số sai.

60. Kiểm thử tích hợp

Ngoài kiểm thử riêng từng hàm, cần kiểm tra:

1. Lexer tạo đúng token.
2. Parser tạo đúng cây cú pháp.
3. Semantic analyzer chấp nhận và từ chối đúng biểu thức.
4. Runtime tính đúng kết quả.
5. Chương trình chính đọc đúng tệp.
6. Thông báo lỗi không bị mất.
7. Build sạch hoạt động sau khi xóa thư mục "build".
8. Các bài test không phụ thuộc vào thứ tự chạy.

---

Phần XIV — Những gì MathLang chưa nên tuyên bố hỗ trợ

Một ngôn ngữ lập trình hoàn chỉnh cần nhiều hơn một bộ máy tính biểu thức.

Những tính năng sau cần được triển khai và kiểm thử riêng trước khi công bố là ổn định:

- Môi trường biến và phạm vi biến.
- Chạy nhiều câu lệnh trong một chương trình.
- Hàm do người dùng định nghĩa.
- Câu lệnh điều kiện.
- Vòng lặp.
- Kiểu dữ liệu và chuyển đổi kiểu.
- Mảng và ma trận.
- Xử lý lỗi thống nhất.
- Thư viện chuẩn.
- Giao diện tương tác.
- Kiểm thử tự động và tích hợp liên tục.

Việc có cấu trúc AST hoặc parser cho một câu lệnh không tự động chứng minh rằng runtime đã thực thi được câu lệnh đó.

---

Phần XV — Lộ trình phát triển

61. Giai đoạn 1: Math Core

- Số học độ chính xác cao.
- Hằng số toán học.
- Hàm toán học chuẩn.
- Thứ tự ưu tiên rõ ràng.
- So sánh và giai thừa.
- Lỗi toán học chính xác.
- Bộ test biểu thức.

62. Giai đoạn 2: Program Runtime

- Biến.
- Gán giá trị.
- In kết quả.
- Chạy nhiều câu lệnh.
- Quản lý trạng thái chương trình.
- Thông báo lỗi có vị trí nguồn.

63. Giai đoạn 3: Ngôn ngữ lập trình

- Điều kiện.
- Vòng lặp.
- Hàm do người dùng định nghĩa.
- Phạm vi biến.
- Quy tắc kiểu dữ liệu.

64. Giai đoạn 4: Toán học mở rộng

- Vector.
- Ma trận.
- Thống kê.
- Đại số tuyến tính.
- Tính toán số.
- Tích phân và đạo hàm số.
- Công cụ kiểm tra biểu thức.

65. Giai đoạn 5: Công cụ phát triển

- REPL tương tác.
- Định dạng mã nguồn.
- Thông báo lỗi nâng cao.
- Debugger.
- Tài liệu tham khảo tự động.
- Bộ kiểm thử chuẩn cho mỗi bản phát hành.

---

Phụ lục A — Bảng cú pháp nhanh

Nhu cầu| Cú pháp
Cộng| "a + b"
Trừ| "a - b"
Nhân| "a * b"
Chia| "a / b"
Chia lấy dư| "a % b"
Lũy thừa| "a ^ b"
Giai thừa| "n!"
Pi| "Pi()"
Euler| "E()"
Tau| "Tau()"
Tỉ lệ vàng| "GoldenRatio()"
Trị tuyệt đối| "Abs(x)"
Căn bậc hai| "Sqrt(x)"
Căn bậc ba| "Cbrt(x)"
Căn bậc bất kỳ| "Root(x, n)"
Log tự nhiên| "Ln(x)"
Log cơ số 10| "Log10(x)"
Log cơ số 2| "Log2(x)"
Sin radian| "Sin(x)"
Sin độ| "SinDeg(x)"
Cos độ| "CosDeg(x)"
Tan độ| "TanDeg(x)"
Lũy thừa dạng hàm| "Pow(a, b)"
Giá trị nhỏ nhất| "Min(a, b)"
Giá trị lớn nhất| "Max(a, b)"
Giới hạn| "Clamp(x, low, high)"
Làm tròn xuống| "Floor(x)"
Làm tròn lên| "Ceil(x)"
Chuyển radian sang độ| "Deg(x)"
Chuyển độ sang radian| "Rad(x)"

Phụ lục B — Ví dụ tổng hợp

Ví dụ tính diện tích hình tròn:

Pi() * 5 ^ 2

Ví dụ tính cạnh huyền:

Sqrt(3 ^ 2 + 4 ^ 2)

Ví dụ tính sin góc 30 độ:

SinDeg(30)

Ví dụ tính logarithm:

Log(1000, 10)

Ví dụ biểu thức kết hợp:

(2 + 3) ^ 2 / 5

Ví dụ chương trình nhiều câu lệnh theo đặc tả mục tiêu:

radius = 5;
area = Pi() * radius ^ 2;
Print(area)

---

Kết luận

MathLang hướng tới một ngôn ngữ có cú pháp toán học dễ tiếp cận, tên hàm tiếng Anh rõ ràng, độ chính xác số học cao và khả năng mở rộng thành môi trường lập trình hoàn chỉnh.

Nguyên tắc quan trọng nhất khi phát triển là giữ ba thành phần luôn đồng bộ:

1. Đặc tả cú pháp: Người dùng được phép viết gì.
2. Semantic analyzer: Biểu thức nào hợp lệ và vì sao.
3. Runtime: Chương trình thực sự tính toán và thực thi điều gì.

Một tính năng chỉ được coi là hoàn thành khi cả ba thành phần thống nhất và bộ kiểm thử xác nhận hành vi thực tế.