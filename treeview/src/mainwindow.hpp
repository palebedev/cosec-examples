#ifndef UUID_AECAA7D5_DE70_4E1D_8D17_57DF9461CA80
#define UUID_AECAA7D5_DE70_4E1D_8D17_57DF9461CA80

#include <QtCore/QScopedPointer>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE
namespace Ui
{
    class MainWindow;
}
QT_END_NAMESPACE
class Note;
class Notes;
class NotesModel;

class MainWindow : public QWidget
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override;
private:
    QScopedPointer<Ui::MainWindow> ui_;
    QScopedPointer<Notes> notes_;
    NotesModel* model_;
    Note* currentNote = nullptr;

    void insert(void (*inserter)(NotesModel*,const QModelIndex&));
private slots:
    void on_newFolderButton_clicked();
    void on_newItemButton_clicked();
    void on_deleteButton_clicked();
};

#endif
