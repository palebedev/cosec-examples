#include "notesmodel.hpp"

#include "notes.hpp"

#include <QtCore/QDataStream>
#include <QtCore/QMimeData>
#include <QtWidgets/QApplication>

#include <boost/iterator/iterator_adaptor.hpp>

NotesModel::NotesModel(QObject* parent)
    : QAbstractItemModel{parent},
      boldFont_{QApplication::font()}
{
    boldFont_.setBold(true);
}

void NotesModel::setNotes(Notes* newNotes)
{
    if(notes_!=newNotes){
        beginResetModel();
        notes_ = newNotes;
        endResetModel();
    }
}

QModelIndex NotesModel::index(int row,int column,const QModelIndex& parent) const
{
    if(!notes_||row<0||column)
        return {};
    auto p = parent.isValid()?maybe_folder(parent):&notes_->root();
    if(!p||std::size_t(row)>=p->items().size())
        return {};
    return createIndex(row,0,p->items()[std::size_t(row)].get());
}

QModelIndex NotesModel::parent(const QModelIndex& index) const
{
    if(!notes_||!index.isValid())
        return {};
    auto p = entity(index);
    NoteEntity* parent = p->parent();
    if(parent==&notes_->root())
        return {};
    auto& items = parent->parent()->items();
    for(std::size_t i=0;i<items.size();++i)
        if(items[i].get()==parent)
            return createIndex(int(i),0,parent);
    // Unreachable
    return {};
}

int NotesModel::columnCount(const QModelIndex& /*parent*/) const
{
    return bool(notes_);
}

int NotesModel::rowCount(const QModelIndex& parent) const
{
    if(!notes_)
        return 0;
    auto p = parent.isValid()?maybe_folder(parent):&notes_->root();
    return p?int(p->items().size()):0;
}

Qt::ItemFlags NotesModel::flags(const QModelIndex& index) const
{
    if(!notes_)
        return {};
    if(!index.isValid())
        return Qt::ItemIsDropEnabled;
    Qt::ItemFlags flags = Qt::ItemIsEnabled|Qt::ItemIsSelectable|Qt::ItemIsEditable|
                          Qt::ItemIsDragEnabled;
    if(maybe_folder(index))
        flags |= Qt::ItemIsDropEnabled;
    else
        flags |= Qt::ItemNeverHasChildren;
    return flags;
}

QVariant NotesModel::data(const QModelIndex& index,int role) const
{
    if(notes_&&index.isValid()){
        auto p = entity(index);
        switch(role){
            case Qt::DisplayRole:
            case Qt::EditRole:
                return p->name();
            case Qt::FontRole:
                if(dynamic_cast<NotesFolder*>(p))
                    return boldFont_;                
        }
    }
    return {};
}

bool NotesModel::setData(const QModelIndex& index,const QVariant& value,int role)
{
    if(notes_&&index.isValid()){
        auto p = entity(index);
        switch(role){
            case Qt::EditRole:
                p->setName(value.toString());
                return true;
        }
    }
    return false;
}

namespace
{
    class note_generator : public boost::iterator_adaptor<
        note_generator,
        std::ptrdiff_t,
        std::unique_ptr<NoteEntity>,
        boost::random_access_traversal_tag,
        std::unique_ptr<NoteEntity>,
        std::ptrdiff_t
    >
    {
    public:
        explicit note_generator(NotesFolder* folder)
            : note_generator::iterator_adaptor_{0},
              folder_{folder}
        {}

        explicit note_generator(std::ptrdiff_t i = 0) noexcept
            : note_generator::iterator_adaptor_{i}
        {}
    private:
        friend class boost::iterator_core_access;

        NotesFolder* folder_ = nullptr; 

        std::unique_ptr<NoteEntity> dereference() const noexcept
        {
            return std::make_unique<Note>(folder_);
        }
    };
}

bool NotesModel::insertRows(int row,int count,const QModelIndex& parent)
{
    if(!notes_||count<=0)
        return false;
    auto p = parent.isValid()?maybe_folder(parent):&notes_->root();
    if(!p)
        return false;
    auto& items = p->items();
    beginInsertRows(parent,row,row+count-1);
    // vector::insert(where,count,what) cannot be used for move-only elements.
    // To avoid multiple shifts, we use iterator pair insertion with note_generator.
    // This is still somewhat complex due to exception safety guarantees, but better.
    items.insert(items.begin()+row,note_generator{p},note_generator{std::ptrdiff_t(count)});
    endInsertRows();
    return true;
}

bool NotesModel::insertFolder(int row,const QModelIndex& parent)
{
    if(!notes_)
        return false;
    auto p = parent.isValid()?maybe_folder(parent):&notes_->root();
    if(!p)
        return false;
    auto& items = p->items();
    beginInsertRows(parent,row,row);
    items.insert(items.begin()+row,std::make_unique<NotesFolder>(p));
    endInsertRows();
    return true;
}

bool NotesModel::removeRows(int row,int count,const QModelIndex& parent)
{
    if(!notes_||count<=0||row<0)
        return false;
    auto p = parent.isValid()?maybe_folder(parent):&notes_->root();
    if(!p||std::size_t(row+count)>p->items().size())
        return false;
    auto& items = p->items();
    beginRemoveRows(parent,row,row+count-1);
    items.erase(items.begin()+row,items.begin()+(row+count));
    endInsertRows();
    return true;
}

// This is not needed for the move through item views, as they use dropMimeData+removeRows,
// but we provide this for consistency.
bool NotesModel::moveRows(const QModelIndex& sourceParent,int sourceRow,int count,
                          const QModelIndex& destinationParent,int destinationChild)
{
    if(!notes_||count<=0||sourceRow<0||destinationChild<0)
        return false;
    auto sp = sourceParent.isValid()?maybe_folder(sourceParent):&notes_->root();
    if(!sp||std::size_t(sourceRow+count)>sp->items().size())
        return false;
    auto dp = destinationParent.isValid()?maybe_folder(destinationParent):&notes_->root();
    if(!dp||std::size_t(destinationChild)>dp->items().size())
        return false;
    auto& sitems = sp->items();
    auto& ditems = dp->items();
    if(!beginMoveRows(sourceParent,sourceRow,sourceRow+count-1,destinationParent,destinationChild))
        return false;
    ditems.insert(ditems.begin()+destinationChild,
                  std::move_iterator{sitems.begin()+sourceRow},
                  std::move_iterator{sitems.begin()+(sourceRow+count)});
    if(sp==dp&&destinationChild<sourceRow)
        sourceRow += count;
    sitems.erase(sitems.begin()+sourceRow,sitems.begin()+(sourceRow+count));
    endMoveRows();
    return true;
}

Qt::DropActions NotesModel::supportedDropActions() const
{
    return {Qt::IgnoreAction|Qt::CopyAction|Qt::MoveAction};
}

namespace
{
    const QString mimeType = QStringLiteral("application/x.treeview-notes");
}

QStringList NotesModel::mimeTypes() const
{
    return {mimeType};
}

QMimeData* NotesModel::mimeData(const QModelIndexList& indexes) const
{
    if(!notes_||indexes.isEmpty())
        return nullptr;
    // Refuse to move items from different parents.
    for(int i=1;i<indexes.size();++i)
        if(indexes[i].parent()!=indexes[0].parent())
            return nullptr;
    auto md = new QMimeData;
    QByteArray ba;
    {
        QDataStream ds{&ba,QIODevice::WriteOnly};
        ds << indexes.size();
        for(const QModelIndex& index:indexes)
            ds << *entity(index);
    }
    md->setData(mimeType,std::move(ba));
    return md;
}

bool NotesModel::dropMimeData(const QMimeData* data,Qt::DropAction action,int row,int column,
                              const QModelIndex& parent)
{
    if(!notes_||column>0||!canDropMimeData(data,action,row,column,parent))
        return false;
    if(action==Qt::IgnoreAction)
        return true;
    auto p = parent.isValid()?maybe_folder(parent):&notes_->root();
    if(!p)
        return false;
    auto ba = data->data(mimeType);
    QDataStream ds{&ba,QIODevice::ReadOnly};
    int n;
    ds >> n;
    std::vector<std::unique_ptr<NoteEntity>> items;
    items.resize(std::size_t(n));
    for(auto& item:items){
        ds >> item;
        item->setParent(p);
    }
    auto& pitems = p->items();
    if(row<0)
        row = int(pitems.size());
    beginInsertRows(parent,row,row+n-1);
    pitems.insert(pitems.begin()+row,
                  std::move_iterator{items.begin()},std::move_iterator{items.end()});
    endInsertRows();
    return true;    
}
