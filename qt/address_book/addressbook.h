#ifndef ADDRESSBOOK_H
#define ADDRESSBOOK_H

#include <QWidget>
#include <QMap>

class QLabel;
class QLineEdit;
class QTextEdit;
class QPushButton;

class AddressBook : public QWidget
{
	Q_OBJECT

public:
	AddressBook(QWidget *parent = 0);

public slots:
	void addContact();
	void submitContact();
	void cancel();

private:
	QPushButton *addButton;
	QPushButton *submitButton;
	QPushButton *cancelButton;
	QLineEdit *nameLine;
	QTextEdit *addressText;

	QMap<QString, QString> contacts;
	QString oldName;
	QString oldAddress;
};

#endif /* ADDRESSBOOK_H */
