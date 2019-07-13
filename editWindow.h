#ifndef EDIT_WINDOW
#define EDIT_WINDOW

#include <QWidget>
#include <QLineEdit>

class Window : public QWidget
{


public:
	Window();

public slots:
	void echoChanged(int);
	void validatorChanged(int);
	void alignmentChanged(int);
	void inputMaskChanged(int);
	void accessChanged(int);

private:
	QLineEdit * echoLineEdit;
	QLineEdit *validatorLineEdit;
	QLineEdit *alignmentLineEdit;
	QLineEdit *inputMaskLineEdit;
	QLineEdit *accessLineEdit;
};

#endif
