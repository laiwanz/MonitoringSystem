#include <QTableWidget>
#include <QHeaderView>

// 自定义的 QTableWidget 子类
class CustomTableWidget : public QTableWidget {
public:
    CustomTableWidget(QWidget* parent = nullptr) : QTableWidget(parent) {}

protected:
    void sortItems(int column, Qt::SortOrder order) override {
        // 在这里实现你的自定义排序逻辑
        // 例如，可以使用 QTableWidget 的 sortItems() 方法进行排序
        QTableWidget::sortItems(column, order);
    }
};
