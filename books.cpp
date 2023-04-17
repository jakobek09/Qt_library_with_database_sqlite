#include "books.h"
#include "ui_books.h"

Books::Books(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Books)
{
    ui->setupUi(this);

    connect(ui->pushButtonAddName, &QPushButton::clicked, this, &Books::addPerson);
    connect(ui->pushButtonRmName, &QPushButton::clicked, this, &Books::removePerson);

    connect(ui->pushButtonAddBook, &QPushButton::clicked, this, &Books::addBook);
    connect(ui->pushButtonRmBook, &QPushButton::clicked, this, &Books::removeBook);

    connect(ui->pushButtonAddRent, &QPushButton::clicked, this, &Books::addRent);
    connect(ui->pushButtonRmRent, &QPushButton::clicked, this, &Books::removeRent);

    connect(ui->tableView, &QTableView::clicked, this, &Books::tab_clicked);
    connect(ui->tableView_2, &QTableView::clicked, this, &Books::tab_clicked2);

    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("C:\\Users\\jakóbek\\OneDrive\\Pulpit\\PROJEKTY\\QT\\BooksSql\\database.sqlite");
    if (!db.open())
    {
        qDebug() << "blad";

         return ;
    }
    else
        qDebug() << "zajebiscie";

    model = new QSqlTableModel(this, db);
    model->setTable("Person");
    model->setEditStrategy(QSqlTableModel::OnFieldChange);
    model->select();
    ui->tableView->setModel(model);

    model2 = new QSqlTableModel(this, db);
    model2->setTable("Books");
    model2->setEditStrategy(QSqlTableModel::OnFieldChange);
    model2->select();
    ui->tableView_2->setModel(model2);

    model3 = new QSqlTableModel(this, db);
    model3->setTable("Rent");
    model3->setEditStrategy(QSqlTableModel::OnFieldChange);
    model3->select();
    ui->tableView_3->setModel(model3);
}

Books::~Books()
{
    delete ui;
    db.close();
}

void Books::addRent()
{
    QModelIndex personIndex = ui->tableView->currentIndex();
    int personId = model->record(personIndex.row()).value("id").toInt();
    QString name = model->record(personIndex.row()).value("name").toString();
    QString lName = model->record(personIndex.row()).value("LastName").toString();
    int age = model->record(personIndex.row()).value("age").toInt();

    QModelIndex bookIndex = ui->tableView_2->currentIndex();
    int bookId = model2->record(bookIndex.row()).value("id").toInt();
    QString title = model2->record(bookIndex.row()).value("title").toString();
    QString author = model2->record(bookIndex.row()).value("author").toString();
    int year = model2->record(bookIndex.row()).value("year").toInt();

    QSqlQuery query(db);
    query.prepare("INSERT INTO Rent (name, lastName, title, author, bookId, PersonId) VALUES (?, ?, ?, ?, ?, ?)");
    query.bindValue(0, name);
    query.bindValue(1, lName);
    query.bindValue(2, title);
    query.bindValue(3, author);
    query.bindValue(4, bookId);
    query.bindValue(5, personId);
    if (!query.exec()) {
        qDebug() << "Error during inserting record:" << query.lastError().text();
        qDebug() << personId << " " << bookId << "\n";
    } else {
        qDebug() << "Record inserted successfully.";
        model3->select();
    }
}

void Books::removeRent()
{
    QModelIndex index = ui->tableView_3->currentIndex();
    int rentId = model3->record(index.row()).value("id").toInt();

    QSqlQuery query(db);
    query.prepare("DELETE FROM Rent WHERE id = ?");
    query.bindValue(0, rentId);

    if (!query.exec()) {
        qDebug() << "Error during inserting record:" << query.lastError().text();
    } else {
        qDebug() << "Record deleted successfully.";
        model3->select();
    }
}
void Books::tab_clicked(const QModelIndex &index)
{
    int personId = model->record(index.row()).value("id").toInt();
    QString name = model->record(index.row()).value("name").toString();
    QString lName = model->record(index.row()).value("LastName").toString();
    int age = model->record(index.row()).value("age").toInt();

    ui->textEditName->setText(name);
    ui->textEditLastName->setText(lName);
    ui->textEditAge->setText(QString::number(age));


}

void Books::tab_clicked2(const QModelIndex &index)
{

    int bookId = model2->record(index.row()).value("id").toInt();
    QString title = model2->record(index.row()).value("title").toString();
    QString author = model2->record(index.row()).value("author").toString();
    int year = model2->record(index.row()).value("year").toInt();


    ui->textEditTitle->setText(title);
    ui->textEditAuthor->setText(author);
    ui->textEditYear->setText(QString::number(year));

}

void Books::addPerson()
{
    QPushButton *button = (QPushButton*)sender();

    QSqlQuery query(db);
    QString name = ui->textEditName->toPlainText();
    QString lName = ui->textEditLastName->toPlainText();
    int age = ui->textEditAge->toPlainText().toInt();
    if(name == "" || lName == "" || ui->textEditAge->toPlainText() == "")
    {
        name = NULL;
        lName = NULL;
        age = NULL;
    }
    query.prepare("INSERT INTO Person (name, lastName, age) VALUES (?, ?, ?)");
    query.bindValue(0, name); // ustawienie wartości pierwszego parametru
    query.bindValue(1, lName); // ustawienie wartości drugiego parametru
    query.bindValue(2, age);
    if (!query.exec()) {
        qDebug() << "Error during inserting record:" << query.lastError().text();
    } else {
        qDebug() << "Record inserted successfully.";
        model->select();
    }
        //DBInfo(db);
        //RecordInfo(db, "Person");
}

void Books::removePerson()
{
    QPushButton *button = (QPushButton*)sender();

    QModelIndex index = ui->tableView->currentIndex();
    int personId = model->record(index.row()).value("id").toInt();

    // check if the person is already rented
    QSqlQuery query(db);
    query.prepare("SELECT * FROM Rent WHERE PersonId = ?");
    query.bindValue(0, personId);
    if (!query.exec()) {
        qDebug() << "Error during checking rent records:" << query.lastError().text();
        return;
    }

    if (query.next()) {
        qDebug() << "This person is already rented a book, cannot remove the record.";
        return;
    }

    // remove the person record
    query.prepare("DELETE FROM Person WHERE id = ?");
    query.bindValue(0, personId);
    if (!query.exec()) {
        qDebug() << "Error during deleting person record:" << query.lastError().text();
    } else {
        qDebug() << "Person record deleted successfully.";
        model->select();
    }
}

void Books::addBook()
{
    QPushButton *button = (QPushButton*)sender();

    QSqlQuery query(db);
    QString title = ui->textEditTitle->toPlainText();
    QString author = ui->textEditAuthor->toPlainText();
    int year = ui->textEditYear->toPlainText().toInt();
    if(title == "" || author == "" || ui->textEditYear->toPlainText() == "")
    {
        title = NULL;
        author = NULL;
        year = NULL;
    }
    query.prepare("INSERT INTO Books (title, author, year) VALUES (?, ?, ?)");
    query.bindValue(0, title);
    query.bindValue(1, author);
    query.bindValue(2, year);
    if (!query.exec()) {
        qDebug() << "Error during deleting record:" << query.lastError().text();
        qDebug() << title << " " << author << " " <<  year << "\n";
    } else {
        qDebug() << "Record inserted successfully.";
        model2->select();
    }
        //DBInfo(db);
        //RecordInfo(db, "Person");
}

void Books::removeBook()
{
    QPushButton *button = (QPushButton*)sender();

    QModelIndex index = ui->tableView_2->currentIndex();
    int bookId = model2->record(index.row()).value("id").toInt();

    // check if the book is already rented
    QSqlQuery query(db);
    query.prepare("SELECT * FROM Rent WHERE bookId = ?");
    query.bindValue(0, bookId);
    if (!query.exec()) {
        qDebug() << "Error during checking rent records:" << query.lastError().text();
        return;
    }

    if (query.next()) {
        qDebug() << "This book is already rented, cannot remove the record.";
        return;
    }

    // remove the book record
    query.prepare("DELETE FROM Books WHERE id = ?");
    query.bindValue(0, bookId);
    if (!query.exec()) {
        qDebug() << "Error during deleting book record:" << query.lastError().text();
    } else {
        qDebug() << "Book record deleted successfully.";
        model2->select();
    }
}
void Books::DBInfo(QSqlDatabase& _db)
{
    qDebug() << "databaseName = " << ((const char*)_db.databaseName().toLatin1())
            << ", driverName = " << ((const char*)_db.driverName().toLatin1()) << "\n";



    if ( _db.isOpen() )
    {
        //wyswietlamy liste tabel
        displayStringList(_db.tables( QSql::Tables));
        // QStringList list = db.tables( QSql::Tables);

    }
    else
    {
        qDebug() << "Baza danych: " << ((const char*)_db.databaseName().toLatin1()) << " jest zamknieta\n"
                << "Blad: " << ((const char*)_db.lastError().databaseText().toLatin1());

    }
}

void Books::RecordInfo(QSqlDatabase& _db, QString _name)
{
    QSqlRecord rec = _db.record(_name);


    //
    //wyswietlamy podstawowe informacje o rekordzie
    //

    qDebug() << "---------------------\n"
         << "  Info o rekordzie\n"
         << "---------------------\n"
         <<"Tabela " << ((const char*)_name.toLatin1()) << " zawiera " << rec.count() << " pola:\n";



    //
    // wyswietlamy liste pol w rekordzie
    //

    for (int i = 0; i < rec.count(); i++)
    {
        qDebug() << "Pole " << i << ": " << ((const char*)rec.fieldName(i).toLatin1()) << "\n";
    }


    qDebug() << "---------------------\n";


    //
    //budujemy zapytanie SQL
    //
    DisplayQuery(_db, QString("select * from ") + _name);

    //
    //budujemy zapytanie SQL
    //
    DisplayQuery(_db, QString("select * from ") + _name + QString(" where nazwisko=\'Nowak\'"));

}

void Books::DisplayQuery(QSqlDatabase& _db, QString _q)
{
    qDebug() << "---------------------\n"
         << "  Zapytanie do bazy\n"
         << "---------------------\n";



    QSqlQuery query(_q, _db);

    qDebug() << "isActive() = " << (query.isActive() ? "true" : "false") << "\n"
         << "isSelect() = " << (query.isSelect() ? "true" : "false") << "\n"
         << "size()= " << query.size() << "\n"
         << "numRowsAffected() = " << query.numRowsAffected() << "\n"
         << "executedQuery()= " << ((const char*)query.executedQuery().toLatin1()) << "\n";



    qDebug() << "---------------------\n";

    //
    // wyswietlamy zawartosc rekordu
    //


    while(query.next())
    {
        qDebug() << "Wiersz " << query.at() << ": ";
        for(int i= 0; i < query.record().count(); i++)
        {
            qDebug() << ((const char*)query.value(i).toString().toLatin1()) << "; ";
        }
        qDebug() << "\n";
    }

    query.finish();
}

void Books::displayStringList(const QStringList& _list)
{
    int i =0;
    QStringList list = _list;
    qDebug() << "Elementow na liscie: " << list.length() << "\n";
    QStringList::Iterator it = list.begin();
    while( it != list.end() )
    {
        // we save the name of the first table for later
        //if (table.isEmpty()) table = *it;
        qDebug() << "Item " << i << ":\t" << ((const char*)(*it).toLatin1()) << "\n";
        ++it;
        ++i;
    }
}
