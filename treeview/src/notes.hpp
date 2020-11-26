#ifndef UUID_6228D964_6096_498C_83B1_89FDBED474E9
#define UUID_6228D964_6096_498C_83B1_89FDBED474E9

#include <QtCore/QCoreApplication>

#include <memory>
#include <vector>

class NotesFolder;

class NoteEntity
{
public:
    virtual ~NoteEntity();

    NotesFolder* parent() const noexcept
    {
        return parent_;
    }

    void setParent(NotesFolder* newParent) noexcept
    {
        parent_ = newParent;
    }

    const QString name() const noexcept
    {
        return name_;
    }

    void setName(QString newName) noexcept
    {
        name_ = std::move(newName);
    }
protected:
    NoteEntity(NotesFolder* parent,QString defaultName) noexcept
        : parent_{parent},
          name_{std::move(defaultName)}
    {}
private:
    NotesFolder* parent_;
    QString name_;
};

class Note : public NoteEntity
{
    Q_DECLARE_TR_FUNCTIONS(Note)
public:
    Note(NotesFolder* parent = nullptr)
        : NoteEntity{parent,tr("Note")}
    {}
    ~Note();

    const QString& text() const noexcept
    {
        return text_;
    }

    void setText(QString newText) noexcept
    {
        text_ = std::move(newText);
    }
private:
    QString text_;
};

class NotesFolder : public NoteEntity
{
    Q_DECLARE_TR_FUNCTIONS(NotesFolder)
public:
    NotesFolder(NotesFolder* parent = nullptr)
        : NoteEntity(parent,tr("Folder"))
    {}
    ~NotesFolder();

    std::vector<std::unique_ptr<NoteEntity>>& items() noexcept
    {
        return items_;
    }

    const std::vector<std::unique_ptr<NoteEntity>>& items() const noexcept
    {
        return items_;
    }
private:
    // QScopedPointer is not moveable and QVector doesn't like move-only objects.
    std::vector<std::unique_ptr<NoteEntity>> items_;
};

class Notes
{
public:
    NotesFolder& root() noexcept
    {
        return root_;
    }

    const NotesFolder& root() const noexcept
    {
        return root_;
    }
private:
    NotesFolder root_;
};

QDataStream& operator<<(QDataStream& ds,const NoteEntity& ne);
QDataStream& operator>>(QDataStream& ds,std::unique_ptr<NoteEntity>& ne);

#endif
