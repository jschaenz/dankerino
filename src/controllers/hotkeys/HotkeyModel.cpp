#include "HotkeyModel.hpp"

#include "util/StandardItemHelper.hpp"

namespace chatterino {

HotkeyModel::HotkeyModel(QObject *parent)
    : SignalVectorModel<std::shared_ptr<Hotkey>>(4, parent)
{
}

// turn a vector item into a model row
std::shared_ptr<Hotkey> HotkeyModel::getItemFromRow(
    std::vector<QStandardItem *> &row, const std::shared_ptr<Hotkey> &original)
{
    auto keySeq = QKeySequence(row[1]->data(Qt::EditRole).toString());
    auto action = row[2]->data(Qt::EditRole).toString();
    auto args = row[3]->data(Qt::EditRole).toString();
    return std::make_shared<Hotkey>(original->scope(), keySeq, action,
                                    original->arguments(), original->name());
}

// turns a row in the model into a vector item
void HotkeyModel::getRowFromItem(const std::shared_ptr<Hotkey> &item,
                                 std::vector<QStandardItem *> &row)
{
    setStringItem(row[0], item->name(), false);
    row[0]->setData(QFont("Segoe UI", 10), Qt::FontRole);

    setStringItem(row[1], item->toPortableString(), true);
    row[1]->setData(QFont("Segoe UI", 10), Qt::FontRole);

    setStringItem(row[2], item->action(), true);
    row[2]->setData(QFont("Segoe UI", 10), Qt::FontRole);

    if (item->arguments().size() == 0)
    {
        setStringItem(row[3], "", true);
    }
    else
    {
        QString text;
        for (auto &arg : item->arguments())
        {
            text.append(arg + ", ");
        }
        text.chop(2);  // remove last ", "
        setStringItem(row[3], text, true);
    }
    row[3]->setData(QFont("Segoe UI", 10), Qt::FontRole);
}

int HotkeyModel::beforeInsert(const std::shared_ptr<Hotkey> &item,
                              std::vector<QStandardItem *> &row,
                              int proposedIndex)
{
    const auto category = item->getCategory();
    if (this->categoryCount_[category]++ == 0)
    {
        auto newRow = this->createRow();

        setStringItem(newRow[0], category, false, false);
        newRow[0]->setData(QFont("Segoe UI Light", 16), Qt::FontRole);

        this->insertCustomRow(std::move(newRow), proposedIndex);

        return proposedIndex + 1;
    }

    return proposedIndex;
}

void HotkeyModel::afterRemoved(const std::shared_ptr<Hotkey> &item,
                               std::vector<QStandardItem *> &row, int index)
{
    auto it = this->categoryCount_.find(item->getCategory());
    assert(it != this->categoryCount_.end());

    if (it->second <= 1)
    {
        this->categoryCount_.erase(it);
        this->removeCustomRow(index - 1);
    }
    else
    {
        it->second--;
    }
}

}  // namespace chatterino
