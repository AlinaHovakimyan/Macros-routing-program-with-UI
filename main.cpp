#include <iostream>
#include <fstream>
#include "dyplomaWork.h"
#include "Tokenizer.h"
#include "Parser.h"
#include "Exception.h"
#include "Route.h"
#include "Drawer.h"
#include <QtWidgets/QApplication>


int main(int argc, char *argv[])
{

	QApplication a(argc, argv);
	chipView view;
	QWidget centralWidget;
	view.setCentralWidget(&centralWidget);
	try {
		Renderer* drawer = new Renderer(&centralWidget);
		view.setRenderer(drawer);
		drawer->show();
	}
	catch (ExceptionError exception) {
		std::cout << "Error: " << exception.message() << std::endl;
	}
	view.show();
	a.exec();
}