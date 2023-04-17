#ifndef BOOKS_H
#define BOOKS_H

#include <QMainWindow>
#include <QtSql>
#include <QDebug>
#include <QFileInfo>

QT_BEGIN_NAMESPACE
namespace Ui { class Books; }
QT_END_NAMESPACE

class Books : public QMainWindow
{
    Q_OBJECT

public:
    Books(QWidget *parent = nullptr);
    ~Books();
    void addPerson();
    void removePerson();
    void addBook();
    void removeBook();
    void addRent();
    void removeRent();
    void tab_clicked(const QModelIndex &index);
    void tab_clicked2(const QModelIndex &index);
    void DBInfo(QSqlDatabase& _db);
    void RecordInfo(QSqlDatabase& _db, QString _name);

    void DisplayQuery(QSqlDatabase& _db, QString _q);

    void displayStringList(const QStringList& _list);

private:
    Ui::Books *ui;
    QSqlDatabase db;
    QSqlTableModel *model;
    QSqlTableModel *model2;
    QSqlTableModel *model3;
};
#endif // BOOKS_H
