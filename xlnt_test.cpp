#include <xlnt/xlnt.hpp>

int main()
{
    xlnt::workbook wb;
    xlnt::worksheet ws = wb.active_sheet();

    ws.cell("A1").value("Hello");
    ws.cell("B1").value("World");
    ws.cell("A2").value(1234);

    wb.save("output.xlsx");  // 保存到文件

    return 0;
}
