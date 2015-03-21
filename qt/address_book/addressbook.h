#ifndef ADDRESSBOOK_H
#define ADDRESSBOOK_H

#include <QWidget>

class QLabel;
class QLineEdit;
class QTextEdit;

class AddressBook : public QWidget
{
	Q_OBJECT

public:
	AddressBook(QWidget *parent = 0);

private:
	QLineEdit *nameLine;
	QTextEdit *addressText;
};

#endif /* ADDRESSBOOK_H */
