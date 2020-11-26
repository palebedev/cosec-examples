#include "notes.hpp"

#include <QtCore/QDataStream>

NoteEntity::~NoteEntity() = default;
Note::~Note() = default;
NotesFolder::~NotesFolder() = default;

QDataStream& operator<<(QDataStream& ds,const NoteEntity& ne)
{
    ds << ne.name();
    auto folder = dynamic_cast<const NotesFolder*>(&ne);
    ds << bool(folder);
    if(folder){
        ds << int(folder->items().size());
        for(auto& item:folder->items())
            ds << *item;
    }else
        ds << static_cast<const Note&>(ne).text();
    return ds;
}

QDataStream& operator>>(QDataStream& ds,std::unique_ptr<NoteEntity>& ne)
{
    QString name;
    ds >> name;
    bool isFolder;
    ds >> isFolder;
    if(isFolder){
        auto folder = std::make_unique<NotesFolder>();
        int n;
        ds >> n;
        folder->items().reserve(std::size_t(n));
        std::unique_ptr<NoteEntity> e;
        for(int i=0;i<n;++i){
            ds >> e;
            folder->items().push_back(std::move(e));
        }
        ne = std::move(folder);
    }else{
        auto note = std::make_unique<Note>();
        QString text;
        ds >> text;
        note->setText(std::move(text));
        ne = std::move(note);
    }
    ne->setName(name);
    return ds;
}
