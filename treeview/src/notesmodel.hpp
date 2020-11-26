#ifndef UUID_40D42F4C_55AA_42F8_A601_E665B124186A
#define UUID_40D42F4C_55AA_42F8_A601_E665B124186A

#include "notes.hpp"

#include <QtCore/QAbstractItemModel>
#include <QtGui/QFont>
#include <qnamespace.h>

inline NoteEntity* entity(const QModelIndex& mi) noexcept
{
    return static_cast<NoteEntity*>(mi.internalPointer());
}

inline NotesFolder* maybe_folder(const QModelIndex& mi) noexcept
{
    return dynamic_cast<NotesFolder*>(entity(mi));
}

inline Note* maybe_note(const QModelIndex& mi) noexcept
{
    return dynamic_cast<Note*>(entity(mi));
}

class NotesModel : public QAbstractItemModel
{
public:
    explicit NotesModel(QObject* parent = nullptr);

    Notes* notes() const noexcept
    {
        return notes_;
    }

    void setNotes(Notes* newNotes);

    QModelIndex index(int row,int column,const QModelIndex& parent = {}) const override;
    QModelIndex parent(const QModelIndex& index) const override;
    int columnCount(const QModelIndex& parent = {}) const override;
    int rowCount(const QModelIndex& parent = {}) const override;

    Qt::ItemFlags flags(const QModelIndex& index) const override;
    QVariant data(const QModelIndex& index,int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex& index,const QVariant& value,int role = Qt::EditRole) override;

    // This inserts note items. To insert a folder see next function.
    bool insertRows(int row,int count,const QModelIndex& parent = {}) override;
    bool insertFolder(int row,const QModelIndex& parent = {});
    bool removeRows(int row,int count,const QModelIndex& parent = {}) override;

    bool moveRows(const QModelIndex& sourceParent,int sourceRow,int count,
                  const QModelIndex& destinationParent,int destinationChild) override;

    Qt::DropActions supportedDropActions() const override;
    QStringList mimeTypes() const override;
    QMimeData* mimeData(const QModelIndexList& indexes) const override;
    bool dropMimeData(const QMimeData* data,Qt::DropAction action,int row,int column,
                      const QModelIndex& parent) override;
private:
    Notes* notes_ = nullptr;
    QFont boldFont_;
};

#endif
