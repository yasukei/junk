#include <QtWidgets>
#include "addressbook.h"

AddressBook::AddressBook(QWidget *parent)
	: QWidget(parent)
{
    QLabel *nameLabel = new QLabel(tr("Name:"));
	nameLine = new QLineEdit;
	nameLine->setReadOnly(true);

    QLabel *addressLabel = new QLabel(tr("Address:"));
	addressText = new QTextEdit;
	addressText->setReadOnly(true);

	addButton = new QPushButton(tr("&add"));
	addButton->show();
	submitButton = new QPushButton(tr("&Submit"));
	submitButton->hide();
	cancelButton = new QPushButton(tr("&Cancel"));
	cancelButton->hide();
	connect(addButton, SIGNAL(clicked()), this, SLOT(addContact()));
	connect(submitButton, SIGNAL(clicked()), this, SLOT(submitContact()));
	connect(cancelButton, SIGNAL(clicked()), this, SLOT(cancel()));

	QVBoxLayout *buttonLayout1 = new QVBoxLayout;
	buttonLayout1->addWidget(addButton, Qt::AlignTop);
	buttonLayout1->addWidget(submitButton);
	buttonLayout1->addWidget(cancelButton);
	buttonLayout1->addStretch();

	QGridLayout *mainLayout = new QGridLayout;
    mainLayout->addWidget(nameLabel, 0, 0);
	mainLayout->addWidget(nameLine, 0, 1);
    mainLayout->addWidget(addressLabel, 1, 0, Qt::AlignTop);
	mainLayout->addWidget(addressText, 1, 1);
	mainLayout->addLayout(buttonLayout1, 1, 2);

	setLayout(mainLayout);
	setWindowTitle(tr("Simple Address Book"));
}

void AddressBook::addContact()
{
	oldName = nameLine->text();
	oldAddress = addressText->toPlainText();

	nameLine->clear();
	addressText->clear();

	nameLine->setReadOnly(false);
	nameLine->setFocus(Qt::OtherFocusReason);
	addressText->setReadOnly(false);

	addButton->setEnabled(false);
	submitButton->show();
	cancelButton->show();
}

void AddressBook::submitContact()
{
	QString name = nameLine->text();
	QString address = addressText->toPlainText();

	if(name.isEmpty() || address.isEmpty())
	{
		QMessageBox::information(this, tr("Empty Field"),
				tr("Please enter a name and address."));
		return;
	}

	if(! contacts.contains(name))
	{
		contacts.insert(name, address);
		QMessageBox::information(this, tr("Add Successful"),
				tr("\"%1\" has been added to your address book.").arg(name));
	}
	else
	{
		QMessageBox::information(this, tr("Add Unsuccessful"),
				tr("Sorry, \"%1\" is already in your address book.").arg(name));
		return;
	}

	if(contacts.isEmpty())
	{
		nameLine->clear();
		addressText->clear();
	}

	nameLine->setReadOnly(true);
	addressText->setReadOnly(true);
	addButton->setEnabled(true);
	submitButton->hide();
	cancelButton->hide();
}

void AddressBook::cancel()
{
	nameLine->setText(oldName);
	nameLine->setReadOnly(true);

	addressText->setText(oldName);
	addressText->setReadOnly(true);

	addButton->setEnabled(true);
	submitButton->hide();
	cancelButton->hide();
}
